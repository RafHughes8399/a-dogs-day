#include "raglib.h"
bool raglib::bounding_box_2::operator==(const bounding_box_2& other){
    return Vector2Equals(min, other.min) && Vector2Equals(max, other.max);
}
