#include "component_managers.hpp"
#include "entity_systems/entity_systems.hpp"
#include "hud_systems/hud_systems.hpp"
#include "debug_log_interface.h"
#include "system.h"
#include <string>

// ! THESE FUNCTIONS SHOULD TRIGGER EVENTS ? I THINK MOST OF THEM SHOULD


bool hud_systems::click_system::click(Vector2 position){
    debug::log("[click_system::click] position: " + std::to_string(position.x) + ", " + std::to_string(position.y));
    auto hit = hud_spatial_system::get_instance().check_collision_with(position);
    debug::log("[click_system::click] collision check hit: " + std::to_string(hit));
    if(hit == game_config::empty_entity){ return false; }
    auto hud_id = static_cast<size_t>(hit);
    auto* clickable = component_managers::hud_clickable_manager_.get_component(hud_id);
    if(clickable == nullptr){
        debug::log("[click_system::click] no clickable component on hud_id: " + std::to_string(hud_id));
        return true;
    }
    debug::log("[click_system::click] clickable component action: " + std::to_string(clickable->get_action()));
    if(clickable->get_action() >= defined_clicks_.size()){
        debug::log("[click_system::click] action index out of range for defined_clicks_, size: " + std::to_string(defined_clicks_.size()));
        return true;
    }
    debug::log("[click_system::click] invoking defined click behaviour, action: " + std::to_string(clickable->get_action()) + " hud_id: " + std::to_string(hud_id));
    defined_clicks_[clickable->get_action()](hud_id, clickable->get_payload());
    return true;
}
void hud_systems::click_system::select_recipe(size_t button, const components::click_payload& payload){
    auto* owner = component_managers::hud_owner_manager_.get_component(button);
    auto* choice = std::get_if<components::recipe_choice>(&payload);
    if(owner == nullptr or choice == nullptr){ return; }
    auto* recipes = component_managers::entity_recipes_manager_.get_component(owner->get_owner());
    if(recipes == nullptr){ return; }
    recipes->select_recipe(choice->get_recipe_id());
}
void hud_systems::click_system::change_page(size_t button, const components::click_payload& payload){
    (void) button;
    auto* turn = std::get_if<components::page_turn>(&payload);
    if(turn == nullptr){ return; }
    if(turn->get_direction() < 0){
        systems::menu_navigation_system::get_instance().previous_page();
        return;
    }
    systems::menu_navigation_system::get_instance().next_page();
}
void hud_systems::click_system::change_category(size_t button, const components::click_payload& payload){
    (void) button;
    auto* choice = std::get_if<components::category_choice>(&payload);
    if(choice == nullptr){ return; }
    systems::menu_navigation_system::get_instance().set_group(menus::menu_ids::inventory, choice->get_category());
}
void hud_systems::click_system::select_item(size_t button, const components::click_payload& payload){
    (void) button;
    (void) payload;
}
void hud_systems::click_system::move_decoration(size_t button, const components::click_payload& payload){
    (void) payload;
    auto owner = component_managers::hud_owner_manager_.get_component(button);
    // get the cursror carrier component
    auto cursor_id = systems::control_input_system::get_instance().get_cursor_id();
    auto cursor_carrier = component_managers::entity_carrier_manager_.get_component(cursor_id);
    if(not cursor_carrier or not owner) {return;}
    cursor_carrier->set_carried_entity(owner->get_owner());
    
    // i think some hud chnges are needed here - i.e the grid, or maybe like an event to signal that a decoration has been picked up could be 
    // helpful

}
void hud_systems::click_system::sell_decoration(size_t button, const components::click_payload& payload){
    (void) payload;
    // get the owner, get the item and get the sell price of the item
    auto owner = component_managers::hud_owner_manager_.get_component(button);
    if(not owner) {return;}
    auto item = component_managers::entity_item_manager_.get_component(owner->get_owner());
    // TODO LOOKUP SELL PRICE IN ITEM SYSTEM DEFINITIONS
    (void) item;
    int price = 0;
    std::unique_ptr<events::event> sell_decoration = std::make_unique<events::sell_decoration>(owner->get_owner(), price);
    event_interface::queue_event(sell_decoration);
    
}
void hud_systems::click_system::store_decoration(size_t button, const components::click_payload& payload){
    (void) payload;
    // get the entity through the owner component
    auto owner = component_managers::hud_owner_manager_.get_component(button);
    if(not owner) {return; }
    // store decoration event  with the owner
    auto item = component_managers::entity_item_manager_.get_component(owner->get_owner());
    if(not item){return;}
    std::unique_ptr<events::event> store_decoration = std::make_unique<events::store_decoration>(owner->get_owner(),
        item->get_ref().get_category(), item->get_ref().get_id());
    event_interface::queue_event(store_decoration);
}
