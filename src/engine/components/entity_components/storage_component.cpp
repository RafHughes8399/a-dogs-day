#include "component.h"

bool components::storage_component::empty() const{
    return items_.empty();
}
size_t components::storage_component::size() const{
    return items_.size();
}
item_stack::item& components::storage_component::head(){
    return items_.head();
}
size_t components::storage_component::take(){
    auto& h = head();
    auto h_id = h.get_id();
    --h;
    if(h.get_count() == 0){
        items_.pop();
    }
    return h_id;
}
void components::storage_component::place(size_t item_id){
    items_.push(item_id);
}
