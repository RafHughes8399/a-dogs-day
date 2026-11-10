#ifndef INVENTORY_SYSTEM_H
#define INVENTORY_SYSTEM_H
#include "events.h"
#include "events_interface.h"
#include "inventory.hpp"

namespace inventory {
    class inventory_system {
    public:
        static inventory_system& get_instance(){
            static inventory_system instance;
            return instance;
        }
        ~inventory_system(){
            event_interface::unsubscribe<events::store_decoration>(store_decoration_handler_);
        }
        inventory_system(const inventory_system& other) = delete;
        inventory_system(inventory_system&& other) = delete;

        inventory_system& operator=(const inventory_system& other) = delete;
        inventory_system& operator=(inventory_system&& other) = delete;

        void on_stored_decoration(const events::store_decoration& event);
        inventory& get_inventory(){
            return inventory_;
        }
    private:
        inventory_system()
        : store_decoration_handler_([this](const events::store_decoration& event) -> void{on_stored_decoration(event);}){
            event_interface::subscribe<events::store_decoration>(store_decoration_handler_);
        }
        inventory inventory_;
        events::event_handler<events::store_decoration> store_decoration_handler_;
    };
}
#endif
