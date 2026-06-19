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
#include <queue>
#include <unordered_map>

namespace maitre_d{
    inline constexpr size_t empty_id = static_cast<size_t>(-1);

    enum table_status{
        free = 0,
        reserved = 1,
        occupied = 2
    };

    enum customer_status{
        registered = 0,
        waiting_for_table = 1,
        going_to_table = 2,
        seated = 3,
        eating = 4,
        leaving = 5
    };

    // Table availability lives here rather than on the table entity. A table
    // entity is a physical prop in the level; the maitre d' owns the cafe
    // meaning of whether that prop is free, reserved, or occupied.
    struct table_record{
        size_t table_id;
        table_status status;
        size_t customer_id;
    };

    struct customer_record{
        size_t customer_id;
        customer_status status;
        size_t table_id;
    };

    // A queue slot is a physical waiting spot in the cafe. The queue order is
    // the dog's place in line; its position is the world target the level can
    // path the dog toward. empty_id means the slot is free.
    struct queue_slot{
        Vector2 position;
        size_t dog_id;
    };

    namespace interface{
        // Small id-based facade for reporting cafe-domain facts to the maitre d'.
        // Entities and lower-level systems can use this without depending on the
        // full singleton class. The real maitre d' sits behind this interface.
        void register_table(size_t table_id);
        void register_customer(size_t customer_id);
        void request_table_for_customer(size_t customer_id);
    }

    class maitre_d {
        public:
            static maitre_d& get_instance(){
                static maitre_d instance;
                return instance;
            }
            maitre_d(const maitre_d& other) = delete;
            maitre_d(maitre_d&& other) = delete;

            maitre_d& operator=(const maitre_d& other) = delete;
            maitre_d& operator=(maitre_d&& other) = delete;

            void register_table(size_t table_id);
            void register_customer(size_t customer_id);
            void request_table_for_customer(size_t customer_id);

            // Processes cafe actions gathered from events or direct interface
            // calls. This mirrors the event dispatcher style: collect facts,
            // then resolve them into command events during the game loop.
            void process_events();

            void on_registered_table_event(const events::registered_table& event);
            void on_registered_customer_event(const events::registered_customer& event);
            void on_requested_customer_table_event(const events::requested_customer_table& event);
            void on_customer_dog_arrived_event(const events::customer_dog_arrived& event);

        private:
            maitre_d()
            : registered_table_handler_([this](const events::registered_table& event) -> void {on_registered_table_event(event);}),
            registered_customer_handler_([this](const events::registered_customer& event) -> void {on_registered_customer_event(event);}),
            requested_customer_table_handler_([this](const events::requested_customer_table& event) -> void {on_requested_customer_table_event(event);}),
            customer_dog_arrived_handler_([this](const events::customer_dog_arrived& event) -> void {on_customer_dog_arrived_event(event);}){
                event_interface::subscribe<events::registered_table>(registered_table_handler_);
                event_interface::subscribe<events::registered_customer>(registered_customer_handler_);
                event_interface::subscribe<events::requested_customer_table>(requested_customer_table_handler_);
                event_interface::subscribe<events::customer_dog_arrived>(customer_dog_arrived_handler_);
            }
            ~maitre_d(){
                event_interface::unsubscribe<events::registered_table>(registered_table_handler_);
                event_interface::unsubscribe<events::registered_customer>(registered_customer_handler_);
                event_interface::unsubscribe<events::requested_customer_table>(requested_customer_table_handler_);
                event_interface::unsubscribe<events::customer_dog_arrived>(customer_dog_arrived_handler_);              
            }

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
            std::unordered_map<size_t, customer_record> customers_;

            // Physical customer queue sketch:
            // customer_dog_arrived(customer_id)
            //   -> place the dog in the first free queue slot
            //   -> emit/request pathing to queue_slot.position
            //
            // customer_dog_sent_to_table(customer_id, table_id)
            //   -> clear the dog's queue slot
            //   -> use the head slot position as the table pathing start
            //   -> compact_queue()
            //
            // move_queue_forward()
            //   -> each dog behind the head moves one slot forward
            //   -> emit/request pathing to the new queue_slot.position
            std::queue<queue_slot> waiting_customer_queue_;

            events::event_handler<events::registered_table> registered_table_handler_;
            events::event_handler<events::registered_customer> registered_customer_handler_;
            events::event_handler<events::requested_customer_table> requested_customer_table_handler_;
            events::event_handler<events::customer_dog_arrived> customer_dog_arrived_handler_;
    };
}

#endif
