#include "expediter.h"
#include "dog_actions.h"
#include "events.h"
#include "events_interface.h"
#include "raymath.h"
#include <algorithm>

entities::table *expediter::expediter::find_table(int table_id) {
  auto it = tables_.find(static_cast<size_t>(table_id));
  return it == tables_.end() ? nullptr : it->second;
}

entities::waiter_dog *expediter::expediter::find_waiter(size_t waiter_id) {
  auto it = waiters_.find(waiter_id);
  return it == waiters_.end() ? nullptr : it->second;
}

entities::food_counter *expediter::expediter::find_counter(size_t counter_id) {
  auto it = food_counters_.find(counter_id);
  return it == food_counters_.end() ? nullptr : it->second;
}

entities::dishwasher *
expediter::expediter::find_dishwasher(size_t dishwasher_id) {
  auto it = dishwashers_.find(dishwasher_id);
  return it == dishwashers_.end() ? nullptr : it->second;
}

void expediter::expediter::fulfill_order(serving_job &job) {
  // Dispatch: send the assigned waiter to the counter to collect food. The
  // rest of the journey (counter -> table -> served) is driven by
  // on_dog_completed_path_event as the waiter reaches each stop. Called only
  // right after assign_waiter_to_order()/pick_food_counter() succeed in the
  // same pass, so both ids are guaranteed to still resolve here - no event
  // processing happens between picking and dispatching.
  auto *waiter = find_waiter(job.waiter_id);
  auto *counter = find_counter(job.counter_id);

  // ! must we set the waiter's state (like kick the state machine off)
  auto counter_interaction = counter->get_interaction_positions().left;
  dog_actions::send_dog_to_station(waiter->get_id(), counter_interaction,
                                   counter->get_id(), counter->get_position());
}

void expediter::expediter::process_orders() {
  // Reconcile first: a serving job whose waiter, counter, or table no
  // longer resolves (removed mid-service) can't continue. If the waiter is
  // still around, free it back to idle before dropping the reference -
  // otherwise it stays stuck in the serving state forever (is_available_for_
  // order() returns false for it) with no order left to ever call
  // set_idle() on it. Ids don't dangle, so removal never needed to reach
  // into orders_ itself; this is the one place that check happens; see
  // remove_waiter() for the one piece of cleanup that *does* have to happen
  // synchronously at removal time (reading is_carrying_food() before the
  // waiter is gone).
  for (auto &job : orders_) {
    if (job.status != order_status::serving) {
      continue;
    }
    auto *waiter = find_waiter(job.waiter_id);
    auto *counter = find_counter(job.counter_id);
    auto *table = find_table(static_cast<int>(job.table_id));
    if (waiter != nullptr && counter != nullptr && table != nullptr) {
      continue; // still healthy
    }
    if (waiter != nullptr) {
      waiter->set_idle();
    }
    job.waiter_id = empty_id;
    job.counter_id = empty_id;
    job.status = order_status::created;
  }

  // Drop orders that are done, or whose table no longer exists (nothing
  // left to deliver to - unlike waiter/counter, there's no "re-queue"
  // recovery for a missing table; the reconciliation pass above already
  // freed any waiter these jobs were holding).
  orders_.erase(std::remove_if(orders_.begin(), orders_.end(),
                               [this](const serving_job &j) -> bool {
                                 return j.status == order_status::fulfilled ||
                                        find_table(static_cast<int>(
                                            j.table_id)) == nullptr;
                               }),
                orders_.end());

  // Status + availability driven: only dispatch a created order when both a
  // free waiter and a stocked counter exist. Binding a waiter flips it to
  // unavailable, so a second order in the same pass can't grab the same one.
  for (auto &job : orders_) {
    if (job.status != order_status::created) {
      continue;
    }
    if (!are_waiters_available() || !are_counters_available()) {
      break;
    }
    auto *waiter = assign_waiter_to_order(job);
    auto *counter = pick_food_counter(job);
    if (waiter == nullptr || counter == nullptr) {
      break;
    }
    counter->reserve(); // this item is promised to this order until collected
    waiter->set_serving();
    job.status = order_status::serving;
    fulfill_order(job);
  }
}

