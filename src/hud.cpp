#include "hud.h"



// ----------------------------------- builder ---------------------------- // 
hud::hud_builder hud::h_builder_;


// ---------------------------------- hud element ----------------------------- //
// ------------------------------- draw strategies ------------------------------- //
void hud::hud_element::sprite_draw::draw(Vector2 position){
    sprite_.render(position);
}    
void hud::hud_element::rectangle_draw::draw(Vector2 position){
    DrawRectangle(rectangle_.x, rectangle_.y, rectangle_.width, rectangle_.height, colour_);
}
// position is assumed to be Vector2Zero(), the grid will cover the whole screen (for now)
void hud::hud_element::grid_draw::draw(Vector2 position){
    // x is column, y is row
    for(int x = position.x; x <= level_config::screen_width; x += level_config::edge_weight){
    }
    for(int y = position.y; y <= level_config::screen_height; y += level_config::edge_weight){
    }
}
// ------------------------------- draw strategies ------------------------------- //


void hud::hud_element::draw(){
    draw_strategy_->draw(position_);
}   
// ---------------------------------- button --------------------------------- //
void hud::button::on_menu_interact(const events::interact_menu& event){
    const hitbox::hitbox& hitbox = event.get_hitbox();
    
    // does the current button interact with the event hitbox
    if(CheckCollisionRecs(outline_, hitbox.get_box())){
        press_button();
    } 
}

void hud::button::press_button(){
    // placeholder pending actual button press functionality 
}
void hud::button::subscribe(){
    event_interface::subscribe(menu_interact_handler_);
}
void hud::button::unsubscribe(){
    event_interface::unsubscribe(menu_interact_handler_);

}

// ---------------------------------- hud  ---------------------------------- //

void hud::hud::buttons_subscribe(){
    for(auto & component : elements_){
        button* button_cast = dynamic_cast<button*>(component.get());
        if(button_cast){
            button_cast->subscribe();
        }
    }
}
void hud::hud::buttons_unsubscribe(){
    for(auto & component : elements_){
        button* button_cast = dynamic_cast<button*>(component.get());
        if(button_cast){
            button_cast->unsubscribe();
        }
    }
}