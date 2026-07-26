/** Cafe/dog-domain events: customer and waiter dog lifecycle, pathing
 * commands, station arrivals, and order fulfillment.
 */
#ifndef EVENTS_DOG_EVENTS_H
#define EVENTS_DOG_EVENTS_H

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
// Cafe-domain fact: a table entity exists in the level and can be tracked by
// the maitre d'. The event carries value data so the cafe/order system does
// not need concrete entity types or ownership of level entities.
// (registered_table/removed_table live in entity_events.h alongside the
// other station registration facts.)

// Cafe-domain fact: a customer dog exists in the level and can enter the
// restaurant flow. The maitre d' records the id, not the dog object.
class registered_customer : public event {
public:
  registered_customer(size_t customer_id)
      : event(ids::register_customer), customer_id_(customer_id) {}

  static int get_static_type() { return ids::register_customer; }
  size_t get_customer_id() const { return customer_id_; }

private:
  const size_t customer_id_;
};
// Cafe-domain fact/request: a customer dog needs a table assignment. The
// maitre d' listens for this and resolves it into command events later.
class requested_customer_table : public event {
public:
  requested_customer_table(size_t customer_id)
      : event(ids::request_customer_table), customer_id_(customer_id) {}

  static int get_static_type() { return ids::request_customer_table; }
  size_t get_customer_id() const { return customer_id_; }

private:
  const size_t customer_id_;
};
// Cafe-domain fact: a customer dog has entered the cafe and should be placed
// into the physical waiting queue managed by the maitre d'.
class customer_dog_created : public event {
public:
  customer_dog_created(entities::customer_dog *customer, size_t customer_id,
                       Vector2 position)
      : event(ids::customer_arrived), customer_(customer),
        customer_id_(customer_id), position_(position) {}

  static int get_static_type() { return ids::customer_arrived; }
  // Non-owning; the level owns the dog. Carried so the maitre d' can track the
  // customer by pointer the same way the expediter tracks waiters, rather than
  // holding an id it has to round-trip through a command event to act on.
  entities::customer_dog *get_customer() const { return customer_; }
  size_t get_customer_id() const { return customer_id_; }
  Vector2 get_position() const { return position_; }

private:
  entities::customer_dog *const customer_;
  const size_t customer_id_;
  const Vector2 position_;
};
// Cafe-domain fact: a customer dog was removed from the level; the maitre d'
// must drop its pointer to avoid dereferencing a destroyed entity. Mirrors
// removed_waiter, and like it is executed in place (not queued) at removal
// time - see quadtree::notify_removals.
class removed_customer : public event {
public:
  removed_customer(size_t customer_id)
      : event(ids::customer_removed), customer_id_(customer_id) {}

  static int get_static_type() { return ids::customer_removed; }
  size_t get_customer_id() const { return customer_id_; }

private:
  const size_t customer_id_;
};
class dog_completed_path : public event {
public:
  dog_completed_path(size_t dog_id, Vector2 destination)
      : event(ids::dog_path_complete), id_(dog_id), destination_(destination) {}

  static int get_static_type() { return ids::dog_path_complete; }
  Vector2 get_destination() const { return destination_; }
  size_t get_id() const { return id_; }

private:
  const size_t id_;
  const Vector2 destination_;
};
class give_dog_path : public event {
public:
  give_dog_path(size_t dog_id, std::vector<Vector2> path)
      : event(ids::give_dog_path_id), dog_id_(dog_id), path_(std::move(path)) {}

  static int get_static_type() { return ids::give_dog_path_id; }
  size_t get_dog_id() const { return dog_id_; }
  const std::vector<Vector2> &get_path() const { return path_; }

private:
  const size_t dog_id_;
  const std::vector<Vector2> path_;
};
class send_dog_to_station : public event {
public:
  // * std:nullopt - default is the dog's current position,
  send_dog_to_station(size_t dog_id, Vector2 destination, size_t station_id,
                      Vector2 station_position,
                      std::optional<Vector2> source = std::nullopt)
      : event(ids::dog_to_station), dog_id_(dog_id), source_(source),
        destination_(destination), station_id_(station_id),
        station_position_(station_position) {}

