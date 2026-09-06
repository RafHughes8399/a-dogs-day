#include "component.h"
#include <utility>

sprite::sprite& components::renderable_component::sprite_layer::get_active_sprite(){
    return sprites_[active_index_];
}
std::vector<sprite::sprite>& components::renderable_component::sprite_layer::get_sprites(){
    return sprites_;
}
size_t components::renderable_component::sprite_layer::get_active_index() const{
    return active_index_;
}
size_t components::renderable_component::sprite_layer::num_sprites() const{
    return sprites_.size();
}
void components::renderable_component::sprite_layer::set_index(size_t index){
    active_index_ = index;
}
std::vector<components::renderable_component::sprite_layer>& components::renderable_component::get_layers(){
    return layers_;
}
components::renderable_component::sprite_layer* components::renderable_component::get_sprite_layer(size_t index){
    return index < layers_.size() ? &layers_[index] : nullptr;
}
size_t components::renderable_component::num_sprite_layers() const{
    return layers_.size();
}
void components::renderable_component::add_sprite_layer(sprite_layer layer){
    layers_.push_back(std::move(layer));
}
void components::renderable_component::remove_sprite_layer(size_t index){
    if(index >= layers_.size()){ return; }
    layers_.erase(layers_.begin() + static_cast<decltype(layers_)::difference_type>(index));
}
void components::renderable_component::set_sprite_layer(size_t index, sprite_layer layer){
    if(index >= layers_.size()){ return; }
    layers_[index] = std::move(layer);
}
