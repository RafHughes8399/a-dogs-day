#include "raglib.h"
bool raglib::bounding_box_2::operator==(const bounding_box_2& other){
    return Vector2Equals(min, other.min) && Vector2Equals(max, other.max);
}

bool raglib::bounding_box_2::contains(const bounding_box_2& other) const{
    // other is contained in this
    bool contained_x = other.min.x >= min.x && max.x >= other.max.x; // greater than min less than max
    bool contained_y = other.min.y >= min.y && max.y >= other.max.y;
    return contained_x && contained_y;
}
