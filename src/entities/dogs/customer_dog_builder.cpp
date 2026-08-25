#include "entities.h"
#include "texture.h"
#include "debug_log_interface.h"
#include "raglib.h"
#include <optional>
#include <vector>
// ------------------------------- builder ------------------------------- //
// NPC dog sprite art/config pending.
std::unique_ptr<entities::entity> entities::entity_builder::build_customer_dog(int id, int dog_type, Vector2 position, std::optional<Vector2> destination){
    debug::log(
        "[entity_builder::build_customer_dog, building npc dog] "
        "dog_id: " + std::to_string(id)
        + ", dog_type: " + std::to_string(dog_type)
        + ", position: " + raglib::vector_to_string(position)
        + ", has_destination: " + std::to_string(destination.has_value())
        + (destination.has_value()
            ? ", destination: " + raglib::vector_to_string(destination.value())
            : ""));
    auto customer_left_texture = textures::textures_.get_texture(textures::mack_left, entity_config::mack_left_path);
    auto customer_right_texture = textures::textures_.get_texture(textures::mack_right, entity_config::mack_right_path);
    auto customer_left_sprite = sprite::sprite(customer_left_texture,
        entity_config::mack_across_attributes[entity_config::attributes::frame_width],
        entity_config::mack_across_attributes[entity_config::attributes::frame_height],
        entity_config::mack_across_attributes[entity_config::attributes::frames],
        entity_config::mack_across_attributes[entity_config::attributes::animations]);
    auto customer_right_sprite = sprite::sprite(customer_right_texture,
        entity_config::mack_across_attributes[entity_config::attributes::frame_width],
        entity_config::mack_across_attributes[entity_config::attributes::frame_height],
        entity_config::mack_across_attributes[entity_config::attributes::frames],
        entity_config::mack_across_attributes[entity_config::attributes::animations]);

    auto across_hitbox = hitbox_builders::build_dog_across_hitbox(position);
    std::vector<sprite::sprite> sprites;
    sprites.push_back(std::move(customer_left_sprite));
    sprites.push_back(std::move(customer_right_sprite));

    std::vector<hitbox::hitbox> hitboxes;
    hitboxes.push_back(across_hitbox);
    hitboxes.push_back(across_hitbox);

    auto body = body::body(hitboxes, sprites);
    auto head = body::body();
    if(destination.has_value()){
        debug::log(
            "[entity_builder::build_customer_dog, constructing customer dog with destination] "
            "dog_id: " + std::to_string(id)
            + ", dog_type: " + std::to_string(dog_type)
            + ", position: " + raglib::vector_to_string(position)
            + ", destination: " + raglib::vector_to_string(destination.value()));
        return std::make_unique<entities::customer_dog>(
        std::move(body),
        std::move(head),
        position,
        destination.value(),
        id,
        next_debug_id(entity_config::customer_dog_debug_id_prefix));
    }else{
        debug::log(
            "[entity_builder::build_customer_dog, constructing customer dog without destination] "
            "dog_id: " + std::to_string(id)
            + ", dog_type: " + std::to_string(dog_type)
            + ", position: " + raglib::vector_to_string(position));
        return std::make_unique<entities::customer_dog>(
            std::move(body),
            std::move(head),
            position,
            id,
            next_debug_id(entity_config::customer_dog_debug_id_prefix));
    }
}
