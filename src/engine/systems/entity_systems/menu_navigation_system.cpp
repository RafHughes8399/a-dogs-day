#include "system.h"

void systems::menu_navigation_system::init(){
    for(size_t menu = 0; menu < static_cast<size_t>(menus::menu_ids_size); ++menu){
        if(not factory_.has_root(menu)){ continue; }
        auto root = entity_lifespan_system::get_instance().create_hud_root(
            [this, menu](size_t id) -> void { factory_.build_menu(menu, id); });
        menus_.add_root(menu, root);
    }
}
void systems::menu_navigation_system::on_key_press(const events::key_press& event){
    auto old_menu = menus_.current_;
    auto new_menu = menus_.navigate(event.get_key());
    if(new_menu == old_menu){ return; }
    hide(old_menu);
    show(new_menu);
}
void systems::menu_navigation_system::add_root(size_t menu, size_t root){
    menus_.add_root(menu, root);
}
void systems::menu_navigation_system::next_page(){
    hide(menus_.current_);
    menus_.get_node(menus_.current_).next_root();
    show(menus_.current_);
}
void systems::menu_navigation_system::previous_page(){
    hide(menus_.current_);
    menus_.get_node(menus_.current_).previous_root();
    show(menus_.current_);
}
const std::vector<size_t>& systems::menu_navigation_system::pages_of(size_t menu){
    return menus_.get_node(menu).get_roots();
}
void systems::menu_navigation_system::show(size_t menu){
    auto root = menus_.get_node(menu).get_active_root();
    if(not root){ return; }
    auto* hud = component_managers::hud_manager_.get_component(*root);
    if(hud == nullptr){ return; }
    hud->set_active(true);
}
void systems::menu_navigation_system::hide(size_t menu){
    auto root = menus_.get_node(menu).get_active_root();
    if(not root){ return; }
    auto* hud = component_managers::hud_manager_.get_component(*root);
    if(hud == nullptr){ return; }
    hud->set_active(false);
}
