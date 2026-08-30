#include "config.h"
#include "sprite.h"
#include "texture.h"
sprite::sprite sprite_builders::build_sprite(Texture2D texture, float frame_width, float frame_height, float frames, float animations,
    Vector2 draw_position_offset, Color tint){
    return sprite::sprite(texture, frame_width, frame_height, frames, animations, draw_position_offset, tint);
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
std::vector<sprite::sprite> sprite_builders::build_gianluca_sprites(){
    std::vector<sprite::sprite> sprites;
    sprites.push_back(build_dog_sprite(textures::gianluca_left,
        entity_config::gianluca_left_path, entity_config::gianluca_across_hitbox_attributes));
    sprites.push_back(build_dog_sprite(textures::gianluca_right,
        entity_config::gianluca_right_path, entity_config::gianluca_across_hitbox_attributes));
    return sprites;
}
sprite::sprite sprite_builders::build_test_decoration_sprite(const float attributes[entity_config::attributes::size], Color tint){
    return build_sprite(textures::textures_.get_texture(textures::test_decoration, entity_config::test_decoration_path),
        attributes[entity_config::attributes::frame_width],
        attributes[entity_config::attributes::frame_height],
        attributes[entity_config::attributes::frames],
        attributes[entity_config::attributes::animations],
        Vector2Zero(),
        tint);
}
sprite::sprite sprite_builders::build_decoration_sprite(size_t texture_id, const char* decoration_path, const float attributes[entity_config::attributes::size],
    Vector2 draw_position_offset){
    return build_sprite(textures::textures_.get_texture(texture_id, decoration_path), 
    attributes[entity_config::attributes::frame_width],
    attributes[entity_config::attributes::frame_height],
    attributes[entity_config::attributes::frames],
    attributes[entity_config::attributes::animations],
    draw_position_offset);
}
sprite::sprite sprite_builders::build_poker_table(){
    return build_decoration_sprite(textures::poker_table, entity_config::poker_table_decoration_path, entity_config::poker_table_attributes);
}
sprite::sprite sprite_builders::build_dog_painting(){
    return build_decoration_sprite(textures::dog_painting, entity_config::dog_painting_decoration_path, entity_config::dog_painting_attributes);
}
sprite::sprite sprite_builders::build_gargoyle(){
        return build_decoration_sprite(textures::gargoyle_void, entity_config::gargoyle_void_decoration_path, entity_config::gargoyle_decoration_attributes);
}
sprite::sprite sprite_builders::build_table_sprite(){
    return build_test_decoration_sprite(entity_config::table_attributes, RED);
}
    sprite::sprite sprite_builders::build_dining_table_sprite(){
        return build_decoration_sprite(textures::dining_table, entity_config::dining_table_station_path, entity_config::dining_table_attributes);
    }
    sprite::sprite sprite_builders::build_food_counter_sprite(){
        return build_decoration_sprite(textures::food_counter, entity_config::food_counter_station_path, entity_config::food_counter_attributes);
    }
sprite::sprite sprite_builders::build_dishwasher_sprite(){
    return build_test_decoration_sprite(entity_config::dishwasher_attributes, PURPLE);
}
sprite::sprite sprite_builders::build_stove_sprite(){
    return build_test_decoration_sprite(entity_config::stove_attributes, ORANGE);
}
sprite::sprite sprite_builders::build_food_sprite(){
    return build_test_decoration_sprite(entity_config::test_food_attributes, RED);
}
std::vector<sprite::sprite> sprite_builders::build_food_sprites(){
    std::vector<sprite::sprite> sprites;
    sprites.push_back(build_decoration_sprite(textures::lasagna,
        entity_config::lasagna_food_path, entity_config::lasagna_attributes,
        entity_config::food_draw_offset));
    sprites.push_back(build_decoration_sprite(textures::coffee,
        entity_config::coffee_food_path, entity_config::coffee_attributes,
        entity_config::food_draw_offset));
    return sprites;
}
sprite::sprite sprite_builders::build_background_sprite(){
    auto background_texture = textures::textures_.get_texture(textures::background, entity_config::background_path);
    return build_sprite(background_texture,
        entity_config::background_attributes[entity_config::attributes::frame_width],
        entity_config::background_attributes[entity_config::attributes::frame_height],
        entity_config::background_attributes[entity_config::attributes::frames],
        entity_config::background_attributes[entity_config::attributes::animations]);
}
