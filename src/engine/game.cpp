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
    if(IsKeyPressed(KEY_L)){
        auto queue_side = maitre_d_.get_customer_queue_side();
        auto spawn_position = maitre_d_.get_customer_spawn_position(queue_side);
        std::unique_ptr<events::event> build_dog = std::make_unique<events::build_dog>(
            cafe_config::debug_customer_dog_type,
            spawn_position,
            queue_side);
        event_interface::queue_event(build_dog);
    }
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
