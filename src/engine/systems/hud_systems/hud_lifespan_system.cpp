#include "hud_config.h"
#include "hud_systems/hud_systems.hpp"
#include "menus.h"
#include "system.h"
#include "component_helpers.hpp"
#include "hud_element_builders.hpp"
#include <raylib.h>

size_t hud_systems::hud_lifespan_system::create_menu_page(std::function<void(size_t)> build){
    auto id = systems::entity_lifespan_system::get_instance().next_id();
    build(id);
    return id;
}
size_t hud_systems::hud_lifespan_system::create_inventory_menu(){
    auto frame = hud_config::inventory_page_frame;
    auto anchor = GetScreenToWorld2D(Vector2{hud_config::inventory_page_frame.x, hud_config::inventory_page_frame.y}, systems::rendering_system::get_instance().get_camera());
    auto root = create_menu_page([frame, anchor](size_t id) -> void { page_builders::build_inventory_menu(id, frame, anchor); });
    auto buffer = hud_config::page_nav_button_buffer;
    auto size = hud_config::page_nav_button_size;
    auto bottom =  frame.y + frame.height - size - buffer;
    request_button(root, hud_config::button_kinds::previous_page_button,
        Vector2{frame.x + buffer, bottom});
    request_button(root, hud_config::button_kinds::next_page_button,
        Vector2{ frame.x + frame.width - size - buffer, bottom});
    systems::menu_navigation_system::get_instance().add_root(menus::menu_ids::inventory, root);
    return root;
}
size_t hud_systems::hud_lifespan_system::create_shop_menu(){
    auto frame = hud_config::shop_page_frame;
    auto anchor = GetScreenToWorld2D(Vector2{hud_config::shop_page_frame.x, hud_config::shop_page_frame.y}, systems::rendering_system::get_instance().get_camera());
    auto root = create_menu_page([frame, anchor](size_t id) -> void { page_builders::build_shop_menu(id, frame, anchor); });

    systems::menu_navigation_system::get_instance().add_root(menus::menu_ids::shop, root);
    return root;
}
size_t hud_systems::hud_lifespan_system::create_quest_menu(){
    auto frame = hud_config::quest_page_frame;
    auto anchor = GetScreenToWorld2D(Vector2{hud_config::quest_page_frame.x, hud_config::quest_page_frame.y}, systems::rendering_system::get_instance().get_camera());
    auto root = create_menu_page([frame, anchor](size_t id) -> void { page_builders::build_quest_menu(id, frame, anchor); });

    systems::menu_navigation_system::get_instance().add_root(menus::menu_ids::quest, root);
    return root;
}
std::optional<size_t> hud_systems::hud_lifespan_system::request_button(size_t owner, size_t kind, Vector2 position, size_t subject){
    queries::create_hud_button_query query(owner, kind, position, subject);
    auto id = queries::query_value<size_t>(query_interface::execute_query(query));
    if(not id or *id == static_cast<size_t>(game_config::empty_entity)){ return std::nullopt; }
    return id;
}
size_t hud_systems::hud_lifespan_system::create_element(size_t owner, std::function<void(size_t)> build){
    if(component_managers::hud_manager_.get_component(owner) == nullptr){
        return static_cast<size_t>(game_config::empty_entity);
    }
    auto id = systems::entity_lifespan_system::get_instance().next_id();
    build(id);
    component_helpers::add_hud_owner_component(id, owner);
    component_managers::hud_manager_.get_component(owner)->add_element(id);
    events::create_hud_element created{id, owner};
    event_interface::execute_event(created);
    return id;
}
void hud_systems::hud_lifespan_system::destroy_element(size_t hud_id){
    auto* owner_component = component_managers::hud_owner_manager_.get_component(hud_id);
    if(owner_component == nullptr){ return; }
    auto owner = owner_component->get_owner();
    events::remove_hud_element removed{hud_id, owner};
    event_interface::execute_event(removed);

    if(auto* hud = component_managers::hud_manager_.get_component(owner)){
        hud->remove_element(hud_id);
    }
    component_helpers::unregister_all_hud_components(hud_id);
    systems::entity_lifespan_system::get_instance().recycle(hud_id);
}
void hud_systems::hud_lifespan_system::destroy_elements_of(size_t owner){
    auto* hud = component_managers::hud_manager_.get_component(owner);
    if(hud == nullptr){ return; }
    auto elements = hud->get_elements();
    for(auto element : elements){
        destroy_element(element);
    }
}
