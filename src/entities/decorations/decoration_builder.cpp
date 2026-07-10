#include "config.h"
#include "entities.h"
#include "hitbox.h"
#include "texture.h"
// ------------------------------ builds -------------------------------- //

std::unique_ptr<entities::entity> entities::entity_builder::build_test_decoration(Vector2 position, int id){
    // load the sprite and the hitbox

    auto sprite = sprite::sprite(
        textures::textures_.get_texture(textures::test_decoration, entity_config::test_decoration_path),
        entity_config::test_decoration_attributes[entity_config::attributes::frame_width],
        entity_config::test_decoration_attributes[entity_config::attributes::frame_height],
        entity_config::test_decoration_attributes[entity_config::attributes::frames],
        entity_config::test_decoration_attributes[entity_config::attributes::animations]
    );
    auto hitbox = hitbox::h_builder_.build_test_decoration_hitbox(position);
    std::vector<sprite::sprite> sprites = {sprite};
    std::vector<hitbox::hitbox> hitboxes = {hitbox};
    auto body = body::body(hitboxes, sprites);

    return std::make_unique<entities::decoration>(body, position, id, next_debug_id(entity_config::decoration_debug_id_prefix));
}

std::unique_ptr<entities::entity> entities::entity_builder::build_gargoyle(Vector2 position, int id){
    auto gargoyle_void = sprite::sprite(
        textures::textures_.get_texture(textures::gargoyle_void, entity_config::gargoyle_void_decoration_path),
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_width],
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_height],
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::frames],
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::animations]
    );
    auto gargoyle_sick_of_it = sprite::sprite(
        textures::textures_.get_texture(textures::gargoyle_sick_of_it, entity_config::gargoyle_void_decoration_path),
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_width],
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_height],
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::frames],
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::animations]
    );

    auto hitbox = hitbox::h_builder_.build_gargoyle_hitbox(position);

    std::vector<sprite::sprite> sprites = {gargoyle_void, gargoyle_sick_of_it};
    std::vector<hitbox::hitbox> hitboxes = {hitbox, hitbox};
    auto body = body::body(hitboxes, sprites);

    return std::make_unique<entities::decoration>(body, position, id, next_debug_id(entity_config::decoration_debug_id_prefix));
}
