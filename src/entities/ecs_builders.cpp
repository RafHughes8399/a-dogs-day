#include "component.h"
#include "config.h"
#include "entity.h"
#include "sprite.h"
#include "texture.h"
#include <raylib.h>

// qualified with ecs_entities:: so a name that doesn't match a declaration in
// entity.h is a compile error here, not an undefined symbol at link time

namespace {
    // the same spawns level_builder::build_main_level places them at
    const Vector2 mack_start = Vector2{level_config::edge_weight * 7.0f,
                                       level_config::edge_weight * 4.0f};
    const Vector2 khiri_start = Vector2{level_config::edge_weight * 4.0f,
                                        level_config::edge_weight * 3.5f};

    // every dog sprite is the same four attribute lookups off a cached texture
    sprite::sprite build_dog_sprite(int texture_key, const char* path,
        const float attributes[entity_config::attributes::size]){
        return sprite_builders::build_sprite(
            textures::textures_.get_texture(texture_key, path),
            attributes[entity_config::attributes::frame_width],
            attributes[entity_config::attributes::frame_height],
            attributes[entity_config::attributes::frames],
            attributes[entity_config::attributes::animations]);
    }

    // * what khiri and mack share: a position, one sprite_component holding the
    // * direction-indexed body sprites, and a hitbox per facing running parallel
    // * to them - so the two indices are one facing, as collision_component's
    // * note requires. both start facing right, matching movement_component's
    // * default direction.
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
    // * only the across sprites are ported. the old builder also carried outline
    // * sprites and an empty head body: the outlines are selection cosmetics that
    // * ecs_layer::draw would render unconditionally, since every sprite_component
    // * on an entity draws every frame and nothing in the ECS knows which dog is
    // * selected yet. they land with the state machine component.
    // TODO no movement_component yet - these dogs hold a position but no speed,
    // TODO direction or path queue, so nothing can move them.
    void ecs_entities::build_khiri(size_t id){
        // ! left right, parallel to level_config::directions
        std::vector<sprite::sprite> sprites;
        sprites.push_back(build_dog_sprite(textures::khiri_left,
            entity_config::khiri_left_path, entity_config::khiri_across_attributes));
        sprites.push_back(build_dog_sprite(textures::khiri_right,
            entity_config::khiri_right_path, entity_config::khiri_across_attributes));

        build_player_dog_components(id, khiri_start, std::move(sprites));
    }
    void ecs_entities::build_mack(size_t id){
        // ! left right, parallel to level_config::directions
        std::vector<sprite::sprite> sprites;
        sprites.push_back(build_dog_sprite(textures::mack_left,
            entity_config::mack_left_path, entity_config::mack_across_attributes));
        sprites.push_back(build_dog_sprite(textures::mack_right,
            entity_config::mack_right_path, entity_config::mack_across_attributes));

        build_player_dog_components(id, mack_start, std::move(sprites));
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
// build text
//  */

// * the cursor is the case that settled where direction lives and how the
// * different kinds of movement are told apart.
// *
// * 1. direction belongs in movement_component, not position_component. it is
// *    one half of a velocity (dog::move_toward_current_waypoint is
// *    position + move_speed * direction * delta) and move_speed already lives
// *    in movement. it is also derived state - determine_direction recomputes it
// *    from position -> next waypoint every time the waypoint changes - so it
// *    only exists while something is moving. a table would carry the field
// *    forever and never read it. its one non-movement consumer is sprite facing
// *    (set_direction_index), and that becomes the movement system writing an
// *    index into renderable_component, not a reason to keep it in position.
// *
// * 2. no movement strategy object, and no marker component either. what makes
// *    an entity mouse-positioned is simply holding a mouse_input_component:
// *      dog    = position + movement (speed, direction, path queue), integrated
// *               by movement_system from paths the npc system sets
// *      cursor = position + mouse_input, position synced from the device by
// *               control_input_system
// *    a strategy would have put behaviour inside data and needed a unique_ptr
// *    member, making this the one move-only component manager. a dedicated
// *    cursor tag would have cost a whole manager plus five hand-maintained
// *    entries in component.h to mark exactly one entity forever.
// *    mouse_input_component avoids both: it carries real data (the button
// *    bindings) and doubles as the mark, because "driven by the mouse" and
// *    "positioned by the mouse" are the same fact.
// *
// * so the cursor takes no movement_component and no direction. there is no
// * "omnidirectional" direction to give it - not having the component IS the
// * answer. (level_config::directions::all was an attempt at one: {1,1} is not a
// * unit vector so it moves ~1.41x too fast, it has no matching sprite in the
// * direction-indexed arrays, and position_to_node snaps it identically to
// * right.)
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