  static int get_static_type() { return ids::dog_to_station; }
  size_t get_dog_id() const { return dog_id_; }
  std::optional<Vector2> get_source() const { return source_; }
  Vector2 get_destination() const { return destination_; }
  size_t get_station_id() const { return station_id_; }
  Vector2 get_station_position() const { return station_position_; }

private:
  const size_t dog_id_;
  const std::optional<Vector2> source_;
  const Vector2 destination_;
  const size_t station_id_;
  const Vector2 station_position_;
};
class dog_reached_station : public event {
public:
  dog_reached_station(size_t dog_id, size_t station_id,
                      Vector2 station_position)
      : event(ids::dog_reached_station_id), dog_id_(dog_id),
        station_id_(station_id), station_position_(station_position) {}

  static int get_static_type() { return ids::dog_reached_station_id; }
  size_t get_dog_id() const { return dog_id_; }
  size_t get_station_id() const { return station_id_; }
  Vector2 get_station_position() const { return station_position_; }

private:
  const size_t dog_id_;
  const size_t station_id_;
  const Vector2 station_position_;
};
// Cafe-domain fact: a customer dog has left the cafe. Carries only the id
// (not a dog object) - the maitre d' uses it both as coarse
// arrival-pressure input and to resolve which table to clear, by
// matching against table::get_assigned_dog_id().
class customer_dog_left : public event {
public:
  customer_dog_left(size_t customer_id)
      : event(ids::customer_left), customer_id_(customer_id) {}

  static int get_static_type() { return ids::customer_left; }
  size_t get_customer_id() const { return customer_id_; }

private:
  const size_t customer_id_;
};
// Cafe-domain command: the maitre d' has assigned a customer dog to a
// physical world position. The level owns pathfinding and entity mutation.
class send_dog_to_position : public event {
public:
  send_dog_to_position(size_t customer_id, Vector2 destination)
      : event(ids::send_customer_position), customer_id_(customer_id),
        source_(Vector2{0.0f, 0.0f}), destination_(destination),
        has_source_(false) {}

  send_dog_to_position(size_t customer_id, Vector2 source, Vector2 destination)
      : event(ids::send_customer_position), customer_id_(customer_id),
        source_(source), destination_(destination), has_source_(true) {}

  static int get_static_type() { return ids::send_customer_position; }
  size_t get_customer_id() const { return customer_id_; }
  bool has_source() const { return has_source_; }
  Vector2 get_source() const { return source_; }
  Vector2 get_destination() const { return destination_; }

private:
  const size_t customer_id_;
  const Vector2 source_;
  const Vector2 destination_;
  const bool has_source_;
};
// Cafe-domain fact: a waiter dog exists and can be assigned service work by
// the expediter. The expediter records ids only, not dog references.
class registered_waiter : public event {
public:
  registered_waiter(entities::waiter_dog *waiter, size_t waiter_id)
      : event(ids::register_waiter), waiter_(waiter), waiter_id_(waiter_id) {}

  static int get_static_type() { return ids::register_waiter; }
  entities::waiter_dog *get_waiter() const { return waiter_; }
  size_t get_waiter_id() const { return waiter_id_; }

private:
  entities::waiter_dog *const waiter_;
  const size_t waiter_id_;
};
// Cafe-domain fact: a waiter dog was removed from the level; the expediter
// must drop its pointer to avoid dereferencing a destroyed entity.
class removed_waiter : public event {
public:
  removed_waiter(size_t waiter_id)
      : event(ids::waiter_removed), waiter_id_(waiter_id) {}

  static int get_static_type() { return ids::waiter_removed; }
  size_t get_waiter_id() const { return waiter_id_; }

private:
  const size_t waiter_id_;
};
// Cafe-domain command: the expediter has assigned a waiter to an order. The
// waiter should route through the pickup point before continuing to the table.
class send_waiter_to_table : public event {
public:
  send_waiter_to_table(size_t waiter_id, size_t order_id, Vector2 pickup_point,
                       Vector2 table_position)
      : event(ids::send_waiter_table), waiter_id_(waiter_id),
        order_id_(order_id), pickup_point_(pickup_point),
        table_position_(table_position) {}

  static int get_static_type() { return ids::send_waiter_table; }
  size_t get_waiter_id() const { return waiter_id_; }
  size_t get_order_id() const { return order_id_; }
  Vector2 get_pickup_point() const { return pickup_point_; }
  Vector2 get_table_position() const { return table_position_; }

private:
  const size_t waiter_id_;
  const size_t order_id_;
  const Vector2 pickup_point_;
  const Vector2 table_position_;
};
// Cafe-domain fact: a waiter reached the table for the assigned order.
class waiter_arrived_at_table : public event {
public:
  waiter_arrived_at_table(size_t waiter_id, size_t order_id)
      : event(ids::waiter_arrived_table), waiter_id_(waiter_id),
        order_id_(order_id) {}

