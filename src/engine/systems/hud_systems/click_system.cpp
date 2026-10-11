#include "hud_systems/hud_systems.hpp"

bool hud_systems::click_system::click(Vector2 mouse_screen){
    (void) mouse_screen;
    return false;
}
void hud_systems::click_system::select_recipe(size_t button, int payload){
    (void) button;
    (void) payload;
}
void hud_systems::click_system::change_page(size_t button, int payload){
    (void) button;
    (void) payload;
}
