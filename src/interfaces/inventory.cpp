#include "inventory.hpp"
#include "items.h"
#include "debug_log_interface.h"
#include <algorithm>
#include <string>

bool inventory::inventory::store_item(size_t item_id){
    auto & page = pages_[pages_.size()-1];
    if(not page.store_item(item_id)){
        // build a new page and insert 
        // auto new_page = build_inventory_page(); 
        //pages_.push_back(new_page));
    }
}
bool inventory::inventory::take_item(size_t item_id){

}
std::optional<size_t> inventory::inventory::take(size_t item_id){

}
bool inventory::inventory::empty() const{

}
void inventory::inventory::render(){

}
void inventory::inventory::next_page(){

}
void inventory::inventory::previous_page(){

}
void inventory::inventory::add_page(){

}
void inventory::inventory::remove_page(){

}

bool inventory::inventory::inventory_page::store_item(size_t item_id){
    if(item_page_.can_fit(item_id)){
        item_page_.store(item_id);
        return true;
    }
    return false;
}