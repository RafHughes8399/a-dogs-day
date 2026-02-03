#include "entities.h"

// ------------------------ decorations -----------------------------------// 


// ------------------------------ builds -------------------------------- //

std::unique_ptr<entities::entity> entities::entity_builder::build_test_decoration(Vector2 position, int id){
    // load the sprite and the hitbox

    auto sprite = sprite::sprite(
        textures::textures_.get_texture(textures::test_decoration, assets_config::test_decoration_path),
        assets_config::test_decoration_attributes[assets_config::attributes::frame_width],
        assets_config::test_decoration_attributes[assets_config::attributes::frame_height],
        assets_config::test_decoration_attributes[assets_config::attributes::frames],
        assets_config::test_decoration_attributes[assets_config::attributes::animations]
    );
    auto hitbox = hitbox::h_builder_.build_test_decoration_hitbox(position);
    std::vector<sprite::sprite> sprites = {sprite};
    std::vector<hitbox::hitbox> hitboxes = {hitbox};

    return std::make_unique<entities::decoration>(sprites, hitboxes, position, id);
}