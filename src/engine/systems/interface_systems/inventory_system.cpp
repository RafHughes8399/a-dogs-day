#include "hud_events.h"
#include "interface_systems/interface_systems.hpp"
#include "menus.h"
#include "system.h"
#include "debug_log_interface.h"
#include <string>

void systems::inventory_system::on_stored_decoration(const events::store_decoration& event){
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
    debug::log("[inventory_system::store_item] definition_id: " + std::to_string(definition_id)
        + ", category: " + std::to_string(category)
        + ", footprint: " + std::to_string(footprint.x) + " x " + std::to_string(footprint.y)
        + ", pages: " + std::to_string(pages.size()));
    if(pages.empty()){
        debug::log("[inventory_system::store_item] no inventory pages exist, giving up");
        return false;
    }
    auto page = pages.back();
    auto* storage = component_managers::hud_inventory_storage_manager_.get_component(page);
    debug::log("[inventory_system::store_item] trying tail page: " + std::to_string(page)
        + ", storage component found: " + std::to_string(storage != nullptr)
        + ", items on page: " + std::to_string(storage != nullptr ? storage->size() : 0));
    auto position = storage != nullptr ? storage->place(definition_id, footprint) : std::nullopt;
    if(not position){
        debug::log("[inventory_system::store_item] tail page " + std::to_string(page)
            + " rejected the item, creating a new tail");
        auto previous = page;
        page = hud_systems::hud_lifespan_system::get_instance().create_inventory_tail(page, category);
        storage = component_managers::hud_inventory_storage_manager_.get_component(page);
        debug::log("[inventory_system::store_item] new tail page: " + std::to_string(page)
            + " after " + std::to_string(previous) + ", storage component found: " + std::to_string(storage != nullptr)
            + ", pages now: " + std::to_string(menu_navigation_system::get_instance().pages_of(menus::menu_ids::inventory, category).size()));
        position = storage != nullptr ? storage->place(definition_id, footprint) : std::nullopt;
    }
    if(not position){
        debug::log("[inventory_system::store_item] item does not fit even on a fresh page, giving up");
        return false;
    }
    auto* page_position = component_managers::entity_positional_manager_.get_component(page);
    if(page_position == nullptr){
        debug::log("[inventory_system::store_item] page " + std::to_string(page) + " has no positional component");
        return false;
    }
    auto origin = page_position->get_position();
    auto button_position = Vector2Add(origin, *position);
    auto button = hud_systems::hud_lifespan_system::get_instance().request_button(page,
        hud_config::button_kinds::inventory_item_button, button_position, ref.pack());
    if(button.has_value()){ storage->attach_button(*button); }
    debug::log("[inventory_system::store_item] page: " + std::to_string(page)
        + ", relative: " + std::to_string(position->x) + ", " + std::to_string(position->y)
        + ", origin: " + std::to_string(origin.x) + ", " + std::to_string(origin.y)
        + ", button position: " + std::to_string(button_position.x) + ", " + std::to_string(button_position.y)
        + ", button created: " + std::to_string(button.has_value()));
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
