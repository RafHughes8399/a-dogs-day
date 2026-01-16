
/**
 * inline definitions for certain values like sprite dimensions, world attributes
 * key values, etc
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include "raylib.h"
namespace game_config {
    inline const int twenty_seconds = 1200;
}
namespace level_config{
    // world dimensions
    inline float world_x = 4096.0f;
    inline float world_y = 4096.0f;
    
    // for the main level graph
    inline const float edge_weight = 64.0f; // placeholder

    inline const size_t mack_id = 2;
    inline const size_t khiri_id = 1;

    inline const Vector2 frame_move = Vector2{375, 375};

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
        // up-left
        // up-right
        Vector2{0, 1},  // down
        // down-left
        // down-right
        Vector2{-1, 0}, // left
        Vector2{1, 0} // right
    };
    
}
namespace assets_config{
    // file paths
    inline const char* background_path = "../sprites/background.png" ;
    inline const char* cursor_path = "../sprites/cursor.png";
    inline const char* paw_mark_path = "../sprites/paw_mark.png";

    inline const char* khiri_left_path = "../sprites/khiri_left.png";
    inline const char* khiri_right_path = "../sprites/khiri_right.png";
    inline const char* khiri_up_path = "../sprites/khiri_up.png";
    inline const char* khiri_down_path = "../sprites/khiri_down.png";

    inline const char* khiri_left_outline_path = "../sprites/khiri_left_outline.png";
    inline const char* khiri_right_outline_path = "../sprites/khiri_right_outline.png";
    inline const char* khiri_up_outline_path = "../sprites/khiri_up_outline.png";
    inline const char* khiri_down_outline_path = "../sprites/khiri_down_outline.png";


    inline const char* mack_left_path = "../sprites/mack_left.png";
    inline const char* mack_right_path = "../sprites/mack_right.png";
    inline const char* mack_up_path = "../sprites/mack_up.png";
    inline const char* mack_down_path = "../sprites/mack_down.png";

    inline const char* mack_left_outline_path = "../sprites/mack_left_outline.png";
    inline const char* mack_right_outline_path = "../sprites/mack_right_outline.png";
    inline const char* mack_up_outline_path = "../sprites/mack_up_outline.png";
    inline const char* mack_down_outline_path = "../sprites/mack_down_outline.png";
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
    inline const float khiri_across_attributes[attributes::size] =  {level_config::edge_weight * 2, level_config::edge_weight * 0.75, 1.0f, 1.0f}; // TODO update values (4/11)
    inline const float khiri_down_attributes[attributes::size] =  {level_config::edge_weight * 0.75, level_config::edge_weight * 2, 1.0f, 1.0f}; // TODO update values (4/11)
    inline const float mack_across_attributes[attributes::size] =  {level_config::edge_weight * 2, level_config::edge_weight * 0.75, 1.0f, 1.0f}; // TODO update values (4/11)
    inline const float mack_down_attributes[attributes::size] =  {level_config::edge_weight * 0.75, level_config::edge_weight * 2, 1.0f, 1.0f}; // TODO update values (4/11)

    inline const Vector2 dog_move_speed = {level_config::edge_weight, level_config::edge_weight};
    
}
namespace controls_config{
    // controls 
    inline std::vector<int> mouse_controls = std::vector<int>{MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT};
    enum key_press_actions{
        switch_dog = KEY_F,
        open_shop = KEY_S,
        open_inventory = KEY_I,
        open_menu = KEY_TAB,
        open_quests = KEY_Q,
        open_map = KEY_M,

    };
    inline std::vector<int> key_controls = std::vector<int>{KEY_F};
    enum key_hold_actions{
        edit_mode = KEY_E,
        move_down = KEY_DOWN,
        move_up  = KEY_UP,
        move_left = KEY_LEFT,
        move_right = KEY_RIGHT
    };
    // ? antiicpating the need for multiple control schemes, one for the home level, and one for the 
    // ? resource collecting levels 
}
#endif