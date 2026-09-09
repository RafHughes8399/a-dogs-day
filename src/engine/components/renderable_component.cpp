#include "component.h"
#include <utility>

sprite::sprite& components::renderable_component::body::get_active_sprite(){
    return sprites_[active_index_];
}
std::vector<sprite::sprite>& components::renderable_component::body::get_sprites(){
    return sprites_;
}
size_t components::renderable_component::body::get_active_index() const{
    return active_index_;
}
size_t components::renderable_component::body::num_sprites() const{
    return sprites_.size();
}
void components::renderable_component::body::set_index(size_t index){
    active_index_ = index;
}
std::vector<components::renderable_component::body>& components::renderable_component::get_layers(){
    return body_;
}
components::renderable_component::body* components::renderable_component::get_sprite_layer(size_t index){
    return index < body_.size() ? &body_[index] : nullptr;
}
size_t components::renderable_component::num_sprite_layers() const{
    return body_.size();
}
void components::renderable_component::add_sprite_layer(body layer){
    body_.push_back(std::move(layer));
}
void components::renderable_component::remove_sprite_layer(size_t index){
    if(index >= body_.size()){ return; }
    body_.erase(body_.begin() + static_cast<decltype(body_)::difference_type>(index));
}
void components::renderable_component::set_sprite_layer(size_t index, body layer){
    if(index >= body_.size()){ return; }
    body_[index] = std::move(layer);
}
