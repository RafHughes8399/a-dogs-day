#include "raglib.h"
bool raglib::bounding_box_2::operator==(const bounding_box_2& other){
    return Vector2Equals(min, other.min) && Vector2Equals(max, other.max);
}

bool raglib::bounding_box_2::contains(const bounding_box_2& other) const{
    return  min.x <= other.min.x &&  min.y <= other.min.y &&
    other.max.x <= max.x  && other.max.y <= max.y;
}