#include "component.h"
#include "system.h"
#include <raylib.h>
#include <raymath.h>

// TODO stub - the loop calls this every frame, nothing to do yet

// ---------------- input dispatch ----------------
void systems::control_input_system::check_inputs(size_t id, std::vector<game_config::input>& controls, float delta){
    for(auto it = controls.begin(); it != controls.end(); ++it){
        auto input = (*it).key_;
        auto action = (*it).action_;
        switch (action) {
            case game_config::key_press:
                if(IsKeyPressed(input)){
                    // perfform the control that the manager has at the key's index
                    dispatch(input, action, id, delta);
                }
                break;
            case game_config::key_hold:
                if(IsKeyDown(input)){
                    dispatch(input, action, id, delta);
                }
                break;
            case game_config::mouse_press:
                if(IsMouseButtonPressed(input)){
                    dispatch(input, action, id, delta);
                }
            break;
            case game_config::mouse_hold:
                if(IsMouseButtonDown(input)){
                    dispatch(input, action, id, delta);
                }
                break;
            case game_config::mouse_released:
                if(IsMouseButtonReleased(input)){
                    dispatch(input, action, id, delta);
                }
                break;
            case game_config::mouse_up:
                if(IsMouseButtonUp(input)){
                    dispatch(input, action, id, delta);
                }
                break;
        }
    }
}

// an entity can hold a binding this system has no action for - that is a scheme
// that has not been built yet, not an error
void systems::control_input_system::dispatch(int key, int action, size_t id, float delta){
    auto control = control_function_map_.find({key, action});
    if(control == control_function_map_.end()){ return; }
    control->second(id, delta);
}

void systems::control_input_system::update(float delta){
    // * check key components
    for(auto it = component_managers::control_manager_.begin(); it != component_managers::control_manager_.end(); ++it){
        size_t id = static_cast<size_t>(*&it->first);
        auto inputs = it->second.get_inputs();
        check_inputs(id, inputs, delta);
    }
    // * check mouse components

    // * and check mouse delta, this retursn an int, figure out what that int means
    // * 1 is true, 0 is false
    for(auto it = component_managers::mouse_input_manager_.begin(); it != component_managers::mouse_input_manager_.end(); ++it){
        size_t id = static_cast<size_t>(*&it->first);
        auto inputs = it->second.get_inputs();
        if(not Vector2Equals(GetMouseDelta(), Vector2Zero())){
            auto current_position = component_managers::positional_manager_.get_component(id)->get_position();
            systems::movement_system::get_instance().update_position(id, Vector2Add(current_position, GetMouseDelta()));
        }
        // * check for mouse input also
        check_inputs(id, inputs, delta);
    }
}

// ------------------------------- control map ------------------------------- //

