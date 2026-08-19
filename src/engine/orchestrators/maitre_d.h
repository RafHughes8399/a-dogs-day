/**
 * Cafe/order orchestration system.
 *
 * The maitre d' is the big robot dog puppet master for restaurant flow. There
 * should only be one for the game. It listens to cafe-domain events and also
 * exposes an id-based interface for systems that need to report facts directly.
 *
 * It holds non-owning raw pointers to the table and customer-dog entities it
 * tracks (the level owns them; register/remove events keep the pointers valid),
 * the same tracking shape the expediter uses for waiters/counters/dishwashers.
 * Reading position and cafe-state live from the entity avoids caching stale
 * copies and removes the need to listen for moved events. When it decides that
 * a world mutation should happen it still emits command events for a
 * world-owning system, such as the level, to execute.
 */
#ifndef MAITRE_D_H
#define MAITRE_D_H

#include "dog_actions.h"
#include "events.h"
#include "entities.h"
#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <cassert>
#include <utility>
#include <cstddef>
#include <unordered_map>
#include <vector>
#include <iostream>
namespace maitre_d{
    inline constexpr size_t empty_id = static_cast<size_t>(-1);
    inline Vector2 entrance_ = Vector2 {
        level_config::edge_weight * (cafe_config::queue_width_edges + cafe_config::queue_x_edges),
        cafe_config::queue_midpoint_y
    };
    


    // The maitre d' tracks table entities by id in an unordered_map (register/
    // remove events keep it current), resolving the live pointer on demand.
    // An id can't dangle, so remove_table() is a single map erase - no need
    // to scan any other structure for a stale pointer. Table cafe-state
    // (available/reserved/occupied) lives on the table entity itself; the
    // maitre d' reads and mutates it through the entity's own interface
    // rather than duplicating a freeness flag here.
    // A queue slot is a physical waiting spot in the cafe. The queue order is
    // the dog's place in line; its position is the world target the level can
    // path the dog toward. empty_id means the slot is free.
    struct queue_slot{
        Vector2 position;
        size_t dog_id;
    };

    struct queued_dog{
        int dog_id;
        Vector2 dog_position;
        Vector2 queue_position;
        bool reached_queue_position;
    };
    const queued_dog empty_dog = {
        -1,
        Vector2Zero(),
        Vector2Zero(),
        false
    };
    inline bool operator==(const queued_dog& a, const queued_dog&b){
        return a.dog_id == b.dog_id;
    }
    struct queue_lane{
        Vector2 head;
        std::vector<queued_dog> dogs;
    };

    struct dequeue_result{
        queued_dog dog;
        std::vector<queued_dog> moved_dogs;
    };

    // define the empty dog and the empty table 
    // define equality chceks based on id 
    // so you can easily check if and mepty dog / table was returned
    class dog_queue{
        public:
            dog_queue() = default;
            dog_queue(const dog_queue& other) = default;
            dog_queue(dog_queue&& other) = default;

            dog_queue& operator=(const dog_queue& other) = default;
            dog_queue& operator=(dog_queue&& other) = default;

            // The maitre d' interacts with this as a queue: dogs enter, dogs
            // leave, and resolved target positions are hidden behind the queue.
            // Internally this uses a vector so position offsets can be
            // recalculated when dog sizes change the space behind them.
            queued_dog enqueue(size_t dog_id, int side);
            dequeue_result dequeue();

            void update_dog_position(size_t dog_id, Vector2 position);
            bool dog_at_head(queue_lane queue);
            bool dog_has_reached_queue_position(size_t dog_id) const;
            bool contains(size_t dog_id) const;
            bool empty() const;
            bool full() const;
            size_t size() const;

            int pick_side();
            
            Vector2 get_enqueue_position(int side) const;
            Vector2 get_position(int side, size_t index) const;
            Vector2 get_spawn_position(events::customer_queue_side queue_side) const;
            Vector2 get_target_position(size_t dog_id) const;

        private:
            std::vector<queued_dog> compact_lane(int queue_side);
            queue_lane& lane_for_side(int queue_side);
            const queue_lane& lane_for_side(int queue_side) const;
            std::vector<queued_dog>& dogs_for_side(events::customer_queue_side queue_side);
            const std::vector<queued_dog>& dogs_for_side(events::customer_queue_side queue_side) const;
            
            queue_lane left_queue_{cafe_config::left_queue_head, {}};
            queue_lane right_queue_{cafe_config::right_queue_head, {}};
            int previous_side_ = cafe_config::queue_sides::right;
    };

    class maitre_d {
        public:
            maitre_d();
            ~maitre_d();

            maitre_d(const maitre_d& other) = delete;
            maitre_d(maitre_d&& other) = delete;

            maitre_d& operator=(const maitre_d& other) = delete;
            maitre_d& operator=(maitre_d&& other) = delete;

            void register_table(entities::table* table);
            void remove_table(size_t table_id);
            entities::table* find_table(size_t table_id);

