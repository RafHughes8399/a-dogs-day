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
sprite::sprite sprite_builders::build_dog_sprite(int texture_key, const char* path,
    const float attributes[entity_config::attributes::size]){
    return build_sprite(textures::textures_.get_texture(texture_key, path),
        attributes[entity_config::attributes::frame_width],
        attributes[entity_config::attributes::frame_height],
        attributes[entity_config::attributes::frames],
        attributes[entity_config::attributes::animations]);
}
sprite::sprite sprite_builders::build_background_sprite(){
    auto background_texture = textures::textures_.get_texture(textures::background, entity_config::background_path);
    return build_sprite(background_texture,
        entity_config::background_attributes[entity_config::attributes::frame_width],
        entity_config::background_attributes[entity_config::attributes::frame_height],
        entity_config::background_attributes[entity_config::attributes::frames],
        entity_config::background_attributes[entity_config::attributes::animations]);
}
