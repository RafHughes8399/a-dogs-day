#include "entities.h"
#include "texture.h"
// -------------------------------- builds -------------------------------- //
std::unique_ptr<entities::entity> entities::entity_builder::build_cursor(Vector2 position, int id){
    auto cursor_texture = textures::textures_.get_texture(textures::cursor, entity_config::cursor_path);
    auto cursor_hitbox = hitbox::h_builder_.build_cursor_hitbox(position);
    // otherwise load it
    auto cursor_sprite = sprite::sprite(cursor_texture,
        entity_config::cursor_attributes[entity_config::attributes::frame_width],
        entity_config::cursor_attributes[entity_config::attributes::frame_height],
        entity_config::cursor_attributes[entity_config::attributes::frames],
        entity_config::cursor_attributes[entity_config::attributes::animations]);
    auto sprites = std::vector<sprite::sprite>{cursor_sprite};
    auto hitboxes = std::vector<hitbox::hitbox>{cursor_hitbox};
    auto body = body::body(hitboxes, sprites);
    return std::make_unique<entities::cursor>(
        body,
        GetMousePosition(),
        id,
        next_debug_id(entity_config::cursor_debug_id_prefix)
    );
}
std::unique_ptr<entities::entity> entities::entity_builder::build_paw_mark(Vector2 position, int id){
    auto paw_texture = textures::textures_.get_texture(textures::paw_mark, entity_config::paw_mark_path);
    auto paw_hitbox = hitbox::h_builder_.build_paw_mark_hitbox(position);
    auto paw_sprite =         sprite::sprite(paw_texture,
        entity_config::paw_mark_attributes[entity_config::attributes::frame_width],
        entity_config::paw_mark_attributes[entity_config::attributes::frame_height],
        entity_config::paw_mark_attributes[entity_config::attributes::frames],
        entity_config::paw_mark_attributes[entity_config::attributes::animations]);


    auto sprites = std::vector<sprite::sprite>{paw_sprite};
    auto hitboxes = std::vector<hitbox::hitbox>{paw_hitbox};
    auto body = body::body(hitboxes, sprites);
    return std::make_unique<entities::paw_mark>(
        body,
        position,
        id,
        next_debug_id(entity_config::paw_mark_debug_id_prefix)
    );
}
