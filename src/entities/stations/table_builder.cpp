#include "config.h"
#include "entities.h"
#include "hitbox.h"
#include "texture.h"
// ------------------------------ builds -------------------------------- //
std::unique_ptr<entities::entity> entities::entity_builder::build_table(Vector2 position, int id){
    auto table_sprite = sprite::sprite(
        textures::textures_.get_texture(textures::test_decoration, entity_config::test_decoration_path),
        entity_config::table_attributes[entity_config::attributes::frame_width],
        entity_config::table_attributes[entity_config::attributes::frame_height],
        entity_config::table_attributes[entity_config::attributes::frames],
        entity_config::table_attributes[entity_config::attributes::animations]
    );

    auto hitbox = hitbox::h_builder_.build_table_hitbox(position);
    std::vector<sprite::sprite> sprites = {table_sprite};
    std::vector<hitbox::hitbox> hitboxes = {hitbox};
    auto body = body::body(hitboxes, sprites);

    return std::make_unique<entities::table>(body, position, id, next_debug_id(entity_config::table_debug_id_prefix));
}
