#include "component.h"

void components::recipes_component::select_recipe(size_t index){
    if(index < recipes_.size()){
        selected_recipe_ = index;
    }
}
std::vector<recipe::recipe>& components::recipes_component::get_selected_recipes(){
    return recipes_;
}
recipe::recipe& components::recipes_component::get_selected_recipe(){
    return recipes_[selected_recipe_];
}