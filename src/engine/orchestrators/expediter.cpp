#include "expediter.h"
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

void expediter::expediter::process_serving_job(serving_job &job) {
  // First leg only. The waiter is already in serving_counter (process_serving_
  // jobs sets it just before calling here, with the table for leg 2);
  // everything after this dispatch is driven by the waiter's own states.
  auto *counter = find_counter(job.counter_id);
  auto counter_interaction = counter->get_interaction_positions().left;
  // Executed, not queued - see process_clearing_job for why.
  const events::send_dog_to_station send_to_counter(
      job.waiter_id, counter_interaction, job.counter_id,
      counter->get_position());
  event_interface::execute_event(send_to_counter);
}

void expediter::expediter::process_serving_jobs() {
  // Reconcile first: a serving job whose waiter, counter, or table no
  // longer resolves (removed mid-service) can't continue. If the waiter is
  // still around, free it back to idle before dropping the reference -
  // otherwise it stays stuck in the serving state forever (is_available_for_
  // order() returns false for it) with no order left to ever call
  // set_idle() on it. Ids don't dangle, so removal never needed to reach
  // into serving_jobs_ itself; this is the one place that check happens; see
  // remove_waiter() for the one piece of cleanup that *does* have to happen
  // synchronously at removal time (reading is_carrying_food() before the
  // waiter is gone).
  for (auto &job : serving_jobs_) {
    if (job.status != serving_job_status::serving) {
      continue;
    }
    auto *waiter = find_waiter(job.waiter_id);
    auto *counter = find_counter(job.counter_id);
    auto *table = find_table(static_cast<int>(job.table_id));
    if (waiter != nullptr and counter != nullptr and table != nullptr) {
      continue; // still healthy
    }
    if (waiter != nullptr) {
      waiter->set_idle();
    }
    job.waiter_id = empty_id;
    job.counter_id = empty_id;
    job.status = serving_job_status::created;
  }

  // Drop orders that are done, or whose table no longer exists (nothing
  // left to deliver to - unlike waiter/counter, there's no "re-queue"
  // recovery for a missing table; the reconciliation pass above already
  // freed any waiter these jobs were holding).
  serving_jobs_.erase(
      std::remove_if(serving_jobs_.begin(), serving_jobs_.end(),
                     [this](const serving_job &j) -> bool {
                       return j.status == serving_job_status::fulfilled or
                              find_table(static_cast<int>(j.table_id)) ==
                                  nullptr;
                     }),
      serving_jobs_.end());

  // Status + availability driven: only dispatch a created order when both a
  // free waiter and a stocked counter exist. Binding a waiter flips it to
  // unavailable, so a second order in the same pass can't grab the same one.
  for (auto &job : serving_jobs_) {
    if (job.status != serving_job_status::created) {
      continue;
    }
    if (not are_waiters_available() or not are_counters_available()) {
      break;
    }
    auto *waiter = assign_waiter_to_serving_job(job);
    auto *counter = pick_food_counter(job);
    auto *table = find_table(static_cast<int>(job.table_id));
    if (waiter == nullptr or counter == nullptr or table == nullptr) {
      break;
    }
    counter->reserve(); // this item is promised to this order until collected
    waiter->set_serving(table->get_interaction_positions().right);
    job.status = serving_job_status::serving;
    process_serving_job(job);
  }
}

