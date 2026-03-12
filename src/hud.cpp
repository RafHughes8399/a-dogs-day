#include "hud.h"
#include "config.h"
#include "raylib.h"



// ----------------------------------- builder ---------------------------- // 
hud::hud_builder hud::h_builder_;

hud::hud hud::hud_builder::build_player_hud(){
    hud player_hud = hud();
    player_hud.add_element(std::move(h_builder_.build_edit_wheel()));
    player_hud.add_element(std::move(h_builder_.build_decoration_grid()));
    return player_hud;
}
std::unique_ptr<hud::hud_element> hud::hud_builder::build_edit_wheel(){
    auto texture = textures::textures_.get_texture(textures::texture_keys::hud_edit_wheel, hud_config::cursor_edit_progres_wheel);
    auto sprite = sprite::sprite(
        texture,
        hud_config::edit_wheel_attributes[entity_config::attributes::frame_width],            
        hud_config::edit_wheel_attributes[entity_config::attributes::frame_height],            
        hud_config::edit_wheel_attributes[entity_config::attributes::frames],            
        hud_config::edit_wheel_attributes[entity_config::attributes::animations]        
    );
    auto cursor_position = GetMousePosition();

    auto draw_strat = std::make_unique<hud_element::sprite_draw>(sprite, cursor_position);
    auto event_strat = std::make_unique<hud_element::edit_wheel_strategy>(draw_strat->get_sprite(), draw_strat->get_position_ptr());


    return std::make_unique<hud_element>(Rectangle{cursor_position.x, cursor_position.y, hud_config::edit_wheel_attributes[entity_config::attributes::frame_width],            
        hud_config::edit_wheel_attributes[entity_config::attributes::frame_height]}, std::move(draw_strat), std::move(event_strat));       

}

std::unique_ptr<hud::hud_element> hud::hud_builder::build_decoration_grid(){
    auto draw_strategy = std::make_unique<hud_element::grid_draw>();
    auto empty_handler_strategy = std::make_unique<hud_element::empty_handler_strategy>();
    return std::make_unique<hud_element>(Rectangle {0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())}, std::move(draw_strategy), std::move(empty_handler_strategy));
}

// ---------------------------------- hud element ----------------------------- //
Vector2 hud::hud_element::get_position(){
    return draw_strategy_->get_position();
}
void hud::hud_element::set_position(Vector2 position){
    draw_strategy_->set_position(position);
}
// ------------------------------- draw strategies ------------------------------- //
void hud::hud_element::sprite_draw::draw(){
    std::cout << "draw hud spirte " << std::endl;
    sprite_.render(position_, 0);
}    
void hud::hud_element::rectangle_draw::draw(){
    DrawRectangle(position_.x, position_.y, rectangle_.width, rectangle_.height, colour_);
}
// position is assumed to be Vector2Zero(), the grid will cover the whole screen (for now)
void hud::hud_element::grid_draw::draw(){
    std::cout << "draw grid " << std::endl;
    float screen_w = static_cast<float>(GetScreenWidth());
    float screen_h = static_cast<float>(GetScreenHeight());
    for(int x = position_.x; x <= screen_w; x += level_config::edge_weight){
        DrawLineEx({(float)x, position_.y}, {(float)x, screen_h}, hud_config::decoration_grid_thickness, hud_config::decoration_grid_highlight);
    }
    for(int y = position_.y; y <= screen_h; y += level_config::edge_weight){
        DrawLineEx({position_.x, (float)y}, {screen_w, (float)y}, hud_config::decoration_grid_thickness, hud_config::decoration_grid_highlight);
    }
}
// ------------------------------- draw strategies ------------------------------- //


void hud::hud_element::draw(){
    draw_strategy_->draw();
}   
// ---------------------------------- button --------------------------------- //
/* void hud::button::on_menu_interact(const events::interact_menu& event){
    const hitbox::hitbox& hitbox = event.get_hitbox();
    
    // does the current button interact with the event hitbox
    if(CheckCollisionRecs(outline_, hitbox.get_box())){
        press_button();
    } 
}
 */
void hud::button::press_button(){
    // placeholder pending actual button press functionality 
}
void hud::button::subscribe(){
   // event_interface::subscribe(menu_interact_handler_);
}
void hud::button::unsubscribe(){
    //event_interface::unsubscribe(menu_interact_handler_);

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

void hud::hud::add_element(std::unique_ptr<hud_element> element){
    elements_.push_back(std::move(element));
}

void hud::hud::render(){
    for(const std::unique_ptr<hud_element> & element : elements_){

        element->draw();
    }
}