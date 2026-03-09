#include "entities.h"

// -------------------------------- interaction strategies --------------------------------//
void entities::cursor::default_strategy::interact(cursor& cursor, entity& other){
    (void) cursor;
    return;
}
void entities::cursor::left_click_strategy::interact(cursor& cursor, entity& other){\
    cursor.state_->left_click(cursor, other);
}
void entities::cursor::right_click_strategy::interact(cursor& cursor, entity& other){
    cursor.state_->right_click(cursor, other); 
}

// ------------------------------- cursor states ---------------------------- //

void entities::cursor::state::create_move_event(cursor& cursor){
    // don't make an event
}
void entities::cursor::state::left_click(cursor& cursor, entity& other){
    // for "selecting a dog" and bringing up the hud
    // pending implementation 
    (void) cursor;
    (void) other;
}
void entities::cursor::state::right_click(cursor& cursor, entity& other){
    // for creating a paw mark, telling a dog where to go or what to start interacting with
    (void) cursor;
    (void) other;
    return;
}


void entities::cursor::carrying_decoration::create_move_event(cursor& cursor){
    // do make a move event
    std::unique_ptr<events::event> cursor_moved_event = std::make_unique<events::moved_cursor>(cursor.position_);
    event_interface::queue_event(cursor_moved_event);
}

void entities::cursor::editing::left_click(cursor& cursor, entity& other){
    // makes other subscribe to cursor move events
    if(decoration* decoration_cast = dynamic_cast<decoration*>(&other)){

        decoration_cast->pick_up();
        cursor.state_ = std::make_unique<carrying_decoration>(decoration_cast);
    }
    
    return;
}
void entities::cursor::editing::right_click(cursor& cursor, entity& other){
    // override, do nothing

}

void entities::cursor::carrying_decoration::left_click(cursor& cursor, entity& other){
    (void) other;
    if(decoration* decoration_cast = dynamic_cast<decoration*>(carried_decoration_)){

        bool can_place = decoration_cast->can_place_down(); 
        if(can_place){

            decoration_cast->place_down();
            cursor.state_ = std::make_unique<editing>();
        }
        else{
        }
    }
}

// -------------------------------- cursor --------------------------------//

int entities::cursor::update(float delta){
    auto old_position = position_;
    position_ =  GetMousePosition();

    if(! Vector2Equals(old_position, position_)) {
        hitboxes_[sprites_.index()].update(position_);
        create_move_event();

        // create the query and execute it

        // TODO change how this works to test your interaction theory  !
        std::unique_ptr<queries::query> colliding_query = std::make_unique<queries::is_colliding_query>(hitboxes_[sprites_.index()], id_);
        // the listener (singular) does something with the query and returns the information
        bool is_colliding = queries::bool_executor_.execute_query(*colliding_query);

        if(is_colliding){
            // switch to colliding 
            sprites_[sprites_.index()].get_animation().goto_animation(animation_tags::hover);

        }
        else{
            sprites_[sprites_.index()].get_animation().goto_animation(animation_tags::base);
            // switch to default anim
        }
    
        return status_codes::moved;
    }
    return status_codes::nothing;
}

void entities::cursor::create_move_event(){
    state_->create_move_event(*this);
}
void entities::cursor::interact(entities::entity& other){
    interaction_strategy_->interact(*this, other);
}


void entities::cursor::on_enter_edit_mode_event(const events::enter_edit_mode& event){
    // change the state of the cursor
    state_ = std::make_unique<editing>();
    return;
}
void entities::cursor::on_exit_edit_mode_event(const events::exit_edit_mode& event){
    // change the state of the cursor
    state_ = std::make_unique<state>();
    return;
}

void entities::cursor::on_left_mouse_click_event(const events::left_mouse_click& event){
    auto position = event.get_mouse_position();
    auto hitbox = event.get_hitbox();
    // check interactions within the quadtree, use the correct interaction stategy (the left click one)
    interaction_strategy_ = std::make_unique<left_click_strategy>();
    
    // ?  can I pass the event the function I want called ? no
    // ? the state overrides the
    
    // strategy is just state->left_click();
    std::unique_ptr<events::event> interaction_event = std::make_unique<events::interact_entity>(id_, hitboxes_[sprites_.index()]);
    event_interface::execute_event(*interaction_event);
    // then return to default interaciton
    interaction_strategy_ = std::make_unique<default_strategy>();
}
void entities::cursor::on_move_view_frame_event(const events::move_view_frame& event){
    // look, its left mouse down 
    auto mouse_delta = event.get_delta();
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
    auto& animation = sprites_[sprites_.index()].get_animation();
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
    auto cursor_texture = textures::textures_.get_texture(textures::cursor, entity_config::cursor_path);
    auto cursor_hitbox = hitbox::h_builder_.build_cursor_hitbox(position);
    // otherwise load it 
    auto cursor_sprite = sprite::sprite(cursor_texture, 
        entity_config::cursor_attributes[entity_config::attributes::frame_width],
        entity_config::cursor_attributes[entity_config::attributes::frame_height],
        entity_config::cursor_attributes[entity_config::attributes::frames],
        entity_config::cursor_attributes[entity_config::attributes::animations]);
    auto sprites = std::vector<sprite::sprite>{cursor_sprite};
    auto hitboxes = std::vector<hitbox::hitbox>{cursor_hitbox};
    return std::make_unique<entities::cursor>(
        sprites,
        hitboxes,
        GetMousePosition(),
        id
    );
}
std::unique_ptr<entities::entity> entities::entity_builder::build_paw_mark(Vector2 position, int id){
    auto paw_texture = textures::textures_.get_texture(textures::paw_mark, entity_config::paw_mark_path); 
    auto paw_hitbox = hitbox::h_builder_.build_paw_mark_hitbox(position);
    auto paw_sprite =         sprite::sprite(paw_texture,
        entity_config::paw_mark_attributes[entity_config::attributes::frame_width],
        entity_config::paw_mark_attributes[entity_config::attributes::frame_height],
        entity_config::paw_mark_attributes[entity_config::attributes::frames],
        entity_config::paw_mark_attributes[entity_config::attributes::animations]);


    auto sprites = std::vector<sprite::sprite>{paw_sprite};
    auto hitboxes = std::vector<hitbox::hitbox>{paw_hitbox};
    return std::make_unique<entities::paw_mark>(
        sprites, 
        hitboxes,
        position,
        id
    );
}