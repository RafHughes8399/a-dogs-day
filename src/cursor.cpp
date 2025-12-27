#include "entities.h"

// -------------------------------- interaction states --------------------------------//
void entities::cursor::left_click_strategy::interact(cursor& cursor, entity& other){
    std::cout << "left click interaction " << std::endl;
}
void entities::cursor::right_click_strategy::interact(cursor& cursor, entity& other){
    std::cout << "right click interaction " << std::endl;
    
}
void entities::cursor::default_strategy::interact(cursor& cursor, entity& other){
    std::cout << "default click interaction " << std::endl;
    
}
// -------------------------------- cursor --------------------------------//

int entities::cursor::update(float delta){
    auto old_position = position_;
    position_ =  GetMousePosition();

    if(! Vector2Equals(old_position, position_)) {
        hitbox_.update(position_);

        // create the query and execute it

        // TODO change how this works to test your interaction theory  !
        std::unique_ptr<queries::query> colliding_query = std::make_unique<queries::is_colliding_query>(hitbox_, id_);
        // the listener (singular) does something with the query and returns the information
        bool is_colliding = queries::bool_executor_.execute_query(*colliding_query);
        //bool is_colliding = query_interface::execute_query(*colliding_query);
        if(is_colliding){
            // switch to colliding 
            sprite_.get_animation().goto_animation(animation_tags::hover);
        }
        else{
            sprite_.get_animation().goto_animation(animation_tags::base);
            // switch to default anim
        }
        

        return status_codes::moved;
    }
    return status_codes::nothing;
}

void entities::cursor::interact(entities::entity& other){
    interaction_strategy_->interact(*this, other);
}
void entities::cursor::on_left_mouse_click_event(const events::left_mouse_click& event){
    auto position = event.get_mouse_position();
    auto hitbox = event.get_hitbox();
    std::cout << "cursor handle left click" << std::endl;
    // check interactions within the quadtree, use the correct interaction stategy (the left click one)
    interaction_strategy_ = std::make_unique<left_click_strategy>();
    
    
    // ! note that the interaction strategy changes back on the current frame, but the interaction 
    // ! event would not be executed until the next frame, so that could be an issue, if it is, just 
    // ! execute it directly
    

    // ! lets test this theory
    // ! your theory was correct yay, i think, do one more test 
    // create the interaction event
    std::cout << "cursor create interaction event " << std::endl;
    std::unique_ptr<events::event> interaction_event = std::make_unique<events::interact_entity>(id_, hitbox_);
    event_interface::execute_event(*interaction_event);
    // then return to default interaciton
    interaction_strategy_ = std::make_unique<default_strategy>();

}
void entities::cursor::on_left_mouse_down_event(const events::left_mouse_down& event){


    // look, its left mouse down 
    auto mouse_delta = event.get_mouse_delta();
    auto new_position = Vector2Add(position_, mouse_delta);
    position_ = new_position;
    interaction_strategy_ = std::make_unique<left_click_strategy>();
    std::unique_ptr<events::event> moved_event = std::make_unique<events::move_entity>(id_);

    event_interface::queue_event(moved_event); 


    // return to default interaction, for now
    interaction_strategy_ = std::make_unique<default_strategy>();
}

void entities::cursor::on_right_mouse_click_event(const events::right_mouse_click& event){
    // for now just change the interact state
    interaction_strategy_ = std::make_unique<right_click_strategy>();
    // do something, then go back
    interaction_strategy_ = std::make_unique<default_strategy>();
}
// -------------------------------- paw mark --------------------------------//
int entities::paw_mark::update(float delta){
    auto& animation = sprite_.get_animation();
    animation.next_frame(false);
    auto new_frame = animation.get_current_frame();

    if(new_frame == animation.num_frames() - 1){
        return status_codes::dead;
    }
    else{
        return status_codes::nothing;
    }
}
void entities::paw_mark::interact(entities::entity& other){
    return;
}

// -------------------------------- builds -------------------------------- //
std::unique_ptr<entities::entity> entities::entity_builder::build_cursor(Vector2 position, int id){
    auto cursor_texture = textures::textures_.get_texture(textures::cursor, assets_config::cursor_path);
    auto cursor_hitbox = hitbox::h_builder_.build_cursor_hitbox(position);
    // otherwise load it 
    return std::make_unique<entities::cursor>(
        sprite::sprite(cursor_texture, 
        assets_config::cursor_attributes[assets_config::attributes::frame_width],
        assets_config::cursor_attributes[assets_config::attributes::frame_height],
        assets_config::cursor_attributes[assets_config::attributes::frames],
        assets_config::cursor_attributes[assets_config::attributes::animations]),
        cursor_hitbox, 
        GetMousePosition(),
        id
    );
}
std::unique_ptr<entities::entity> entities::entity_builder::build_paw_mark(Vector2 position, int id){
    auto paw_texture = textures::textures_.get_texture(textures::paw_mark, assets_config::paw_mark_path); 
    auto paw_hitbox = hitbox::h_builder_.build_paw_mark_hitbox(position);
    return std::make_unique<entities::paw_mark>(
        sprite::sprite(paw_texture,
        assets_config::paw_mark_attributes[assets_config::attributes::frame_width],
        assets_config::paw_mark_attributes[assets_config::attributes::frame_height],
        assets_config::paw_mark_attributes[assets_config::attributes::frames],
        assets_config::paw_mark_attributes[assets_config::attributes::animations]),
        paw_hitbox,
        position,
        id
    );
}