/**
 * Service/order orchestration system.
 *
 * The expediter coordinates waiter dogs and order service flow. It mirrors the
 * maitre d' boundary: it works with ids and positions, not entity references,
 * and emits command events for world-owning systems to execute.
 */
#ifndef EXPEDITER_H
#define EXPEDITER_H

#include "events.h"
#include "raylib.h"
#include <cstddef>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

namespace expediter{
    inline constexpr size_t empty_id = static_cast<size_t>(-1);

    struct waiter_record;
    struct order_record;
    class expediter;

    class waiter_status{
        public:
            virtual ~waiter_status() = default;
            virtual bool is_available() const = 0;
    };

    class available : public waiter_status{
        public:
            bool is_available() const override;
    };

    class assigned : public waiter_status{
        public:
            bool is_available() const override;
    };

    class busy : public waiter_status{
        public:
            bool is_available() const override;
    };

    class order_state{
        public:
            virtual ~order_state() = default;
            virtual void on_assigned(expediter& manager, order_record& order) = 0;
            virtual void on_waiter_arrived(expediter& manager, order_record& order) = 0;
            virtual void on_food_served(expediter& manager, order_record& order) = 0;
            virtual void on_customer_finished(expediter& manager, order_record& order) = 0;
            virtual void on_table_cleared(expediter& manager, order_record& order) = 0;
    };

    class waiting_for_waiter : public order_state{
        public:
            void on_assigned(expediter& manager, order_record& order) override;
            void on_waiter_arrived(expediter& manager, order_record& order) override;
            void on_food_served(expediter& manager, order_record& order) override;
            void on_customer_finished(expediter& manager, order_record& order) override;
            void on_table_cleared(expediter& manager, order_record& order) override;
    };

    class waiter_assigned : public order_state{
        public:
            void on_assigned(expediter& manager, order_record& order) override;
            void on_waiter_arrived(expediter& manager, order_record& order) override;
            void on_food_served(expediter& manager, order_record& order) override;
            void on_customer_finished(expediter& manager, order_record& order) override;
            void on_table_cleared(expediter& manager, order_record& order) override;
    };

    class serving_food : public order_state{
        public:
            void on_assigned(expediter& manager, order_record& order) override;
            void on_waiter_arrived(expediter& manager, order_record& order) override;
            void on_food_served(expediter& manager, order_record& order) override;
            void on_customer_finished(expediter& manager, order_record& order) override;
            void on_table_cleared(expediter& manager, order_record& order) override;
    };

    class customer_eating : public order_state{
        public:
            void on_assigned(expediter& manager, order_record& order) override;
            void on_waiter_arrived(expediter& manager, order_record& order) override;
            void on_food_served(expediter& manager, order_record& order) override;
            void on_customer_finished(expediter& manager, order_record& order) override;
            void on_table_cleared(expediter& manager, order_record& order) override;
    };

    class waiting_to_clear : public order_state{
        public:
            void on_assigned(expediter& manager, order_record& order) override;
            void on_waiter_arrived(expediter& manager, order_record& order) override;
            void on_food_served(expediter& manager, order_record& order) override;
            void on_customer_finished(expediter& manager, order_record& order) override;
            void on_table_cleared(expediter& manager, order_record& order) override;
    };

    class complete : public order_state{
        public:
            void on_assigned(expediter& manager, order_record& order) override;
            void on_waiter_arrived(expediter& manager, order_record& order) override;
            void on_food_served(expediter& manager, order_record& order) override;
            void on_customer_finished(expediter& manager, order_record& order) override;
            void on_table_cleared(expediter& manager, order_record& order) override;
    };

    struct food_counter{
        size_t counter_id;
        Vector2 position;
    };

    struct waiter_record{
        size_t waiter_id;
        std::unique_ptr<waiter_status> status;
        size_t current_order_id;
    };

    struct order_record{
        size_t order_id;
        size_t table_id;
        size_t customer_id;
        size_t waiter_id;
        size_t food_counter_id;
        Vector2 pickup_point;
        Vector2 table_position;
        std::unique_ptr<order_state> state;
    };

    namespace interface{
        void register_waiter(size_t waiter_id);
        void register_food_counter(size_t counter_id, Vector2 position);
        void request_order_service(size_t order_id, size_t table_id, size_t customer_id, Vector2 table_position);
    }

    class expediter{
        public:
            static expediter& get_instance();

            expediter(const expediter& other) = delete;
            expediter(expediter&& other) = delete;

            expediter& operator=(const expediter& other) = delete;
            expediter& operator=(expediter&& other) = delete;

            void register_waiter(size_t waiter_id);
            void register_food_counter(size_t counter_id, Vector2 position);
            void request_order_service(size_t order_id, size_t table_id, size_t customer_id, Vector2 table_position);
            void mark_waiter_arrived_at_table(size_t waiter_id, size_t order_id);
            void mark_food_served(size_t waiter_id, size_t order_id);
            void mark_customer_finished(size_t customer_id, size_t order_id);
            void mark_table_cleared(size_t waiter_id, size_t order_id);
            void process_events();

            void set_order_state(order_record& order, std::unique_ptr<order_state> state);
            void set_waiter_status(waiter_record& waiter, std::unique_ptr<waiter_status> status);
            void release_waiter(size_t waiter_id);
            void queue_clear_order(size_t order_id);
            void assign_next_order();

            void on_registered_waiter_event(const events::registered_waiter& event);
            void on_registered_food_counter_event(const events::registered_food_counter& event);
            void on_requested_order_service_event(const events::requested_order_service& event);
            void on_waiter_arrived_at_table_event(const events::waiter_arrived_at_table& event);
            void on_waiter_served_food_event(const events::waiter_served_food& event);
            void on_customer_finished_eating_event(const events::customer_finished_eating& event);
            void on_waiter_cleared_table_event(const events::waiter_cleared_table& event);

        private:
            expediter();
            ~expediter() = default;

            waiter_record* find_waiter(size_t waiter_id);
            waiter_record* find_available_waiter();
            food_counter* choose_food_counter(Vector2 table_position);
            order_record* find_order(size_t order_id);

            std::vector<waiter_record> waiters_;
            std::vector<food_counter> food_counters_;
            std::queue<size_t> pending_order_ids_;
            std::queue<size_t> pending_clear_order_ids_;
            std::unordered_map<size_t, order_record> orders_;

            events::event_handler<events::registered_waiter> registered_waiter_handler_;
            events::event_handler<events::registered_food_counter> registered_food_counter_handler_;
            events::event_handler<events::requested_order_service> requested_order_service_handler_;
            events::event_handler<events::waiter_arrived_at_table> waiter_arrived_at_table_handler_;
            events::event_handler<events::waiter_served_food> waiter_served_food_handler_;
            events::event_handler<events::customer_finished_eating> customer_finished_eating_handler_;
            events::event_handler<events::waiter_cleared_table> waiter_cleared_table_handler_;
    };
}

#endif
