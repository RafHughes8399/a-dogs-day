#ifndef RAGLIB_H
#define RAGLIB_H
#include "raylib.h"
#include "raymath.h"
namespace raglib{
    struct bounding_box_2{
        Vector2 min;
        Vector2 max;

        
        bool operator==(const bounding_box_2& other);
        
    };
}
#endif