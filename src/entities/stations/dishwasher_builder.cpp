#include "config.h"
#include "entities.h"
#include "hitbox.h"
#include "texture.h"
// ------------------------------ builds -------------------------------- //
std::unique_ptr<entities::entity> entities::entity_builder::build_dishwasher(Vector2 position, int id){
    auto dishwasher_sprite = sprite::sprite(
        textures::textures_.get_texture(textures::test_decoration, entity_config::test_decoration_path),
        entity_config::dishwasher_attributes[entity_config::attributes::frame_width],
        entity_config::dishwasher_attributes[entity_config::attributes::frame_height],
        entity_config::dishwasher_attributes[entity_config::attributes::frames],
        entity_config::dishwasher_attributes[entity_config::attributes::animations],
        Vector2Zero(),
        PURPLE
    );

    auto hitbox = hitbox::h_builder_.build_dishwasher_hitbox(position);
    std::vector<sprite::sprite> sprites = {dishwasher_sprite};
    std::vector<hitbox::hitbox> hitboxes = {hitbox};
    auto body = body::body(hitboxes, sprites);

    return std::make_unique<entities::dishwasher>(body, position, id, next_debug_id(entity_config::dishwasher_debug_id_prefix));
}
