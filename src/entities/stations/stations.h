/**
 * station entities: the interactive furniture a dog paths to and uses.
 * station derives from decoration; food_counter stores food.
 */
#ifndef STATIONS_H
#define STATIONS_H

#include "decorations.h"
#include "food.h"

namespace entities{
    class station;

    // Whether a station is currently being worked by a dog, expressed as an
    // explicit state machine (mirrors customer_dog_state/waiter_dog_state)
    // rather than a derived flag - consistent with how dog behaviour is
    // modelled elsewhere in the codebase. Station owns the actual dog-id/
    // capacity data; states operate on it via the station& reference, same
    // as dog states operate on the owning dog.


    class station : public decoration {
        public:
            class station_state{
                public:
                    virtual ~station_state() = default;
                    station_state() = default;
                    station_state(const station_state& other) = default;
                    station_state(station_state&& other) = default;

                    station_state& operator=(const station_state& other) = default;
                    station_state& operator=(station_state&& other) = default;

                    virtual bool enter(station& station, int dog_id) = 0;
                    virtual bool is_interacting() const = 0;
                    virtual void leave(station& station, int dog_id) = 0;
                    // Human-readable state name for inspection/tests.
            };            class unworked : public station_state{
                public:
                    bool enter(station& station, int dog_id) override;
                    bool is_interacting() const override { return false; }
                    void leave(station& station, int dog_id) override;
            };

            class worked : public station_state{
                public:
                    bool enter(station& station, int dog_id) override;
                    bool is_interacting() const override { return true; }
                    void leave(station& station, int dog_id) override;
            };
            struct interaction_positions{
                Vector2 left;
                Vector2 right;
            };

            station(body::body body, Vector2 position, int id, std::string debug_id,
                    size_t capacity = 1)
            : decoration(body, position, id, std::move(debug_id)), interaction_positions_{},
            capacity_(capacity), interacting_dog_ids_(), state_(){
                update_interaction_positions();
                set_state(default_state());
            }
            // Non-copyable: state_ is a polymorphic unique_ptr<station_state>
            // (no clone() - the two states carry no data of their own, so
            // deep-copy support isn't worth the complexity). Matches
            // food_counter/dishwasher, which are already copy-deleted.
            station(const station& other) = delete;
            station(station&& other) = default;

            station& operator=(const station& other) = delete;
            station& operator=(station&& other) = delete;

            // Interaction positions are the walkable nodes flanking the station
            // (left and right) that a dog paths to in order to interact with it.
            // Centralised here so every station type (table, food counter) shares
            // one implementation instead of each recomputing its own.
            interaction_positions get_interaction_positions() const;
            void interact(entity& other) override;

            // Generic "is a dog physically at this station" tracking, keyed by
            // dog id (not a raw pointer - avoids station needing to handle
            // entity-removal lifetime like maitre_d/expediter do for their
            // pointer-holding table/counter tracking). Station is the sole
            // source of truth: dogs never store a reference back. Backed by an
            // explicit unworked/worked state machine (see station_state).
            size_t capacity() const;
            bool enter(int dog_id);
            bool is_interacting() const;
            bool can_accept_dog();
            void leave(int dog_id);
            void set_state(std::unique_ptr<station_state> state){
                state_ = std::move(state);
            }

        protected:
            void update_interaction_positions();
            interaction_positions interaction_positions_;

            // Only the concrete states need to touch the dog-id/capacity data
            // directly; everyone else goes through enter()/leave()/is_interacting().

            static std::unique_ptr<station_state> default_state();

            size_t capacity_;
            std::vector<int> interacting_dog_ids_;
            std::unique_ptr<station_state> state_;
    };
    class table : public station {
        public:

            table(body::body body, Vector2 position, int id, std::string debug_id)
            : station(body, position, id, std::move(debug_id)){}; // default constructed with capacity = 1
            table(const table& other) = delete; // station is non-copyable
            table(table&& other) = default;

            table& operator=(const table& other) = delete;
            table& operator=(table&& other) = delete;

            bool can_accept_dog();
            void clear();
            int get_assigned_dog_id();
            void place_down() override;
        private:
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
            dishwasher& operator=(dishwasher&& other) = delete;
        private:
            // Interacting-dog tracking (station::enter/leave/is_interacting) is
            // inherited from station, keyed by dog id and driven generically by
            // level's arrival wiring - no dishwasher-specific handling needed.
            // dishwasher itself remains an unwired stub (no .cpp, no CMakeLists
            // registration, no orchestrator system): wash-cycle gameplay is a
            // separate follow-up.
            capacity_state dish_capacity_;
            int max_plates_;
            int num_plates_;
    };
}
#endif
