#include "hud.h"



// ----------------------------------- builder ---------------------------- // 
hud::hud_builder hud::h_builder_;


// ---------------------------------- hud element ----------------------------- //
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