#include "button_factory.hpp"
void factories::button_factory::build_button(size_t kind, size_t id, Vector2 position, size_t subject){
    if(kind >= builders_.size()){ return; }
    builders_[kind](id, position, subject);
}
