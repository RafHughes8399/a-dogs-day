#include "player.h"
#include <iostream>

// ----------------------- player ----------------------- //



void player::player::setup_control_maps(){
    // ! figure out how to assign 
    key_press_controls_[key_press_actions::dog_switch] = [this]() -> void {switch_dog();};
    key_press_controls_[key_press_actions::shop_open] = [this]() -> void{open_shop();};
    key_press_controls_[key_press_actions::inventory_open] = [this]() -> void {open_inventory();};
    key_press_controls_[key_press_actions::menu_open] = [this]() -> void {open_menu();};
    key_press_controls_[key_press_actions::quests_open] = [this]() -> void {open_quests();};
    key_press_controls_[key_press_actions::map_open] = [this]() -> void {open_map();};


    key_hold_controls_[key_hold_actions::move_up] = [this](float delta) -> void {move(level_config::direction_scalars[level_config::directions::up], delta);};
    key_hold_controls_[key_hold_actions::move_down] = [this](float delta) -> void {move(level_config::direction_scalars[level_config::directions::down], delta);};
    key_hold_controls_[key_hold_actions::move_left] = [this](float delta) -> void {move(level_config::direction_scalars[level_config::directions::left], delta);};
    key_hold_controls_[key_hold_actions::move_right] = [this](float delta) -> void {move(level_config::direction_scalars[level_config::directions::right], delta);};
}
void player::player::on_selected_dog(const events::selected_dog& event){
    auto id = event.get_id();
    selected_dog_ = id;

}

void player::player::move(Vector2 direction_scalar, float delta){
    
    auto move_vector = Vector2Scale(Vector2Multiply(level_config::frame_move, direction_scalar), delta);

    std::unique_ptr<events::event> move_view_frame_event = std::make_unique<events::move_view_frame>(move_vector);
    event_interface::queue_event(move_view_frame_event);
    return;
}

// TODO implement all 
void player::player::switch_dog(){
    return;
}
void player::player::open_inventory(){
    return;
}
void player::player::open_map(){
    return;
}
void player::player::open_menu(){
    return;
}
void player::player::open_quests(){
    return;
}
void player::player::open_shop(){
    return;
}

void player::player::update(float delta){
    

    // ! working on new control scheme
    /**
     *   ! click and drag is gone, now move screen through arrow keys
     *  ! right click for path selection stays 
     * ! click to select dog is gone, but left click interactions should still stay 
     *
     *   
    */ 

    // check pressed keys 
    for(auto & key : key_press_controls_){
        if(IsKeyPressed(key.first)) {
            auto action = key.second;
            action();
        }
    }

    // check held down keys
    for(auto & key : key_hold_controls_){
        if(IsKeyDown(key.first)){
            auto action = key.second;
            action(delta);
        }
    }

    // check mouse inputs

    // check controls that have been pressed and create the approproiate events
    if(IsMouseButtonPressed(mouse_controls_[mouse::left_mouse])){
        // do event for check selct interaciotn

        // what would a click event need  ?
        // the position of the click
        // what information would the level need
            // the position of the cursor for sure
            // and its hitbox, do i have that information ? , i think so 
        std::unique_ptr<events::event> left_mouse_click_event = std::make_unique<events::left_mouse_click>(GetMousePosition(), 
        assets_config::cursor_attributes[assets_config::attributes::frame_width], assets_config::cursor_attributes[assets_config::attributes::frame_height]);
        event_interface::queue_event(left_mouse_click_event);


        // the left mouse click checks if the cursor interacts with anything
    }

    /*     
    else if(IsMouseButtonDown(mouse_controls_[mouse::left_mouse])){
        // do event for dragging
        std::unique_ptr<events::event> move_view_frame_event = std::make_unique<events::move_view_frame>(GetMouseDelta());
        event_interface::queue_event(move_view_frame_event);
    } 
    */
    
    else if(IsMouseButtonPressed(mouse_controls_[mouse::right_mouse])){
        // do event for creating visual thing
        std::unique_ptr<events::event> right_mouse_click_event = std::make_unique<events::right_mouse_click>(GetMousePosition(), selected_dog_);
        event_interface::queue_event(right_mouse_click_event);

        // pass in the dog id
        
    }
    else {
        // set default state
    }
    return;
}
void player::player::render(){
    return;
}