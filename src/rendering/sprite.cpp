#include "sprite.h"
// ----------------------- sprite ----------------------- // 

animation::animation& sprite::sprite::get_animation(){
    return animations_[animation_index_];
}
animation::animation& sprite::sprite::get_animation(size_t index){
    return animations_[index];
}
size_t sprite::sprite::get_animation_index() const{
    return animation_index_;
}
size_t sprite::sprite::num_animations() const{
    return animations_.size();
}
void sprite::sprite::set_animation(size_t index){
    if(index < animations_.size()){
        animation_index_ = index;
    }
}
const Texture2D& sprite::sprite::get_texture(){
    return sprite_texture_;
}
Vector2 sprite::sprite::get_draw_position_offset() const{
    return draw_position_offset_;
}

void sprite::sprite::render(Vector2 position, int frame){
    if(animations_.empty()){ return; }
    auto& animation = animations_[animation_index_];
    animation.advance(frame);

    DrawTextureRec(sprite_texture_, animation.get_frame(), Vector2Add(position, draw_position_offset_), tint_);
}



// ----------------------- spriteset ----------------------- //

size_t sprite::spriteset::index(){
    return current_;
}
sprite::sprite& sprite::spriteset::get_sprite(){
    return sprites_[current_];
}
std::vector<sprite::sprite>& sprite::spriteset::get_sprites(){
    return sprites_;
}
void sprite::spriteset::set_index(size_t index){
    current_ = index;
}

void sprite::spriteset::render(Vector2 position, int frame){
    sprites_[current_].render(position, frame);
}
