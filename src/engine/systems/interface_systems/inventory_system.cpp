#include "menus.h"
#include "system.h"
#include "debug_log_interface.h"
#include <string>

void systems::inventory_system::on_stored_decoration(const events::store_decoration& event){
    debug::log("[inventory_system::on_stored_decoration] received store_decoration event, entity_id: "
        + std::to_string(event.get_entity_id()) + ", definition_id: " + std::to_string(event.get_definition_id()));
    if(not store_item(event.get_definition_id())){ return; }
    entity_lifespan_system::get_instance().destroy_decoration(event.get_entity_id());
}
bool systems::inventory_system::store_item(size_t definition_id){
    auto footprint = items::inventory_layout_of(definition_id).get_footprint();
    auto pages = menu_navigation_system::get_instance().pages_of(menus::menu_ids::inventory);
    debug::log("[inventory_system::store_item] definition_id: " + std::to_string(definition_id)
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
        page = hud_systems::hud_lifespan_system::get_instance().create_inventory_tail(page);
        storage = component_managers::hud_inventory_storage_manager_.get_component(page);
        debug::log("[inventory_system::store_item] new tail page: " + std::to_string(page)
            + " after " + std::to_string(previous) + ", storage component found: " + std::to_string(storage != nullptr)
            + ", pages now: " + std::to_string(menu_navigation_system::get_instance().pages_of(menus::menu_ids::inventory).size()));
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
        hud_config::button_kinds::inventory_item_button, button_position, definition_id);
    debug::log("[inventory_system::store_item] page: " + std::to_string(page)
        + ", relative: " + std::to_string(position->x) + ", " + std::to_string(position->y)
        + ", origin: " + std::to_string(origin.x) + ", " + std::to_string(origin.y)
        + ", button position: " + std::to_string(button_position.x) + ", " + std::to_string(button_position.y)
        + ", button created: " + std::to_string(button.has_value()));
    return button.has_value();
}
std::optional<size_t> systems::inventory_system::take_item(size_t item_id){
    (void) item_id;
    // attempt to take teh item from the current page
    return std::nullopt;
}
void systems::inventory_system::init(){
    hud_systems::hud_lifespan_system::get_instance().create_inventory_head();
}
