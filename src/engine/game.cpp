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
    expediter_.process_orders();
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
