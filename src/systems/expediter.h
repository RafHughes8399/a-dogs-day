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
#include "events_interface.h"
#include "dog_actions.h"
#include "raylib.h"
#include <cstddef>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

namespace expediter{

    enum order_status{
        created = 0,
        scheduled = 2,
        serving = 1,
        served = 3,
        clearing = 4,
        cleared = 5,
        fulfilled = 6
    };
    struct table_record{
        int id;
        Vector2 interaction_position;
        Vector2 position;
        bool operator==(const table_record& other) const{
            return id == other.id;
        }
    };
    struct food_counter_record{
        int id;
        Vector2 position;
        Vector2 interaction_position;
        bool operator==(const food_counter_record& other) const{
            return id == other.id;
        }
    };
    struct waiter{
        int id;
        Vector2 position;
        bool assigned_to_order;

        bool operator==(const waiter& other) const{
            return id == other.id;
        }
    };

    struct order{
        size_t order_id;
        size_t customer_id;
        waiter& assigned_waiter;
        table_record table;
        food_counter_record& food_counter;
        order_status status;
    };

    inline waiter empty_waiter = {-1, Vector2(-1, -1), false};
    inline table_record empty_table = {-1, Vector2{-1, -1}};
    inline food_counter_record empty_counter = {-1, Vector2{-1, -1}};

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
            void register_food_counter(size_t counter_id, Vector2 position, Vector2 interaction_position);
            void request_order_service(size_t order_id, size_t table_id, size_t customer_id, Vector2 table_position);

            void process_orders();
            void fulfill_order(order& order);
            void clear_table();
            bool can_create_order(waiter& waiter, food_counter_record& food_counter);
            void create_order(waiter& waiter, food_counter_record& food_counter, table_record table, size_t customer_id);
            void schedule_order(table_record table, size_t customer_id);
            void check_scheduled_orders();
            waiter& assign_waiter_to_order();
            food_counter_record& find_counter();
            void send_dog_to_counter(int dog_id, food_counter_record counter);
            void send_dog_to_table(int dog_id, table_record table);
            void send_dog_to_furniture(int dog_id, int table_id, Vector2 position);
            void on_registered_waiter_event(const events::registered_waiter& event);
            void on_registered_food_counter_event(const events::registered_food_counter& event);
            void on_dog_reached_table_event(const events::dog_reached_table& event);
            void on_customer_finished_eating_event(const events::customer_finished_eating& event);
            void on_waiter_cleared_table_event(const events::waiter_cleared_table& event);
        private:
            expediter();
            ~expediter() = default;

            std::vector<waiter> waiters_;
            std::vector<food_counter_record> food_counters_;
            std::vector<order> orders_;
            std::vector<order> scheduled_orders_;
            size_t next_order_id_;

            events::event_handler<events::registered_waiter> registered_waiter_handler_;
            events::event_handler<events::registered_food_counter> registered_food_counter_handler_;
            events::event_handler<events::dog_reached_table> dog_reached_table_handler_;
            events::event_handler<events::customer_finished_eating> customer_finished_eating_handler_;
            events::event_handler<events::waiter_cleared_table> waiter_cleared_table_handler_;
    };
}

#endif
