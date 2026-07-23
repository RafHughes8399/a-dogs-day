#include "debug_log_interface.h"
#include "entities.h"
#include "raglib.h"
#include "texture.h"
#include <vector>

// ------------------------------- waiter dog state bases
// ------------------------------- //
void entities::waiter_dog_state::on_path_finished(waiter_dog &dog,
                                                  Vector2 destination) {
  (void)dog;
  (void)destination;
  return;
}

void entities::waiter_dog_state::set_path(waiter_dog &dog,
                                          const std::vector<Vector2> &path) {
  dog.dog::set_path(path);
}
void entities::waiter_dog_state::set_path(waiter_dog &dog,
                                          const std::vector<Vector2> &path,
                                          int station_id,
                                          Vector2 station_position) {
  (void)station_id;
  (void)station_position;
  dog.dog::set_path(path);
}

bool entities::waiter_dog_traveling_state::is_available_for_order() {
  return false;
}

void entities::waiter_dog_traveling_state::on_path_finished(
    waiter_dog &dog, Vector2 destination) {
  if (Vector2Distance(destination, destination_) >
      level_config::edge_weight * 0.05f) {
    return;
  }
  on_arrived(dog);
}

// ------------------------------- waiter dog states
// ------------------------------- //
bool entities::waiter_dog::idle::is_available_for_order() { return true; }
int entities::waiter_dog::idle::update(waiter_dog &dog, float delta,
                                       int frame) {
  (void)dog;
  (void)delta;
  (void)frame;
  // Idle waiters do nothing until the expediter assigns them an order.
  return status_codes::nothing;
}

bool entities::waiter_dog::serving::is_available_for_order() { return false; }
int entities::waiter_dog::serving::update(waiter_dog &dog, float delta,
                                          int frame) {
  (void)dog;
  (void)delta;
  (void)frame;
  // The expediter drives the serving journey via dog_completed_path; the
  // serving state itself just marks the waiter busy.
  // TODO: when the waiter reaches the counter (animation::picking_up_food)
  // and the table (animation::placing_food), hold here for the
  // animation's duration before expediter::on_dog_completed_path_event is
  // allowed to advance the leg - needs an elapsed_-style timer, see
  // customer_dog::eating for the existing pattern.
  return status_codes::nothing;
}
// TODO: [entities::waiter_dog::serving::update, whole function] [per-frame
// busy marker] change from [`serving` above: single class, no-op every
// frame, leg told apart reactively by expediter via is_carrying_food()] to
// [delete this function/class; replace with serving_counter::update and
// serving_table::update, each still a same-shaped no-op (arrival is no
// longer this function's job either - see on_arrived below). The
// elapsed_-style animation-hold TODO stays here, per-class, same as today.]

// TODO: [entities::waiter_dog::serving_counter::on_arrived, new function]
// [point in flow: replaces expediter::on_dog_completed_path_event's
// !waiter->is_carrying_food() branch] change from [expediter reactively
// matches the completed-path event to an orders_ entry by waiter id, then
// mutates the job and calls dog_actions::send_dog_to_station itself] to
// [dog self-handles: hold_food(counter->take()) equivalent info arrives via
// queries::next_serving_target_query(dog.get_id()) executed against
// queries::leg_target_executor_; if response.has_next, dog.set_path(...) to
// response.station_position itself (query_interface::execute_query(path_executor_,...)
// same as customer_dog::leave() does) and dog.set_state(make_unique<serving_table>(...));
// if !response.has_next (order vanished mid-flight - table/counter gone),
// fall back to dog.set_idle() itself instead of expediter's reconciliation
// pass catching it later].

// TODO: [entities::waiter_dog::serving_table::on_arrived, new function]
// [point in flow: replaces the `waiter->is_carrying_food()` == true branch
// of on_dog_completed_path_event, i.e. the served->fulfilled transition] to
// [dog self-handles: release_food(), fire a fact event (e.g. reuse/extend
// events::order_served) so expediter marks the order fulfilled and erases
// it, then dog.set_idle() itself - mirrors customer_dog::eating calling
// dog.leave() as the last thing it does on `this`].

