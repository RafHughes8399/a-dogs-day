#include "component.h"
#include "config.h"
#include "debug_log_interface.h"
#include "debug_logger.h"
#include "raglib.h"
#include "system.h"
#include "system_events.h"
#include <cstddef>
#include <optional>
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
    control_function_map_[{controls_config::key_press_actions::debug_toggle, game_config::key_press}] =
        [this](size_t id, float delta) -> void {(void) id; (void) delta; toggle_debug_logger();};

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
        [this](size_t id, float delta) -> void {(void) delta; left_click(id);};
    control_function_map_[{MOUSE_BUTTON_RIGHT, game_config::mouse_press}] =
        [this](size_t id, float delta) -> void {(void) delta; right_click(id);};
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
void systems::control_input_system::toggle_debug_logger(){
    debug::logger::get_instance().toggle();
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
    rendering_system::get_instance().move_frame(move_vector);
}


void systems::control_input_system::left_click(size_t id){
    auto click_position = GetMousePosition();
    debug::log("[control_input_system::left_click, clicked] asked by: "
        + std::to_string(id)
        + ", position: " + raglib::vector_to_string(click_position));

    int entity_id = spatial_system::get_instance().check_collision_with(id, click_position);
    if(entity_id == game_config::empty_entity){
        debug::log("[control_input_system::left_click, hit nothing] deselecting, was: "
            + std::to_string(selection_system::get_instance().selected()));
        selection_system::get_instance().deselect();
        return;
    }
    else{
        debug::log("[control_input_system::left_click, hit entity] selecting: "
            + std::to_string(entity_id));
        selection_system::get_instance().select(static_cast<size_t>(entity_id));
        debug::log("[control_input_system::left_click, selection settled] selected: "
            + std::to_string(selection_system::get_instance().selected()));
    }

}
// * right click tells the selected player dog entity where to go 
// ? maybe extendable to waiters too ? if i wanted to change how the waiter interaction stuff goes
void systems::control_input_system::right_click(size_t id){
    auto click_position = GetMousePosition();
    debug::log("[control_input_system::right_click, clicked] asked by: "
        + std::to_string(id)
        + ", position: " + raglib::vector_to_string(click_position));

    int selected = selection_system::get_instance().selected();
    if(selected == game_config::empty_entity){
        debug::log("[control_input_system::right_click, nothing selected] no path requested");
        return;
    }
    auto selected_id = static_cast<size_t>(selected);
    auto* selectable = component_managers::selectable_manager_.get_component(selected_id);
    if(selectable->get_kind() != entity_config::selectable_kinds::player_dog_kind){
        debug::log("[control_input_system::right_click, selection is not a player dog] id: "
            + std::to_string(selected_id)
            + ", kind: " + std::to_string(selectable->get_kind()));
        return;
    }

    int entity_id = spatial_system::get_instance().check_collision_with(id, click_position);
    // ! i think there is something wrong with this check here 
    // ! the path to a destination entity is not correct.
    // ! the click resovles the detination entity correctly and the position but something lese 
    // ! is missing
    
    // ! no the problem is that it cannot resolve a path to an occupied node. so need to adjust
    // ! the position that we send the dog to when going to a entity
    // ? perhaps need to revive the idea of interaction positions, based on the hitbox ? 
    std::optional<size_t> destination_entity = entity_id == game_config::empty_entity
        ? std::optional<size_t>{}
        : std::make_optional<size_t>(entity_id);
    debug::log("[control_input_system::right_click, resolved destination] entity: "
        + (destination_entity.has_value() ? std::to_string(destination_entity.value())
                                          : std::string("none, bare position")));

    std::unique_ptr<events::event> create_path_event = std::make_unique<events::create_path_to>(
            selected_id, click_position, path::replace, destination_entity
        );
    event_interface::queue_event(create_path_event);
    debug::log("[control_input_system::right_click, queued create_path_to] dog: "
        + std::to_string(selected_id)
        + ", destination: " + raglib::vector_to_string(click_position)); 
}

