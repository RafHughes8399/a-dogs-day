#ifndef DOG_FACTORY_H
#define DOG_FACTORY_H
#define CUSTOMERS 6
#define SPECIAL_CUSTOMERS 1
#define SPAWN_NODES 2  
#include <array>
#include <functional>
#include "raylib.h"
namespace dog_factory{
    enum customers{
        tex = 0,
        saba = 1,
        customers_size
    };
    enum special_customers{
        garfield = customers_size,
        cumulative_customers_size
    };
    class dog_factory{
        public:
            ~dog_factory() = default;
            dog_factory()
            :dogs_({}){
                refresh_dogs();
            }
            dog_factory(const dog_factory& other) = default;
            dog_factory(dog_factory&& other) = default;
            dog_factory& operator=(const dog_factory& other) = default;
            dog_factory& operator=(dog_factory&& other) = default;
            
            void build_dog(size_t id, Vector2 position);

        private:
            // define the customer marble bag as per that video
            int pick_dog();
            void refresh_dogs();
            // * parallel array in a sense to the dogs enum, the dog type is an index, 
            // * and the value at the index is the number that dog left and the function to build that dog
            std::vector<int> dogs_;
            int index_ = 0;
            std::array<std::function<void(size_t, Vector2)>, cumulative_customers_size> builders_;

            // need spawn
            std::array<int, SPAWN_NODES> spawn_nodes_;

    };
}
#endif