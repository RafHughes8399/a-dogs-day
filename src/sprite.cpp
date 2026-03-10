#include "sprite.h"
// ----------------------- sprite ----------------------- // 

animation::animation& sprite::sprite::get_animation(){
    return sprite_animation_;
}
const Texture2D& sprite::sprite::get_texture(){
    return sprite_texture_;
}

void sprite::sprite::render(Vector2 position){
    DrawTextureRec(sprite_texture_, sprite_animation_.get_frame(), position, WHITE);
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

void sprite::spriteset::render(Vector2 position){
    sprites_[current_].render(position);
}