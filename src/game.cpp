#include "game.h"

void game::game::update(float delta){
    // update the level
    level_.update(delta);
    // then the player
    player_.update(delta);

    // deal with the event queue
    events::global_dispatcher_.process_events(delta);
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