#include "component.h"

std::vector<game_config::input>& components::key_input_component::get_inputs(){
    return controls_;
}
