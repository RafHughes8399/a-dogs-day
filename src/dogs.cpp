#include "entities.h"

// ------------------------------- player dogs ------------------------------- //
int entities::player_dog::update(float delta){
    (void) delta;
    /**
     * the dog should store its move path
     * if the dog path is empty, then the dog is not moving,
     * for now do nothing 
     */
    return status_codes::nothing;
}
void entities::player_dog::interact(entity& other){
    (void) other;
    return;
}
void on_right_click_event(const events::right_mouse_click& event){
    auto destination = event.get_mouse_position();
    // update the dog's movement path, query the level graph
    (void) destination;
    return;
}

// ------------------------------- builder ------------------------------- //
std::unique_ptr<entities::entity> entities::entity_builder::build_khiri(Vector2 position, int id){
    auto khiri_texture = textures::textures_.get_texture(textures::khiri, assets_config::khiri_path);
    return std::make_unique<entities::player_dog>(
        sprite::sprite(khiri_texture,
        assets_config::khiri_attributes[assets_config::attributes::frame_width],
        assets_config::khiri_attributes[assets_config::attributes::frame_height],
        assets_config::khiri_attributes[assets_config::attributes::frames],
        assets_config::khiri_attributes[assets_config::attributes::animations]),

        raglib::bounding_box_2{position, Vector2Add(position, 
        Vector2{assets_config::khiri_attributes[assets_config::attributes::frame_width], assets_config::khiri_attributes[assets_config::attributes::frame_height]} )}, // TODO change
        position,
        id,
        Vector2Zero());
}
std::unique_ptr<entities::entity> entities::entity_builder::build_mack(Vector2 position, int id){
    auto mack_texture = textures::textures_.get_texture(textures::mack, assets_config::mack_path);
    return std::make_unique<entities::player_dog>(
        sprite::sprite(mack_texture,
        assets_config::mack_attributes[assets_config::attributes::frame_width],
        assets_config::mack_attributes[assets_config::attributes::frame_height],
        assets_config::mack_attributes[assets_config::attributes::frames],
        assets_config::mack_attributes[assets_config::attributes::animations]),

        raglib::bounding_box_2{position, Vector2Add(position, 
        Vector2{assets_config::mack_attributes[assets_config::attributes::frame_width], assets_config::mack_attributes[assets_config::attributes::frame_height]} )}, // TODO change
        position,
        id,
        Vector2Zero());
}