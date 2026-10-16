#include "shop.hpp"

size_t shop::item_definition::get_id() const{
    return id_;
}
const std::string& shop::item_definition::get_name() const{
    return name_;
}
int shop::item_definition::get_price() const{
    return price_;
}
int shop::item_definition::get_level_requirement() const{
    return level_requirement_;
}
bool shop::item_definition::is_unlocked(int level) const{
    return level >= level_requirement_;
}

const std::vector<shop::item_definition>& shop::shop::get_catalogue() const{
    return catalogue_;
}
bool shop::shop::can_buy(size_t item_id, int level, int funds) const{
    (void) item_id;
    (void) level;
    (void) funds;
    return false;
}
bool shop::shop::buy(size_t item_id, int level, int& funds, inventory::inventory& into){
    (void) item_id;
    (void) level;
    (void) funds;
    (void) into;
    return false;
}
