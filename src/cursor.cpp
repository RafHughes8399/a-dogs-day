#include "entities.h"

// -------------------------------- interaction states --------------------------------//
void entities::cursor::left_click_state::interact(cursor& cursor, entity& other){
    std::cout << "left click interaction " << std::endl;
}
void entities::cursor::right_click_state::interact(cursor& cursor, entity& other){
    std::cout << "right click interaction " << std::endl;
    
}
void entities::cursor::default_state::interact(cursor& cursor, entity& other){
    std::cout << "default click interaction " << std::endl;
    
}
// -------------------------------- cursor --------------------------------//

int entities::cursor::update(float delta){
    auto old_position = position_;
    position_ =  GetMousePosition();

    if(! Vector2Equals(old_position, position_)) {
        hitbox_.update(position_);

        // create the query and execute it
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
    // down cast to a dog because that's all we have at the moment will, figure something 
    // else out 
    
    /* lol that's not how you cast safely */
    //player_dog& dog_cast = dynamic_cast<player_dog&>(other);

    interaction_state_->interact(*this, other);
    return;
}
void entities::cursor::on_left_mouse_click_event(const events::left_mouse_click& event){
    auto position = event.get_mouse_position();
    auto hitbox = event.get_hitbox();

    // find the entity being collided with 

    
    std::cout << "clicked left mouse " << std::endl;
    // query the quadtree


    // ! consider what happens if colliding with multiple, how do you prioritise  ? 
    std::unique_ptr<queries::query> entity_collision = std::make_unique<queries::collision_query>(hitbox_, id_);
    int colliding_entity_id = query_interface::execute_query(queries::int_executor_, *entity_collision);

    // because at some point the interact function must be called, that is how interaction behaviour occurs, that
    // occcurs through the quadtree, the quad tree is how you access the actual entity rather than just the id


    
}
void entities::cursor::on_left_mouse_down_event(const events::left_mouse_down& event){
    // handles dragging
    // two cases, dragging occurs only if the mouse is moving after 
    // ! slow your roll cowboy, you're looking in the wrong spot, this is already handleed 

    // look, its left mouse down 
    auto mouse_delta = event.get_mouse_delta();
    auto new_position = Vector2Add(position_, mouse_delta);
    position_ = new_position;
    // create a moved entities event or something and debug 
    std::unique_ptr<events::event> moved_event = std::make_unique<events::move_entity>(id_);

    // current thought is that, the move in the tre does not happen until next frame, 
    // ? that could cause issues ? maybe ? 
    event_interface::queue_event(moved_event); 

    // check for collisions 
}

void entities::cursor::on_right_mouse_click_event(const events::right_mouse_click& event){
    // for now just change the interact state
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