#include "component.h"

bool components::selectable_component::is_selected() const{
    return is_selected_;
}
size_t components::selectable_component::get_kind() const{
    return kind_;
}
void components::selectable_component::select(){
    is_selected_ = true;
}
void components::selectable_component::unselect(){
    is_selected_ = false;
}
