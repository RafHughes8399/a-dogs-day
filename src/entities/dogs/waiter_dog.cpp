#include "config.h"
#include "debug_log_interface.h"
#include "entities.h"
#include "events.h"
#include "events_interface.h"
#include "raglib.h"
#include "texture.h"
#include <memory>
#include <vector>

// ------------------------------- waiter dog states
// ------------------------------- //
bool entities::waiter_dog::idle::is_available_for_order() { return true; }
int entities::waiter_dog::idle::update(waiter_dog &dog, float delta, int frame,
                                       int status) {
  (void)dog;
  (void)delta;
  (void)frame;
  (void)status;
  // Idle waiters do nothing until the expediter assigns them an order.
  return status_codes::nothing;
}

bool entities::waiter_dog::animating::is_available_for_order() { return false; }

void entities::waiter_dog::animating::on_enter(waiter_dog &dog) {
  dog.play_animation(anim_);
}

int entities::waiter_dog::animating::update(waiter_dog &dog, float delta,
                                            int frame, int status) {
  (void)frame;
  (void)status;
  // delta-based rather than read off the sprite: next_frame() wraps, so an
  // animation never reports "done", and advance() is driven from render, which
  // the headless test harness never calls.
  elapsed_ += delta;
  if (elapsed_ < duration_) {
    return status_codes::nothing;
  }
  // set_state frees this instance, so it must be the last touch on `this`.
  dog.set_state(std::move(next_));
  return status_codes::nothing;
}

bool entities::waiter_dog::serving_counter::is_available_for_order() {
  return false;
}
int entities::waiter_dog::serving_counter::update(waiter_dog &dog, float delta,
                                                  int frame, int status) {
  (void)delta;
  (void)frame;
  if (dog.has_arrived(status)) {
    dog.set_state(std::make_unique<animating>(
        animation::picking_up_food, cafe_config::animation_duration_s,
        std::make_unique<walking_to_table>(table_destination_)));
    return status_codes::nothing;
  }
  // After arrival, never before - see clearing_table.
  if (!dog.has_path()) {
    dog.set_state(std::make_unique<abandoned_serving>());
  }
  return status_codes::nothing;
}

bool entities::waiter_dog::walking_to_table::is_available_for_order() {
  return false;
}
void entities::waiter_dog::walking_to_table::on_enter(waiter_dog &dog) {
  // Executed, not queued: the expediter hands over the food inside this call,
  // so the waiter is carrying it before it takes a step.
  const events::waiter_collected_food collected(
      static_cast<size_t>(dog.get_id()));
  event_interface::execute_event(collected);
  dog.path_to(table_destination_);
}
int entities::waiter_dog::walking_to_table::update(waiter_dog &dog, float delta,
                                                   int frame, int status) {
  (void)delta;
  (void)frame;
  if (dog.has_arrived(status)) {
    dog.set_state(std::make_unique<animating>(
        animation::placing_food, cafe_config::animation_duration_s,
        std::make_unique<finished_serving>()));
    return status_codes::nothing;
  }
  if (!dog.has_path()) {
    dog.set_state(std::make_unique<abandoned_serving>());
  }
  return status_codes::nothing;
}

bool entities::waiter_dog::finished_serving::is_available_for_order() {
  return false;
}
void entities::waiter_dog::finished_serving::on_enter(waiter_dog &dog) {
  // Executed, not queued: update() sets the waiter idle on the very next frame,
  // so a queued handler could still be holding the job open (and the food) once
  // the waiter already looks available.
  const events::waiter_served_order served(static_cast<size_t>(dog.get_id()));
  event_interface::execute_event(served);
}
int entities::waiter_dog::finished_serving::update(waiter_dog &dog, float delta,
                                                   int frame, int status) {
  (void)delta;
  (void)frame;
  (void)status;
  dog.set_idle();
  return status_codes::nothing;
}

