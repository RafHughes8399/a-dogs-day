#include "items.h"

size_t items::item::get_id(){
    return item_id_;
}
std::uint8_t items::item::get_count() const{
    return count_;
}
void items::item::adjust_count(size_t delta){
    count_ += delta;
}
