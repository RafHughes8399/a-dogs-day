#include "sprite.h"
// ----------------------- sprite ----------------------- // 

animation::animation& sprite::sprite::get_animation(){
    return sprite_animation_;
}
const Texture2D& sprite::sprite::get_texture(){
    return sprite_texture_;
}
Vector2 sprite::sprite::get_draw_position_offset() const{
    return draw_position_offset_;
}

void sprite::sprite::render(Vector2 position, int frame){
    sprite_animation_.advance(frame);

    DrawTextureRec(sprite_texture_, sprite_animation_.get_frame(), Vector2Add(position, draw_position_offset_), tint_);
    // * partial rendering such that it is frame
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
