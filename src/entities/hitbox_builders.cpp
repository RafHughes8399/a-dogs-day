#include "hitbox.h"
#include "config.h"

hitbox::hitbox hitbox_builders::build_hitbox(Vector2 position, float frame_width, float frame_height){
    return hitbox::hitbox(Rectangle{position.x, position.y, frame_width, frame_height});
}

hitbox::hitbox hitbox_builders::build_cursor_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::cursor_attributes[entity_config::attributes::frame_width],
        entity_config::cursor_attributes[entity_config::attributes::frame_height]);
}
hitbox::hitbox hitbox_builders::build_paw_mark_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::paw_mark_attributes[entity_config::attributes::frame_width],
        entity_config::paw_mark_attributes[entity_config::attributes::frame_height]);
}
hitbox::hitbox hitbox_builders::build_dog_across_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::khiri_across_attributes[entity_config::attributes::frame_width],
        entity_config::khiri_across_attributes[entity_config::attributes::frame_height]);
}
hitbox::hitbox hitbox_builders::build_dog_down_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::khiri_down_attributes[entity_config::attributes::frame_width],
        entity_config::khiri_down_attributes[entity_config::attributes::frame_height]);
}

hitbox::hitbox hitbox_builders::build_gianluca_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::gianluca_attributes[entity_config::attributes::frame_width],
        entity_config::gianluca_attributes[entity_config::attributes::frame_height]);
}
hitbox::hitbox hitbox_builders::build_lionel_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::lionel_attributes[entity_config::attributes::frame_width],
        entity_config::lionel_attributes[entity_config::attributes::frame_height]);
}

hitbox::hitbox hitbox_builders::build_test_decoration_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::test_decoration_attributes[entity_config::attributes::frame_width],
        entity_config::test_decoration_attributes[entity_config::attributes::frame_height]);
}
hitbox::hitbox hitbox_builders::build_gargoyle_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_width],
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_height]);
}
hitbox::hitbox hitbox_builders::build_poker_table_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::poker_table_attributes[entity_config::attributes::frame_width],
        entity_config::poker_table_attributes[entity_config::attributes::frame_height]);
}
hitbox::hitbox hitbox_builders::build_dog_painting_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::dog_painting_attributes[entity_config::attributes::frame_width],
        entity_config::dog_painting_attributes[entity_config::attributes::frame_height]);
}
hitbox::hitbox hitbox_builders::build_table_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::table_attributes[entity_config::attributes::frame_width],
        entity_config::table_attributes[entity_config::attributes::frame_height]);
}
hitbox::hitbox hitbox_builders::build_food_counter_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::food_counter_attributes[entity_config::attributes::frame_width],
        entity_config::food_counter_attributes[entity_config::attributes::frame_height]);
}
hitbox::hitbox hitbox_builders::build_dishwasher_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::dishwasher_attributes[entity_config::attributes::frame_width],
        entity_config::dishwasher_attributes[entity_config::attributes::frame_height]);
}
hitbox::hitbox hitbox_builders::build_stove_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::stove_attributes[entity_config::attributes::frame_width],
        entity_config::stove_attributes[entity_config::attributes::frame_height]);
}
hitbox::hitbox hitbox_builders::build_food_hitbox(Vector2 position){
    return build_hitbox(position,
        entity_config::test_food_attributes[entity_config::attributes::frame_width],
        entity_config::test_food_attributes[entity_config::attributes::frame_height]);
}
