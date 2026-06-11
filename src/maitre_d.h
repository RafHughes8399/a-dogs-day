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
#include <cstddef>
#include <unordered_map>

namespace maitre_d{
    inline constexpr size_t empty_id = static_cast<size_t>(-1);

    enum table_status{
        free = 0,
        reserved = 1,
        occupied = 2
    };

    enum customer_status{
        waiting_for_table = 0,
        going_to_table = 1,
        seated = 2,
        eating = 3,
        leaving = 4
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
            static maitre_d& get_instance();

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

        private:
            maitre_d();
            ~maitre_d() = default;

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

            events::event_handler<events::registered_table> registered_table_handler_;
            events::event_handler<events::registered_customer> registered_customer_handler_;
            events::event_handler<events::requested_customer_table> requested_customer_table_handler_;
    };
}

#endif
