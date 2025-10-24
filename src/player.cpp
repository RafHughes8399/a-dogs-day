#include "player.h"


// ----------------------- player ----------------------- //
void player::player::update(float delta){
    (void) delta;
    // check controls that have been pressed and create the approproiate events
    if(IsMouseButtonDown(mouse_controls_[mouse::left_mouse])){
        // do event for dragging
        std::unique_ptr<events::event> left_mouse_down_event = std::make_unique<events::left_mouse_down>(GetMouseDelta());
        event_interface::queue_event(left_mouse_down_event);
    }
    else if(IsMouseButtonPressed(mouse_controls_[mouse::right_mouse])){
        // do event for creating visual thing
        std::unique_ptr<events::event> right_mouse_click_event = std::make_unique<events::right_mouse_click>(GetMousePosition());
        event_interface::queue_event(right_mouse_click_event);
        
    }
    return;
}
void player::player::render(){
    cursor_.render(GetMousePosition());
    return;
}