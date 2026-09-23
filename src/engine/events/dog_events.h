/** Cafe/dog-domain events: customer and waiter dog lifecycle, pathing,
 * and order fulfillment.
 */
#ifndef EVENTS_DOG_EVENTS_H
#define EVENTS_DOG_EVENTS_H

#include <optional>

#include "event_core.h"

namespace events {
// for when a dog is selected, main listener is the player to update the id
class selected_dog : public event {
public:
  selected_dog(size_t id) : event(ids::select_dog), id_(id) {}

  static int get_static_type() { return ids::select_dog; }
  size_t get_id() const { return id_; }

private:
  const size_t id_;
};
class dog_completed_path : public event {
public:
  dog_completed_path(size_t dog_id, Vector2 destination,
      std::optional<size_t> destination_entity = std::nullopt)
      : event(ids::dog_path_complete), id_(dog_id), destination_(destination),
        destination_entity_(destination_entity) {}

  static int get_static_type() { return ids::dog_path_complete; }
  Vector2 get_destination() const { return destination_; }
  size_t get_id() const { return id_; }
  std::optional<size_t> get_destination_entity() const { return destination_entity_; }

private:
  const size_t id_;
  const Vector2 destination_;
  const std::optional<size_t> destination_entity_;
};
class dog_started_path : public event {
public:
  dog_started_path(size_t dog_id)
      : event(ids::dog_started_path_id), id_(dog_id) {}

  static int get_static_type() { return ids::dog_started_path_id; }
  size_t get_id() const { return id_; }

private:
  const size_t id_;
};
// Cafe-domain fact: a customer dog has left the cafe. Carries only the id
// (not a dog object) - the maitre d' uses it both as coarse
// arrival-pressure input and to resolve which table to clear, by
// matching against table::get_assigned_dog_id().
class customer_finished_meal : public event {
public:
  customer_finished_meal(size_t customer_id)
      : event(ids::customer_finished_meal_id), customer_id_(customer_id) {}

  static int get_static_type() { return ids::customer_finished_meal_id; }
  size_t get_customer_id() const { return customer_id_; }

private:
  const size_t customer_id_;
};
class customer_dog_left : public event {
public:
  customer_dog_left(size_t customer_id)
      : event(ids::customer_left), customer_id_(customer_id) {}

  static int get_static_type() { return ids::customer_left; }
  size_t get_customer_id() const { return customer_id_; }

private:
  const size_t customer_id_;
};
// Cafe-domain fact: a waiter has served food for an order.
class order_served : public event {
public:
  order_served(size_t waiter_id, size_t table_id)
      : event(ids::order_served_id), waiter_id_(waiter_id), table_id_(table_id) {}

  static int get_static_type() { return ids::order_served_id; }
  size_t get_waiter_id() const { return waiter_id_; }
  size_t get_table_id() const { return table_id_; }

private:
  const size_t waiter_id_;
  const size_t table_id_;
};
class food_dropped : public event{
  public:
  food_dropped(size_t waiter_id)
      : event(ids::food_dropped_id), waiter_id_(waiter_id) {}

  static int get_static_type() { return ids::food_dropped_id; }
  size_t get_waiter_id() const { return waiter_id_; }


private:
  const size_t waiter_id_;
};



// Cafe-domain fact: a waiter has reached the counter and picked up the food.
// The expediter does the actual handoff - the dog can't resolve a counter id.
class waiter_collected_food : public event {
public:
  waiter_collected_food(size_t waiter_id,
                        std::optional<size_t> food_id = std::nullopt)
      : event(ids::waiter_collected_food_id), waiter_id_(waiter_id),
        food_id_(food_id) {}

  static int get_static_type() { return ids::waiter_collected_food_id; }
  size_t get_waiter_id() const { return waiter_id_; }
  std::optional<size_t> get_food_id() const { return food_id_; }

private:
  const size_t waiter_id_;
  const std::optional<size_t> food_id_;
};


} // namespace events

#endif