  static int get_static_type() { return ids::waiter_arrived_table; }
  size_t get_waiter_id() const { return waiter_id_; }
  size_t get_order_id() const { return order_id_; }

private:
  const size_t waiter_id_;
  const size_t order_id_;
};
// Cafe-domain fact: a waiter has served food for an order.
class order_served : public event {
public:
  order_served(size_t order_id, size_t waiter_id, size_t customer_id,
               size_t table_id, Vector2 table_position)
      : event(ids::order_served_id), order_id_(order_id), waiter_id_(waiter_id),
        customer_id_(customer_id), table_id_(table_id),
        table_position_(table_position) {}

  static int get_static_type() { return ids::order_served_id; }
  size_t get_order_id() const { return order_id_; }
  size_t get_waiter_id() const { return waiter_id_; }
  size_t get_customer_id() const { return customer_id_; }
  size_t get_table_id() const { return table_id_; }
  Vector2 get_table_position() const { return table_position_; }

private:
  const size_t order_id_;
  const size_t waiter_id_;
  const size_t customer_id_;
  const size_t table_id_;
  const Vector2 table_position_;
};
// for when we need to build a dog
class build_customer_dog : public event {
public:
  build_customer_dog(int dog_type, Vector2 position)
      : event(ids::build_customer_dog_id), dog_type_(dog_type),
        position_(position) {}

  static int get_static_type() { return ids::build_customer_dog_id; }
  int get_dog_type() const { return dog_type_; }
  Vector2 get_position() const { return position_; }

private:
  const int dog_type_;
  const Vector2 position_;
};

// Cafe-domain fact: a table has been vacated and needs clearing before it
// can be reassigned. The maitre d' resolves and fires this (see
// on_customer_dog_left_event); the expediter is the sole listener,
// dispatching a waiter to physically clear it.
class clear_table : public event {
public:
  clear_table(entities::table *table)
      : event(ids::table_cleared), table_(table) {}

  static int get_static_type() { return ids::table_cleared; }
  entities::table *get_table() const { return table_; }

private:
  entities::table *const table_;
};

// Cafe-domain fact: a waiter has dropped the dirty plate at the dishwasher, so
// its clearing job is done. The expediter only erases the job - the waiter has
// already set itself idle.
class waiter_finished_clearing : public event {
public:
  waiter_finished_clearing(size_t waiter_id)
      : event(ids::waiter_finished_clearing_id), waiter_id_(waiter_id) {}

  static int get_static_type() { return ids::waiter_finished_clearing_id; }
  size_t get_waiter_id() const { return waiter_id_; }

private:
  const size_t waiter_id_;
};

// Cafe-domain fact: a waiter has reached the counter and picked up the food.
// The expediter does the actual handoff - the dog can't resolve a counter id.
class waiter_collected_food : public event {
public:
  waiter_collected_food(size_t waiter_id)
      : event(ids::waiter_collected_food_id), waiter_id_(waiter_id) {}

  static int get_static_type() { return ids::waiter_collected_food_id; }
  size_t get_waiter_id() const { return waiter_id_; }

private:
  const size_t waiter_id_;
};

// Cafe-domain fact: a waiter has placed the food on the table. The expediter
// turns this into order_served, which needs the table's assigned customer.
class waiter_served_order : public event {
public:
  waiter_served_order(size_t waiter_id)
      : event(ids::waiter_served_order_id), waiter_id_(waiter_id) {}

  static int get_static_type() { return ids::waiter_served_order_id; }
  size_t get_waiter_id() const { return waiter_id_; }

private:
  const size_t waiter_id_;
};

// Cafe-domain fact: a waiter gave up on its serving job (counter or table
// unreachable). Distinct from waiter_served_order because the customer must NOT
// be told food arrived - the job goes back in the queue instead.
class waiter_abandoned_serving : public event {
public:
  waiter_abandoned_serving(size_t waiter_id)
      : event(ids::waiter_abandoned_serving_id), waiter_id_(waiter_id) {}

  static int get_static_type() { return ids::waiter_abandoned_serving_id; }
  size_t get_waiter_id() const { return waiter_id_; }

private:
  const size_t waiter_id_;
};
} // namespace events

#endif
