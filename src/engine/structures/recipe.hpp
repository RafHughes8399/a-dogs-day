#ifndef RECIPE_H
#define RECIPE_H
#include <stddef.h>
#include <stdint.h>
#include <vector>
namespace recipe{
    class recipe{
        public:
            ~recipe() = default;
            recipe(size_t food, uint8_t duration)
            : food_id_(food), duration_(duration), count_(0), elapsed_(0){}
            recipe(const recipe& other) = default;
            recipe(recipe&& other) = default;

            recipe& operator=(const recipe& other) = delete;
            recipe& operator=(recipe&& other) = delete;

            void cook(float delta);
            size_t get_food_id() const;
            size_t get_count();
            void materialise();

        private:
            const size_t food_id_;
            uint8_t duration_;
            size_t count_;
            uint8_t elapsed_;
    };
}
namespace recipe_builders{
    std::vector<recipe::recipe> build_recipes(std::vector<int>& food_ids);
}

#endif