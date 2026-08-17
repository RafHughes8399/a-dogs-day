#include "component.h"
#include "config.h"
#include "entity.h"
#include "sprite.h"
#include "texture.h"
#include <raylib.h>

// qualified with ecs_entities:: so a name that doesn't match a declaration in
// entity.h is a compile error here, not an undefined symbol at link time
    // ! left right, parallel to level_config::directions
    // * TEMP HELPER UNTIL UNIQUE DOG ART IS IN PLACE
    std::vector<sprite::sprite> build_mack_sprites(){
        std::vector<sprite::sprite> sprites;
        sprites.push_back(sprite_builders::build_dog_sprite(textures::mack_left,
            entity_config::mack_left_path, entity_config::mack_across_attributes));
        sprites.push_back(sprite_builders::build_dog_sprite(textures::mack_right,
            entity_config::mack_right_path, entity_config::mack_across_attributes));
        return sprites;
    }


void ecs_entities::build_player(size_t player_id, size_t cursor_id){
    // build control components, start with the array keys,
    // TODO  literals for now, witll deffine in enum when refactor is complete
    component_helpers::add_key_input_component(player_id, game_config::player_controls);
    // build the cursor,
    // ?  do we need an attachment compnent, a has_a_component ? that way we can manage destruction properly
    build_cursor(cursor_id);
}
void ecs_entities::build_player_dog(size_t id, Vector2 position,
    std::vector<sprite::sprite> sprites, size_t kind, float reach){
    component_helpers::add_positional_component(id, position);

    std::vector<components::renderable_component::sprite_component> sprite_components = {
        component_builders::build_sprite_component(sprites, level_config::directions::right)};
    component_helpers::add_renderable_component(id, sprite_components);

    auto across_hitbox = hitbox_builders::build_player_dog_across_hitbox(position);
    std::vector<hitbox::hitbox> hitboxes = {across_hitbox, across_hitbox};
    component_helpers::add_collision_component(id,
        component_builders::build_hitbox_component(hitboxes,
            level_config::directions::right));

    component_helpers::add_movement_component(id, dog_config::dog_move_speed,
        level_config::direction_scalars[level_config::directions::right]);

    component_helpers::add_selectable_component(id, kind);

    component_helpers::add_interactor_component(id, reach);
}
    void ecs_entities::build_khiri(size_t id){
        // ! left right, parallel to level_config::directions
        std::vector<sprite::sprite> sprites;
        sprites.push_back(sprite_builders::build_dog_sprite(textures::khiri_left,
            entity_config::khiri_left_path, entity_config::khiri_across_attributes));
        sprites.push_back(sprite_builders::build_dog_sprite(textures::khiri_right,
            entity_config::khiri_right_path, entity_config::khiri_across_attributes));

        build_player_dog(id, level_config::khiri_start, std::move(sprites),
            entity_config::selectable_kinds::player_dog_kind, dog_config::dog_reach);
    }
    void ecs_entities::build_mack(size_t id){
        std::vector<sprite::sprite> sprites;
        sprites.push_back(sprite_builders::build_dog_sprite(textures::mack_left,
            entity_config::mack_left_path, entity_config::mack_across_attributes));
        sprites.push_back(sprite_builders::build_dog_sprite(textures::mack_right,
            entity_config::mack_right_path, entity_config::mack_across_attributes));
        build_player_dog(id, level_config::mack_start, std::move(sprites),
            entity_config::selectable_kinds::player_dog_kind, dog_config::dog_reach);
    }

// TODO mack's art stands in until npc dog sprites exist
void ecs_entities::build_customer_dog(size_t id, Vector2 position){
    build_player_dog(id, position, build_mack_sprites(),
        entity_config::selectable_kinds::customer_dog_kind, dog_config::dog_reach);
}
    //**

    // .
    // .
    // .
    // build duck_hunt_dog();
    //  */

void ecs_entities::build_waiter_dog(size_t id, Vector2 position){
    build_player_dog(id, position, build_mack_sprites(),
        entity_config::selectable_kinds::waiter_dog_kind, dog_config::dog_reach);
}
//**
// build_saba()
// build tex()

