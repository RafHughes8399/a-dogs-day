#include "inventory_system.hpp"
#include "debug_log_interface.h"
#include <string>

void inventory::inventory_system::on_stored_decoration(const events::store_decoration& event){
    debug::log("[inventory_system::on_stored_decoration] received store_decoration event, entity_id: "
        + std::to_string(event.get_entity_id()) + ", item_id: " + std::to_string(event.get_item_id()));
    auto stored = inventory_.store_item(event.get_item_id());
    debug::log("[inventory_system::on_stored_decoration] store_item returned: " + std::to_string(stored));
}
