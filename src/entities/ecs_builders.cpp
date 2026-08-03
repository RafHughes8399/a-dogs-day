#include "component.h"
#include "config.h"
#include "entity.h"
#include <raylib.h>

// qualified with ecs_entities:: so a name that doesn't match a declaration in
// entity.h is a compile error here, not an undefined symbol at link time

void ecs_entities::build_player(size_t player_id, size_t cursor_id){
    // build control components, start with the array keys, 
    // TODO  literals for now, witll deffine in enum when refactor is complete
    component_helpers::register_controls_component(player_id,
         component_builders::build_controls_component(game_config::player_controls));
    // build the cursor,
    // ?  do we need an attachment compnent, a has_a_component ?
    build_cursor(cursor_id);
}
void ecs_entities::build_player_dog(size_t id){
    (void) id;
}
    void ecs_entities::build_khiri(size_t id){
        (void) id;
    }
    void ecs_entities::build_mack(size_t id){
        (void) id;
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

// * the cursor is the case that settled two open questions - direction's home,
// * and whether "different kinds of movement" wants a strategy object.
// *
// * 1. direction belongs in movement_component, not position_component.
// *    it is one half of a velocity (dog::move_toward_current_waypoint is
// *    position + move_speed * direction * delta) and move_speed already lives
// *    in movement. it is also derived state - determine_direction recomputes it
// *    from position -> next waypoint every time the waypoint changes - so it
// *    only exists while something is moving. a table would carry the field
// *    forever and never read it. its one non-movement consumer is sprite facing
// *    (set_direction_index), and that becomes the movement system writing an
// *    index into renderable_component, not a reason to keep it in position.
// *
// * 2. no movement strategy object. the variation is "what produces the
// *    position delta", and in ECS that is expressed by which components an
// *    entity has, not by a polymorphic member:
// *      dog    = position + movement (speed, direction, path queue), written by
// *               the pathing/npc system and integrated by movement_system
// *      cursor = position + controls, written straight from the mouse by
// *               control_input_system
// *    the cursor is the proof - it has no speed, no facing, no interpolation
// *    (see cursor::on_move_view_frame_event: position + mouse_delta), so it has
// *    nothing to put in a shared movement abstraction. a strategy would also
// *    need a unique_ptr member, which deletes the copy ctors every other
// *    component here defaults, making this one manager move-only alone.
// *    if a third mover ever appears (knockback, conveyor), split a
// *    velocity_component out as the shared *output* and let each producer
// *    system write to it - that is where the abstraction earns its keep.
// *
// * so the cursor takes no movement_component, and no direction. there is no
// * "omnidirectional" direction to give it - not having the component IS the
// * answer. (level_config::directions::all was an attempt at one: {1,1} is not a
// * unit vector so it moves ~1.41x too fast, it has no matching sprite in the
// * direction-indexed arrays, and position_to_node snaps it identically to
// * right.)
void ecs_entities::build_cursor(size_t id){
    // TODO re-enable once position_component drops its direction_scalar_ param
    // component_helpers::register_positional_component(id,
    //     component_builders::build_positional_component(GetMousePosition()));
    // TODO register_controls_component - cursor controls aren't defined yet
    (void) id;
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

void ecs_entities::build_background(size_t id){
    (void) id;
    // the background is just a sprite right ?, att a draw level
}
