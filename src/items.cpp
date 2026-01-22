#include "items.h"
// ------------------------- builder ------------------------------------- // 
items::item_builder items::i_builder_;


// -------------------------------------- item ---------------------------------- //
// -------------------------------------- inventory item ---------------------------------- //
// -------------------------------------- shop item ---------------------------------- //
bool items::shop_item::can_buy(){
    return locked_state_->can_buy(); // and if the price is greater or equal
}
bool items::shop_item::is_locked(){
    return locked_state_->is_locked();
}

void items::shop_item::on_level_up_event(const events::level_up& event){
    int new_level = event.get_new_level();
    if(new_level >= level_requirement_){
        locked_state_ = std::make_unique<unlocked>();
    }
}