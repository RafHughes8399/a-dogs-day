#include "shared_systems/hud_systems.hpp"
#include "debug_log_interface.h"
#include <string>

void hud_systems::hud::init(){

}
void hud_systems::hud::update(float delta_time){
    (void) delta_time;
}
void hud_systems::hud::render(int frame){
    debug::log("[hud::render] frame: " + std::to_string(frame));
    hud_rendering_.render(frame);
}