void expediter::expediter::process_clearing_jobs() {
  // Drop jobs whose table no longer exists - nothing left to clear. Free
  // the waiter first if one is still assigned (and still around), same
  // reasoning as process_orders()'s reconciliation pass: otherwise it's
  // left stuck in the clearing state forever with no job left to call
  // set_idle() on it.
  for (auto &job : clearing_jobs_) {
    if (job.waiter_id == empty_id ||
        find_table(static_cast<int>(job.table_id)) != nullptr) {
      continue;
    }
    auto *waiter = find_waiter(job.waiter_id);
    if (waiter != nullptr) {
      waiter->set_idle();
    }
  }
  clearing_jobs_.erase(
      std::remove_if(clearing_jobs_.begin(), clearing_jobs_.end(),
                     [this](const clearing_job &j) -> bool {
                       return find_table(static_cast<int>(j.table_id)) ==
                              nullptr;
                     }),
      clearing_jobs_.end());

  // Mirrors process_orders(): clearing_job has no status field, so
  // waiter_id == empty_id IS the "created/unassigned" signal (see the
  // struct comment in expediter.h). There is no "erase if cleared" sweep
  // here - once dispatch_clearing_job()'s future completion logic finishes
  // a job (plate placed at the dishwasher), it should erase that job from
  // clearing_jobs_ directly rather than flagging a status for later removal.
  for (auto &job : clearing_jobs_) {
    if (job.waiter_id != empty_id) {
      continue; // already assigned/in progress
    }
    if (!are_waiters_available()) {
      break;
    }
    auto *waiter = assign_waiter_to_clear_table(job);
    if (waiter == nullptr) {
      break;
    }
    waiter->set_clearing();
    dispatch_clearing_job(job);
  }
}

bool expediter::expediter::are_waiters_available() const {
  return std::any_of(waiters_.begin(), waiters_.end(),
                     [](const auto &entry) -> bool {
                       return entry.second->is_available_for_order();
                     });
}

bool expediter::expediter::are_counters_available() const {
  return std::any_of(food_counters_.begin(), food_counters_.end(),
                     [](const auto &entry) -> bool {
                       return entry.second->has_available_food();
                     });
}

entities::waiter_dog *
expediter::expediter::assign_waiter_to_order(serving_job &job) {
  for (auto &entry : waiters_) {
    if (entry.second->is_available_for_order()) {
      job.waiter_id = entry.first;
      return entry.second;
    }
  }
  job.waiter_id = empty_id;
  return nullptr;
}

entities::food_counter *
expediter::expediter::pick_food_counter(serving_job &job) {
  for (auto &entry : food_counters_) {
    if (entry.second->has_available_food()) {
      job.counter_id = entry.first;
      return entry.second;
    }
  }
  job.counter_id = empty_id;
  return nullptr;
}

entities::waiter_dog *
expediter::expediter::assign_waiter_to_clear_table(clearing_job &job) {
  for (auto &entry : waiters_) {
    if (entry.second->is_available_for_order()) {
      job.waiter_id = entry.first;
      return entry.second;
    }
  }
  job.waiter_id = empty_id;
  return nullptr;
}

void expediter::expediter::register_waiter(entities::waiter_dog *dog) {
  waiters_[static_cast<size_t>(dog->get_id())] = dog;
}

