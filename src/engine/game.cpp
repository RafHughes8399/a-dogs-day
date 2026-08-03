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
    // the player and its cursor - two entities, so two creates
    auto cursor_id = lifespan_.create(&ecs_entities::build_cursor, level_config::cursor);
    lifespan_.create([cursor_id](size_t id){
        ecs_entities::build_player(id, cursor_id);
    }, level_config::hud);

    lifespan_.create(&ecs_entities::build_background, level_config::background);

    // TODO starting entities - khiri and mack, the starting stations
    // ecs_entities::build_khiri(...); ecs_entities::build_mack(...);
    // ecs_entities::build_table(...); ecs_entities::build_counter(...);

    // TODO menus and hud

    return;
}

void game::ecs_game::update(float delta){
    events::global_dispatcher_.process_events(delta);

    // tick order is the member declaration order in game.h
    lifespan_.update(delta);
    input_.update(delta);
    npc_.update(delta);
    movement_.update(delta);
    spatial_.update(delta);
    collision_.update(delta);
    interaction_.update(delta);

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
