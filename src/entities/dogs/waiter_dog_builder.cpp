#include "entities.h"
#include "texture.h"
#include "debug_log_interface.h"
#include "raglib.h"
#include <optional>
#include <vector>
// ------------------------------- builder ------------------------------- //
std::unique_ptr<entities::entity> entities::entity_builder::build_waiter_dog(int id, int dog_type, Vector2 position, std::optional<Vector2> destination){
    debug::log(
        "[entity_builder::build waiter dog] "
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

    auto across_hitbox = hitbox::h_builder_.build_player_dog_across_hitbox(position);
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
            "[entity_builder::build_waiter_dog, constructing waiter dog with destination] "
            "dog_id: " + std::to_string(id)
            + ", dog_type: " + std::to_string(dog_type)
            + ", position: " + raglib::vector_to_string(position)
            + ", destination: " + raglib::vector_to_string(destination.value()));
    }
    else{
        debug::log(
            "[entity_builder::build_waiter_dog, constructing waiter dog without destination] "
            "dog_id: " + std::to_string(id)
            + ", dog_type: " + std::to_string(dog_type)
            + ", position: " + raglib::vector_to_string(position));
    }
    // A waiter starts idle and receives its paths from the expediter via events,
    // so the destination argument is not applied at construction (unlike a
    // customer, whose destination seeds its walk-to-table path).
    return std::make_unique<entities::waiter_dog>(
        std::move(body),
        std::move(head),
        position,
        id,
        next_debug_id(entity_config::waiter_dog_debug_id_prefix),
        level_config::directions::left);
}
