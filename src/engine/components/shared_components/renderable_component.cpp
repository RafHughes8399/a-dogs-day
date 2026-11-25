#include "component.h"
#include <algorithm>
#include <raymath.h>
#include <utility>

sprite::sprite& components::renderable_component::sprite_part::get_active_sprite(){
    return sprites_[active_index_];
}
std::vector<sprite::sprite>& components::renderable_component::sprite_part::get_sprites(){
    return sprites_;
}
size_t components::renderable_component::sprite_part::get_active_index() const{
    return active_index_;
}
size_t components::renderable_component::sprite_part::num_sprites() const{
    return sprites_.size();
}
void components::renderable_component::sprite_part::set_index(size_t index){
    active_index_ = index;
}
Vector2 components::renderable_component::sprite_part::get_scale() const{
    return scale_;
}
void components::renderable_component::sprite_part::scale_by(Vector2 factor){
    scale_ = Vector2{base_scale_.x * factor.x, base_scale_.y * factor.y};
}
void components::renderable_component::sprite_part::reset_scale(){
    scale_ = base_scale_;
}
Vector2 components::renderable_component::sprite_part::get_offset() const{
    return offset_;
}
std::vector<components::renderable_component::sprite_part>& components::renderable_component::get_layers(){
    return sprite_;
}
components::renderable_component::sprite_part* components::renderable_component::get_body(size_t index){
    return index < sprite_.size() ? &sprite_[index] : nullptr;
}
size_t components::renderable_component::num_bodys() const{
    return sprite_.size();
}
void components::renderable_component::add_body(sprite_part layer){
    sprite_.push_back(std::move(layer));
}
void components::renderable_component::remove_body(size_t index){
    if(index >= sprite_.size()){ return; }
    sprite_.erase(sprite_.begin() + static_cast<decltype(sprite_)::difference_type>(index));
}
void components::renderable_component::set_body(size_t index, sprite_part layer){
    if(index >= sprite_.size()){ return; }
    sprite_[index] = std::move(layer);
}


void components::renderable_component::scale_by(Vector2 factor){
    std::for_each(sprite_.begin(), sprite_.end(), [factor](auto& body) -> void { body.scale_by(factor); });
}
void components::renderable_component::reset_scale(){
    std::for_each(sprite_.begin(), sprite_.end(), [](auto& body) -> void { body.reset_scale(); });
}
