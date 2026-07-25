#include "debug_log_interface.h"
#include "entities.h"
#include "raglib.h"
#include "texture.h"
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

bool entities::waiter_dog::serving::is_available_for_order() { return false; }
int entities::waiter_dog::serving::update(waiter_dog &dog, float delta,
                                          int frame, int status) {
  (void)dog;
  (void)delta;
  (void)frame;
  (void)status;
  // Still expediter-driven off dog_completed_path; this state only marks the
  // waiter busy. `status` is deliberately ignored until the leg split lands
  // (see the serving TODO in dogs.h) - that is what moves the counter/table
  // transitions in here, keyed off status_codes::completed_path.
  // TODO: when the waiter reaches the counter (animation::picking_up_food)
  // and the table (animation::placing_food), hold here for the animation's
  // duration before the leg is allowed to advance - needs an elapsed_-style
  // timer, see customer_dog::eating for the existing pattern.
  return status_codes::nothing;
}

bool entities::waiter_dog::clearing::is_available_for_order() { return false; }
int entities::waiter_dog::clearing::update(waiter_dog &dog, float delta,
                                           int frame, int status) {
  (void)dog;
  (void)delta;
  (void)frame;
  (void)status;
  // Same as `serving`: busy marker only, until clearing splits into
  // clearing_table/clearing_dishwasher. Once it does, the table -> dishwasher
  // progression happens here on status_codes::completed_path rather than in
  // expediter::on_dog_completed_path_event, and the dishwasher position comes
  // from the second path leg the expediter queued at dispatch - the state
  // never needs to cache or re-query it.
  // TODO: when the waiter reaches the table (animation::picking_up_plate) and
  // the dishwasher (animation::placing_plate), hold for the animation's
  // duration before advancing - same elapsed_-style timer as `serving`.
  return status_codes::nothing;
}

// TODO: [event-wire, new event] add events::waiter_finished_clearing via the
// event-wire skill (skills/event-wire/SKILL.md) - a Cafe-domain fact, same
// shape as events::customer_dog_left, carrying just the waiter's dog id. Fired
// by clearing_dishwasher once the plate is placed so the expediter can erase
// the clearing_job; the waiter calls set_idle() on itself rather than having
// the expediter do it.

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

int entities::waiter_dog::update(float delta, int frame) {
  auto status = npc_dog::update(delta, frame);
  auto state_status = state_->update(*this, delta, frame, status);
  // Same combining rule as customer_dog::update - see the note there. No
  // waiter state signals dead today, but keeping the shape identical means
  // the two dogs stay readable side by side.
  return state_status == status_codes::dead ? state_status : status;
}
