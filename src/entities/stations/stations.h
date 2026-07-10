/**
 * station entities: the interactive furniture a dog paths to and uses.
 * station derives from decoration; food_counter stores food.
 */
#ifndef STATIONS_H
#define STATIONS_H

#include "decorations.h"
#include "food.h"

namespace entities{
    class station : public decoration {
        public:
            struct interaction_positions{
                Vector2 left;
                Vector2 right;
            };
            station(body::body body, Vector2 position, int id, std::string debug_id)
            : decoration(body, position, id, std::move(debug_id)), interaction_positions_{}{
                update_interaction_positions();
            }
            station(const station& other) = default;
            station(station&& other) = default;

            station& operator=(const station& other) = delete;
            station& operator=(station&& other) = delete;

            // Interaction positions are the walkable nodes flanking the station
            // (left and right) that a dog paths to in order to interact with it.
            // Centralised here so every station type (table, food counter) shares
            // one implementation instead of each recomputing its own.
            interaction_positions get_interaction_positions() const;
            void interact(entity& other) override;

        protected:
            void update_interaction_positions();
            interaction_positions interaction_positions_; 

        private:
    };

    class table : public station {
        public:
            enum table_state{
                available = 0,
                reserved = 1,
                occupied = 2
            };

            table(body::body body, Vector2 position, int id, std::string debug_id)
            : station(body, position, id, std::move(debug_id)),
            assigned_dog_id_(level_config::empty_node), state_(table_state::available){}
            table(const table& other) = default;
            table(table&& other) = default;

            table& operator=(const table& other) = delete;
            table& operator=(table&& other) = delete;

            bool can_accept_dog();
            void clear();
            int get_assigned_dog_id();
            table_state get_state();
            void occupy();
            void place_down() override;
            bool reserve_for(int dog_id);

        private:
            int assigned_dog_id_;
            table_state state_;
    };

    // A food_counter stores food as a FILO stack, up to a fixed capacity. Producers
    // store() food onto it; waiters take() the top for service (ownership moves out).
    class food_counter : public station {
        public:
            enum counter_status{
                empty = 0,
                has_food = 1,
                full = 2
            };

            food_counter(body::body body, Vector2 position, int id, std::string debug_id)
            : station(body, position, id, std::move(debug_id)),
            max_capacity_(entity_config::food_counter_capacity), stored_food_(){}
            food_counter(const food_counter& other) = delete;
            food_counter(food_counter&& other) = default;

            food_counter& operator=(const food_counter& other) = delete;
            food_counter& operator=(food_counter&& other) = delete;

            // Reservations: food promised to an in-flight order that hasn't been
            // collected yet. available_capacity() = stored - reserved, so a second
            // order can't claim the same item before the first waiter picks it up.
            size_t available_capacity() const;
            size_t current_capacity() const;
            bool has_available_food() const;
            bool is_empty() const;
            size_t max_capacity() const;
            void place_down() override;
            void release_reservation();
            void render(Vector2 draw_position, int frame) override;
            void reserve();
            size_t reserved() const;
            counter_status status() const;
            // store: push food onto the stack, rejects (returns false) when full.
            bool store(std::unique_ptr<food> item);
            // take: pop and move out the top of the stack. Precondition: !is_empty().
            std::unique_ptr<food> take();

        private:
            size_t max_capacity_;
            size_t reserved_ = 0;
            std::vector<std::unique_ptr<food>> stored_food_;
    };
    class dishwasher : public station{
        public:
            enum capacity_state{
                empty = 0,
                non_empty = 1,
                partially_full = 2,
                near_full = 3,
                full = 4
            };
            enum capcity_dishes{
                
            };
            dishwasher(body::body body, Vector2 position, int id, std::string debug_id)
            : station(body, position, id, std::move(debug_id)){}
            dishwasher(const dishwasher& other) = delete;
            dishwasher(dishwasher&& other) = default;

            dishwasher& operator=(const dishwasher& other) = delete;
            dishwasher& operator=(food_counter&& other) = delete;
        private:
            // dogs - the dogs currently interacting with the dishwasher - raw pointers ? // this is the interacting behaviour thing we need to discuss
            // i dont think the dishwasher need the dog points like the 
            
            // ? in general stations should have an interacting state and a non-interacting state, i.e is the station being worked, yes or no
            // ? now how can we measure that ?
            // ? theory one, we tether a dog to a station through a pointer

            // ? state switch when a dog leaves a station
            // ? state switch when a dog arrives at a station, can be done through events ? but we then run into that event bloat problem right ?, emitted to heaps of listens but 
            // ? only care about one

            // ? 
            
            capacity_state dish_capacity_;
            int max_plates_;
            int num_plates_;
    };
}
#endif
