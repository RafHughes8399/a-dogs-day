#ifndef TESTING_HELPERS_H
#define TESTING_HELPERS_H
#include "config.h"
#include "system.h"
#include <cstddef>
namespace helpers{
    
    inline bool is_dog(size_t dog){
        switch(dog){
            case entity_config::player_dog_kind:
            return true;
            break;
        case entity_config::waiter_dog_kind:
            return true;
            break;
        case entity_config::customer_dog_kind:
        return true;
        break;
        default:
        return false;
        break;
    }
}

    inline void add_items_to_counter(size_t counter, size_t item, int count){
        for(int i = 0; i < count; ++i){
            systems::item_system::get_instance().place_item(counter, item);
        }
    }
}
#endif
