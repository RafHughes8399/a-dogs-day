#include "game.h"

void game::game::update(float delta){
    // update the level
    //std::cout << "frame: " << frame_count_ << std::endl; 
    events::global_dispatcher_.process_events(delta);
    level_.update(delta);
    // then the player
    player_.update(delta);
    //frame_count_++;

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