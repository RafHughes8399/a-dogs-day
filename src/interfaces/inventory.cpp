#include "inventory.hpp"
#include "items.h"
#include "debug_log_interface.h"
#include <algorithm>
#include <string>

bool inventory::inventory::empty() const{
    return pages_.empty();
}
void inventory::inventory::next_page(){

}
void inventory::inventory::previous_page(){

}
void inventory::inventory::add_page(size_t root){
    pages_.push_back(root);
}
void inventory::inventory::remove_page(){

}