bool entities::waiter_dog::clearing::is_available_for_order() { return false; }
int entities::waiter_dog::clearing::update(waiter_dog &dog, float delta,
                                           int frame) {
  (void)dog;
  (void)delta;
  (void)frame;
  // The expediter drives the table -> dishwasher journey via
  // dog_completed_path (see expediter::dispatch_clearing_job); this state
  // just marks the waiter busy, same shape as `serving`.
  // TODO: when the waiter reaches the table (animation::picking_up_plate)
  // and the dishwasher (animation::placing_plate), hold here for the
  // animation's duration before the expediter is allowed to advance the
  // leg - same elapsed_-style timer as noted on `serving` above.
  return status_codes::nothing;
}
// TODO: [entities::waiter_dog::clearing::update, whole function] [per-frame
// busy marker] change from [`clearing` above: single class, no-op every
// frame - this is where the original "check if completed path, get path to
// the dishwasher, where do I get dishwasher info, maybe the state should
// carry it" question lived] to [delete this function/class; the answer to
// that question is: the state does NOT carry dishwasher info as a cached
// member (expediter already owns dishwashers_/find_dishwasher() and can
// resolve a live position on demand, same reasoning as fulfill_order()
// calling counter->get_position() fresh rather than caching it) - instead
// replace with clearing_table/clearing_dishwasher below, which pull what
// they need via query on arrival rather than storing it up front.]

// TODO: [entities::waiter_dog::clearing_table::on_arrived, new function]
// [point in flow: replaces expediter::on_dog_completed_path_event's
// job.dishwasher_id == empty_id branch, i.e. "reached the table, plate not
// yet picked up"] change from [expediter reactively finds the clearing_job
// by waiter id and mutates it in place] to [dog self-handles: play
// animation::picking_up_plate (hold via elapsed_ per the update() TODO
// above), then execute queries::next_clearing_target_query(dog.get_id())
// against queries::leg_target_executor_ - expediter's handler picks an
// available dishwasher (mirrors pick_food_counter's shape) and returns its
// position via leg_target. If response.has_next, dog.set_path(...) to
// response.station_position and dog.set_state(make_unique<clearing_dishwasher>(
// response.station_position, table_id_, static_cast<size_t>(response.station_id)));
// if !response.has_next (no dishwasher available/registered), stay parked
// here and re-query next frame rather than stranding the waiter idle
// mid-table with a dirty plate still held].

// TODO: [entities::waiter_dog::clearing_dishwasher::on_arrived, new
// function] [point in flow: replaces the job.dishwasher_id != empty_id
// branch of on_dog_completed_path_event, i.e. the plate-placed/job-cleared
// transition] change from [expediter reactively erases the clearing_job and
// calls waiter->set_idle() itself from inside the event handler] to [dog
// self-handles: play animation::placing_plate (hold via elapsed_), fire a
// new fact event (e.g. events::waiter_finished_clearing carrying dog.get_id())
// so expediter's (now tiny) handler just erases the matching clearing_jobs_
// entry - no more job mutation happening reactively off a shared
// dog_completed_path event - then dog.set_idle() itself, last, same
// ordering constraint noted on customer_dog::eating's dog.leave() call].

// TODO: [event-wire, new event] [supporting change, not in dogs.h/.cpp
// directly] add events::waiter_finished_clearing via the event-wire skill
// (skills/event-wire/SKILL.md) - a Cafe-domain fact, same shape as
// events::customer_dog_left, carrying just the waiter's dog id.

// ------------------------------- waiter dog ------------------------------- //
entities::waiter_dog::~waiter_dog() = default;

void entities::waiter_dog::hold_food(std::unique_ptr<food> item) {
  held_food_ = std::move(item);
}
bool entities::waiter_dog::is_available_for_order() {
  return state_->is_available_for_order();
}
bool entities::waiter_dog::is_carrying_food() const {
  return held_food_ != nullptr;
}
std::unique_ptr<entities::food> entities::waiter_dog::release_food() {
  return std::move(held_food_);
}
void entities::waiter_dog::set_idle() { set_state(std::make_unique<idle>()); }
void entities::waiter_dog::set_serving() {
  set_state(std::make_unique<serving>());
}
void entities::waiter_dog::set_clearing() {
  set_state(std::make_unique<clearing>());
}
