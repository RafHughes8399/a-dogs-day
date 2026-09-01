#include "component.h"

Vector2 components::position_component::get_position(){
    return position_;
}
void components::position_component::set_position(Vector2 position){
    position_ = position;
}
