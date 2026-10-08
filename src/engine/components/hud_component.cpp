#include "component.h"

bool components::hud_component::is_active() const{
    return active_;
}
void components::hud_component::set_active(bool active){
    active_ = active;
}
sprite::nine_sprite& components::hud_component::get_base(){
    return base_;
}
Vector2 components::hud_component::get_offset() const{
    return offset_;
}
