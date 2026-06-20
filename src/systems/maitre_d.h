/**
 * Cafe/order orchestration system.
 *
 * The maitre d' is the big robot dog puppet master for restaurant flow. There
 * should only be one for the game. It listens to cafe-domain events and also
 * exposes an id-based interface for systems that need to report facts directly.
 *
 * It works with entity ids rather than entity references so it does not own
 * level entities, depend on concrete subclasses, or create include cycles with
 * the entity hierarchy. When it decides that a world mutation should happen, it
 * emits command events for a world-owning system, such as the level, to execute.
 */
#ifndef MAITRE_D_H
#define MAITRE_D_H

#include "events.h"
#include "raylib.h"
#include <cstddef>
#include <unordered_map>
#include <vector>

namespace maitre_d{
    inline constexpr size_t empty_id = static_cast<size_t>(-1);

    enum table_status{
        free = 0,
        reserved = 1,
        occupied = 2
    };

    // Table availability lives here rather than on the table entity. A table
    // entity is a physical prop in the level; the maitre d' owns the cafe
    // meaning of whether that prop is free, reserved, or occupied.
    struct table_record{
        size_t table_id;
        table_status status;
        size_t customer_id;
    };

    // A queue slot is a physical waiting spot in the cafe. The queue order is
    // the dog's place in line; its position is the world target the level can
    // path the dog toward. empty_id means the slot is free.
    struct queue_slot{
        Vector2 position;
        size_t dog_id;
    };

    struct queued_dog{
        size_t dog_id;
        float height_edges;
        Vector2 target_position;
    };

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
            void enqueue(size_t dog_id, float height_edges);
            void dequeue(size_t dog_id);
            bool contains(size_t dog_id) const;
            bool empty() const;
            bool full() const;
            size_t size() const;
            Vector2 get_next_open_position() const;
            Vector2 get_target_position(size_t dog_id) const;

        private:
            void recalculate_positions();

            std::vector<queued_dog> dogs_;
    };

    namespace interface{
        // Small id-based facade for reporting cafe-domain facts to the maitre d'.
        // Entities and lower-level systems can use this without depending on the
        // full singleton class. The real maitre d' sits behind this interface.
        void register_table(size_t table_id);
        void register_customer(size_t customer_id);
        void request_table_for_customer(size_t customer_id);
        void configure_customer_queue_layout();
    }

    class maitre_d {
        public:
            static maitre_d& get_instance();

            maitre_d(const maitre_d& other) = delete;
            maitre_d(maitre_d&& other) = delete;

            maitre_d& operator=(const maitre_d& other) = delete;
            maitre_d& operator=(maitre_d&& other) = delete;

            void register_table(size_t table_id);
            void register_customer(size_t customer_id);
            void request_table_for_customer(size_t customer_id);

            void update(float delta);

            void on_registered_table_event(const events::registered_table& event);
            void on_registered_customer_event(const events::registered_customer& event);
            void on_requested_customer_table_event(const events::requested_customer_table& event);
            void on_customer_dog_arrived_event(const events::customer_dog_arrived& event);
            void on_customer_dog_left_event(const events::customer_dog_left& event);

        private:
            maitre_d();
            ~maitre_d() = default;

            // Processes cafe actions gathered from events or direct interface
            // calls. This mirrors the event dispatcher style: collect facts,
            // then resolve them into command events during the game loop.
            void process_events();
            void check_customer_arrivals(float delta);

            // Table lifecycle sketch:
            // register_table(table_id)
            //   -> tables_[table_id] = {table_id, free, empty_id}
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
            std::unordered_map<size_t, table_record> tables_;

            // Physical customer queue sketch:
            // customer_dog_arrived(customer_id)
            //   -> enqueue the dog with its height in edge units
            //   -> emit/request pathing to the resolved queue target position
            //
            // customer_dog_sent_to_table(customer_id, table_id)
            //   -> dequeue the dog
            //   -> use the head slot position as the table pathing start
            //   -> dog_queue recalculates positions for the remaining dogs
            dog_queue waiting_customer_queue_;
            float seconds_since_customer_arrived_;
            float dogs_left_window_seconds_;
            int dogs_left_in_window_;
            int pending_customer_builds_;

            events::event_handler<events::registered_table> registered_table_handler_;
            events::event_handler<events::registered_customer> registered_customer_handler_;
            events::event_handler<events::requested_customer_table> requested_customer_table_handler_;
            events::event_handler<events::customer_dog_arrived> customer_dog_arrived_handler_;
            events::event_handler<events::customer_dog_left> customer_dog_left_handler_;
    };
}

#endif
