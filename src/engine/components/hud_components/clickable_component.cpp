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
size_t components::category_choice::get_category() const{
    return category_;
}
const items::item_ref& components::item_choice::get_ref() const{
    return ref_;
}
