#include "game.h"
#include "config.h"
#include "entity.h"
// #include <iostream>
void game::game::update(float delta){
    // ---------------- debug behaviours ----------------
    run_debug_behaviours();
    // ---------------- debug behaviours ----------------

    // deal with queued events
    // std::cout << "[game update]: update" << std::endl;
    logger_.set_frame(frame_count_);
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
    // toggle subscribes the log handler, so it has to come first or every step
    // below logs into nothing
    debug::logger::get_instance().toggle();
    debug::log("[ecs_game::init, start] building the starting world");

    // the background is an entity now - position and renderable only, drawn by
    // the background layer like anything else. created first so it is bottom of
    // its layer's insertion order.
    auto background_id = lifespan_.create(&ecs_entities::build_background, level_config::background);
    debug::log("[ecs_game::init, built background] id "
        + std::to_string(background_id));

    // the player and its cursor - two entities, so two creates
    auto cursor_id = lifespan_.create(&ecs_entities::build_cursor, level_config::cursor);
    debug::log("[ecs_game::init, built cursor] id " + std::to_string(cursor_id));

    auto player_id = lifespan_.create([cursor_id](size_t id){
        ecs_entities::build_player(id, cursor_id);
    }, level_config::hud);
    debug::log("[ecs_game::init, built player] id " + std::to_string(player_id)
        + " holding cursor " + std::to_string(cursor_id));

    auto khiri_id = lifespan_.create([](size_t id) -> void{
        ecs_entities::build_khiri(id);
    }, level_config::dogs);
    debug::log("[ecs_game::init, built khiri] id " + std::to_string(khiri_id));

    auto mack_id = lifespan_.create([](size_t id) -> void{
        ecs_entities::build_mack(id);
    }, level_config::dogs);
    debug::log("[ecs_game::init, built mack] id " + std::to_string(mack_id));

    auto counter_id = lifespan_.create([](size_t id) -> void {
        ecs_entities::build_counter(id, Vector2 {level_config::edge_weight * 12, level_config::edge_weight * 4});
    }, level_config::draw_layers::decoration);
    auto table_id = lifespan_.create([](size_t id) -> void {
        ecs_entities::build_table(id, Vector2 {level_config::edge_weight * 6, level_config::edge_weight * 6});
    }, level_config::draw_layers::decoration);
    auto stove_id = lifespan_.create([](size_t id) -> void {
        ecs_entities::build_stove(id, Vector2 {level_config::edge_weight * 16, level_config::edge_weight * 7});
    }, level_config::draw_layers::decoration);
    
    // TODO menus and hud

    debug::log("[ecs_game::init, done] built "
        + std::to_string(component_helpers::num_registered_components(khiri_id))
        + " components on khiri");
    debug::log("[ecs_game::init, done] built "
        + std::to_string(component_helpers::num_registered_components(mack_id))
        + " components on mack");

    return;
}

void game::ecs_game::update(float delta){
    debug::logger::get_instance().set_frame(frame_count_);
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
    debug::logger::get_instance().render();
    return;
}
