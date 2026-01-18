#include "raglib.h"
bool raglib::bounding_box_2::operator==(const bounding_box_2& other){
    return Vector2Equals(min, other.min) && Vector2Equals(max, other.max);
}
std::ostream& raglib::operator<<(std::ostream& stream, raglib::bounding_box_2& box){
    stream << box.min.x << ", " << box.min.y << " | " << box.max.x << ", " << box.max.y;
    return stream;
}


bool raglib::bounding_box_2::contains(const bounding_box_2& other) const{
    // other is contained in this
    bool contained_x = other.min.x >= min.x && max.x >= other.max.x; // greater than min less than max
    bool contained_y = other.min.y >= min.y && max.y >= other.max.y;
    return contained_x && contained_y;
}
bool raglib::bounding_box_2::contains(const Rectangle& box) const{
    // other is contained in this
    auto min = Vector2 {box.x, box.y};
    auto max = Vector2 {box.x + box.width, box.y + box.height};
    auto bounds = bounding_box_2(min, max);
    return contains(bounds);

}
