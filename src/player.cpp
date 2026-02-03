#include "player.h"
#include <iostream>

// ----------------------- player ----------------------- //



void player::player::setup_control_maps(){
    // ! figure out how to assign 
    key_press_controls_[controls_config::key_press_actions::dog_switch] = [this]() -> void {switch_dog();};
    key_press_controls_[controls_config::key_press_actions::shop_open] = [this]() -> void{open_shop();};
    key_press_controls_[controls_config::key_press_actions::inventory_open] = [this]() -> void {open_inventory();};
    key_press_controls_[controls_config::key_press_actions::menu_open] = [this]() -> void {open_menu();};
    key_press_controls_[controls_config::key_press_actions::quests_open] = [this]() -> void {open_quests();};
    key_press_controls_[controls_config::key_press_actions::map_open] = [this]() -> void {open_map();};
    key_press_controls_[controls_config::key_press_actions::back] = [this]() -> void {back();};

    key_hold_controls_[controls_config::key_hold_actions::edit_mode] = [this](float delta) -> void {edit(delta);};
    key_hold_controls_[controls_config::key_hold_actions::move_up] = [this](float delta) -> void {move(level_config::direction_scalars[level_config::directions::up], delta);};
    key_hold_controls_[controls_config::key_hold_actions::move_down] = [this](float delta) -> void {move(level_config::direction_scalars[level_config::directions::down], delta);};
    key_hold_controls_[controls_config::key_hold_actions::move_left] = [this](float delta) -> void {move(level_config::direction_scalars[level_config::directions::left], delta);};
    key_hold_controls_[controls_config::key_hold_actions::move_right] = [this](float delta) -> void {move(level_config::direction_scalars[level_config::directions::right], delta);};
}
void player::player::on_selected_dog(const events::selected_dog& event){
    auto id = event.get_id();
    selected_dog_ = id;
}

void player::player::back(){
    std::cout << "back " << std::endl;
    std::unique_ptr<events::event> key_press = std::make_unique<events::key_press>(controls_config::key_press_actions::back);
    event_interface::queue_event(key_press);

}

void player::player::edit(float delta){
    // ! needs to be bidirectional - i.e hold to go into edit and then hold again to go out of edit
    if(edit_meter_ < game_config::edit_mode_hold){
        edit_meter_ += (1 * delta);
        std::cout << "edit progress: " << edit_meter_ << std::endl;
    }
    
    if(edit_meter_ >= game_config::edit_mode_hold){
        // change the mode of the cursor
        edit_mode_ ^= 1;
        // create event
        std::unique_ptr<events::event> edit_switch_event = std::make_unique<events::edit_mode_switch>(edit_mode_);
        event_interface::queue_event(edit_switch_event);
        edit_meter_ = 0; // then reset the meter
    }
    else{
        if(! IsKeyDown(controls_config::key_hold_actions::edit_mode)){
            edit_meter_ = 0;
        }
    }
}
void player::player::move(Vector2 direction_scalar, float delta){
    
    auto move_vector = Vector2Scale(Vector2Multiply(level_config::frame_move, direction_scalar), delta);

    std::unique_ptr<events::event> move_view_frame_event = std::make_unique<events::move_view_frame>(move_vector);
    event_interface::queue_event(move_view_frame_event);
    return;
}

// TODO implement all 
void player::player::select_dog(){
    std::unique_ptr<events::event> select_dog_event = std::make_unique<events::selected_dog>(selected_dog_);
    event_interface::queue_event(select_dog_event);
}
void player::player::switch_dog(){
    selected_dog_ ^= 1; // flip the dog id 
    select_dog();
    return;
}
void player::player::open_inventory(){
    std::cout << "open inventory" << std::endl;
    std::unique_ptr<events::event> key_press = std::make_unique<events::key_press>(controls_config::key_press_actions::inventory_open);
    event_interface::queue_event(key_press);

    return;
}
void player::player::open_map(){
    std::cout << "open map" << std::endl;
    std::unique_ptr<events::event> key_press = std::make_unique<events::key_press>(controls_config::key_press_actions::map_open);
    event_interface::queue_event(key_press);
    return;
}
void player::player::open_menu(){
    std::cout << "open menu" << std::endl;
    std::unique_ptr<events::event> key_press = std::make_unique<events::key_press>(controls_config::key_press_actions::menu_open);
    event_interface::queue_event(key_press);
    return;
}
void player::player::open_quests(){
    std::cout << "open quest" << std::endl;
    std::unique_ptr<events::event> key_press = std::make_unique<events::key_press>(controls_config::key_press_actions::quests_open);
    event_interface::queue_event(key_press);
    return;
}
void player::player::open_shop(){
    std::cout << "open shop" << std::endl;
    std::unique_ptr<events::event> key_press = std::make_unique<events::key_press>(controls_config::key_press_actions::shop_open);
    event_interface::queue_event(key_press);
    return;
}

void player::player::update(float delta){

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


    // mouse inputs depend on the state of the player
    // check mouse inputs
    if(IsMouseButtonPressed(mouse_controls_[mouse::left_mouse])){
        // do event for check selct interaciotn

        std::unique_ptr<events::event> left_mouse_click_event = std::make_unique<events::left_mouse_click>(GetMousePosition(), 
        assets_config::cursor_attributes[assets_config::attributes::frame_width], assets_config::cursor_attributes[assets_config::attributes::frame_height]);
        event_interface::queue_event(left_mouse_click_event);


        // the left mouse click checks if the cursor interacts with anything
    }    
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