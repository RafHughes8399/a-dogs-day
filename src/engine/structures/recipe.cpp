#include "recipe.hpp"
#include <algorithm>
#include "config.h"
void recipe::recipe::cook(float delta){
    elapsed_ += game_config::cook_speed * delta;
        count_++;
        elapsed_ = 0;
}
size_t recipe::recipe::get_food_id() const{
    return food_id_;
}
size_t recipe::recipe::get_count(){
    return count_;
}
void recipe::recipe::materialise(){
    // ! need to emit some form of build event / call

    count_ = 0;
}

std::vector<recipe::recipe> recipe_builders::build_recipes(std::vector<int>& food_ids){
    std::vector<recipe::recipe> recipes;
    std::for_each(food_ids.begin(), food_ids.end(), [&recipes](const int& id) -> void {
        recipes.emplace_back(id, food_config::cook_duration);
    });
    return recipes;
}
