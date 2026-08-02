#include "entity.h"

// * definitions are qualified with ecs_entities:: so they bind to the
// * declarations in entity.h. A name that doesn't match one is a compile error
// * here, rather than a silently unrelated function that leaves the declaration
// * undefined until a call site fails to link.

void ecs_entities::build_player(size_t player_id, size_t cursor_id){
    // build control components, start with the array keys, 
    // TODO  literals for now, witll deffine in enum when refactor is complete
    component_helpers::register_controls_component(player_id,
         component_builders::build_controls_component(game_config::player_controls));
    // build the cursor,
    // ?  do we need an attachment compnent, a has_a_component ?
    build_cursor(cursor_id);
}
void ecs_entities::destroy_player(size_t player_id, size_t cursor_id){
    component_helpers::unregister_all_components(player_id);
    destroy_cursor(cursor_id);
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
void ecs_entities::destroy_player_dog(size_t id){
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
void ecs_entities::destroy_customer_dog(size_t id){
    (void) id;
}

void ecs_entities::build_waiter_dog(size_t id){
    (void) id;
}
//**
// build_saba()
// build text
//  */
void ecs_entities::destroy_waiter_dog(size_t id){
    (void) id;
}

void ecs_entities::build_cursor(size_t id){
    // 
    (void) id;
}
void ecs_entities::destroy_cursor(size_t id){
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
void ecs_entities::destroy_decoration(size_t id){
    (void) id;
}

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
void ecs_entities::destroy_station(size_t id){
    (void) id;
}

void ecs_entities::build_background(size_t id){
    // the background is just a sprite right ?, att a draw level
}
void ecs_entities::destroy_background(size_t id){
    (void) id;
}
