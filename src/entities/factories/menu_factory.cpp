#include "menu_factory.hpp"
bool factories::menu_factory::has_root(size_t menu) const{
    return menu < builders_.size() and static_cast<bool>(builders_[menu]);
}
void factories::menu_factory::build_menu(size_t menu, size_t root_id){
    if(not has_root(menu)){ return; }
    builders_[menu](root_id);
}
