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
 * it's next used (see process_serving_jobs()/process_clearing_jobs()), the same
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

enum serving_job_status {
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
  size_t job_id;
  size_t waiter_id;  // empty_id while unassigned
  size_t table_id;   // set at creation
  size_t counter_id; // empty_id while unassigned
  serving_job_status status;
};

// A table-clearing job: mirrors `serving_job`'s shape but routes a waiter
// table -> dishwasher (collect the dirty plate, drop it off) instead of
// counter -> table. Has no explicit status field - state is read off the
// ids themselves: waiter_id == empty_id means unassigned/created, anything
// else means dispatched. Both the table and the dishwasher are fixed when the
// job is dispatched, so neither id tracks progress through the journey - the
// waiter's own state does. A job is "cleared" simply by being erased from
// clearing_jobs_ once the drop-off completes - no separate status value is
// needed to mark that, unlike serving_job's richer state machine.
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
    event_interface::unsubscribe<events::clear_table>(clear_table_handler_);
    event_interface::unsubscribe<events::waiter_finished_clearing>(
        waiter_finished_clearing_handler_);
    event_interface::unsubscribe<events::waiter_collected_food>(
        waiter_collected_food_handler_);
    event_interface::unsubscribe<events::waiter_served_order>(
        waiter_served_order_handler_);
    event_interface::unsubscribe<events::waiter_abandoned_serving>(
        waiter_abandoned_serving_handler_);
  }
  expediter()
      : next_serving_job_id_(0), next_clearing_job_id_(0),
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
        clear_table_handler_([this](const events::clear_table &event) -> void {
          on_clear_table(event);
        }),
        waiter_finished_clearing_handler_(
            [this](const events::waiter_finished_clearing &event) -> void {
              on_waiter_finished_clearing_event(event);
            }),
        waiter_collected_food_handler_(
            [this](const events::waiter_collected_food &event) -> void {
              on_waiter_collected_food_event(event);
            }),
        waiter_served_order_handler_(
            [this](const events::waiter_served_order &event) -> void {
              on_waiter_served_order_event(event);
            }),
        waiter_abandoned_serving_handler_(
            [this](const events::waiter_abandoned_serving &event) -> void {
              on_waiter_abandoned_serving_event(event);
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
    event_interface::subscribe<events::clear_table>(clear_table_handler_);
    event_interface::subscribe<events::waiter_finished_clearing>(
        waiter_finished_clearing_handler_);
    event_interface::subscribe<events::waiter_collected_food>(
        waiter_collected_food_handler_);
    event_interface::subscribe<events::waiter_served_order>(
        waiter_served_order_handler_);
    event_interface::subscribe<events::waiter_abandoned_serving>(
        waiter_abandoned_serving_handler_);
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
  size_t num_serving_jobs() const { return serving_jobs_.size(); }
  size_t num_clearing_jobs() const { return clearing_jobs_.size(); }
  // Status of the first order (for tests); serving_job_status::created acts as
  // the "no order" sentinel when the list is empty.
  serving_job_status first_serving_job_status() const {
    return serving_jobs_.empty() ? serving_job_status::created : serving_jobs_.front().status;
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

  void process_serving_jobs();
  void process_serving_job(serving_job &job);
  bool are_waiters_available() const;
  bool are_counters_available() const;
  bool are_dishwashers_available() const;
  entities::waiter_dog *assign_waiter_to_serving_job(serving_job &job);
  entities::food_counter *pick_food_counter(serving_job &job);
  entities::dishwasher *pick_dishwasher(clearing_job &job);
  // The in-flight serving job this waiter is running, or null.
  serving_job *find_serving_job_for_waiter(size_t waiter_id);
  // Give the counter's reservation back and return the job to created so it can
  // be re-dispatched. Distinct from process_serving_jobs()'s reconciliation
  // pass, which handles vanished entities and must NOT release the reservation
  // (remove_waiter already did that).
  void abandon_serving_job(serving_job &job);
  entities::table *find_table(int table_id);
  entities::waiter_dog *find_waiter(size_t waiter_id);
  entities::food_counter *find_counter(size_t counter_id);
  entities::dishwasher *find_dishwasher(size_t dishwasher_id);

  // Table-clearing counterpart to process_serving_jobs()/process_serving_job()/
  // assign_waiter_to_serving_job() above.
  void process_clearing_jobs();
  void process_clearing_job(clearing_job &job);
  entities::waiter_dog *assign_waiter_to_clearing_job(clearing_job &job);

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
  // TODO (25 / 8 / 26) delete once the waiter states self-handle both legs.
  void on_clear_table(const events::clear_table &event);
  void on_waiter_finished_clearing_event(
      const events::waiter_finished_clearing &event);
  void on_waiter_collected_food_event(
      const events::waiter_collected_food &event);
  void on_waiter_served_order_event(const events::waiter_served_order &event);
  void on_waiter_abandoned_serving_event(
      const events::waiter_abandoned_serving &event);

private:
  std::unordered_map<size_t, entities::waiter_dog *> waiters_;
  std::unordered_map<size_t, entities::food_counter *> food_counters_;
  std::unordered_map<size_t, entities::table *> tables_;
  std::unordered_map<size_t, entities::dishwasher *> dishwashers_;
  std::vector<serving_job> serving_jobs_;
  size_t next_serving_job_id_;
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
  events::event_handler<events::clear_table> clear_table_handler_;
  events::event_handler<events::waiter_finished_clearing>
      waiter_finished_clearing_handler_;
  events::event_handler<events::waiter_collected_food>
      waiter_collected_food_handler_;
  events::event_handler<events::waiter_served_order>
      waiter_served_order_handler_;
  events::event_handler<events::waiter_abandoned_serving>
      waiter_abandoned_serving_handler_;
};
} // namespace expediter

#endif
