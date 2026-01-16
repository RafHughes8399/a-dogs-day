#include "game.h"

void game::game::update(float delta){
    // update the level
    events::global_dispatcher_.process_events(delta);
    level_.update(delta);
    // then the player
    player_.update(delta);
    
    // resets at 20 second intervals 
    // assuming 60 FPS, 20 seconds is 1200 frames
    frame_count_++;
    if(frame_count_ == game_config::twenty_seconds){
        frame_count_ = 0;
    }

    // deal with the event queue
    return;
}

void game::game::render(float delta){
    (void) delta;
    level_.render();
    player_.render();
    return;
}

void game::game::debug(float delta){
    (void) delta;
    return;
}