void expediter::expediter::remove_waiter(size_t waiter_id) {
  // The only cleanup that must happen HERE, synchronously, before the
  // waiter is gone: reading is_carrying_food() one last time to decide
  // whether to release a food reservation back to the counter. Everything
  // else - re-queueing the order, dropping the stale id from a job - is
  // handled lazily by process_orders()'s reconciliation pass the next time
  // it resolves waiter_id to a failed lookup, so removal itself stays a
  // single map erase plus this one necessary read.
  auto *waiter = find_waiter(waiter_id);
  if (waiter != nullptr && !waiter->is_carrying_food()) {
    for (auto &job : orders_) {
      if (job.waiter_id == waiter_id) {
        auto *counter = find_counter(job.counter_id);
        if (counter != nullptr) {
          counter->release_reservation();
        }
      }
    }
  }
  waiters_.erase(waiter_id);
}

void expediter::expediter::register_food_counter(
    entities::food_counter *counter) {
  food_counters_[static_cast<size_t>(counter->get_id())] = counter;
}

void expediter::expediter::remove_food_counter(size_t counter_id) {
  // No synchronous read needed here (unlike remove_waiter's food-carrying
  // check) - process_orders()'s reconciliation pass detects a job's
  // counter_id no longer resolving the same way it detects a missing
  // waiter.
  food_counters_.erase(counter_id);
}

void expediter::expediter::register_table(entities::table *table) {
  tables_[static_cast<size_t>(table->get_id())] = table;
}

void expediter::expediter::remove_table(size_t table_id) {
  // No scanning here either: an order/clearing_job whose table_id no longer
  // resolves is dropped in process_orders()/process_clearing_jobs() (a
  // gone table has no "re-queue" recovery, unlike waiter/counter).
  tables_.erase(table_id);
}

void expediter::expediter::register_dishwasher(
    entities::dishwasher *dishwasher) {
  dishwashers_[static_cast<size_t>(dishwasher->get_id())] = dishwasher;
}

void expediter::expediter::remove_dishwasher(size_t dishwasher_id) {
  // dispatch_clearing_job() is still a stub (see its TODO), so no clearing
  // job can be actively routed to a dishwasher yet; once implemented, a
  // clearing_job's dishwasher_id no longer resolving should be handled the
  // same way process_orders() handles a missing counter (reset to empty_id,
  // let the job be picked up again).
  dishwashers_.erase(dishwasher_id);
}

void expediter::expediter::on_registered_waiter_event(
    const events::registered_waiter &event) {
  register_waiter(event.get_waiter());
}

void expediter::expediter::on_removed_waiter_event(
    const events::removed_waiter &event) {
  remove_waiter(event.get_waiter_id());
}

void expediter::expediter::on_registered_food_counter_event(
    const events::registered_food_counter &event) {
  register_food_counter(event.get_counter());
}

void expediter::expediter::on_removed_food_counter_event(
    const events::removed_food_counter &event) {
  remove_food_counter(event.get_counter_id());
}

void expediter::expediter::on_registered_table_event(
    const events::registered_table &event) {
  register_table(event.get_table());
}

void expediter::expediter::on_removed_table_event(
    const events::removed_table &event) {
  remove_table(event.get_table_id());
}

void expediter::expediter::on_registered_dishwasher_event(
    const events::registered_dishwasher &event) {
  register_dishwasher(event.get_dishwasher());
}

void expediter::expediter::on_removed_dishwasher_event(
    const events::removed_dishwasher &event) {
  remove_dishwasher(event.get_dishwasher_id());
}

void expediter::expediter::on_clear_table(const events::clear_table &event) {
  // Records the job; process_clearing_jobs() (called alongside
  // process_orders() from the game loop) picks it up once a waiter is
  // free, same shape as on_dog_reached_station_event creating an order for
  // process_orders() to pick up.
  auto *table = event.get_table();
  clearing_jobs_.push_back(clearing_job{
      next_clearing_job_id_++, static_cast<size_t>(table->get_id()),
      empty_id,   // waiter: unassigned until process_clearing_jobs() picks one
      empty_id}); // dishwasher: not yet picked until the plate is collected
}