bool entities::waiter_dog::abandoned_serving::is_available_for_order() {
  return false;
}
void entities::waiter_dog::abandoned_serving::on_enter(waiter_dog &dog) {
  // Executed for the same reason as finished_serving's.
  const events::waiter_abandoned_serving abandoned(
      static_cast<size_t>(dog.get_id()));
  event_interface::execute_event(abandoned);
}
int entities::waiter_dog::abandoned_serving::update(waiter_dog &dog,
                                                    float delta, int frame,
                                                    int status) {
  (void)delta;
  (void)frame;
  (void)status;
  dog.set_idle();
  return status_codes::nothing;
}

bool entities::waiter_dog::clearing_table::is_available_for_order() {
  return false;
}
int entities::waiter_dog::clearing_table::update(waiter_dog &dog, float delta,
                                                 int frame, int status) {
  (void)delta;
  (void)frame;
  if (dog.has_arrived(status)) {
    // set_state frees this instance - last touch on `this`.
    dog.set_state(std::make_unique<animating>(
        animation::picking_up_plate, cafe_config::animation_duration_s,
        std::make_unique<walking_to_dishwasher>(dishwasher_destination_)));
    return status_codes::nothing;
  }
  // Checked after arrival, never before - an arriving dog also has no path
  // left. process_clearing_job dispatches this leg synchronously, so the path
  // exists before the first update; nothing to walk means an unreachable table.
  if (!dog.has_path()) {
    dog.set_state(std::make_unique<finished_clearing>());
  }
  return status_codes::nothing;
}

bool entities::waiter_dog::walking_to_dishwasher::is_available_for_order() {
  return false;
}
void entities::waiter_dog::walking_to_dishwasher::on_enter(waiter_dog &dog) {
  // Pathed on entry, not at dispatch, so a dishwasher moved mid-journey is
  // routed to where it actually is. Failure is handled in update() - on_enter
  // must not set_state.
  dog.path_to(dishwasher_destination_);
}
int entities::waiter_dog::walking_to_dishwasher::update(waiter_dog &dog,
                                                        float delta, int frame,
                                                        int status) {
  (void)delta;
  (void)frame;
  if (dog.has_arrived(status)) {
    dog.set_state(std::make_unique<animating>(
        animation::placing_plate, cafe_config::animation_duration_s,
        std::make_unique<finished_clearing>()));
    return status_codes::nothing;
  }
  // Checked after arrival, never before - an arriving dog also has no path
  // left. Reaching here with nothing to walk means on_enter's path_to failed.
  if (!dog.has_path()) {
    dog.set_state(std::make_unique<finished_clearing>());
  }
  return status_codes::nothing;
}

bool entities::waiter_dog::finished_clearing::is_available_for_order() {
  return false;
}
void entities::waiter_dog::finished_clearing::on_enter(waiter_dog &dog) {
  // Executed for the same reason as finished_serving's - the job must be closed
  // before update() makes the waiter look available again.
  const events::waiter_finished_clearing finished(
      static_cast<size_t>(dog.get_id()));
  event_interface::execute_event(finished);
}
int entities::waiter_dog::finished_clearing::update(waiter_dog &dog,
                                                    float delta, int frame,
                                                    int status) {
  (void)delta;
  (void)frame;
  (void)status;
  // set_idle frees this instance - last touch on `this`.
  dog.set_idle();
  return status_codes::nothing;
}

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
void entities::waiter_dog::set_serving(Vector2 table_destination) {
  set_state(std::make_unique<serving_counter>(table_destination));
}
void entities::waiter_dog::set_clearing(Vector2 dishwasher_destination) {
  set_state(std::make_unique<clearing_table>(dishwasher_destination));
}

int entities::waiter_dog::update(float delta, int frame) {
  auto status = npc_dog::update(delta, frame);
  // Same flag merge as customer_dog::update - see the note there.
  return status | state_->update(*this, delta, frame, status);
}
