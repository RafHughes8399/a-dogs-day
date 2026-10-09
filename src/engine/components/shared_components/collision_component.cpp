#include "component.h"

std::vector<hitbox::hitbox>& components::collision_component::hitbox_component::get_hitboxes(){
    return hitboxes_;
}
hitbox::hitbox& components::collision_component::hitbox_component::get_hitbox(){
    return hitboxes_[hitbox_index_];
}
size_t components::collision_component::hitbox_component::get_hitbox_index() const{
    return hitbox_index_;
}
size_t components::collision_component::hitbox_component::num_hitboxes() const{
    return hitboxes_.size();
}
void components::collision_component::hitbox_component::set_index(size_t index){
    hitbox_index_ = index;
}
components::collision_component::hitbox_component& components::collision_component::get_hitbox_component(){
    return hitbox_component_;
}
