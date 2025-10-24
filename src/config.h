
/**
 * inline definitions for certain values like sprite dimensions, world attributes
 * key values, etc
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include "raylib.h"
namespace config{
    // file paths
    inline const char* background_path = "../sprites/background.png" ;
    inline const char* cursor_path = "../sprites/cursor.png";
    inline const char* paw_mark_path = "../sprites/paw_mark.png";
    // sprite attributes, stored as an array of four numbers [frame width, frame height, frames, animations]
    enum attributes{
        frame_width = 0,
        frame_height = 1,
        frames = 2,
        animations = 3,
        size = 4
    };
    inline const float background_attributes[attributes::size] = {3840.0f, 2160.0f, 1.0f, 1.0f};
    inline const float cursor_attributes[attributes::size] = {10.0f, 10.0f, 1.0f, 1.0f}; // TODO make bigger
    inline const float paw_mark_attributes[attributes::size] =  {20.0f, 20.0f, 30.0, 1.0f};
    // world dimensions
    inline float world_x = 4096.0f;
    inline float world_y = 4096.0f;

    // controls 
    inline std::vector<int> mouse_controls = std::vector<int>{MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT};
    // ? antiicpating the need for multiple control schemes, one for the home level, and one for the 
    // ? resource collecting levels 
}
#endif