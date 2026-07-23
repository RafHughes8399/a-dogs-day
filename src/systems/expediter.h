/**
 * Service/order orchestration system.
 *
 * The expediter coordinates waiter dogs and order service flow. It tracks
 * waiter/food-counter/table/dishwasher entities by id in unordered_maps
 * keyed on entity id (register/remove events keep these maps current), and
 * resolves the live pointer on demand via find_waiter()/find_counter()/
 * find_table()/find_dishwasher(). Job structs (serving_job/clearing_job)
 * store ids rather than pointers for the same reason: an id can't dangle,
 * so removal is a single map erase - no scanning every in-flight job to null
 * out a pointer. A stale id is simply detected as a failed lookup wherever
 * it's next used (see process_orders()/process_clearing_jobs()), the same
 * as any other "is this still around" check. It still emits command events
 * for world-owning systems to execute world mutations such as pathing.
 */
#ifndef EXPEDITER_H
#define EXPEDITER_H

#include "dog_actions.h"
#include "entities.h"
#include "events.h"
#include "events_interface.h"
#include "raylib.h"
#include <cstddef>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>


namespace expediter {

inline constexpr size_t empty_id = static_cast<size_t>(-1);

enum order_status {
  created = 0,
  assigned = 1,
  scheduled = 2,
  serving = 3,
  served = 4,
  fulfilled = 5
};
// A serving job: an in-flight order routing a waiter counter -> table.
// Stores ids rather than live pointers (see file header); the customer id
// is resolved via find_table(table_id)->get_assigned_dog_id() when needed
// (e.g. building order_served), so it isn't duplicated here.
struct serving_job {
  size_t order_id;
  size_t waiter_id;  // empty_id while unassigned
  size_t table_id;   // set at creation
  size_t counter_id; // empty_id while unassigned
  order_status status;
};

// A table-clearing job: mirrors `serving_job`'s shape but routes a waiter
// table -> dishwasher (collect the dirty plate, drop it off) instead of
// counter -> table. Has no explicit status field - state is read off the
// ids themselves: waiter_id == empty_id means unassigned/created;
// waiter_id != empty_id && dishwasher_id == empty_id means en route to the
// table; dishwasher_id != empty_id means the plate has been picked up and
// the waiter is en route to drop it off. A job is "cleared" simply by being
// erased from clearing_jobs_ once the drop-off completes - no separate
// status value is needed to mark that, unlike serving_job's richer state
// machine.
struct clearing_job {
  size_t job_id;
  size_t table_id;      // set at creation
  size_t waiter_id;     // empty_id while unassigned
  size_t dishwasher_id; // empty_id until the plate is picked up
};

class expediter {
public:
  ~expediter() {
    event_interface::unsubscribe<events::registered_waiter>(
        registered_waiter_handler_);
    event_interface::unsubscribe<events::removed_waiter>(
        removed_waiter_handler_);
    event_interface::unsubscribe<events::registered_food_counter>(
        registered_food_counter_handler_);
    event_interface::unsubscribe<events::removed_food_counter>(
        removed_food_counter_handler_);
    event_interface::unsubscribe<events::registered_table>(
        registered_table_handler_);
    event_interface::unsubscribe<events::removed_table>(removed_table_handler_);
    event_interface::unsubscribe<events::registered_dishwasher>(
        registered_dishwasher_handler_);
    event_interface::unsubscribe<events::removed_dishwasher>(
        removed_dishwasher_handler_);
    event_interface::unsubscribe<events::dog_reached_station>(
        dog_reached_station_handler_);
    event_interface::unsubscribe<events::dog_completed_path>(
        dog_completed_path_handler_);
    event_interface::unsubscribe<events::clear_table>(clear_table_handler_);
    // TODO: [expediter destructor] [teardown] change from [unsubscribe
    // dog_completed_path_handler_ here, since expediter listens to every
    // waiter's completed path reactively] to [remove this unsubscribe (and
    // the handler member/subscribe call below) entirely once
    // serving/clearing self-handle via query - expediter no longer needs to
    // observe dog_completed_path at all for waiter legs (see
    // waiter_dog.cpp's on_arrived TODOs). Add
    // queries::leg_target_executor_.unsubscribe(...) calls for
    // next_serving_target/next_clearing_target instead, mirroring how
    // level_graph unsubscribes its query handlers on teardown.]
  }
  expediter()
      : next_order_id_(0), next_clearing_job_id_(0),
        registered_waiter_handler_(
            [this](const events::registered_waiter &event) -> void {
              on_registered_waiter_event(event);
            }),
        removed_waiter_handler_(
            [this](const events::removed_waiter &event) -> void {
              on_removed_waiter_event(event);
            }),
        registered_food_counter_handler_(
            [this](const events::registered_food_counter &event) -> void {
              on_registered_food_counter_event(event);
            }),
        removed_food_counter_handler_(
            [this](const events::removed_food_counter &event) -> void {
              on_removed_food_counter_event(event);
            }),
        registered_table_handler_(
            [this](const events::registered_table &event) -> void {
              on_registered_table_event(event);
            }),
        removed_table_handler_(
            [this](const events::removed_table &event) -> void {
              on_removed_table_event(event);
            }),
        registered_dishwasher_handler_(
            [this](const events::registered_dishwasher &event) -> void {
              on_registered_dishwasher_event(event);
            }),
        removed_dishwasher_handler_(
            [this](const events::removed_dishwasher &event) -> void {
              on_removed_dishwasher_event(event);
            }),
        dog_reached_station_handler_(
            [this](const events::dog_reached_station &event) -> void {
              on_dog_reached_station_event(event);
            }),
        dog_completed_path_handler_(
            [this](const events::dog_completed_path &event) -> void {
              on_dog_completed_path_event(event);
            }),
        clear_table_handler_([this](const events::clear_table &event) -> void {
          on_clear_table(event);
        }) {
    event_interface::subscribe<events::registered_waiter>(
        registered_waiter_handler_);
    event_interface::subscribe<events::removed_waiter>(removed_waiter_handler_);
    event_interface::subscribe<events::registered_food_counter>(
        registered_food_counter_handler_);
    event_interface::subscribe<events::removed_food_counter>(
        removed_food_counter_handler_);
    event_interface::subscribe<events::registered_table>(
        registered_table_handler_);
    event_interface::subscribe<events::removed_table>(removed_table_handler_);
    event_interface::subscribe<events::registered_dishwasher>(
        registered_dishwasher_handler_);
    event_interface::subscribe<events::removed_dishwasher>(
        removed_dishwasher_handler_);
    event_interface::subscribe<events::dog_reached_station>(
        dog_reached_station_handler_);
    event_interface::subscribe<events::dog_completed_path>(
        dog_completed_path_handler_);
    event_interface::subscribe<events::clear_table>(clear_table_handler_);
  }

