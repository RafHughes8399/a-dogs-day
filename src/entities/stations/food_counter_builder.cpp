#include "config.h"
#include "entities.h"
#include "hitbox.h"
#include "texture.h"
// ------------------------------ builds -------------------------------- //
std::unique_ptr<entities::entity> entities::entity_builder::build_food_counter(Vector2 position, int id){
    auto food_counter_sprite = sprite::sprite(
        textures::textures_.get_texture(textures::test_decoration, entity_config::test_decoration_path),
        entity_config::food_counter_attributes[entity_config::attributes::frame_width],
        entity_config::food_counter_attributes[entity_config::attributes::frame_height],
        entity_config::food_counter_attributes[entity_config::attributes::frames],
        entity_config::food_counter_attributes[entity_config::attributes::animations],
        Vector2Zero(),
        BLUE
    );

    auto hitbox = hitbox::h_builder_.build_food_counter_hitbox(position);
    std::vector<sprite::sprite> sprites = {food_counter_sprite};
    std::vector<hitbox::hitbox> hitboxes = {hitbox};
    auto body = body::body(hitboxes, sprites);

    return std::make_unique<entities::food_counter>(body, position, id, next_debug_id(entity_config::food_counter_debug_id_prefix));
}