void ecs_entities::build_cursor(size_t id){
    // positional component
    component_helpers::add_positional_component(id, GetMousePosition());
    component_helpers::add_mouse_input_component(id, game_config::cursor_controls);
    std::vector<sprite::sprite> sprites = {sprite_builders::build_cursor_sprite()};
    std::vector<components::renderable_component::sprite_component> sprite_components = {component_builders::build_sprite_component(sprites, 0)};
    component_helpers::add_renderable_component(id, sprite_components);

    std::vector<hitbox::hitbox> hitboxes = {hitbox_builders::build_cursor_hitbox(GetMousePosition())};
    component_helpers::add_collision_component(id,
        component_builders::build_hitbox_component(hitboxes, 0));
}
void ecs_entities::build_decoration(size_t id, Vector2 position,
    sprite::sprite decoration_sprite, hitbox::hitbox decoration_hitbox){
    component_helpers::add_positional_component(id, position);

    std::vector<sprite::sprite> sprites = {decoration_sprite};
    std::vector<components::renderable_component::sprite_component> sprite_components = {
        component_builders::build_sprite_component(sprites, 0)};
    component_helpers::add_renderable_component(id, sprite_components);

    std::vector<hitbox::hitbox> hitboxes = {decoration_hitbox};
    component_helpers::add_collision_component(id,
        component_builders::build_hitbox_component(hitboxes, 0));
}
    void ecs_entities::build_test_decoration(size_t id, Vector2 position){
        build_decoration(id, position,
            sprite_builders::build_test_decoration_sprite(),
            hitbox_builders::build_test_decoration_hitbox(position));
        component_helpers::add_selectable_component(id,
            entity_config::selectable_kinds::decoration_kind);
    }
    //**
    // void build_gargoyle();
    //  */

void ecs_entities::build_station(size_t id, Vector2 position,
    sprite::sprite station_sprite, hitbox::hitbox station_hitbox,
    float station_reach, size_t station_capacity){
    build_decoration(id, position, station_sprite, station_hitbox);
    component_helpers::add_selectable_component(id,
        entity_config::selectable_kinds::station_kind);
    component_helpers::add_interactable_component(id, station_reach, station_capacity);
}
    void ecs_entities::build_counter(size_t id, Vector2 position){
        build_station(id, position,
            sprite_builders::build_food_counter_sprite(),
            hitbox_builders::build_food_counter_hitbox(position),
            entity_config::station_reach, entity_config::food_counter_capacity);
    }
    void ecs_entities::build_table(size_t id, Vector2 position){
        build_station(id, position,
            sprite_builders::build_table_sprite(),
            hitbox_builders::build_table_hitbox(position),
            entity_config::station_reach, entity_config::table_capacity);
    }
    void ecs_entities::build_dishwasher(size_t id, Vector2 position){
        build_station(id, position,
            sprite_builders::build_dishwasher_sprite(),
            hitbox_builders::build_dishwasher_hitbox(position),
            entity_config::station_reach, entity_config::dishwasher_capacity);
    }
    void ecs_entities::build_stove(size_t id, Vector2 position){
        build_station(id, position,
            sprite_builders::build_stove_sprite(),
            hitbox_builders::build_stove_hitbox(position),
            entity_config::station_reach, entity_config::stove_capacity);
    }
    /**
        // void build_stove();
    */

void ecs_entities::build_food(size_t id, Vector2 position){
    build_decoration(id, position,
        sprite_builders::build_food_sprite(),
        hitbox_builders::build_food_hitbox(position));
}

// position and renderable only - no hitbox, so it is never in the spatial index
// and is_entity_in_frame never culls it
void ecs_entities::build_background(size_t id){
    component_helpers::add_positional_component(id, Vector2{0.0f, 0.0f});

    std::vector<sprite::sprite> sprites = {sprite_builders::build_background_sprite()};
    std::vector<components::renderable_component::sprite_component> sprite_components = {component_builders::build_sprite_component(sprites, 0)};
    component_helpers::add_renderable_component(id, sprite_components);
}
