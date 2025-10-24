#include "sprite.h"
// ----------------------- sprite ----------------------- // 

animation::animation& sprite::sprite::get_animation(){
    return sprite_animation_;
}
Texture2D& sprite::sprite::get_texture(){
    return sprite_texture_;
}

void sprite::sprite::render(Vector2 position){
    DrawTextureRec(sprite_texture_, sprite_animation_.get_frame(), position, WHITE);
}

