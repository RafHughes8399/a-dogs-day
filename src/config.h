
/**
 * inline definitions for certain values like sprite dimensions, world attributes
 * key values, etc
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include "raylib.h"
namespace assets_config{
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
    inline const float cursor_attributes[attributes::size] = {25.0f, 25.0f, 1.0f, 1.0f}; 
    inline const float paw_mark_attributes[attributes::size] =  {20.0f, 20.0f, 81.0f, 1.0f};
    
}
namespace dimensions_config{
    // world dimensions
    inline float world_x = 3840.0f;
    inline float world_y = 2160.0f;
    
    // for the main level graph
    inline const float edge_weight = 64.0f; // placeholder
}
namespace controls_config{
    // controls 
    inline std::vector<int> mouse_controls = std::vector<int>{MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT};
    // ? antiicpating the need for multiple control schemes, one for the home level, and one for the 
    // ? resource collecting levels 
}
#endif