  expediter(const expediter &other) = delete;
  expediter(expediter &&other) = delete;

  expediter &operator=(const expediter &other) = delete;
  expediter &operator=(expediter &&other) = delete;

  void register_waiter(entities::waiter_dog *dog);
  void remove_waiter(size_t waiter_id);
  void register_food_counter(entities::food_counter *counter);
  void remove_food_counter(size_t counter_id);
  void register_table(entities::table *table);
  void remove_table(size_t table_id);
  void register_dishwasher(entities::dishwasher *dishwasher);
  void remove_dishwasher(size_t dishwasher_id);

  // Counts of tracked entities / orders. Exposed for tests to assert
  // registration, removal, and order processing.
  size_t num_waiters() const { return waiters_.size(); }
  size_t num_counters() const { return food_counters_.size(); }
  size_t num_tables() const { return tables_.size(); }
  size_t num_dishwashers() const { return dishwashers_.size(); }
  size_t num_orders() const { return orders_.size(); }
  // Status of the first order (for tests); order_status::created acts as
  // the "no order" sentinel when the list is empty.
  order_status first_order_status() const {
    return orders_.empty() ? order_status::created : orders_.front().status;
  }
  // First tracked waiter / counter / dishwasher (for tests to drive
  // availability without depending on entity ids). nullptr when none are
  // tracked. Map iteration order is unspecified, but only ever one entity
  // of each kind exists in the current main level, so "first" is
  // unambiguous in practice.
  entities::waiter_dog *first_waiter() const {
    return waiters_.empty() ? nullptr : waiters_.begin()->second;
  }
  entities::food_counter *first_counter() const {
    return food_counters_.empty() ? nullptr : food_counters_.begin()->second;
  }
  entities::dishwasher *first_dishwasher() const {
    return dishwashers_.empty() ? nullptr : dishwashers_.begin()->second;
  }

  void process_orders();
  void fulfill_order(serving_job &job);
  bool are_waiters_available() const;
  bool are_counters_available() const;
  entities::waiter_dog *assign_waiter_to_order(serving_job &job);
  entities::food_counter *pick_food_counter(serving_job &job);
  entities::table *find_table(int table_id);
  entities::waiter_dog *find_waiter(size_t waiter_id);
  entities::food_counter *find_counter(size_t counter_id);
  entities::dishwasher *find_dishwasher(size_t dishwasher_id);

