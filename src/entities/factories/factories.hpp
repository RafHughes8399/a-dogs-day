#ifndef FACTORIES_H
#define FACTORIES_H
#define CUSTOMERS 6
#define SPECIAL_CUSTOMERS 1
#define SPAWN_POSITIONS 2  
#define DESTINATION_POSITIONS 2
#include <array>
#include "entities.h"
#include <functional>
#include "raylib.h"
namespace factories{

    class dog_factory{
        public:
            ~dog_factory() = default;
            dog_factory()
            :customer_dogs_({}), customer_builders_({
                [](size_t id, Vector2 position) -> void{ecs_entities::build_tex(id, position);},
                [](size_t id, Vector2 position) -> void{ecs_entities::build_garfield(id, position);}
            }), waiter_builders_({
                [](size_t id, Vector2 position) -> void{ecs_entities::build_gianluca(id, position);},
                [](size_t id, Vector2 position) -> void{ecs_entities::build_lionel(id, position);},
            }),
             customer_spawn_positions_({
                Vector2{level_config::edge_weight,
                    level_config::footpath_y + level_config::edge_weight},
                Vector2{level_config::edge_weight * 3,
                    level_config::footpath_y + level_config::footpath_height - level_config::edge_weight}
            }), customer_destination_positions_({
                Vector2{level_config::edge_weight,
                    level_config::footpath_y + level_config::footpath_height - level_config::edge_weight},
                Vector2{level_config::edge_weight * 3,
                    level_config::footpath_y + level_config::edge_weight}
            }){
                refresh_dogs();
            }
            dog_factory(const dog_factory& other) = default;
            dog_factory(dog_factory&& other) = default;
            dog_factory& operator=(const dog_factory& other) = default;
            dog_factory& operator=(dog_factory&& other) = default;
            
            void build_customer_dog(size_t id);
            void build_waiter_dog(size_t waiter, size_t entity_id, Vector2 position);
        private:
            // define the customer marble bag as per that video
            size_t pick_dog();
            size_t pick_route();
            void refresh_dogs();
            // * parallel array in a sense to the dogs enum, the dog type is an index, 
            // * and the value at the index is the number that dog left and the function to build that dog
            std::vector<size_t> customer_dogs_;
            size_t index_ = 0;

            std::array<std::function<void(size_t, Vector2)>, entity_config::cumulative_customers_size> customer_builders_;
            std::array<std::function<void(size_t, Vector2)>, entity_config::waiters_size> waiter_builders_;
            

            std::array<Vector2, SPAWN_POSITIONS> customer_spawn_positions_;
            std::array<Vector2, DESTINATION_POSITIONS> customer_destination_positions_;

    };

    // TODO 25 / 8 : table building within the station factory
    // ? need to better understand how stations would be built. is differetn to dogs as it is not a random selection
    // ? but rather derived from an item that the player would place down, so it would also need to specify the sprite
    // ? we could setup another enum and builder structure, that could work, to keep the general shape 
    // * so the call order would look soemthing build specific table takes in a position and id, and then calls build table which
    // * takes the id, the position and the builder, maybe build station is not needed
    // * the item would have an id that for the individual item and that for the decoration / station
    // * then that id would be used to obtain the builder and then build ? 

    // * so like say dining_table has an id of 2 in the config 
    // * then when a dining_table item is selected, it carries the id 2
    // * which the factory then takes and does a builder_[2] lookup

    // * similar logic to the ecustomer dogs, but just not randomised
    class station_factory{
        public:
            ~station_factory() = default;
            station_factory()
            :table_builders_({
                [](size_t id, Vector2 position) -> void{ecs_entities::build_dining_table(id, position);},
            }), counter_builders_({
                [](size_t id, Vector2 position) -> void{ecs_entities::build_food_counter(id, position);},
            }){

            }
            station_factory(const station_factory& other) = default;
            station_factory(station_factory&& other) = default;
            station_factory& operator=(const station_factory& other) = default;
            station_factory& operator=(station_factory&& other) = default;

            void build_station(size_t id, Vector2 position);
            void build_table(size_t table, size_t entity_id, Vector2 position);
            void build_counter(size_t counter, size_t entity_id, Vector2 position);
            void build_stove(size_t id, Vector2 position);
        private:
            std::array<std::function<void(size_t, Vector2)>, entity_config::tables_size> table_builders_;
            std::array<std::function<void(size_t, Vector2)>, entity_config::counters_size> counter_builders_;
    };
}
#endif
