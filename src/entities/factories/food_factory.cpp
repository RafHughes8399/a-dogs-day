#include "factories.hpp"
void factories::food_factory::build_food(size_t food, size_t entity_id, Vector2 position){
    auto food_builder = food_builders_[food];
    food_builder(entity_id, position);
}
