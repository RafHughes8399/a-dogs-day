#include "entities.h"
#include <iostream>
// --------------------------- entity --------------------------- // 
int entities::entity::get_id(){
    return id_;
}
raglib::bounding_box_2& entities::entity::get_bounds(){
    return bounds_;
}

sprite::sprite& entities::entity::get_sprite(){
    return sprite_;
}

Vector2 entities::entity::get_position(){
    return position_;
}


void entities::entity::update_bounds(Vector2 delta){
    bounds_.min = Vector2Add(bounds_.min, delta);
    bounds_.max = Vector2Add(bounds_.max, delta);
}
void entities::entity::render(){
    sprite_.render(position_);
}

// -------------------------------- cursor --------------------------------//

int entities::cursor::update(float delta){
    auto old_position = position_;
    position_ = GetMousePosition();

    if(! Vector2Equals(old_position, position_)) {
        Vector2 position_delta = Vector2Subtract(position_, old_position);
        bounds_.min = Vector2Add(bounds_.min, position_delta);
        bounds_.max = Vector2Add(bounds_.max, position_delta);
        return status_codes::moved;
    }
    return status_codes::nothing;
}

void entities::cursor::interact(entities::entity& other){
    (void) other;
    return;
}
void entities::cursor::on_left_mouse_event(const events::left_mouse_down& event){
    std::cout << "cursor left click event " << std::endl;
    auto mouse_delta = event.get_mouse_delta();
    auto new_position = Vector2Add(position_, mouse_delta);
    position_ = new_position;
    // create a moved entities event or something and debug 
    std::cout << "make moved event " << std::endl;
    std::unique_ptr<events::event> moved_event = std::make_unique<events::move_entity>(id_);
    event_interface::queue_event(moved_event);
}

// -------------------------------- paw mark --------------------------------//
int entities::paw_mark::update(float delta){
    auto& animation = sprite_.get_animation();
    animation.next_frame(false);
    auto new_frame = animation.get_current_frame();
    return status_codes::nothing;
}
void entities::paw_mark::interact(entities::entity& other){
    (void) other;
    return;
}
// --------------------------- builder --------------------------- //

// defining the builder
entities::entity_builder entities::e_builder;

std::unique_ptr<entities::entity> entities::entity_builder::build_cursor(Vector2 position, int id){
    auto cursor_texture = textures::textures_.get_texture(textures::cursor, assets_config::cursor_path);
    // otherwise load it 
    return std::make_unique<entities::cursor>(
        sprite::sprite(cursor_texture, 
        assets_config::cursor_attributes[assets_config::attributes::frame_width],
        assets_config::cursor_attributes[assets_config::attributes::frame_height],
        assets_config::cursor_attributes[assets_config::attributes::frames],
        assets_config::cursor_attributes[assets_config::attributes::animations]),
        raglib::bounding_box_2{GetMousePosition(), Vector2Add(GetMousePosition(), 
        Vector2 {assets_config::cursor_attributes[assets_config::attributes::frame_width], assets_config::cursor_attributes[assets_config::attributes::frame_height]})},
        GetMousePosition(),
        id
    );
}
std::unique_ptr<entities::entity> entities::entity_builder::build_paw_mark(Vector2 position, int id){
    auto paw_texture = textures::textures_.get_texture(textures::paw_mark, assets_config::paw_mark_path); 
    return std::make_unique<entities::entity>(
        sprite::sprite(paw_texture,
        assets_config::paw_mark_attributes[assets_config::attributes::frame_width],
        assets_config::paw_mark_attributes[assets_config::attributes::frame_height],
        assets_config::paw_mark_attributes[assets_config::attributes::frames],
        assets_config::paw_mark_attributes[assets_config::attributes::animations]),
        
        raglib::bounding_box_2{position, Vector2Add(position, 
        Vector2{assets_config::paw_mark_attributes[assets_config::attributes::frame_width], assets_config::paw_mark_attributes[assets_config::attributes::frame_height]} )}, // TODO change
        position,
        id
    );
}
