#ifndef DOG_FACTORY_H
#define DOG_FACTORY_H
#define CUSTOMERS 6
#define SPECIAL_CUSTOMERS 1
#define SPAWN_POSITIONS 2  
#define DESTINATION_POSITIONS 2
#include <array>
#include "entities.h"
#include <functional>
#include "raylib.h"
namespace dog_factory{
    enum customers{
        tex = 0,
        customers_size = 1
    };
    enum special_customers{
        garfield = customers_size,
        cumulative_customers_size
    };
    class dog_factory{
        public:
            ~dog_factory() = default;
            dog_factory()
            :dogs_({}), builders_({
                [](size_t id, Vector2 position) -> void{ecs_entities::build_tex(id, position);}
            }), spawn_positions_({
                Vector2{level_config::edge_weight, -2 * level_config::edge_weight  },
                Vector2{level_config::edge_weight * 3, (level_config::cafe_y + level_config::cafe_height) + 2 * level_config::edge_weight}
            }), destination_positions_({
                Vector2{level_config::edge_weight,(level_config::cafe_y + level_config::cafe_height) + 2 * level_config::edge_weight},
                Vector2{level_config::edge_weight * 3, -2 * level_config::edge_weight}, 
            }){
                refresh_dogs();
            }
            dog_factory(const dog_factory& other) = default;
            dog_factory(dog_factory&& other) = default;
            dog_factory& operator=(const dog_factory& other) = default;
            dog_factory& operator=(dog_factory&& other) = default;
            
            void build_customer_dog(size_t id);

        private:
            // define the customer marble bag as per that video
            int pick_dog();
            Vector2 pick_spawn();
            Vector2 pick_destination();
            void refresh_dogs();
            // * parallel array in a sense to the dogs enum, the dog type is an index, 
            // * and the value at the index is the number that dog left and the function to build that dog
            std::vector<int> dogs_;
            int index_ = 0;
            std::array<std::function<void(size_t, Vector2)>, cumulative_customers_size> builders_;

            // need spawn
            std::array<Vector2, SPAWN_POSITIONS> spawn_positions_;
            std::array<Vector2, DESTINATION_POSITIONS> destination_positions_;

    };
}
#endif