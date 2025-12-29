
/**
 * inline definitions for certain values like sprite dimensions, world attributes
 * key values, etc
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include "raylib.h"
namespace level_config{
    // world dimensions
    inline float world_x = 4096.0f;
    inline float world_y = 4096.0f;
    
    // for the main level graph
    inline const float edge_weight = 64.0f; // placeholder

    enum draw_layers{
        background = 0,
        decoration = 1,
        stations = 2,
        dogs = 3,
        hud = 4,
        cursor = 5,
        size = 6
    };

    enum directions{
        up = 0,
        down = 1,
        left = 2,
        right = 3,
        directions_size = 4
    };
    inline const Vector2 direction_scalars[directions::directions_size] = {
        Vector2{0, -1}, // up 
        Vector2{0, 1},  // down
        Vector2{-1, 0}, // left
        Vector2{0, 1} // right
    };
    
}
namespace assets_config{
    // file paths
    inline const char* background_path = "../sprites/background.png" ;
    inline const char* cursor_path = "../sprites/cursor.png";
    inline const char* paw_mark_path = "../sprites/paw_mark.png";
    inline const char* khiri_path = "../sprites/khiri.png";
    inline const char* mack_path = "../sprites/mack.png";
    // sprite attributes, stored as an array of four numbers [frame width, frame height, frames, animations]
    enum attributes{
        frame_width = 0,
        frame_height = 1,
        frames = 2,
        animations = 3,
        size = 4
    };
    inline const float background_attributes[attributes::size] = {3840.0f, 2160.0f, 1.0f, 1.0f};
    inline const float cursor_attributes[attributes::size] = {25.0f, 25.0f, 1.0f, 2.0f}; 
    inline const float paw_mark_attributes[attributes::size] =  {20.0f, 20.0f, 81.0f, 1.0f};
    inline const float khiri_attributes[attributes::size] =  {level_config::edge_weight * 2, level_config::edge_weight * 0.75, 1.0f, 1.0f}; // TODO update values (4/11)
    inline const float mack_attributes[attributes::size] =  {level_config::edge_weight * 2, level_config::edge_weight * 0.75, 1.0f, 1.0f}; // TODO update values (4/11)

    inline const Vector2 dog_move_speed = {2.5f , 2.5f};
    
}
namespace controls_config{
    // controls 
    inline std::vector<int> mouse_controls = std::vector<int>{MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT};
    // ? antiicpating the need for multiple control schemes, one for the home level, and one for the 
    // ? resource collecting levels 
}
#endif