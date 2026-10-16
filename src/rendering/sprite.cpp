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

sprite::nine_sprite::nine_sprite(Rectangle frame, Color tint, float corner_scale)
    : corner_piece_(sprite_builders::build_nine_corner_piece()),
    centre_piece_(sprite_builders::build_nine_centre_piece()),
    vertical_piece_(sprite_builders::build_nine_vertical_piece()),
    horizontal_piece_(sprite_builders::build_nine_horizontal_piece()),
    frame_(frame),
    tint_(tint),
    scale_(Vector2One()),
    corner_scale_(corner_scale){
        // * scale needs to be calculated
        // * IT DEPENDS ON THE BASE DIMENSIONS WE DEFINE AND THE FRAME THAT IS PASSED IN 
        // * THE CENTRE PEICE IS SCALED BY BOTH, THE CENTRE PIECE IS INDENTED BY THE CORNERS ON BOTH DIMENSIONS * 2
        //  so it is essentially frame.x - corner width * 2 / centre_width, and then frame.y 0 - corner-height * 2 / centre height
        scale_.x = (frame.width - (2 * hud_config::nine_sprite_corner_width * corner_scale_)) / hud_config::nine_sprite_centre_width;
        scale_.y = (frame.height - (2 * hud_config::nine_sprite_corner_height * corner_scale_)) / hud_config::nine_sprite_centre_height; 
    }

void sprite::nine_sprite::render(Vector2 position, int frame){
    auto draw = [&](sprite& piece, Vector2 at, Vector2 piece_scale, float rotation) -> void{
        piece.render(at, frame, piece_scale, rotation, tint_);
    };
    const Vector2 corner_scale{corner_scale_, corner_scale_};
    const float corner_width = hud_config::nine_sprite_corner_width * corner_scale_;
    const float corner_height = hud_config::nine_sprite_corner_height * corner_scale_;
    // * top left
    draw(corner_piece_, position, corner_scale, 0.0f);
    // * top right, 
    draw(corner_piece_, {position.x + frame_.width, position.y}, corner_scale, 90.0f);
    // * bottom left
    draw(corner_piece_, {position.x, position.y + frame_.height}, corner_scale, 270.0f);
    // * bottom right
    draw(corner_piece_, {position.x + frame_.width, position.y + frame_.height}, corner_scale, 180.0f);
    // * vertical left
    draw(vertical_piece_, {position.x, position.y + corner_height}, {corner_scale_, scale_.y}, 0.0f);
    // * vertical right
    draw(vertical_piece_, {position.x + frame_.width, position.y + frame_.height - corner_height}, {corner_scale_, scale_.y}, 180.0f);
    // * horizontal top 
    draw(horizontal_piece_, {position.x + corner_width, position.y}, {scale_.x, corner_scale_}, 0.0f);
    // * horizontal bottom 
    draw(horizontal_piece_, {position.x + frame_.width - corner_width, position.y + frame_.height}, {scale_.x, corner_scale_}, 180.0f);
    // * centre
    draw(centre_piece_, {position.x + corner_width, position.y + corner_height}, scale_, 0.0f);
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
