#include "inventory.hpp"
#include "items.h"
#include <algorithm>
#define  MAX_COUNT 64
void inventory::inventory::add(size_t item_id, size_t count){
    auto & items = items_[item_id];
    if(items.empty()) {
        items.emplace_back(items::item(item_id, count));
    }
    for(auto & item : items){
        if(item.get_id() == item_id ){
            if(item.get_count() + count > MAX_COUNT){
                items.emplace_back(items::item(item_id, count));
            }
            else{
                item.adjust_count(count);
            }
        }
    }
}
bool inventory::inventory::remove(size_t item_id, size_t count){
    auto & items = items_[item_id];
    bool removed = false;
    auto it = items.end();
    while (it != items.begin() && count > 0) {
        --it; 
        if (count >= it->get_count()) {
            count -= it->get_count();
            it = items.erase(it);
            removed = true;
        } else {
            it->adjust_count(count);
            count = 0;
            removed = true;
            break;
        }
    }
    return removed;
}
std::optional<size_t> inventory::inventory::take(size_t item_id){
    if(remove(item_id, 1)){
        return item_id;
    }
    return std::nullopt;
}
size_t inventory::inventory::count(size_t item_id) const{
    size_t count = 0;
    auto items = items_.at(item_id);
    std::for_each(items.begin(), items.end(), [&count](const auto& item) -> void {
        count += item.get_count();
        });
    return count;
}
bool inventory::inventory::empty() const{
    return items_.size() == 0;
}
const std::map<size_t, std::vector<items::item>>& inventory::inventory::get_items(){
    return items_;
}

void inventory::inventory_interface::add_page(){

}
void inventory::inventory_interface::remove_page(){

}

void inventory::inventory_interface::next_page(){

}
void inventory::inventory_interface::previous_page(){
    
}