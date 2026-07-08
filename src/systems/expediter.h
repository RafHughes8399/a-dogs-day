/**
 * Service/order orchestration system.
 *
 * The expediter coordinates waiter dogs and order service flow. It holds
 * non-owning raw pointers to the waiter and food-counter entities it tracks
 * (the level owns them; register/remove events keep the pointers valid), which
 * lets it read live positions and take()/store() food from the counter
 * directly. It still emits command events for world-owning systems to execute
 * world mutations such as pathing.
 */
#ifndef EXPEDITER_H
#define EXPEDITER_H

#include "events.h"
#include "events_interface.h"
#include "dog_actions.h"
#include "entities.h"
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
    // A waiter the expediter can assign to orders. The dog entity is held by
    // non-owning pointer (position read live from it); whether the waiter is
    // busy is expediter-domain state and lives here, not on the dog.
    struct waiter{
        int id;
        entities::waiter_dog* ptr;
        bool assigned_to_order;

        bool operator==(const waiter& other) const{
            return id == other.id;
        }
    };

    struct order{
        size_t order_id;
        size_t customer_id;
        int waiter_id;                    // assigned waiter id, -1 while unassigned
        entities::waiter_dog* waiter;     // stable entity pointer, nullptr while unassigned
        table_record table;
        entities::food_counter* counter;  // assigned counter, nullptr while unassigned
        order_status status;
    };

    inline waiter empty_waiter = {-1, nullptr, false};
    inline table_record empty_table = {-1, Vector2{-1, -1}, Vector2{-1, -1}};

    class expediter{
        public:
            expediter();
            ~expediter();

            expediter(const expediter& other) = delete;
            expediter(expediter&& other) = delete;

            expediter& operator=(const expediter& other) = delete;
            expediter& operator=(expediter&& other) = delete;

            void register_waiter(entities::waiter_dog* dog);
            void remove_waiter(size_t waiter_id);
            void register_food_counter(entities::food_counter* counter);
            void remove_food_counter(size_t counter_id);

            void process_orders();
            void fulfill_order(order& order);
            bool are_waiters_available() const;
            bool are_counters_available() const;
            waiter& assign_waiter_to_order();
            entities::food_counter* find_counter();

            void on_registered_waiter_event(const events::registered_waiter& event);
            void on_removed_waiter_event(const events::removed_waiter& event);
            void on_registered_food_counter_event(const events::registered_food_counter& event);
            void on_removed_food_counter_event(const events::removed_food_counter& event);
            void on_dog_reached_table_event(const events::dog_reached_table& event);
        private:
            std::vector<waiter> waiters_;
            std::vector<entities::food_counter*> food_counters_;
            std::vector<order> orders_;
            size_t next_order_id_;

            events::event_handler<events::registered_waiter> registered_waiter_handler_;
            events::event_handler<events::removed_waiter> removed_waiter_handler_;
            events::event_handler<events::registered_food_counter> registered_food_counter_handler_;
            events::event_handler<events::removed_food_counter> removed_food_counter_handler_;
            events::event_handler<events::dog_reached_table> dog_reached_table_handler_;
    };
}

#endif
