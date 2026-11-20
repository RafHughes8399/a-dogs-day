#include "component.h"
#include <algorithm>
#include <raymath.h>
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
Vector2 components::renderable_component::body::get_scale() const{
    return scale_;
}
void components::renderable_component::body::scale_by(Vector2 factor){
    scale_ = Vector2{base_scale_.x * factor.x, base_scale_.y * factor.y};
}
void components::renderable_component::body::reset_scale(){
    scale_ = base_scale_;
}
Vector2 components::renderable_component::body::get_offset() const{
    return offset_;
}
std::vector<components::renderable_component::body>& components::renderable_component::get_layers(){
    return body_;
}
components::renderable_component::body* components::renderable_component::get_body(size_t index){
    return index < body_.size() ? &body_[index] : nullptr;
}
size_t components::renderable_component::num_bodys() const{
    return body_.size();
}
void components::renderable_component::add_body(body layer){
    body_.push_back(std::move(layer));
}
void components::renderable_component::remove_body(size_t index){
    if(index >= body_.size()){ return; }
    body_.erase(body_.begin() + static_cast<decltype(body_)::difference_type>(index));
}
void components::renderable_component::set_body(size_t index, body layer){
    if(index >= body_.size()){ return; }
    body_[index] = std::move(layer);
}


void components::renderable_component::scale_by(Vector2 factor){
    std::for_each(body_.begin(), body_.end(), [factor](auto& body) -> void { body.scale_by(factor); });
}
void components::renderable_component::reset_scale(){
    std::for_each(body_.begin(), body_.end(), [](auto& body) -> void { body.reset_scale(); });
}
