#include "menus.h"
#include "system.h"
#include "debug_log_interface.h"
#include "hud_element_builders.hpp"
#include "query_interface.h"
#include <string>

void systems::inventory_system::on_stored_decoration(const events::store_decoration& event){
    debug::log("[inventory_system::on_stored_decoration] received store_decoration event, entity_id: "
        + std::to_string(event.get_entity_id()) + ", item_id: " + std::to_string(event.get_item_id()));
    auto stored = store_item(event.get_item_id());
    debug::log("[inventory_system::on_stored_decoration] store_item returned: " + std::to_string(stored));
}
bool systems::inventory_system::store_item(size_t item_id){
    (void) item_id;
    auto inventory_pages = menu_navigation_system::get_instance().pages_of(menus::menu_ids::inventory);
    auto last_page = inventory_pages[inventory_pages.size() = 1];
    // get the storage component of the page. and then try to append the item. if full
    last_page;
    return false;
}
std::optional<size_t> systems::inventory_system::take_item(size_t item_id){
    (void) item_id;
    return std::nullopt;
}
void systems::inventory_system::init(){
    build_page();
}
std::optional<Vector2> systems::inventory_system::find_free_position(size_t page, Vector2 size){
    (void) page;
    (void) size;
    return std::nullopt;
}
std::optional<size_t> systems::inventory_system::request_button(size_t owner, size_t kind, Vector2 position, size_t subject){
    queries::create_hud_button_query query(owner, kind, position, subject);
    auto id = queries::query_value<size_t>(query_interface::execute_query(query));
    if(not id or *id == static_cast<size_t>(game_config::empty_entity)){ return std::nullopt; }
    return id;
}
size_t systems::inventory_system::build_page(){
    auto frame = hud_config::inventory_page_frame;
    auto root = entity_lifespan_system::get_instance().create_hud_root(
        [frame](size_t id) -> void { page_builders::build_menu_page(id, frame); });
    auto buffer = hud_config::page_nav_button_buffer;
    auto size = hud_config::page_nav_button_size;
    auto bottom = frame.y + frame.height - size - buffer;
    request_button(root, hud_config::button_kinds::previous_page_button,
        Vector2{frame.x + buffer, bottom});
    request_button(root, hud_config::button_kinds::next_page_button,
        Vector2{frame.x + frame.width - size - buffer, bottom});
    // ? ok but there's no storage compoennt for the hud element to hold anything
    // * so we need an item_page storage compoennt kind of thing. it essentially needs to store an id of the button, for now just consider a list and then
    menu_navigation_system::get_instance().add_root(menus::menu_ids::inventory, root);
    return root;
}
