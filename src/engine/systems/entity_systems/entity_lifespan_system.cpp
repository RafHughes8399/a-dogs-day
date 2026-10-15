#include "system.h"
#include <iostream>
#include <string>
#include "component_helpers.hpp"
#include "hud_element_builders.hpp"
#include <algorithm>

// ---------------- id allocation ----------------
size_t systems::entity_lifespan_system::next_id(){
    if(recycled_ids_.empty()){
        return fresh_id_++;
    }
    auto entity_id = recycled_ids_.front();
    recycled_ids_.pop();
    return entity_id;
}

// create() is a template - defined in system.h.

// ---------------- lifecycle ----------------
void systems::entity_lifespan_system::destroy(size_t entity_id){
    if(auto* hud = component_managers::hud_manager_.get_component(entity_id)){
        auto elements = hud->get_elements();
        for(auto element : elements){
            destroy_hud_element(element);
        }
    }
    // executed before the components go, so listeners can still read them
    events::remove_entity removed{entity_id};
    event_interface::execute_event(removed);

    component_helpers::unregister_all_entity_components(entity_id);
    component_helpers::unregister_all_hud_components(entity_id);
    recycled_ids_.push(entity_id);
}

size_t systems::entity_lifespan_system::create_hud_element(size_t owner, std::function<void(size_t)> build){
    if(component_managers::hud_manager_.get_component(owner) == nullptr){
        return static_cast<size_t>(game_config::empty_entity);
    }
    auto id = next_id();
    build(id);
    component_helpers::add_hud_owner_component(id, owner);
    component_managers::hud_manager_.get_component(owner)->add_element(id);
    events::create_hud_element created{id, owner};
    event_interface::execute_event(created);
    return id;
}
void systems::entity_lifespan_system::destroy_hud_element(size_t hud_id){
    auto* owner_component = component_managers::hud_owner_manager_.get_component(hud_id);
    if(owner_component == nullptr){ return; }
    auto owner = owner_component->get_owner();
    events::remove_hud_element removed{hud_id, owner};
    event_interface::execute_event(removed);

    if(auto* hud = component_managers::hud_manager_.get_component(owner)){
        hud->remove_element(hud_id);
    }
    component_helpers::unregister_all_hud_components(hud_id);
    recycled_ids_.push(hud_id);
}

// TODO (25 / 8 / 26) stub - the loop calls this every frame, nothing to do yet
void systems::entity_lifespan_system::update(float delta){
    (void) delta;
    return;
}
size_t systems::entity_lifespan_system::create_customer_dog(){
    auto id = create([this](size_t id) -> void{ dog_factory_.build_customer_dog(id); },
        level_config::draw_layers::dogs);
    npc_system::get_instance().register_customer(id);
    return id;
}
void systems::entity_lifespan_system::destroy_customer_dog(size_t entity_id){
    destroy(entity_id);
    npc_system::get_instance().unregister_customer(entity_id);
}
size_t systems::entity_lifespan_system::create_waiter_dog(size_t waiter, Vector2 position){
    std::cout << "[entity_lifespan_system::create_waiter_dog] enter waiter=" << waiter
        << " position=(" << position.x << "," << position.y << ")" << std::endl;
    auto id = create([this](size_t waiter_type, size_t entity_id, Vector2 position) -> void{
        std::cout << "[entity_lifespan_system::create_waiter_dog] build lambda"
            << " waiter_type=" << waiter_type << " entity_id=" << entity_id << std::endl;
        dog_factory_.build_waiter_dog(waiter_type, entity_id, position); },
        waiter, position, level_config::draw_layers::dogs);
    std::cout << "[entity_lifespan_system::create_waiter_dog] create_entity announced id="
        << id << std::endl;
    npc_system::get_instance().register_waiter(id);
    std::cout << "[entity_lifespan_system::create_waiter_dog] registered waiter id="
        << id << std::endl;
    return id;
    }
    void systems::entity_lifespan_system::destroy_waiter_dog(size_t entity_id){
        destroy(entity_id);
        npc_system::get_instance().unregister_waiter(entity_id);
}
size_t systems::entity_lifespan_system::create_table(size_t entity, Vector2 position){
    auto id = create([this](size_t id, size_t entity, Vector2 position) -> void{ station_factory_.build_table(id, entity, position); },
        entity, position, level_config::draw_layers::stations);
    npc_system::get_instance().register_table(id);
    return id;
}
void systems::entity_lifespan_system::destroy_table(size_t id){
    destroy(id);
    npc_system::get_instance().unregister_table(id);
}

