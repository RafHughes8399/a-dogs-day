#include "system.h"

void systems::quest_system::init(){
        hud_systems::hud_lifespan_system::get_instance().create_quest_menu();
}