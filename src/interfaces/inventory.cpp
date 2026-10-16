#include "inventory.hpp"

void inventory::inventory::add(size_t item_id, size_t count){
    if(count == 0){ return; }
    items_[item_id] += count;
}
bool inventory::inventory::remove(size_t item_id, size_t count){
    auto item = items_.find(item_id);
    if(item == items_.end() or item->second < count){ return false; }
    item->second -= count;
    if(item->second == 0){
        items_.erase(item);
    }
    return true;
}
size_t inventory::inventory::count(size_t item_id) const{
    auto item = items_.find(item_id);
    return item == items_.end() ? 0 : item->second;
}
bool inventory::inventory::empty() const{
    return items_.empty();
}
const std::unordered_map<size_t, size_t>& inventory::inventory::contents() const{
    return items_;
}