  // Table-clearing counterpart to process_orders()/fulfill_order()/
  // assign_waiter_to_order() above.
  void process_clearing_jobs();
  void dispatch_clearing_job(clearing_job &job);
  entities::waiter_dog *assign_waiter_to_clear_table(clearing_job &job);
  // TODO: [expediter public API] [new query handler entry points] change
  // from [no query handlers exist on expediter; all per-leg logic is
  // reactive, inside on_dog_completed_path_event] to [add
  // queries::leg_target on_next_serving_target_query(const
  // queries::next_serving_target_query& q) and on_next_clearing_target_query
  // analog - these are the functions expediter::expediter()'s constructor
  // subscribes to queries::leg_target_executor_, same subscribe-in-ctor
  // convention already used for every event handler below. The clearing one
  // is where pick_food_counter()'s "loop the registry, pick one, mark it
  // used" shape gets reused for dishwashers_.]

  void on_registered_waiter_event(const events::registered_waiter &event);
  void on_removed_waiter_event(const events::removed_waiter &event);
  void on_registered_food_counter_event(
      const events::registered_food_counter &event);
  void on_removed_food_counter_event(const events::removed_food_counter &event);
  void on_registered_table_event(const events::registered_table &event);
  void on_removed_table_event(const events::removed_table &event);
  void on_registered_dishwasher_event(
      const events::registered_dishwasher &event);
  void on_removed_dishwasher_event(const events::removed_dishwasher &event);
  void on_dog_reached_station_event(const events::dog_reached_station &event);
  // TODO: [expediter::on_dog_completed_path_event decl] [function to remove]
  // change from [handles both orders_ and clearing_jobs_ waiter-leg
  // transitions reactively off one shared event] to [delete once
  // next_serving_target/next_clearing_target query handlers above cover
  // both cases and waiter states self-handle - see the matching TODO on the
  // .cpp definition for what happens to each branch].
  void on_dog_completed_path_event(const events::dog_completed_path &event);
  void on_clear_table(const events::clear_table &event);
  // TODO: [expediter, new handler] [supporting change for
  // clearing_dishwasher::on_arrived's completion signal] add
  // void on_waiter_finished_clearing_event(const
  // events::waiter_finished_clearing &event); - erases the matching
  // clearing_jobs_ entry by waiter_id; no waiter->set_idle() call needed
  // here since the waiter already does that itself on its own on_arrived
  // (see waiter_dog.cpp TODO), same division of labor as
  // customer_dog::leave() firing customer_dog_left and maitre_d's handler
  // only freeing the *table*, not touching the dog.

private:
  std::unordered_map<size_t, entities::waiter_dog *> waiters_;
  std::unordered_map<size_t, entities::food_counter *> food_counters_;
  std::unordered_map<size_t, entities::table *> tables_;
  std::unordered_map<size_t, entities::dishwasher *> dishwashers_;
  std::vector<serving_job> orders_;
  size_t next_order_id_;
  std::vector<clearing_job> clearing_jobs_;
  size_t next_clearing_job_id_;

  events::event_handler<events::registered_waiter> registered_waiter_handler_;
  events::event_handler<events::removed_waiter> removed_waiter_handler_;
  events::event_handler<events::registered_food_counter>
      registered_food_counter_handler_;
  events::event_handler<events::removed_food_counter>
      removed_food_counter_handler_;
  events::event_handler<events::registered_table> registered_table_handler_;
  events::event_handler<events::removed_table> removed_table_handler_;
  events::event_handler<events::registered_dishwasher>
      registered_dishwasher_handler_;
  events::event_handler<events::removed_dishwasher>
      removed_dishwasher_handler_;
  events::event_handler<events::dog_reached_station>
      dog_reached_station_handler_;
  // TODO: [expediter private members] [handler storage] change from
  // [dog_completed_path_handler_ member below, subscribed in ctor,
  // unsubscribed in dtor] to [remove it; add
  // queries::query_handler<queries::next_serving_target_query, queries::leg_target>
  // and the clearing analog, matching how level_graph stores its query
  // handlers (see graph.h) rather than events::event_handler<T>. Also add
  // events::event_handler<events::waiter_finished_clearing>
  // waiter_finished_clearing_handler_ for the new completion event.]
  events::event_handler<events::dog_completed_path> dog_completed_path_handler_;
  events::event_handler<events::clear_table> clear_table_handler_;
};
} // namespace expediter

#endif
