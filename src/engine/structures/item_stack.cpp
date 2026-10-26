#include "item_stack.hpp"



// size is the number of item objects - does not consider count 
size_t item_stack::item_stack::size() const{
    return items_.size();
}
// total is the number of total items - does consider count
size_t item_stack::item_stack::total() const{
    size_t count = 0;
    for(auto item : items_){
        count += item.get_count();
    }
    return count;
}
bool item_stack::item_stack::empty() const { 
    return size() == 0;
}
// ? i think you've got these mixed up, they should be accessing the back not the front
items::item& item_stack::item_stack::head(){
    return items_.back();
}
void item_stack::item_stack::pop(){
    if(not empty()){
        items_.pop_back();
    }
}
void item_stack::item_stack::push(size_t item_id){
   if(empty()){
        items_.push_back(items::item(item_id));
   }
   else{
    auto& h = head();
    if(h.get_id() == item_id and h.get_count() < STACK_LIMIT){
        ++h;
    }
    else{
        items_.push_back(items::item(item_id));
    }
   }
}
size_t item_stack::item_stack::take(){
    auto& h = head();
    auto h_id = h.get_id();
    --h;
    if(h.get_count() == 0){
        pop();
    }
    return h_id;
}
void item_stack::item_stack::place(size_t item_id){
    push(item_id);
}