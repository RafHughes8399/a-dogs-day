
/**
 * inline definitions for certain values like sprite dimensions, world attributes
 * key values, etc
 */
#ifndef CONFIG_H
#define CONFIG_H

namespace config{
    // file paths
    inline const char* background_path = "../sprites/background.png" ;
    inline const char* cursor_path = "../sprites/cursor.png";
    // sprite attributes, stored as an array of four numbers [frame width, frame height, frames, animations]
    enum attributes{
        frame_width = 0,
        frame_height = 1,
        frames = 2,
        animations = 3
    };
    inline const float background_attributes[4] = {3840.0f, 2160.0f, 1.0f, 1.0f};
    inline const float cursor_attributes[4] = {10.0f, 10.0f, 1.0f, 1.0f};
    // world dimensions
    
    inline float world_x = 4096.0f;
    inline float world_y = 4096.0f;
}
#endif