#include "hud.h"
#include "config.h"
#include "raylib.h"
#include <memory>
#include <iostream>


// ----------------------------------- builder ---------------------------- // 
hud::hud_builder hud::h_builder_;

hud::hud hud::hud_builder::build_player_hud(){
    std::cout << "[build player hud] : declare hud " << std::endl;
    hud player_hud = hud();
    // build the base hud 
    player_hud.add_element(h_builder_.build_edit_wheel(), hud::hud_types::base);
    std::cout << "[build player hud] : build edit wheel " << std::endl;
    
    // build the editing hud
    player_hud.add_element(h_builder_.build_decoration_grid(), hud::hud_types::editing); // the deco grid
    std::cout << "[build player hud] : build decoration grid " << std::endl;
    // and the hover rectangle, maybe just one and change the colour, yeah that's better than two
    // initally build the rectangle as a {0,0,0,0}, rectangle and white, then have it listen to a pickup event to resize the rectangle 
    // and a deco move event to update the colour 
    player_hud.add_element(h_builder_.build_decoration_overlay(), hud::hud_types::carrying);
    std::cout << "[build player hud] : build decoration overlay " << std::endl;
    player_hud.subscribe_edit_mode_events();
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

std::unique_ptr<hud::hud_element> hud::hud_builder::build_decoration_overlay(){
    auto draw_strategy = std::make_unique<hud_element::rectangle_draw>(Rectangle{0,0,0,0}, WHITE);
    // for now empty but should respond to a move event
    auto empty_handler_strategy = std::make_unique<hud_element::empty_handler_strategy>();
    return std::make_unique<hud_element>(Rectangle{0,0,0,0}, std::move(draw_strategy), std::move(empty_handler_strategy));
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

    sprite_.render(position_, 0);
}    
void hud::hud_element::rectangle_draw::draw(){
    DrawRectangle(static_cast<int>(position_.x), static_cast<int>(position_.y),
        static_cast<int>(rectangle_.width), static_cast<int>(rectangle_.height), colour_);
}
// position is assumed to be Vector2Zero(), the grid will cover the whole screen (for now)
void hud::hud_element::grid_draw::draw(){

    float screen_w = static_cast<float>(GetScreenWidth());
    float screen_h = static_cast<float>(GetScreenHeight());
    for(int x = static_cast<int>(position_.x); x <= static_cast<int>(screen_w); x += static_cast<int>(level_config::edge_weight)){
        DrawLineEx({static_cast<float>(x), position_.y}, {static_cast<float>(x), screen_h}, hud_config::decoration_grid_thickness, hud_config::decoration_grid_highlight);
    }
    for(int y = static_cast<int>(position_.y); y <= static_cast<int>(screen_h); y += static_cast<int>(level_config::edge_weight)){
        DrawLineEx({position_.x, static_cast<float>(y)}, {screen_w, static_cast<float>(y)}, hud_config::decoration_grid_thickness, hud_config::decoration_grid_highlight);
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

void hud::hud::subscribe_edit_mode_events(){
    enter_edit_mode_handler_ = std::make_unique<events::event_handler<events::enter_edit_mode>>(
        [this](const events::enter_edit_mode& event) -> void {on_enter_edit_mode(event);});
    exit_edit_mode_handler_ = std::make_unique<events::event_handler<events::exit_edit_mode>>(
        [this](const events::exit_edit_mode& event) -> void {on_exit_edit_mode(event);});
    event_interface::subscribe<events::enter_edit_mode>(*enter_edit_mode_handler_);
    event_interface::subscribe<events::exit_edit_mode>(*exit_edit_mode_handler_);
    std::cout << "[hud] : edit-mode event subscribes (player hud)" << std::endl;
}

hud::hud::hud(hud&& other) noexcept(false)
    : index_(other.index_), elements_(std::move(other.elements_)),
      enter_edit_mode_handler_(nullptr), exit_edit_mode_handler_(nullptr){
    other.index_ = hud_types::base;
    const bool other_had_edit = (other.enter_edit_mode_handler_ != nullptr);
    if(other_had_edit){
        event_interface::unsubscribe<events::enter_edit_mode>(*other.enter_edit_mode_handler_);
        event_interface::unsubscribe<events::exit_edit_mode>(*other.exit_edit_mode_handler_);
        other.enter_edit_mode_handler_.reset();
        other.exit_edit_mode_handler_.reset();
        subscribe_edit_mode_events();
    }
}

hud::hud& hud::hud::operator=(hud&& other) noexcept(false){
    if(this == &other){
        return *this;
    }
    if(enter_edit_mode_handler_){
        event_interface::unsubscribe<events::enter_edit_mode>(*enter_edit_mode_handler_);
        event_interface::unsubscribe<events::exit_edit_mode>(*exit_edit_mode_handler_);
    }
    enter_edit_mode_handler_.reset();
    exit_edit_mode_handler_.reset();

    const bool other_had_edit = (other.enter_edit_mode_handler_ != nullptr);
    if(other_had_edit){
        event_interface::unsubscribe<events::enter_edit_mode>(*other.enter_edit_mode_handler_);
        event_interface::unsubscribe<events::exit_edit_mode>(*other.exit_edit_mode_handler_);
        other.enter_edit_mode_handler_.reset();
        other.exit_edit_mode_handler_.reset();
    }

    index_ = other.index_;
    elements_ = std::move(other.elements_);
    other.index_ = hud_types::base;

    if(other_had_edit){
        subscribe_edit_mode_events();
    }
    return *this;
}

void hud::hud::on_enter_edit_mode(const events::enter_edit_mode& event){
    (void) event;
    index_ = hud_types::editing;
}
void hud::hud::on_exit_edit_mode(const events::exit_edit_mode& event){
    (void) event;
    index_ = hud_types::base;
}

void hud::hud::buttons_subscribe(){
    for(auto & component : elements_[index_]){
        button* button_cast = dynamic_cast<button*>(component.get());
        if(button_cast){
            button_cast->subscribe();
        }
    }
}
void hud::hud::buttons_unsubscribe(){
    for(auto & component : elements_[index_]){
        button* button_cast = dynamic_cast<button*>(component.get());
        if(button_cast){
            button_cast->unsubscribe();
        }
    }
}

void hud::hud::add_element(std::unique_ptr<hud_element> element, size_t hud){
    elements_[hud].push_back(std::move(element));
}

void hud::hud::render(){
    for(const std::unique_ptr<hud_element> & element : elements_[index_]){
        element->draw();
    }
}
std::vector<std::unique_ptr<hud::hud_element>>& hud::hud::get_hud(){
    return elements_[index_];
}
void hud::hud::pick_hud(size_t index){
    index_ = index;
}
