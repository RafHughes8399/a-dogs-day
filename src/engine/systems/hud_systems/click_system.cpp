#include "hud_systems/hud_systems.hpp"

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
    (void) button;
    (void) payload;
}
void hud_systems::click_system::sell_decoration(size_t button, const components::click_payload& payload){
    (void) button;
    (void) payload;
}
void hud_systems::click_system::store_decoration(size_t button, const components::click_payload& payload){
    (void) button;
    (void) payload;
}
