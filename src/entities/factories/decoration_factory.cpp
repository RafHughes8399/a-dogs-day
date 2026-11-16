#include "factories.hpp"
void factories::decoration_factory::build_decoration(size_t decoration, size_t entity_id, Vector2 position){
    auto decoration_builder = decoration_builders_[decoration];
    decoration_builder(entity_id, position);
}
