#include "game.h"
// #include <iostream>
void game::game::update(float delta){
    // deal with queued events 
    // std::cout << "[game update]: update" << std::endl;
    events::global_dispatcher_.process_events(delta);
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

void game::game::render(float delta){
    // std::cout << "[game update]: render" << std::endl;
    (void) delta;
    level_.render(frame_count_);
    player_.render();
    menus_.render();
    DrawFPS(25, 25);
    return;
}

void game::game::debug(float delta){
    (void) delta;
    return;
}