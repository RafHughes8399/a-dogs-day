#ifndef RAGLIB_H
#define RAGLIB_H

#include <ostream>
#include "raylib.h"
#include "raymath.h"
#include <string>
namespace raglib{
    struct bounding_box_2{
        Vector2 min;
        Vector2 max;

        bool operator==(const bounding_box_2& other);
        // framed as this.contains(other), is other inside this ?
        bool contains(const bounding_box_2& other) const;
        bool contains(const Rectangle& box) const;
        bool contains(const Vector2& position) const;
    };
    std::ostream& operator<<(std::ostream& stream, bounding_box_2& box);
    

    inline std::string vector_to_string(const Vector2& vector){
        return "{" + std::to_string(vector.x) + ", " + std::to_string(vector.y) + "}";
    }
}
#endif