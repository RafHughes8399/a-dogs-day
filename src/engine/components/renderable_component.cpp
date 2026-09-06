#include "component.h"
#include <utility>

sprite::sprite& components::renderable_component::sprite_component::get_sprite(){
    return sprites_[sprite_index_];
}
std::vector<sprite::sprite>& components::renderable_component::sprite_component::get_sprites(){
    return sprites_;
}
size_t components::renderable_component::sprite_component::get_sprite_index() const{
    return sprite_index_;
}
size_t components::renderable_component::sprite_component::num_sprites() const{
    return sprites_.size();
}
void components::renderable_component::sprite_component::set_index(size_t index){
    sprite_index_ = index;
}
std::vector<components::renderable_component::sprite_component>& components::renderable_component::get_sprites(){
    return sprites_;
}
components::renderable_component::sprite_component* components::renderable_component::get_sprite_component(size_t index){
    return index < sprites_.size() ? &sprites_[index] : nullptr;
}
size_t components::renderable_component::num_sprite_components() const{
    return sprites_.size();
}
void components::renderable_component::add_sprite_component(sprite_component sprite){
    sprites_.push_back(std::move(sprite));
}
void components::renderable_component::remove_sprite_component(size_t index){
    if(index >= sprites_.size()){ return; }
    sprites_.erase(sprites_.begin() + static_cast<decltype(sprites_)::difference_type>(index));
}
void components::renderable_component::set_sprite_component(size_t index, sprite_component sprite){
    if(index >= sprites_.size()){ return; }
    sprites_[index] = std::move(sprite);
}