void expediter::expediter::dispatch_clearing_job(clearing_job &job) {
  // TODO: [expediter::dispatch_clearing_job, whole function] [point in flow:
  // first-leg dispatch only, unchanged by the self-handling refactor] change
  // from [nothing - this function stays as-is]. This is the one piece of
  // clearing_job handling that legitimately stays expediter-driven: the
  // waiter has no job to self-handle yet at this point, so expediter must
  // command the first leg, exactly like fulfill_order() does for serving.
  // What changes is everything AFTER this dispatch: today the table ->
  // dishwasher progression is meant to happen inside
  // on_dog_completed_path_event (see the now-removed TODO block that used
  // to sit here); instead it will happen in
  // waiter_dog::clearing_table's completed_path branch /
  // waiter_dog::clearing_dishwasher's (see waiter_dog.cpp TODOs),
  // driven by next_clearing_target_query rather than this function.
  (void)job;
  // send waiter to table
  auto waiter = waiters_.at(job.waiter_id);
  waiter->set_clearing();
  auto table = tables_.at(job.table_id);
  // TODO decide on which one to pick,
  // TODO maybe a helper is needed to have some decision logic based on position
  // differneces
  auto destination = table->get_interaction_positions().left;
  std::unique_ptr<events::event> send_to_table =
      std::make_unique<events::send_dog_to_station>(
          job.waiter_id, destination, job.table_id, table->get_position());
  // update the waiter state (idle to clearing)
  event_interface::queue_event(send_to_table);
  // waiter goes to the table, collects the plate
  // then routes to the dishwasher
}

// TODO: [expediter::on_next_clearing_target_query, new function] [point in
// flow: replaces the body of on_dog_completed_path_event's clearing branch
// below] change from [reactive: find_if over clearing_jobs_ by waiter id
// inside the event handler, then mutate job.dishwasher_id and queue
// send_dog_to_station directly] to [pull: handler for
// queries::next_clearing_target_query, invoked synchronously from
// waiter_dog::clearing_table on completed_path. Body: find_if clearing_jobs_ by
// event.waiter_id(); if not found or no dishwasher available (mirror
// are_counters_available()/pick_food_counter()'s loop-and-check shape over
// dishwashers_ instead of food_counters_), return {has_next=false}; else
// write job.dishwasher_id, return {has_next=true, dishwasher_id,
// find_dishwasher(dishwasher_id)->get_interaction_positions()....}. Note
// this handler DOES mutate clearing_jobs_ as a side effect of a "query" -
// call this out explicitly if going with the query approach, since every
// other query in queries.h (is_colliding/path/can_place_decoration) is a
// pure lookup; the alternative is to keep the dishwasher pick + assignment
// happening eagerly in process_clearing_jobs() (like counter_id is picked
// eagerly in process_orders(), not lazily on arrival) and have the query be
// a pure read of whatever was already assigned.]

// TODO: [expediter::on_waiter_finished_clearing_event, new function] [point
// in flow: replaces the "once placed, erase this job... mirroring the
// served -> fulfilled transition" note in dispatch_clearing_job's old TODO,
// and the tail of on_dog_completed_path_event's clearing branch below]
// change from [reactive: on_dog_completed_path_event would erase the job and
// call waiter->set_idle() itself] to [handler for the new
// events::waiter_finished_clearing fact event fired by
// clearing_dishwasher - erase_if clearing_jobs_ by waiter_id
// only; do NOT call set_idle() here, the waiter already did that to itself
// as the last step of its completed_path branch (see waiter_dog.cpp), same division of
// labor as on_clear_table not touching the customer dog that fired it.]