void expediter::expediter::process_clearing_jobs() {
  // Drop jobs whose table no longer exists - nothing left to clear. Free
  // the waiter first if one is still assigned (and still around), same
  // reasoning as process_serving_jobs()'s reconciliation pass: otherwise it's
  // left stuck in the clearing state forever with no job left to call
  // set_idle() on it.
  for (auto &job : clearing_jobs_) {
    if (job.waiter_id == empty_id or
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

  // Mirrors process_serving_jobs(): clearing_job has no status field, so
  // waiter_id == empty_id IS the "created/unassigned" signal (see the
  // struct comment in expediter.h). There is no "erase if cleared" sweep
  // here - once process_clearing_job()'s future completion logic finishes
  // a job (plate placed at the dishwasher), it should erase that job from
  // clearing_jobs_ directly rather than flagging a status for later removal.
  for (auto &job : clearing_jobs_) {
    if (job.waiter_id != empty_id) {
      continue; // already assigned/in progress
    }
    if (not are_waiters_available() or not are_dishwashers_available()) {
      break;
    }
    // Dishwasher first: assign_waiter_to_clearing_job writes job.waiter_id,
    // which is this job's "already assigned" marker - claiming it and then
    // failing to find a dishwasher would make the job skip itself forever.
    auto *dishwasher = pick_dishwasher(job);
    if (dishwasher == nullptr) {
      break;
    }
    auto *waiter = assign_waiter_to_clearing_job(job);
    if (waiter == nullptr) {
      break;
    }
    waiter->set_clearing(dishwasher->get_interaction_positions().left);
    process_clearing_job(job);
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

bool expediter::expediter::are_dishwashers_available() const {
  // No capacity model yet - dish_capacity_/num_plates_ on the station are
  // unused stubs, so any registered dishwasher will take a plate.
  return not dishwashers_.empty();
}

entities::waiter_dog *
expediter::expediter::assign_waiter_to_serving_job(serving_job &job) {
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

entities::dishwasher *expediter::expediter::pick_dishwasher(clearing_job &job) {
  // TODO (25 / 8 / 26) pick by capacity/proximity once the dishwasher station is real -
  // for now first registered wins, the counterpart to pick_food_counter's
  // has_available_food() check.
  if (dishwashers_.empty()) {
    job.dishwasher_id = empty_id;
    return nullptr;
  }
  auto &entry = *dishwashers_.begin();
  job.dishwasher_id = entry.first;
  return entry.second;
}

entities::waiter_dog *
expediter::expediter::assign_waiter_to_clearing_job(clearing_job &job) {
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
  // handled lazily by process_serving_jobs()'s reconciliation pass the next time
  // it resolves waiter_id to a failed lookup, so removal itself stays a
  // single map erase plus this one necessary read.
  auto *waiter = find_waiter(waiter_id);
  if (waiter != nullptr and not waiter->is_carrying_food()) {
    for (auto &job : serving_jobs_) {
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
  // check) - process_serving_jobs()'s reconciliation pass detects a job's
  // counter_id no longer resolving the same way it detects a missing
  // waiter.
  food_counters_.erase(counter_id);
}

void expediter::expediter::register_table(entities::table *table) {
  tables_[static_cast<size_t>(table->get_id())] = table;
}

void expediter::expediter::remove_table(size_t table_id) {
  // No scanning here either: an order/clearing_job whose table_id no longer
  // resolves is dropped in process_serving_jobs()/process_clearing_jobs() (a
  // gone table has no "re-queue" recovery, unlike waiter/counter).
  tables_.erase(table_id);
}

void expediter::expediter::register_dishwasher(
    entities::dishwasher *dishwasher) {
  dishwashers_[static_cast<size_t>(dishwasher->get_id())] = dishwasher;
}

void expediter::expediter::remove_dishwasher(size_t dishwasher_id) {
  // TODO (25 / 8 / 26) no job routes to a dishwasher yet, so nothing to reconcile. Once
  // the second leg lands, a clearing_job whose dishwasher_id stops resolving
  // should reset to empty_id and be picked up again, the way process_serving_jobs()
  // handles a missing counter.
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
  // process_serving_jobs() from the game loop) picks it up once a waiter is
  // free, same shape as on_dog_reached_station_event creating an order for
  // process_serving_jobs() to pick up.
  auto *table = event.get_table();
  clearing_jobs_.push_back(clearing_job{
      next_clearing_job_id_++, static_cast<size_t>(table->get_id()),
      empty_id,   // waiter: unassigned until process_clearing_jobs() picks one
      empty_id}); // dishwasher: not yet picked until the plate is collected
}

void expediter::expediter::process_clearing_job(clearing_job &job) {
  // First leg only. The waiter is already in clearing_table (process_clearing_
  // jobs sets it just before calling here, and hands it the dishwasher for leg
  // 2); everything after this dispatch is driven by the waiter's own states.
  auto table = tables_.at(job.table_id);
  // TODO (25 / 8 / 26) picking .left unconditionally - needs a helper that picks the
  // nearer interaction position. process_serving_job() has the same problem.
  auto destination = table->get_interaction_positions().left;
  // Executed, not queued, so the path exists before clearing_table's first
  // update - that lets it treat "nothing to walk" as an unreachable table
  // rather than as a path that simply hasn't arrived yet. Safe to run inline:
  // level is the only subscriber and its handler just paths the dog, so it
  // can't re-enter clearing_jobs_ while process_clearing_jobs iterates it.
  const events::send_dog_to_station send_to_table(
      job.waiter_id, destination, job.table_id, table->get_position());
  event_interface::execute_event(send_to_table);
}

void expediter::expediter::on_waiter_finished_clearing_event(
    const events::waiter_finished_clearing &event) {
  // Erase only - the waiter set itself idle as the last act of its own state,
  // same division of labour as on_clear_table not touching the customer dog.
  auto waiter_id = event.get_waiter_id();
  clearing_jobs_.erase(
      std::remove_if(clearing_jobs_.begin(), clearing_jobs_.end(),
                     [waiter_id](const clearing_job &j) -> bool {
                       return j.waiter_id == waiter_id;
                     }),
      clearing_jobs_.end());
}

expediter::serving_job *
expediter::expediter::find_serving_job_for_waiter(size_t waiter_id) {
  auto it = std::find_if(serving_jobs_.begin(), serving_jobs_.end(),
                         [waiter_id](const serving_job &j) -> bool {
                           return j.status == serving_job_status::serving and
                                  j.waiter_id == waiter_id;
                         });
  return it == serving_jobs_.end() ? nullptr : &*it;
}

void expediter::expediter::abandon_serving_job(serving_job &job) {
  auto *waiter = find_waiter(job.waiter_id);
  // Exactly one of these applies: food already collected means the reservation
  // was consumed at pickup, food not collected means it is still outstanding.
  if (waiter != nullptr and waiter->is_carrying_food()) {
    // TODO (25 / 8 / 26) return it to the counter once food-on-table is modelled.
    waiter->release_food();
  } else if (waiter != nullptr) {
    auto *counter = find_counter(job.counter_id);
    if (counter != nullptr) {
      counter->release_reservation();
    }
  }
  job.waiter_id = empty_id;
  job.counter_id = empty_id;
  job.status = serving_job_status::created;
}

void expediter::expediter::on_waiter_collected_food_event(
    const events::waiter_collected_food &event) {
  auto *job = find_serving_job_for_waiter(event.get_waiter_id());
  if (job == nullptr) {
    return;
  }
  auto *waiter = find_waiter(job->waiter_id);
  auto *counter = find_counter(job->counter_id);
  if (waiter == nullptr or counter == nullptr or counter->is_empty()) {
    return;
  }
  waiter->hold_food(counter->take());
  counter->release_reservation();
}

void expediter::expediter::on_waiter_served_order_event(
    const events::waiter_served_order &event) {
  auto *job = find_serving_job_for_waiter(event.get_waiter_id());
  if (job == nullptr) {
    return;
  }
  auto *table = find_table(static_cast<int>(job->table_id));
  if (table == nullptr) {
    return; // reconciliation drops the job next tick
  }
  // Customer resolved off the table rather than cached, so it can't go stale
  // between order creation and delivery.
  std::unique_ptr<events::event> served = std::make_unique<events::order_served>(
      job->job_id, job->waiter_id,
      static_cast<size_t>(table->get_assigned_dog_id()),
      static_cast<size_t>(table->get_id()), table->get_position());
  event_interface::queue_event(served);
  auto *waiter = find_waiter(job->waiter_id);
  if (waiter != nullptr) {
    waiter->release_food();
  }
  job->status = serving_job_status::fulfilled;
}

void expediter::expediter::on_waiter_abandoned_serving_event(
    const events::waiter_abandoned_serving &event) {
  auto *job = find_serving_job_for_waiter(event.get_waiter_id());
  if (job != nullptr) {
    abandon_serving_job(*job);
  }
}

void expediter::expediter::on_dog_reached_station_event(
    const events::dog_reached_station &event) {
  // The seated customer has requested an order. Record it as created and
  // unassigned; process_serving_jobs() binds a waiter + counter once both are
  // available. Resolves the live table pointer by id rather than caching a
  // copy of the event's id/position - table state (e.g. the assigned
  // customer id used for order_served below) is read straight off the
  // entity, matching how tables_/waiters_/food_counters_ are already
  // tracked.
  auto *table = find_table(static_cast<int>(event.get_station_id()));
  if (table == nullptr) {
    return;
  }
  serving_jobs_.push_back(serving_job{next_serving_job_id_++, empty_id,
                                static_cast<size_t>(table->get_id()), empty_id,
                                serving_job_status::created});
}
