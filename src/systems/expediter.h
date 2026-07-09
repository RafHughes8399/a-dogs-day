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
        assigned = 1,
        scheduled = 2,
        serving = 3,
        served = 4,
        clearing = 5,
        cleared = 6,
        fulfilled = 7
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

    // TODO: ! refactor waiter, only need the ptr, the watier dog should 
    // TODO have a function to return its status of assigned or not based on the 
    // TODO state machine

    struct order{
        size_t order_id;
        size_t customer_id;          // assigned waiter id, -1 while unassigned
        entities::waiter_dog* waiter;     // stable entity pointer, nullptr while unassigned
        table_record table;
        entities::food_counter* counter;  // assigned counter, nullptr while unassigned
        order_status status;
    };

    inline table_record empty_table = {-1, Vector2{-1, -1}, Vector2{-1, -1}};

    class expediter{
        public:
            ~expediter(){
                event_interface::unsubscribe<events::registered_waiter>(registered_waiter_handler_);
                event_interface::unsubscribe<events::removed_waiter>(removed_waiter_handler_);
                event_interface::unsubscribe<events::registered_food_counter>(registered_food_counter_handler_);
                event_interface::unsubscribe<events::removed_food_counter>(removed_food_counter_handler_);
                event_interface::unsubscribe<events::dog_reached_table>(dog_reached_table_handler_);
            }
            expediter()
            : next_order_id_(0),
            registered_waiter_handler_([this](const events::registered_waiter& event) -> void {on_registered_waiter_event(event);}),
            removed_waiter_handler_([this](const events::removed_waiter& event) -> void {on_removed_waiter_event(event);}),
            registered_food_counter_handler_([this](const events::registered_food_counter& event) -> void {on_registered_food_counter_event(event);}),
            removed_food_counter_handler_([this](const events::removed_food_counter& event) -> void {on_removed_food_counter_event(event);}),
            dog_reached_table_handler_([this](const events::dog_reached_table& event) -> void {on_dog_reached_table_event(event);}){
                event_interface::subscribe<events::registered_waiter>(registered_waiter_handler_);
                event_interface::subscribe<events::removed_waiter>(removed_waiter_handler_);
                event_interface::subscribe<events::registered_food_counter>(registered_food_counter_handler_);
                event_interface::subscribe<events::removed_food_counter>(removed_food_counter_handler_);
                event_interface::subscribe<events::dog_reached_table>(dog_reached_table_handler_);
            }


            expediter(const expediter& other) = delete;
            expediter(expediter&& other) = delete;

            expediter& operator=(const expediter& other) = delete;
            expediter& operator=(expediter&& other) = delete;

            void register_waiter(entities::waiter_dog* dog);
            void remove_waiter(size_t waiter_id);
            void register_food_counter(entities::food_counter* counter);
            void remove_food_counter(size_t counter_id);

            // Number of waiters / food counters currently tracked. Exposed for
            // tests to assert registration and removal.
            size_t num_waiters() const { return waiters_.size(); }
            size_t num_counters() const { return food_counters_.size(); }

            void process_orders();
            void fulfill_order(order& order);
            bool are_waiters_available() const;
            bool are_counters_available() const;
            void assign_waiter_to_order(order& order);
            void pick_food_counter(order& order);

            void on_registered_waiter_event(const events::registered_waiter& event);
            void on_removed_waiter_event(const events::removed_waiter& event);
            void on_registered_food_counter_event(const events::registered_food_counter& event);
            void on_removed_food_counter_event(const events::removed_food_counter& event);
            void on_dog_reached_table_event(const events::dog_reached_table& event);
        private:
            std::vector<entities::waiter_dog*> waiters_;
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
