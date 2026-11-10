#include "hud_systems/hud_systems.hpp"

// ! THESE FUNCTIONS SHOULD TRIGGER EVENTS ? I THINK MOST OF THEM SHOULD 


bool hud_systems::click_system::click(Vector2 position){
    auto hit = hud_spatial_system::get_instance().check_collision_with(position);
    if(hit == game_config::empty_entity){ return false; }
    auto hud_id = static_cast<size_t>(hit);
    auto* clickable = component_managers::hud_clickable_manager_.get_component(hud_id);
    if(clickable == nullptr or clickable->get_action() >= defined_clicks_.size()){ return true; }
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
    (void) payload;
}
void hud_systems::click_system::move_decoration(size_t button, const components::click_payload& payload){
    (void) payload;
    // dont need a payload
    // the hud element id, i think we need a way for the hud element to know what entity it is attached to 
    // that way we can grab the entity and then from the entity get the item id that it is 

    // then trigger a store inventory event with the data
    auto owner = component_managers::hud_owner_manager_.get_component(button);
    if(not owner) {return ;}

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
    std::unique_ptr<events::event> store_decoration = std::make_unique<events::store_decoration>(owner->get_owner(), item->get_item());
    event_interface::queue_event(store_decoration);
}
