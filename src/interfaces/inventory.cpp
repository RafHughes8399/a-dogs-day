#include "inventory.hpp"
#include "items.h"
#include <algorithm>

inventory::inventory::item_node* inventory::inventory::find(size_t item_id){
    auto* current = head_;
    while(current != nullptr){
        if(current->item.get_id() == item_id){
            return current;
        }
        current = current->next;
    }
    return nullptr;
}
bool inventory::inventory::add(size_t item_id){
    auto* existing = find(item_id);
    if(existing != nullptr){
        existing->item.adjust_count(1);
        return true;
    }
    auto* new_node = new item_node(items::inventory_item(item_id), nullptr, head_);
    if(head_ != nullptr){
        head_->prev = new_node;
    }
    head_ = new_node;
    return true;
}
bool inventory::inventory::remove(size_t item_id){
    // TODO update inventory item construction - adjust_count only increments,
    // so removing one from a stack of several currently removes the whole node
    auto* node = find(item_id);
    if(node == nullptr){
        return false;
    }
    if(node->prev != nullptr){
        node->prev->next = node->next;
    }
    if(node->next != nullptr){
        node->next->prev = node->prev;
    }
    if(node == head_){
        head_ = node->next;
    }
    delete node;
    return true;
}
std::optional<size_t> inventory::inventory::take(size_t item_id){
    if(remove(item_id)){
        return item_id;
    }
    return std::nullopt;
}

bool inventory::inventory::empty() const{
    return head_ == nullptr;
}
inventory::inventory::item_node* inventory::inventory::get_items(){
    return head_;
}

void inventory::inventory_interface::add_page(){

}
void inventory::inventory_interface::remove_page(){

}

void inventory::inventory_interface::next_page(){

}
void inventory::inventory_interface::previous_page(){
    
}
void inventory::inventory_interface::store_item(size_t item_id){
    inventory_.add(item_id);
}
void inventory::inventory_interface::take_item(size_t item_id){

}
void inventory::inventory_interface::update(float delta){

}
void inventory::inventory_interface::render(){
    // render the current page
}