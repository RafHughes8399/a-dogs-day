#include "path.h"
Vector2 path::path::get_source(){
    return source_;
}
Vector2 path::path::get_destination(){
    return destination_;
}
Vector2 path::path::get_next_position(){
    return is_path_complete() ? destination_ : positions_.front();
}
bool path::path::is_path_complete(){
    return positions_.empty();
}
void path::path::advance(){
    if(positions_.empty()){ return; }
    positions_.erase(positions_.begin());
}

path::path path::build_path(Vector2 source, Vector2 destination, std::vector<Vector2> positions, std::optional<size_t> destination_entity){
    return path(source, destination, positions, destination_entity);
}
