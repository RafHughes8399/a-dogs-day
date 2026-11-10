#include "items.h"

size_t items::food_item::get_id(){
    return item_id_;
}
std::uint8_t items::food_item::get_count() const{
    return count_;
}
void items::food_item::adjust_count(size_t delta){
    count_ += delta;
}

size_t items::inventory_item::get_id() const{
    return item_id_;
}
std::uint8_t items::inventory_item::get_count() const{
    return count_;
}
void items::inventory_item::adjust_count(size_t delta){
    count_ += delta;
}
