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
    if(pages.empty()){ return false; }
    auto page = pages.back();
    auto* storage = component_managers::hud_inventory_storage_manager_.get_component(page);
    auto position = storage != nullptr ? storage->place(definition_id, footprint) : std::nullopt;
    if(not position){
        page = hud_systems::hud_lifespan_system::get_instance().create_inventory_menu(hud_config::inventory_page_frame);
        storage = component_managers::hud_inventory_storage_manager_.get_component(page);
        position = storage != nullptr ? storage->place(definition_id, footprint) : std::nullopt;
    }
    if(not position){ return false; }
    auto origin = component_managers::entity_positional_manager_.get_component(page)->get_position();
    auto button = hud_systems::hud_lifespan_system::get_instance().request_button(page,
        hud_config::button_kinds::inventory_item_button, Vector2Add(origin, *position), definition_id);
    return button.has_value();
}
std::optional<size_t> systems::inventory_system::take_item(size_t item_id){
    (void) item_id;
    // attempt to take teh item from the current page
    return std::nullopt;
}
void systems::inventory_system::init(){
    hud_systems::hud_lifespan_system::get_instance().create_inventory_menu(hud_config::inventory_page_frame);
}
