#include "component.h"

std::vector<game_config::input>& components::mouse_input_component::get_inputs(){
    return inputs_;
}
