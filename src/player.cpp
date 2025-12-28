#include "player.h"
#include <iostream>

// ----------------------- player ----------------------- //

void player::player::on_selected_dog(const events::selected_dog& event){
    auto id = event.get_id();
    selected_dog_ = id;

    std::cout << "player selected dog " << selected_dog_ << std::endl;
}
void player::player::update(float delta){
    (void) delta;
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
    else if(IsMouseButtonDown(mouse_controls_[mouse::left_mouse])){
        // do event for dragging
        std::unique_ptr<events::event> left_mouse_down_event = std::make_unique<events::left_mouse_down>(GetMouseDelta());
        event_interface::queue_event(left_mouse_down_event);
    }
    
    else if(IsMouseButtonPressed(mouse_controls_[mouse::right_mouse])){
        // do event for creating visual thing
        std::unique_ptr<events::event> right_mouse_click_event = std::make_unique<events::right_mouse_click>(GetMousePosition());
        event_interface::queue_event(right_mouse_click_event);
        
    }
    else {
        // set default state
    }
    return;
}
void player::player::render(){
    return;
}