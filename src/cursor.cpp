#include "entities.h"

// -------------------------------- cursor --------------------------------//

int entities::cursor::update(float delta){
    auto old_position = position_;
    position_ =  GetMousePosition();

    if(! Vector2Equals(old_position, position_)) {
        Vector2 position_delta = Vector2Subtract(position_, old_position);
        bounds_.min = Vector2Add(bounds_.min, position_delta);
        bounds_.max = Vector2Add(bounds_.max, position_delta);

        // create the query and execute it
        std::unique_ptr<queries::query> colliding_query = std::make_unique<queries::is_colliding_query>(bounds_);
        // the listener (singular) does something with the query and returns the information
        bool is_colliding = query_interface::execute_query(*colliding_query);

        if(is_colliding){
            // switch to colliding 
            std::cout << "is colliding" << std::endl;
        }
        else{
            // switch to default anim
            std::cout << "is not colliding" << std::endl;
        }


        return status_codes::moved;
    }
    return status_codes::nothing;
}

void entities::cursor::interact(entities::entity& other){
    // down cast to a dog because that's all we have at the moment will, figure something 
    // else out 
    
    player_dog& dog_cast = dynamic_cast<player_dog&>(other);
    
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
    return;
}

// -------------------------------- builds -------------------------------- //
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
    return std::make_unique<entities::paw_mark>(
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