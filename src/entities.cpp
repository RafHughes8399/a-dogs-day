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

void entities::entity::set_position(Vector2 new_position){
    position_ = new_position;
}
void entities::entity::render(){
    sprite_.render(position_);
}

// --------------------------- update strategies --------------------------- //
int entities::entity::cursor_update::update(entity& entity, float delta){
    // update the position
    auto current_position = entity.get_position();
    if(! Vector2Equals(current_position, GetMousePosition())){
        entity.set_position(GetMousePosition());
        return status_codes::moved;
    }
    return status_codes::nothing;
}
int entities::entity::paw_update::update(entity& entity, float delta){
    // set animation to play
    // play until the end
    auto& sprite = entity.get_sprite();
    auto& animation = sprite.get_animation();
    animation.next_frame(false);
    auto new_frame = animation.get_current_frame();
    return status_codes::nothing;
}
// --------------------------- interact strategies --------------------------- //
void entities::entity::default_interaction::interact(entities::entity& interactor, entities::entity& interactee){
    (void) interactor; 
    (void) interactee;
    return;
}
// --------------------------- builder --------------------------- //

// defining the builder
entities::entity_builder entities::e_builder;

std::unique_ptr<entities::entity> entities::entity_builder::build_cursor(Vector2 position, int id){
    std::unique_ptr<entity::interaction_strategy> interact = std::make_unique<entity::default_interaction>();
    std::unique_ptr<entity::update_strategy> update = std::make_unique<entity::paw_update>();
    
    auto cursor_texture = textures::textures_.get_texture(textures::cursor, assets_config::cursor_path);
    // otherwise load it 
    return std::make_unique<entities::cursor>(
        sprite::sprite(cursor_texture, 
        assets_config::cursor_attributes[assets_config::attributes::frame_width],
        assets_config::cursor_attributes[assets_config::attributes::frame_height],
        assets_config::cursor_attributes[assets_config::attributes::frames],
        assets_config::cursor_attributes[assets_config::attributes::animations]),
        raglib::bounding_box_2{Vector2Zero(), Vector2Zero()},
        GetMousePosition(),
        id,
        std::move(interact),
        std::move(update)
    );
}
std::unique_ptr<entities::entity> entities::entity_builder::build_paw_mark(Vector2 position, int id){
    std::unique_ptr<entity::interaction_strategy> interact = std::make_unique<entity::default_interaction>();
    std::unique_ptr<entity::update_strategy> update = std::make_unique<entity::paw_update>();
    
    auto paw_texture = textures::textures_.get_texture(textures::paw_mark, assets_config::paw_mark_path); 
    return std::make_unique<entities::entity>(
        sprite::sprite(paw_texture,
        assets_config::paw_mark_attributes[assets_config::attributes::frame_width],
        assets_config::paw_mark_attributes[assets_config::attributes::frame_height],
        assets_config::paw_mark_attributes[assets_config::attributes::frames],
        assets_config::paw_mark_attributes[assets_config::attributes::animations]),
        
        raglib::bounding_box_2{Vector2Zero(), Vector2Zero()}, // TODO change
        position,
        id,
        std::move(interact),
        std::move(update)
    );
}