            // Number of tables tracked / customers queued. Exposed for tests to
            // assert table registration/removal and customer arrival/seating.
            size_t num_tables() const { return tables_.size(); }
            size_t num_customers() const { return customer_queue_.size(); }
            // Customers tracked by pointer, which outlives queue membership -
            // a seated customer has left the queue but is still tracked here.
            size_t num_tracked_customers() const { return customers_.size(); }
            entities::customer_dog* first_customer() const {
                return customers_.empty() ? nullptr : customers_.begin()->second;
            }
            // Customer dogs are tracked by non-owning raw pointer, the same way
            // the expediter tracks waiters (register on creation, drop on
            // removal). customer_queue_ still holds ids - the queue models
            // physical line order, not entity identity - but anything that
            // needs to read live dog state or command a dog resolves the
            // pointer here instead of round-tripping through a command event.
            void register_customer(entities::customer_dog* customer);
            void remove_customer(size_t customer_id);
            entities::customer_dog* find_customer(size_t customer_id);
            void request_table_for_customer(size_t customer_id);

            void update(float delta);
            void update_dog_position(size_t id, Vector2 position);
            // Debug/testing trigger (the former L-key path): spawns a customer
            // arrival directly. Public so the test harness can fire it.
            void request_customer_arrival();
            events::customer_queue_side get_customer_queue_side() const;
            Vector2 get_customer_spawn_position(events::customer_queue_side queue_side) const;

            void on_registered_table_event(const events::registered_table& event);
            void on_removed_table_event(const events::removed_table& event);
            void on_registered_customer_event(const events::registered_customer& event);
            void on_requested_customer_table_event(const events::requested_customer_table& event);
            void on_customer_dog_created_event(const events::customer_dog_created& event);
            void on_customer_dog_left_event(const events::customer_dog_left& event);
            void on_removed_customer_event(const events::removed_customer& event);
            void on_dog_completed_path_event(const events::dog_completed_path& event);
            // dog_reached_station already names which station a customer
            // reached (the dog's own traveling state resolves and carries that
            // id), so occupying the right table is a direct id match against
            // tables_ - no position scanning against the arrival event.
            void on_dog_reached_station_event(const events::dog_reached_station& event);
        private:
            // Processes cafe actions gathered from events or direct interface
            // calls. This mirrors the event dispatcher style: collect facts,
            // then resolve them into command events during the game loop.
            void assign_tables();
            bool are_tables_free();
            // Finds the available table nearest to entrance_. tables_ isn't
            // kept in any particular order (it's a map keyed by id), so this
            // scans and compares distances directly rather than relying on
            // pre-sorted order.
            entities::table* pick_table();
            Vector2 pick_interaction_position(entities::table* table, Vector2 dog_position) const;
            void send_dog_to_queue_position(size_t id, Vector2 position);
            void check_customer_arrivals(float delta);
            bool can_request_customer_arrival() const;

            // Table lifecycle sketch:
            // register_table(table_id, position)
            //   -> tables_[table_id] = {table_id, position, free, empty_id}
            //
            // reserve_table(table_id, customer_id)
            //   -> table moves free -> reserved and records the customer id
            //
            // sit_at_table(table_id, customer_id)
            //   -> table moves reserved -> occupied once the customer arrives
            //
            // leave_table(table_id, customer_id)
            //   -> table moves occupied -> free and clears the customer id
            //
            // remove_table(table_id)
            //   -> table is removed from maitre d' tracking
            //
            // move_table(table_id)
            //   -> no status change; level owns the physical position
            // Physical customer queue sketch:
            // customer_dog_created(customer, customer_id, position)
            //   -> track the customer pointer in customers_
            //   -> enqueue the dog with its height in edge units
            //   -> emit/request pathing to the resolved queue target position
            //
            // removed_customer(customer_id)
            //   -> drop the pointer; assign_tables() drops the queue entry
            //      lazily when it dequeues an id that no longer resolves
            //
	            // send_dog_to_table(customer_id, table_id, table_position, interaction_position)
            //   -> dequeue the dog
            //   -> use the head slot position as the table pathing start
            //   -> dog_queue recalculates positions for the remaining dogs

            

            std::unordered_map<size_t, entities::table*> tables_;
            std::unordered_map<size_t, entities::customer_dog*> customers_;

            dog_queue customer_queue_;
            float seconds_since_customer_arrived_;
            float dogs_left_window_seconds_;
            int dogs_left_in_window_;
            

            events::event_handler<events::registered_table> registered_table_handler_;
            events::event_handler<events::removed_table> removed_table_handler_;
            events::event_handler<events::registered_customer> registered_customer_handler_;
            events::event_handler<events::requested_customer_table> requested_customer_table_handler_;
            events::event_handler<events::customer_dog_created> customer_dog_created_handler_;
            events::event_handler<events::customer_dog_left> customer_dog_left_handler_;
            events::event_handler<events::removed_customer> removed_customer_handler_;
            events::event_handler<events::dog_completed_path> dog_path_compelte_handler_;
            events::event_handler<events::dog_reached_station> dog_reached_station_handler_;
    };
}

#endif
