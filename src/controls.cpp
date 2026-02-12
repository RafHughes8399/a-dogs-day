#include "player.h"

// --------------------------------------- builds -------------------------------------
void player::controls::build_controls(){
    build_default_controls_state();
    build_editing_controls_state();
}

void player::controls::build_default_controls_state(){
    
    std::map<int, std::function<void()>> mouse_controls = {};
    std::map<int, std::function<void()>> press_controls = {};
    std::map<int, std::function<void(float)>> hold_controls = {};
    
    mouse_controls[mouse::left_mouse] = [this] () -> void {player_->left_click();};
    mouse_controls[mouse::right_mouse] = [this] () -> void {player_->right_click();};
    
    press_controls[controls_config::key_press_actions::dog_switch] = [this]() -> void {player_->switch_dog();};
    press_controls[controls_config::key_press_actions::shop_open] = [this]() -> void{player_->open_shop();};
    press_controls[controls_config::key_press_actions::inventory_open] = [this]() -> void {player_->open_inventory();};
    press_controls[controls_config::key_press_actions::menu_open] = [this]() -> void {player_->open_menu();};
    press_controls[controls_config::key_press_actions::quests_open] = [this]() -> void {player_->open_quests();};
    press_controls[controls_config::key_press_actions::map_open] = [this]() -> void {player_->open_map();};
    press_controls[controls_config::key_press_actions::back] = [this]() -> void {player_->back();};
    
    hold_controls[controls_config::key_hold_actions::edit_mode] = [this](float delta) -> void {player_->edit(delta);};
    hold_controls[controls_config::key_hold_actions::move_up] = [this](float delta) -> void {player_->move(level_config::direction_scalars[level_config::directions::up], delta);};
    hold_controls[controls_config::key_hold_actions::move_down] = [this](float delta) -> void {player_->move(level_config::direction_scalars[level_config::directions::down], delta);};
    hold_controls[controls_config::key_hold_actions::move_left] = [this](float delta) -> void {player_->move(level_config::direction_scalars[level_config::directions::left], delta);};
    hold_controls[controls_config::key_hold_actions::move_right] = [this](float delta) -> void {player_->move(level_config::direction_scalars[level_config::directions::right], delta);};
    
    
    key_press_controls_.push_back(press_controls);
    key_hold_controls_.push_back(hold_controls);
    mouse_controls_.push_back(mouse_controls);
}
void player::controls::build_editing_controls_state(){
    std::map<int, std::function<void()>> mouse_controls = {};
    std::map<int, std::function<void()>> press_controls = {};
    std::map<int, std::function<void(float)>> hold_controls = {};

    mouse_controls[mouse::left_mouse] = [this] () -> void {player_->left_click();};
    mouse_controls[mouse::right_mouse] = [this] () -> void {player_->right_click();};

    press_controls[controls_config::key_press_actions::dog_switch] = [this]() -> void {player_->switch_dog();};
    press_controls[controls_config::key_press_actions::shop_open] = [this]() -> void{player_->open_shop();};
    press_controls[controls_config::key_press_actions::inventory_open] = [this]() -> void {player_->open_inventory();};
    press_controls[controls_config::key_press_actions::menu_open] = [this]() -> void {player_->open_menu();};
    press_controls[controls_config::key_press_actions::quests_open] = [this]() -> void {player_->open_quests();};
    press_controls[controls_config::key_press_actions::map_open] = [this]() -> void {player_->open_map();};
    press_controls[controls_config::key_press_actions::back] = [this]() -> void {player_->back();};
    press_controls[controls_config::key_press_actions::exit_edit] = [this]() -> void {player_->exit_edit();};

    // hold_controls[controls_config::key_hold_actions::edit_mode] = [this](float delta) -> void {player_->edit(delta);};
    hold_controls[controls_config::key_hold_actions::move_up] = [this](float delta) -> void {player_->move(level_config::direction_scalars[level_config::directions::up], delta);};
    hold_controls[controls_config::key_hold_actions::move_down] = [this](float delta) -> void {player_->move(level_config::direction_scalars[level_config::directions::down], delta);};
    hold_controls[controls_config::key_hold_actions::move_left] = [this](float delta) -> void {player_->move(level_config::direction_scalars[level_config::directions::left], delta);};
    hold_controls[controls_config::key_hold_actions::move_right] = [this](float delta) -> void {player_->move(level_config::direction_scalars[level_config::directions::right], delta);};

    key_press_controls_.push_back(press_controls);
    key_hold_controls_.push_back(hold_controls);
    mouse_controls_.push_back(mouse_controls);
}

// -------------------------------- controls ------------------------------
void player::controls::check(float delta){
    // check the controls for 

    for(auto & key : key_press_controls_[current_scheme_]){
        if(IsKeyPressed(key.first)) {
            auto action = key.second;
            action();
        }
    }

    // check held down keys
    for(auto & key : key_hold_controls_[current_scheme_]){
        if(IsKeyDown(key.first)){
            auto action = key.second;
            action(delta);
        }
    }


    // check mouse inputs
    for(auto & mouse : mouse_controls_[current_scheme_]){
        if(IsMouseButtonPressed(mouse.first)){
            auto action = mouse.second;
            action();
        }
    }
}

void player::controls::on_enter_edit_mode(const events::enter_edit_mode& event){
    current_scheme_ = control_states::editing;
}
void player::controls::on_exit_edit_mode(const events::exit_edit_mode& event){
    current_scheme_ = control_states::regular;

}