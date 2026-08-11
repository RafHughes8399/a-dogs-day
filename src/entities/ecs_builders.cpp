#include "component.h"
#include "config.h"
#include "entity.h"
#include "sprite.h"
#include "texture.h"
#include <raylib.h>

// qualified with ecs_entities:: so a name that doesn't match a declaration in
// entity.h is a compile error here, not an undefined symbol at link time

namespace {
    void build_player_dog_components(size_t id, Vector2 position,
        std::vector<sprite::sprite> sprites){
        component_helpers::register_positional_component(id,
            component_builders::build_positional_component(position));

        std::vector<components::renderable_component::sprite_component> sprite_components = {
            component_builders::build_sprite_component(sprites, level_config::directions::right)};
        component_helpers::register_renderable_component(id,
            component_builders::build_renderable_component(sprite_components));

        auto across_hitbox = hitbox_builders::build_player_dog_across_hitbox(position);
        std::vector<hitbox::hitbox> hitboxes = {across_hitbox, across_hitbox};
        component_helpers::register_collision_component(id,
            component_builders::build_collision_component(
                component_builders::build_hitbox_component(hitboxes,
                    level_config::directions::right)));

        component_helpers:: register_movement_component(id,
            component_builders::build_movement_component(
                dog_config::dog_move_speed, level_config::direction_scalars[level_config::directions::right]
            ));

        component_helpers::register_selectable_component(id,
            component_builders::build_selectable_component(
                entity_config::selectable_kinds::player_dog_kind));
    }
} // namespace

void ecs_entities::build_player(size_t player_id, size_t cursor_id){
    // build control components, start with the array keys, 
    // TODO  literals for now, witll deffine in enum when refactor is complete
    component_helpers::register_key_input_component(player_id,
         component_builders::build_key_input_component(game_config::player_controls));
    // build the cursor,
    // ?  do we need an attachment compnent, a has_a_component ? that way we can manage destruction properly
    build_cursor(cursor_id);
}
void ecs_entities::build_player_dog(size_t id){
    (void) id;
}
    // TODO no movement_component yet - these dogs hold a position but no speed,
    // TODO direction or path queue, so nothing can move them.
    void ecs_entities::build_khiri(size_t id){
        // ! left right, parallel to level_config::directions
        std::vector<sprite::sprite> sprites;
        sprites.push_back(sprite_builders::build_dog_sprite(textures::khiri_left,
            entity_config::khiri_left_path, entity_config::khiri_across_attributes));
        sprites.push_back(sprite_builders::build_dog_sprite(textures::khiri_right,
            entity_config::khiri_right_path, entity_config::khiri_across_attributes));
        
        build_player_dog_components(id, level_config::khiri_start, std::move(sprites));
    }
    void ecs_entities::build_mack(size_t id){
        // ! left right, parallel to level_config::directions
        std::vector<sprite::sprite> sprites;
        sprites.push_back(sprite_builders::build_dog_sprite(textures::mack_left,
            entity_config::mack_left_path, entity_config::mack_across_attributes));
        sprites.push_back(sprite_builders::build_dog_sprite(textures::mack_right,
            entity_config::mack_right_path, entity_config::mack_across_attributes));

        build_player_dog_components(id, level_config::mack_start, std::move(sprites));
    }

void ecs_entities::build_customer_dog(size_t id){
    (void) id;
}
    //**

    // .
    // .
    // .
    // build duck_hunt_dog();
    //  */

void ecs_entities::build_waiter_dog(size_t id){
    (void) id;
}
//**
// build_saba()
// build tex()

void ecs_entities::build_cursor(size_t id){
    // positional component
    component_helpers::register_positional_component(id,
        component_builders::build_positional_component(GetMousePosition()));
    component_helpers::register_mouse_input_component(id,
        component_builders::build_mouse_input_component(game_config::cursor_controls));
    std::vector<sprite::sprite> sprites = {sprite_builders::build_cursor_sprite()};
    std::vector<components::renderable_component::sprite_component> sprite_components = {component_builders::build_sprite_component(sprites, 0)};
    component_helpers::register_renderable_component(id, component_builders::build_renderable_component(sprite_components));

    std::vector<hitbox::hitbox> hitboxes = {hitbox_builders::build_cursor_hitbox(GetMousePosition())};
    component_helpers::register_collision_component(id,
        component_builders::build_collision_component(component_builders::build_hitbox_component(hitboxes, 0)));
}

void ecs_entities::build_decoration(size_t id){
    (void) id;
}
    void ecs_entities::build_test_decoration(size_t id){
        (void) id;
    }
    //**
    // void build_gargoyle();
    //  */

void ecs_entities::build_station(size_t id){
    (void) id;
}
    void ecs_entities::build_counter(size_t id){
        (void) id;
    }
    void ecs_entities::build_table(size_t id){
        (void) id;
    }
    void ecs_entities::build_dishwasher(size_t id){
        (void) id;
    }
    /**
        // void build_stove();
    */

// position and renderable only - no hitbox, so it is never in the spatial index
// and is_entity_in_frame never culls it
void ecs_entities::build_background(size_t id){
    component_helpers::register_positional_component(id,
        component_builders::build_positional_component(Vector2{0.0f, 0.0f}));

    std::vector<sprite::sprite> sprites = {sprite_builders::build_background_sprite()};
    std::vector<components::renderable_component::sprite_component> sprite_components = {component_builders::build_sprite_component(sprites, 0)};
    component_helpers::register_renderable_component(id, component_builders::build_renderable_component(sprite_components));
}
