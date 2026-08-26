#include "item_stack.hpp"

size_t item_stack::item::get_id(){
    return item_id_;
}
std::uint8_t item_stack::item::get_count(){
    return count_;
}

// size is the number of item objects - does not consider count 
size_t item_stack::item_stack::size(){
    return items_.size();
}
// total is the number of total items - does consider count
size_t item_stack::item_stack::total(){
    size_t count = 0;
    for(auto item : items_){
        count += item.get_count();
    }
    return count;
}
bool item_stack::item_stack::empty(){
    return size() == 0;
}
// ? i think you've got these mixed up, they should be accessing the back not the front
item_stack::item& item_stack::item_stack::head(){
    return items_.back();
}
void item_stack::item_stack::pop(){
    if(items_.begin() != items_.end()){
        auto end = items_.end();
        end = std::prev(items_.end(), 1);
    }
}
void item_stack::item_stack::push(item item){
   if(empty()){
    items_.push_back(item);
   }
   else{
    auto h = head();
    if(h == item){
        increment(h);
    }
    else{

    }
   }
}
// * is a stack so these two only interact with the head of the list
void item_stack::item_stack::take(){
    decrement(head());
}

void item_stack::item_stack::place(item item){
    increment(item);
}

//  TODO implement 26 / 8 / 26
void item_stack::item_stack::increment(item item){
    (void) item;
}
void item_stack::item_stack::decrement(item item){
    (void) item;
}