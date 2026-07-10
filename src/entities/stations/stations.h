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
            enum station_type{
                table_station = 0,
                food_counter_station = 1
            };

            station(body::body body, Vector2 position, int id, std::string debug_id, station_type type)
            : decoration(body, position, id, std::move(debug_id)), interaction_positions_{}, type_(type){
                update_interaction_positions();
            }
            station(const station& other) = default;
            station(station&& other) = default;

            station& operator=(const station& other) = delete;
            station& operator=(station&& other) = delete;

            station_type get_station_type();
            void interact(entity& other) override;

            // Interaction positions are the walkable nodes flanking the station
            // (left and right) that a dog paths to in order to interact with it.
            // Centralised here so every station type (table, food counter) shares
            // one implementation instead of each recomputing its own.
            interaction_positions get_interaction_positions() const;

        protected:
            void update_interaction_positions();
            interaction_positions interaction_positions_; // TODO ! refactor this type, why the fuck is it under events

        private:
            station_type type_;
    };

    class table : public station {
        public:
            enum table_state{
                available = 0,
                reserved = 1,
                occupied = 2
            };

            table(body::body body, Vector2 position, int id, std::string debug_id)
            : station(body, position, id, std::move(debug_id), station_type::table_station),
            state_(table_state::available), assigned_dog_id_(level_config::empty_node){}
            table(const table& other) = default;
            table(table&& other) = default;

            table& operator=(const table& other) = delete;
            table& operator=(table&& other) = delete;

            bool can_accept_dog();
            bool reserve_for(int dog_id);
            void occupy();
            void clear();
            table_state get_state();
            int get_assigned_dog_id();
            void place_down() override;

        private:
            table_state state_;
            int assigned_dog_id_;
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
            : station(body, position, id, std::move(debug_id), station_type::food_counter_station),
            max_capacity_(entity_config::food_counter_capacity), stored_food_(){}
            food_counter(const food_counter& other) = delete;
            food_counter(food_counter&& other) = default;

            food_counter& operator=(const food_counter& other) = delete;
            food_counter& operator=(food_counter&& other) = delete;

            // store: push food onto the stack, rejects (returns false) when full.
            bool store(std::unique_ptr<food> item);
            // take: pop and move out the top of the stack. Precondition: !is_empty().
            std::unique_ptr<food> take();
            bool is_empty() const;
            size_t current_capacity() const;
            size_t max_capacity() const;
            counter_status status() const;
            // Reservations: food promised to an in-flight order that hasn't been
            // collected yet. available_capacity() = stored - reserved, so a second
            // order can't claim the same item before the first waiter picks it up.
            void reserve();
            void release_reservation();
            size_t reserved() const;
            size_t available_capacity() const;
            bool has_available_food() const;
            void render(Vector2 draw_position, int frame) override;
            void place_down() override;

        private:
            size_t max_capacity_;
            std::vector<std::unique_ptr<food>> stored_food_;
            size_t reserved_ = 0;
    };
}
#endif