void expediter::expediter::on_dog_reached_station_event(
    const events::dog_reached_station &event) {
  // The seated customer has requested an order. Record it as created and
  // unassigned; process_orders() binds a waiter + counter once both are
  // available. Resolves the live table pointer by id rather than caching a
  // copy of the event's id/position - table state (e.g. the assigned
  // customer id used for order_served below) is read straight off the
  // entity, matching how tables_/waiters_/food_counters_ are already
  // tracked.
  auto *table = find_table(static_cast<int>(event.get_station_id()));
  if (table == nullptr) {
    return;
  }
  orders_.push_back(serving_job{next_order_id_++, empty_id,
                                static_cast<size_t>(table->get_id()), empty_id,
                                order_status::created});
}

// TODO: [expediter::on_dog_completed_path_event, whole function] [point in
// flow: the single reactive dispatcher this whole refactor replaces] change
// from [subscribed to events::dog_completed_path; on every waiter path
// completion, find_if-scans orders_ (this function only - clearing_jobs_
// never got its matching branch added, which is the "unruly" growth this
// plan is responding to), infers the leg from is_carrying_food(), and
// mutates job state + dispatches the next leg all in one place] to [delete
// this function, its handler member, and the dog_completed_path
// subscribe/unsubscribe pair in expediter.h once both legs are covered:
//   - the `!waiter->is_carrying_food()` branch below becomes
//     waiter_dog::serving_counter's completed_path branch (waiter_dog.cpp TODO) calling
//     queries::next_serving_target_query
//   - the `else` (served) branch below becomes
//     waiter_dog::serving_table, firing order_served itself and
//     calling dog.set_idle()/release_food() itself instead of expediter
//     doing it to a resolved waiter* pointer
//   - the clearing side (which this function never actually implemented -
//     see the TODO that used to sit in dispatch_clearing_job) becomes
//     on_next_clearing_target_query / on_waiter_finished_clearing_event
//     above, plus the waiter_dog::clearing_table/clearing_dishwasher states
// Net effect: expediter stops being a dog_completed_path subscriber
// entirely for waiter purposes - maitre_d still subscribes to that same
// event independently for customer_dog concerns, so the event itself
// isn't removed, just expediter's use of it.]
void expediter::expediter::on_dog_completed_path_event(
    const events::dog_completed_path &event) {
  auto dog_id = static_cast<size_t>(event.get_id());
  auto it = std::find_if(
      orders_.begin(), orders_.end(), [dog_id](const serving_job &j) -> bool {
        return j.status == order_status::serving && j.waiter_id == dog_id;
      });
  if (it == orders_.end()) {
    return;
  }
  auto &active_order = *it;
  auto *waiter = find_waiter(active_order.waiter_id);
  auto *table = find_table(static_cast<int>(active_order.table_id));
  if (waiter == nullptr || table == nullptr) {
    // Whichever resource vanished mid-flight, process_orders()'s
    // reconciliation pass re-queues or drops this job next tick.
    return;
  }

  if (!waiter->is_carrying_food()) {
    // Reached the counter: collect food (the reservation is now fulfilled)
    // and head to the table.
    auto *counter = find_counter(active_order.counter_id);
    if (counter != nullptr && !counter->is_empty()) {
      waiter->hold_food(counter->take());
      counter->release_reservation();
    }
    Vector2 table_interaction = table->get_interaction_positions().right;
    dog_actions::send_dog_to_station(static_cast<int>(dog_id),
                                     table_interaction, table->get_id(),
                                     table->get_position());
  } else {
    // Reached the table carrying food: serve it, free the waiter. The
    // customer id is resolved from the table itself
    // (table->get_assigned_dog_id()) rather than a cached field, so it can't
    // go stale if the assignment changes between order creation and
    // delivery.
    std::unique_ptr<events::event> served =
        std::make_unique<events::order_served>(
            active_order.order_id, dog_id,
            static_cast<size_t>(table->get_assigned_dog_id()),
            static_cast<size_t>(table->get_id()), table->get_position());
    event_interface::queue_event(served);
    waiter->release_food();
    waiter->set_idle();
    active_order.status = order_status::fulfilled;
  }
}
