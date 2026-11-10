#include "inventory.hpp"
#include "items.h"
#include "debug_log_interface.h"
#include <algorithm>
#include <string>

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
bool inventory::inventory::store_item(size_t item_id){
    auto* existing = find(item_id);
    if(existing != nullptr){
        debug::log("[inventory::store_item, before] item_id: " + std::to_string(item_id)
            + " already held, count: " + std::to_string(existing->item.get_count()));
        existing->item.adjust_count(1);
        debug::log("[inventory::store_item, after] item_id: " + std::to_string(item_id)
            + " count: " + std::to_string(existing->item.get_count()));
        return true;
    }
    debug::log("[inventory::store_item, before] item_id: " + std::to_string(item_id)
        + " not held, creating new node");
    auto* new_node = new item_node(items::inventory_item(item_id), nullptr, head_);
    if(head_ != nullptr){
        head_->prev = new_node;
    }
    head_ = new_node;
    debug::log("[inventory::store_item, after] item_id: " + std::to_string(item_id)
        + " count: " + std::to_string(new_node->item.get_count()) + ", is new head: " + std::to_string(head_ == new_node));
    return true;
}

bool inventory::inventory::take_item(size_t item_id){
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
    if(take_item(item_id)){
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
