#include "sprite.h"
#include <cmath>
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


// TODO this should pass in a scalar or should the sprite have a scalar
// the sprite could own the scalar or the renderable component could whihc is better.
// i suppose the sprite is better because the renderable doesn't need to know nothing
// then it keeps the drawing more localised ? ig 
// the renderable component can hold the scalar, default is 1


// renderable keeps scalar, one sprite many scalars
void sprite::sprite::render(Vector2 position, int frame, Vector2 scale, float rotation, Color tint){
    if(animations_.empty()){ return; }
    auto& animation = animations_[animation_index_];
    animation.advance(frame);

    auto source = animation.get_frame();
    auto draw_position = Vector2Add(position, Vector2Multiply(draw_position_offset_, scale));
    Rectangle destination = {draw_position.x, draw_position.y, std::fabs(source.width) * scale.x, std::fabs(source.height) * scale.y};
    DrawTexturePro(sprite_texture_, source, destination, Vector2Zero(), rotation, tint);
}



//* ----------------------- nine_sprite ----------------------- *//

sprite::nine_sprite::nine_sprite(Rectangle frame, Color tint)
    : corner_piece_(sprite_builders::build_nine_corner_piece()),
    centre_piece_(sprite_builders::build_nine_centre_piece()),
    vertical_piece_(sprite_builders::build_nine_vertical_piece()),
    horizontal_piece_(sprite_builders::build_nine_horizontal_piece()),
    frame_(frame),
    tint_(tint),
    scale_(Vector2One()){
        // * scale needs to be calculated
        // * IT DEPENDS ON THE BASE DIMENSIONS WE DEFINE AND THE FRAME THAT IS PASSED IN 
        // * THE CENTRE PEICE IS SCALED BY BOTH, THE CENTRE PIECE IS INDENTED BY THE CORNERS ON BOTH DIMENSIONS * 2
        //  so it is essentially frame.x - corner width * 2 / centre_width, and then frame.y 0 - corner-height * 2 / centre height
        scale_.x = (frame.width - (2 * hud_config::nine_sprite_corner_width)) / hud_config::nine_sprite_centre_width;
        scale_.y = (frame.height - (2 * hud_config::nine_sprite_corner_height)) / hud_config::nine_sprite_centre_height; 
    }

void sprite::nine_sprite::render(Vector2 position, int frame){
    // draw corners
        // top left, top right, bottom left bottom right
        //corner will be drawn as top left. so its just a horizontal and vertical flip []
    // * top left
    corner_piece_.render(position, frame, Vector2One(), 0.0f, tint_);
    // * top right, 
    corner_piece_.render({position.x + frame_.width - hud_config::nine_sprite_corner_width, position.y}, frame, Vector2One(), 90.0f, tint_);
    // * bottom left
    corner_piece_.render({position.x, position.y + frame_.height - hud_config::nine_sprite_corner_height}, frame, Vector2One(), 270.0f, tint_);
    // * bottom right
    corner_piece_.render({position.x + frame_.width - hud_config::nine_sprite_corner_width, position.y + frame_.height - hud_config::nine_sprite_corner_height}, frame, Vector2One(), 180.0f, tint_);
    // can come from config values
    // * vertical left
    vertical_piece_.render({position.x, position.y + hud_config::nine_sprite_corner_height}, frame, {1.0f,scale_.y}, 0.0f, tint_);
    // * vertical right
    vertical_piece_.render({position.x + frame_.width - hud_config::nine_sprite_corner_width, position.y + hud_config::nine_sprite_corner_height}, frame, {1.0f, scale_.y}, 90.0f, tint_);
    // * horizontal top 
    horizontal_piece_.render({position.x + hud_config::nine_sprite_corner_width, position.y}, frame, {scale_.x, 1.0f}, 0.0f, tint_);
    // * horizontal bottom 
    horizontal_piece_.render({position.x + hud_config::nine_sprite_corner_width, position.y + frame_.width - hud_config::nine_sprite_corner_height}, frame, {scale_.x, 1.0f}, 180.0f, tint_);
    // * centre
    centre_piece_.render({position.x + hud_config::nine_sprite_corner_width, position.y + hud_config::nine_sprite_corner_height}, frame, scale_, 0.0f, tint_);
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
