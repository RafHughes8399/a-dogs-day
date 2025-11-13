#ifndef RAGLIB_H
#define RAGLIB_H

#include <ostream>
#include "raylib.h"
#include "raymath.h"
namespace raglib{
    struct bounding_box_2{
        Vector2 min;
        Vector2 max;

        bool operator==(const bounding_box_2& other);
        // framed as this.contains(other), is other inside this ?
        bool contains(const bounding_box_2& other) const;
        bool contains(const Rectangle& box) const;
    };
    std::ostream& operator<<(std::ostream& stream, bounding_box_2& box);
    
}
#endif