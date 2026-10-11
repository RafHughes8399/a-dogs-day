#include "component.h"

size_t components::clickable_component::get_action() const{
    return action_;
}
int components::clickable_component::get_payload() const{
    return payload_;
}
