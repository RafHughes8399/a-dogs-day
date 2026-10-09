#include "component.h"

Vector2 components::carrier_component::get_previous_position() const{
    return previous_position_;
}
void components::carrier_component::set_previous_position(Vector2 position){
    previous_position_ = position;
}
Vector2* components::carrier_component::get_current_position() const{
    return current_position_;
}
void components::carrier_component::set_current_position(Vector2* position){
    current_position_ = position;
}
std::optional<size_t> components::carrier_component::get_carried_entity() const{
    return carried_entity_;
}
void components::carrier_component::set_carried_entity(std::optional<size_t> entity_id){
    carried_entity_ = entity_id;
}
bool components::carrier_component::is_carrying() const{
    return carried_entity_.has_value();
}

void components::carrier_component::drop(){
    carried_entity_ = std::nullopt;
}