size_t systems::entity_lifespan_system::create_counter(size_t counter, Vector2 position){
    auto id = create([this](size_t id, size_t counter, Vector2 position) -> void{ station_factory_.build_counter(id, counter, position); },
        counter, position, level_config::draw_layers::stations);
    return id;
}
void systems::entity_lifespan_system::destroy_counter(size_t id){
    destroy(id);
}

size_t systems::entity_lifespan_system::create_cooking_station(size_t cooking_station, Vector2 position){
    auto id = create([this](size_t cooking_station, size_t entity_id, Vector2 position) -> void{
        station_factory_.build_cooking_station(cooking_station, entity_id, position); },
        cooking_station, position, level_config::draw_layers::stations);
    create_cooking_station_hud(id);
    debug::log("[entity_lifespan_system::create_cooking_station] built cooking station: "
        + std::to_string(cooking_station)
        + ", id: " + std::to_string(id)
        + ", position: " + raglib::vector_to_string(position));
    return id;
}
void systems::entity_lifespan_system::destroy_cooking_station(size_t id){
    debug::log("[entity_lifespan_system::destroy_cooking_station] removing cooking station id: "
        + std::to_string(id));
    destroy(id);
}
void systems::entity_lifespan_system::create_cooking_station_hud(size_t owner){
    auto* recipes = component_managers::entity_recipes_manager_.get_component(owner);
    auto* position = component_managers::entity_positional_manager_.get_component(owner);
    if(recipes == nullptr or position == nullptr){ return; }

    auto origin = Vector2Add(position->get_position(), hud_config::cook_station_hud_offset);
    auto page_size = static_cast<size_t>(hud_config::recipe_rows * hud_config::recipe_columns);
    auto count = std::min(recipes->get_selected_recipes().size(), page_size);
    for(size_t recipe = 0; recipe < count; ++recipe){
        auto column = static_cast<float>(recipe % static_cast<size_t>(hud_config::recipe_columns));
        auto row = static_cast<float>(recipe / static_cast<size_t>(hud_config::recipe_columns));
        Vector2 button_position{
            origin.x + hud_config::reicpe_icon_width_buffer * 0.5f
                + column * (hud_config::recipe_icon_width + hud_config::reicpe_icon_width_buffer),
            origin.y + hud_config::recipe_icon_height_buffer * 0.5f
                + row * (hud_config::recipe_icon_height + hud_config::recipe_icon_height_buffer)};
        auto food = recipes->get_selected_recipes()[recipe].get_food_id();
        create_hud_element(owner, [food, button_position, recipe](size_t id){
            button_builders::build_recipe_button(food, id, button_position, static_cast<int>(recipe));
        });
    }
}
size_t systems::entity_lifespan_system::create_stove(Vector2 position){
    return create_cooking_station(entity_config::cooking_stations::stove, position);
}
size_t systems::entity_lifespan_system::create_oven(Vector2 position){
    return create_cooking_station(entity_config::cooking_stations::oven, position);
}
size_t systems::entity_lifespan_system::create_espresso_station(Vector2 position){
    return create_cooking_station(entity_config::cooking_stations::espresso_station, position);
}
size_t systems::entity_lifespan_system::create_mini_fridge(Vector2 position){
    return create_cooking_station(entity_config::cooking_stations::mini_fridge, position);
}
size_t systems::entity_lifespan_system::create_bush(Vector2 position){
    return create_cooking_station(entity_config::cooking_stations::bush, position);
}
size_t systems::entity_lifespan_system::create_chopping_board(Vector2 position){
    return create_cooking_station(entity_config::cooking_stations::chopping_board, position);
}

size_t systems::entity_lifespan_system::create_food(size_t food, Vector2 position){
    debug::log("[entity_lifespan_system, create food]: create food: " + std::to_string(food) + " at posiiton " + std::to_string(position.x) + ", " + std::to_string(position.y));
    auto id = create([this](size_t food, size_t entity_id, Vector2 position) -> void{
        food_factory_.build_food(food, entity_id, position); },
        food, position, level_config::draw_layers::dogs);
    return id;
}
void systems::entity_lifespan_system::destroy_food(size_t id){
    destroy(id);
}