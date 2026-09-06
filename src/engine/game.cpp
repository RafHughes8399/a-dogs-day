#include "game.h"
#include "config.h"
#include "entity.h"

void game::game::init(){
    // toggle subscribes the log handler, so it has to come first or every step
    // below logs into nothing
    debug::logger::get_instance().toggle();
    debug::log("[game::init, start] building the starting world");

    // the background is an entity now - position and renderable only, drawn by
    // the background layer like anything else. created first so it is bottom of
    // its layer's insertion order.
    auto background_id = lifespan_.create(&ecs_entities::build_background, level_config::background);
    debug::log("[game::init, built background] id "
        + std::to_string(background_id));

    // the player and its cursor - two entities, so two creates
    auto cursor_id = lifespan_.create(&ecs_entities::build_cursor, level_config::cursor);
    debug::log("[game::init, built cursor] id " + std::to_string(cursor_id));

    auto player_id = lifespan_.create([cursor_id](size_t id){
        ecs_entities::build_player(id, cursor_id);
    }, level_config::hud);
    debug::log("[game::init, built player] id " + std::to_string(player_id)
        + " holding cursor " + std::to_string(cursor_id));

    auto khiri_id = lifespan_.create([](size_t id) -> void{
        ecs_entities::build_khiri(id);
    }, level_config::dogs);
    debug::log("[game::init, built khiri] id " + std::to_string(khiri_id));

    auto mack_id = lifespan_.create([](size_t id) -> void{
        ecs_entities::build_mack(id);
    }, level_config::dogs);
    debug::log("[game::init, built mack] id " + std::to_string(mack_id));

    //* -------------------------------------------------- STATION CREATE --------------------------------------------------------------------
    lifespan_.create_counter(entity_config::counters::food_counter, Vector2{level_config::edge_weight * 12, level_config::edge_weight * 4});
    lifespan_.create_table(entity_config::tables::dining_table, Vector2{level_config::edge_weight * 6, level_config::edge_weight * 6});
    //* ------------------------------------------------- WAITER CREATE---------------------------------------------------------------------
    lifespan_.create_waiter_dog(entity_config::waiters::gianluca, Vector2 {level_config::edge_weight * 13, level_config::edge_weight * 6});
    lifespan_.create_waiter_dog(entity_config::waiters::lionel, Vector2{level_config::edge_weight * 20, level_config::edge_weight * 9});

    // * and some decorations
    auto gargoyle_id = lifespan_.create([](size_t id)-> void {
        ecs_entities::build_gargoyle(id, Vector2{level_config::edge_weight * 7,level_config::edge_weight *2});
    }, level_config::draw_layers::decoration);
    auto poker_table_id = lifespan_.create([](size_t id)-> void {
        ecs_entities::build_poker_table(id, Vector2{level_config::edge_weight * 10,level_config::edge_weight *20});
    }, level_config::draw_layers::decoration);
    auto dog_painting__id = lifespan_.create([](size_t id)-> void {
        ecs_entities::build_dog_painting(id, Vector2{level_config::edge_weight * 9,level_config::edge_weight * 4});
    }, level_config::draw_layers::decoration);
    // TODO menus and hud

    debug::log("[game::init, done] built "
        + std::to_string(component_helpers::num_registered_components(khiri_id))
        + " components on khiri");
    debug::log("[game::init, done] built "
        + std::to_string(component_helpers::num_registered_components(mack_id))
        + " components on mack");

    return;
}

void game::game::update(float delta){
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
    animation_.update(delta);

    frame_count_++;
    if(frame_count_ == game_config::twenty_seconds){
        frame_count_ = 0;
    }
    return;
}

void game::game::render(float delta){
    (void) delta;
    rendering_.render(frame_count_);
    DrawFPS(25, 25);
    return;
}

void game::game::debug(float delta){
    (void) delta;
    debug::logger::get_instance().render();
    return;
}
