#include <cassert>
#include <cmath>
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
    const float attributes[entity_config::attributes::size], Vector2 draw_position_offset){
    return build_sprite(textures::textures_.get_texture(texture_key, path),
        attributes[entity_config::attributes::frame_width],
        attributes[entity_config::attributes::frame_height],
        attributes[entity_config::attributes::frames],
        attributes[entity_config::attributes::animations],
        draw_position_offset);
}
std::vector<std::vector<sprite::sprite>> sprite_builders::build_dog_part_layers(
    const entity_config::dog_part parts[entity_config::dog_sprite_slots_size],
    const int texture_keys[entity_config::dog_sprite_slots_size][entity_config::dog_part_directions_size],
    float expected_total_width){
    std::vector<std::vector<sprite::sprite>> layers;
    float cursor = 0.0f;
    float anchor = 0.0f;

    for(size_t slot = 0; slot < entity_config::dog_sprite_slots_size; ++slot){
        const auto& part = parts[slot];
        auto width = part.attributes[entity_config::attributes::frame_width];
        if(part.advances){
            anchor = cursor;
            cursor += width;
        }
        auto x_left = anchor + part.offset.x;
        auto x_right = expected_total_width - x_left - width;

        std::vector<sprite::sprite> directions;
        directions.push_back(build_dog_sprite(texture_keys[slot][entity_config::dog_part_left],
            part.left_path, part.attributes, Vector2{x_left, part.offset.y}));
        directions.push_back(build_dog_sprite(texture_keys[slot][entity_config::dog_part_right],
            part.right_path, part.attributes, Vector2{x_right, part.offset.y}));
        layers.push_back(std::move(directions));
    }
    assert(std::fabs(cursor - expected_total_width) < 0.01f
        and "dog part widths must sum to the across hitbox width");
    return layers;
}
std::vector<sprite::sprite> sprite_builders::build_gianluca_sprites(){
    std::vector<sprite::sprite> sprites;
    sprites.push_back(build_dog_sprite(textures::gianluca_left,
        entity_config::gianluca_left_path, entity_config::gianluca_attributes));
    sprites.push_back(build_dog_sprite(textures::gianluca_right,
        entity_config::gianluca_right_path, entity_config::gianluca_attributes));
    return sprites;
}
std::vector<sprite::sprite> sprite_builders::build_lionel_sprites(){
    std::vector<sprite::sprite> sprites;
    sprites.push_back(build_dog_sprite(textures::lionel_left,
        entity_config::lionel_left_path, entity_config::lionel_attributes));
    sprites.push_back(build_dog_sprite(textures::lionel_right,
        entity_config::lionel_right_path, entity_config::lionel_attributes));
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
