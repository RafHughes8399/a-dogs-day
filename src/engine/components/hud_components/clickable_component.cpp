#include "component.h"

size_t components::clickable_component::get_action() const{
    return action_;
}
const components::click_payload& components::clickable_component::get_payload() const{
    return payload_;
}
size_t components::recipe_choice::get_recipe_id() const{
    return recipe_id_;
}
int components::page_turn::get_direction() const{
    return direction_;
}
