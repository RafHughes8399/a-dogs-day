#include "component.h"
#include <algorithm>

bool components::hud_component::is_active() const{
    return active_;
}
void components::hud_component::set_active(bool active){
    active_ = active;
}
sprite::nine_sprite& components::hud_component::get_base(){
    return base_;
}
Vector2 components::hud_component::get_offset() const{
    return offset_;
}
const std::vector<size_t>& components::hud_component::get_elements() const{
    return elements_;
}
void components::hud_component::add_element(size_t hud_id){
    elements_.push_back(hud_id);
}
void components::hud_component::remove_element(size_t hud_id){
    elements_.erase(std::remove(elements_.begin(), elements_.end(), hud_id), elements_.end());
}
