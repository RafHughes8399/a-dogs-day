#include "entities.h"
#include "texture.h"
#include "debug_log_interface.h"
#include "raglib.h"
// ------------------------------- builder ------------------------------- //
// dishwasher_dog has no dedicated art yet, so - like customer_dog and
// waiter_dog before it - this reuses the mack player-dog sprites/hitbox as
// placeholder art.
std::unique_ptr<entities::entity> entities::entity_builder::build_dishwasher_dog(int id, Vector2 position){
    debug::log(
        "[entity_builder::build_dishwasher_dog] "
        "dog_id: " + std::to_string(id)
        + ", position: " + raglib::vector_to_string(position));
    auto dishwasher_left_texture = textures::textures_.get_texture(textures::mack_left, entity_config::mack_left_path);
    auto dishwasher_right_texture = textures::textures_.get_texture(textures::mack_right, entity_config::mack_right_path);
    auto dishwasher_left_sprite = sprite::sprite(dishwasher_left_texture,
        entity_config::mack_across_attributes[entity_config::attributes::frame_width],
        entity_config::mack_across_attributes[entity_config::attributes::frame_height],
        entity_config::mack_across_attributes[entity_config::attributes::frames],
        entity_config::mack_across_attributes[entity_config::attributes::animations]);
    auto dishwasher_right_sprite = sprite::sprite(dishwasher_right_texture,
        entity_config::mack_across_attributes[entity_config::attributes::frame_width],
        entity_config::mack_across_attributes[entity_config::attributes::frame_height],
        entity_config::mack_across_attributes[entity_config::attributes::frames],
        entity_config::mack_across_attributes[entity_config::attributes::animations]);

    auto across_hitbox = hitbox_builders::build_dog_across_hitbox(position);
    std::vector<sprite::sprite> sprites;
    sprites.push_back(std::move(dishwasher_left_sprite));
    sprites.push_back(std::move(dishwasher_right_sprite));

    std::vector<hitbox::hitbox> hitboxes;
    hitboxes.push_back(across_hitbox);
    hitboxes.push_back(across_hitbox);

    auto body = body::body(hitboxes, sprites);
    auto head = body::body();
    return std::make_unique<entities::dishwasher_dog>(
        std::move(body),
        std::move(head),
        position,
        id,
        next_debug_id(entity_config::dishwasher_dog_debug_id_prefix));
}
