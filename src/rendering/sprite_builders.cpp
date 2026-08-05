#include "sprite.h"
#include "texture.h"
sprite::sprite sprite_builders::build_sprite(Texture2D texture, float frame_width, float frame_height, float frames, float animations){
    return sprite::sprite(texture, frame_width, frame_height, frames, animations);
}
sprite::sprite sprite_builders::build_cursor_sprite(){
    auto cursor_texture = textures::textures_.get_texture(textures::cursor, entity_config::cursor_path);
    return build_sprite(cursor_texture,entity_config::cursor_attributes[entity_config::attributes::frame_width],
        entity_config::cursor_attributes[entity_config::attributes::frame_height],
        entity_config::cursor_attributes[entity_config::attributes::frames],
        entity_config::cursor_attributes[entity_config::attributes::animations]);
}