#include "component.h"
#include "config.h"
#include <raymath.h>

bool components::movement_component::has_reached_position(Vector2 position){
    if(paths_.empty()){
        return false;
    }
    float position_to_target = Vector2Distance(position, paths_.front().get_next_position());
    return position_to_target <= level_config::edge_weight * 0.05f;
}
path::path& components::movement_component::get_current_path(){
    return paths_.empty() ? path::empty_path : paths_.front();
}
std::queue<path::path>& components::movement_component::get_paths(){
    return paths_;
}
void components::movement_component::append_path(path::path path){
    paths_.push(path);
    return;
}
void components::movement_component::set_path(path::path path){
    if(paths_.empty()){
        append_path(path);
    }
    else{
        // override the current path
        clear_paths();
        paths_.push(path);
    }
}
void components::movement_component::clear_paths(){
    paths_ = {};
}
void components::movement_component::finish_path(){
    if(paths_.empty()){ return; }
    paths_.pop();
}
Vector2 components::movement_component::get_move_speed(){
    return move_speed_;
}
Vector2 components::movement_component::get_direction_scalar(){
    return direction_scalar_;
}
void components::movement_component::set_direction_scalar(Vector2 direction_scalar){
    direction_scalar_ = direction_scalar;
}
