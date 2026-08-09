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