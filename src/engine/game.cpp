#include "game.h"
// #include <iostream>
void game::game::update(float delta){
    // ---------------- debug behaviours ----------------
    run_debug_behaviours();
    // ---------------- debug behaviours ----------------

    // deal with queued events 
    // std::cout << "[game update]: update" << std::endl;
    events::global_dispatcher_.process_events(delta);
    maitre_d_.update(delta);
    expediter_.process_serving_jobs();
    expediter_.process_clearing_jobs();
    logger_.update(delta);
    // update the level
    level_.update(delta, frame_count_);
    // then the player
    controls_.check(delta);
    player_.update(delta);
    
    // resets at 20 second intervals 
    // assuming 60 FPS, 20 seconds is 1200 frames
    frame_count_++;
    if(frame_count_ == game_config::twenty_seconds){
        frame_count_ = 0;
    }
    
    return;
}

void game::game::run_debug_behaviours(){
}

void game::game::render(float delta){
    // std::cout << "[game update]: render" << std::endl;
    (void) delta;
    level_.render(frame_count_);
    player_.render();
    menus_.render();
    DrawFPS(25, 25);
    logger_.render();
    return;
}

void game::game::debug(float delta){
    (void) delta;
    return;
}

// ----------------------------------------- ecs_game ----------------------- //
void game::ecs_game::init(){
    // * ids come from lifecycle_ once it allocates them - literals until then.
    // TODO replace with lifecycle_ id allocation
    size_t player_id = 0;
    size_t cursor_id = 1;

    // the player and its cursor - registers the control components the
    // control_input_system reads
    ecs_entities::build_player(player_id, cursor_id);

    // TODO the backdrop
    // ecs_entities::build_background(lifecycle_ id);

    // TODO starting entities - khiri and mack, the starting stations
    // ecs_entities::build_khiri(...); ecs_entities::build_mack(...);
    // ecs_entities::build_table(...); ecs_entities::build_counter(...);

    // TODO menus and hud

    return;
}

void game::ecs_game::update(float delta){
    events::global_dispatcher_.process_events(delta);

    // TODO tick each system as it gains an update. Order is the member
    // declaration order in game.h - lifecycle first so entities created this
    // frame are visible to everything after it, spatial after movement so the
    // index is current before collision and interaction read it.
    (void) lifecycle_;
    (void) input_;
    (void) npc_;
    (void) movement_;
    (void) spatial_;
    (void) collision_;
    (void) interaction_;

    frame_count_++;
    if(frame_count_ == game_config::twenty_seconds){
        frame_count_ = 0;
    }
    return;
}

void game::ecs_game::render(float delta){
    (void) delta;
    rendering_.render(frame_count_);
    DrawFPS(25, 25);
    return;
}

void game::ecs_game::debug(float delta){
    (void) delta;
    return;
}
