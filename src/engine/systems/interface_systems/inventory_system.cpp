#include "hud_events.h"
#include "interface_systems/interface_systems.hpp"
#include "menus.h"
#include "system.h"
#include "debug_log_interface.h"
#include <string>

void systems::inventory_system::on_stored_item(const events::store_item& event){
    items::item_ref ref{event.get_category(), event.get_definition_id()};
    if(not items::item_encyclopedia::get_instance().has_definition(ref)){ return; }
    if(not store_item(ref)){ return; }
}

void systems::inventory_system::on_item_from_inventory(const events::item_from_inventory& event){
    take_item(event.get_button());
}
bool systems::inventory_system::store_item(const items::item_ref& ref){
    auto definition_id = ref.get_id();
    auto footprint = items::inventory_layout_of(ref).get_footprint();
    auto category = ref.get_category();
    auto pages = menu_navigation_system::get_instance().pages_of(menus::menu_ids::inventory, category);
    if(pages.empty()){
        return false;
    }
    auto page = pages.back();
    auto* storage = component_managers::hud_inventory_storage_manager_.get_component(page);
    if(not storage->can_place()){
        auto previous = page;
        page = hud_systems::hud_lifespan_system::get_instance().create_inventory_tail(page, category);
        storage = component_managers::hud_inventory_storage_manager_.get_component(page);
    }
    auto* page_position = component_managers::entity_positional_manager_.get_component(page);
    if(not page_position or not storage){
        return false;
    }
    auto button_position = Vector2Add(page_position->get_position(), storage->place_at(ref));
    auto button = hud_systems::hud_lifespan_system::get_instance().request_button(page,
        hud_config::button_kinds::inventory_item_button, button_position, ref.pack());
    if(button.has_value()){ storage->attach_button(*button); }
    return button.has_value();
}
bool systems::inventory_system::take_item(size_t button){
    auto page = menu_navigation_system::get_instance().active_page_of(menus::menu_ids::inventory);
    if(not page){ return false; }
    auto* storage = component_managers::hud_inventory_storage_manager_.get_component(*page);
    if(storage == nullptr or not storage->take_item(button)){ return false; }
    hud_systems::hud_lifespan_system::get_instance().destroy_element(button);
    return true;
}
void systems::inventory_system::init(){
    for(size_t category = 0; category < static_cast<size_t>(entity_config::item_categories_size); ++category){
        hud_systems::hud_lifespan_system::get_instance().create_inventory_head(category);
    }
}