// ---------------- control map ----------------
// * the ECS twin of player::controls::build_default_controls_state - same
// * bindings, same calls. it is one map rather than the three parallel ones the
// * player keeps because the {key_, action_} key already says which of press /
// * hold / mouse a binding is.
// * TODO no editing scheme yet. the player swapped schemes by indexing a second
// * map; here which bindings an entity has is the entity's own data, so a scheme
// * swap means swapping the component's bindings. edit_mode / exit_edit stay
// * unbound until that is settled.
void systems::control_input_system::build_control_map(){
    control_function_map_[{controls_config::key_press_actions::dog_switch, game_config::key_press}] =
        [this](size_t id, float delta) -> void {(void) id; (void) delta; switch_dog();};
    control_function_map_[{controls_config::key_press_actions::shop_open, game_config::key_press}] =
        [this](size_t id, float delta) -> void {(void) id; (void) delta; open_shop();};
    control_function_map_[{controls_config::key_press_actions::inventory_open, game_config::key_press}] =
        [this](size_t id, float delta) -> void {(void) id; (void) delta; open_inventory();};
    control_function_map_[{controls_config::key_press_actions::menu_open, game_config::key_press}] =
        [this](size_t id, float delta) -> void {(void) id; (void) delta; open_menu();};
    control_function_map_[{controls_config::key_press_actions::quests_open, game_config::key_press}] =
        [this](size_t id, float delta) -> void {(void) id; (void) delta; open_quests();};
    control_function_map_[{controls_config::key_press_actions::map_open, game_config::key_press}] =
        [this](size_t id, float delta) -> void {(void) id; (void) delta; open_map();};
    control_function_map_[{controls_config::key_press_actions::back, game_config::key_press}] =
        [this](size_t id, float delta) -> void {(void) id; (void) delta; back();};

    // the four game_config::player_controls binds
    control_function_map_[{controls_config::key_hold_actions::move_up, game_config::key_hold}] =
        [this](size_t id, float delta) -> void {(void) id;
            move_view_frame(level_config::direction_scalars[level_config::directions::up], delta);};
    control_function_map_[{controls_config::key_hold_actions::move_down, game_config::key_hold}] =
        [this](size_t id, float delta) -> void {(void) id;
            move_view_frame(level_config::direction_scalars[level_config::directions::down], delta);};
    control_function_map_[{controls_config::key_hold_actions::move_left, game_config::key_hold}] =
        [this](size_t id, float delta) -> void {(void) id;
            move_view_frame(level_config::direction_scalars[level_config::directions::left], delta);};
    control_function_map_[{controls_config::key_hold_actions::move_right, game_config::key_hold}] =
        [this](size_t id, float delta) -> void {(void) id;
            move_view_frame(level_config::direction_scalars[level_config::directions::right], delta);};

    // the two game_config::cursor_controls binds
    control_function_map_[{MOUSE_BUTTON_LEFT, game_config::mouse_press}] =
        [this](size_t id, float delta) -> void {(void) id; (void) delta; left_click();};
    control_function_map_[{MOUSE_BUTTON_RIGHT, game_config::mouse_press}] =
        [this](size_t id, float delta) -> void {(void) id; (void) delta; right_click();};
}

// ------------------------------- the actions ------------------------------- //

// ---------------- actions ----------------
// the menu-opening actions differ only by which action they announce
void systems::control_input_system::queue_key_press(int key){
    std::unique_ptr<events::event> key_press = std::make_unique<events::key_press>(key);
    event_interface::queue_event(key_press);
}

void systems::control_input_system::back(){
    queue_key_press(controls_config::key_press_actions::back);
}
void systems::control_input_system::open_inventory(){
    queue_key_press(controls_config::key_press_actions::inventory_open);
}
void systems::control_input_system::open_map(){
    queue_key_press(controls_config::key_press_actions::map_open);
}
void systems::control_input_system::open_menu(){
    queue_key_press(controls_config::key_press_actions::menu_open);
}
void systems::control_input_system::open_quests(){
    queue_key_press(controls_config::key_press_actions::quests_open);
}
void systems::control_input_system::open_shop(){
    queue_key_press(controls_config::key_press_actions::shop_open);
}

void systems::control_input_system::select_dog(){
    std::unique_ptr<events::event> select_dog_event = std::make_unique<events::selected_dog>(selected_dog_);
    event_interface::queue_event(select_dog_event);
}
void systems::control_input_system::switch_dog(){
    selected_dog_ ^= 1; // flip the dog id
    select_dog();
}

void systems::control_input_system::move_view_frame(Vector2 direction_scalar, float delta){
    auto move_vector = Vector2Scale(Vector2Multiply(level_config::frame_move, direction_scalar), delta);
    std::unique_ptr<events::event> move_view_frame_event = std::make_unique<events::move_view_frame>(move_vector);
    event_interface::queue_event(move_view_frame_event);
}

void systems::control_input_system::left_click(){
    std::unique_ptr<events::event> left_mouse_click_event = std::make_unique<events::left_mouse_click>(GetMousePosition(),
    entity_config::cursor_attributes[entity_config::attributes::frame_width],
    entity_config::cursor_attributes[entity_config::attributes::frame_height]);
    event_interface::queue_event(left_mouse_click_event);
}
void systems::control_input_system::right_click(){
    std::unique_ptr<events::event> right_mouse_click_event = std::make_unique<events::right_mouse_click>(GetMousePosition(),
    static_cast<int>(selected_dog_));
    event_interface::queue_event(right_mouse_click_event);
}

