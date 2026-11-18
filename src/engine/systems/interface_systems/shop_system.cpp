#include "hud_config.h"
#include "system.h"

void systems::shop_system::init(){
    hud_systems::hud_lifespan_system::get_instance().create_shop_menu();

}
