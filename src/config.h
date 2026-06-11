
/**
 * inline definitions for certain values like sprite dimensions, world attributes
 * key values, etc
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include "raylib.h"
namespace game_config {
    inline const int frames = 60;
    inline const int twenty_seconds = frames * 60; // 20 seconds in frames
    inline const float hold_duration = frames * 1.2; // 1.2 seconds in frames
    /** Frames to ignore edit-hold after exiting edit (prevents instant re-enter). */
    inline const int edit_cooldown = static_cast<int>(frames * 0.5);
}
namespace player_config{
    inline const int max_bones = 999999;
    inline const int max_level = 50;
}
namespace level_config{
    // world dimensions
    inline float screen_width = GetScreenWidth();
    inline float screen_height = GetScreenHeight();
    inline float world_x = 4096.0f;
    inline float world_y = 4096.0f;
    
    // for the main level graph
    inline const float edge_weight = 64.0f; // placeholder


    inline const int empty_node = -1;
    inline const size_t mack_id = 0;
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
        left = 0,
        right = 1,
        up = 2,
        down = 3,
        directions_size = 4
    };
    inline const Vector2 direction_scalars[directions::directions_size] = {
        Vector2{-1, 0}, // left  (index 0)
        Vector2{1, 0},  // right (index 1)
        Vector2{0, -1}, // up    (index 2)
        Vector2{0, 1}   // down  (index 3)
    };
    
}
namespace entity_config{
    // file paths
    inline const char* background_path = "../sprites/background.png" ;
    inline const char* cursor_path = "../sprites/cursor.png";
    inline const char* paw_mark_path = "../sprites/paw_mark.png";

    inline const char* khiri_left_path = "../sprites/khiri_left.png";
    inline const char* khiri_right_path = "../sprites/khiri_right.png";
    inline const char* khiri_up_path = "../sprites/khiri_up.png";
    inline const char* khiri_down_path = "../sprites/khiri_down.png";
    // Head sprite art pending.
    // inline const char* khiri_head_left_path = "../sprites/khiri_head_left.png";
    // inline const char* khiri_head_right_path = "../sprites/khiri_head_right.png";

    inline const char* khiri_left_outline_path = "../sprites/khiri_left_outline.png";
    inline const char* khiri_right_outline_path = "../sprites/khiri_right_outline.png";
    inline const char* khiri_up_outline_path = "../sprites/khiri_up_outline.png";
    inline const char* khiri_down_outline_path = "../sprites/khiri_down_outline.png";


    inline const char* mack_left_path = "../sprites/mack_left.png";
    inline const char* mack_right_path = "../sprites/mack_right.png";
    inline const char* mack_up_path = "../sprites/mack_up.png";
    inline const char* mack_down_path = "../sprites/mack_down.png";
    // Head sprite art pending.
    // inline const char* mack_head_left_path = "../sprites/mack_head_left.png";
    // inline const char* mack_head_right_path = "../sprites/mack_head_right.png";

    inline const char* mack_left_outline_path = "../sprites/mack_left_outline.png";
    inline const char* mack_right_outline_path = "../sprites/mack_right_outline.png";
    inline const char* mack_up_outline_path = "../sprites/mack_up_outline.png";
    inline const char* mack_down_outline_path = "../sprites/mack_down_outline.png";

    inline const char* test_decoration_path ="../sprites/test_decoration.png";
    inline const char* gargoyle_void_decoration_path = "../sprites/gargoyle_void.png";
    inline const char* gargoyle_sick_of_it_decoration_path = "../sprites/gargoyle_sick_of_it.png";
    // NPC dog sprite art pending.
    // inline const char* npc_dog_left_path = "../sprites/npc_dog_left.png";
    // inline const char* npc_dog_right_path = "../sprites/npc_dog_right.png";
    // inline const char* npc_dog_head_left_path = "../sprites/npc_dog_head_left.png";
    // inline const char* npc_dog_head_right_path = "../sprites/npc_dog_head_right.png";
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
    inline const float khiri_across_attributes[attributes::size] =  {level_config::edge_weight * 2.0f, level_config::edge_weight * 0.75f, 1.0f, 1.0f}; // TODO update values (4/11)
    inline const float khiri_down_attributes[attributes::size] =  {level_config::edge_weight * 0.75f, level_config::edge_weight * 2.0f, 1.0f, 1.0f}; // TODO update values (4/11)
    // inline const float khiri_head_across_attributes[attributes::size] = {level_config::edge_weight, level_config::edge_weight, 1.0f, 1.0f};
    // inline const Vector2 khiri_head_left_offset = Vector2{0.0f, 0.0f};
    // inline const Vector2 khiri_head_right_offset = Vector2{0.0f, 0.0f};
    inline const float mack_across_attributes[attributes::size] =  {level_config::edge_weight * 2.0f, level_config::edge_weight * 0.75f, 1.0f, 1.0f}; // TODO update values (4/11)
    inline const float mack_down_attributes[attributes::size] =  {level_config::edge_weight * 0.75f, level_config::edge_weight * 2.0f, 1.0f, 1.0f}; // TODO update values (4/11)
    // inline const float mack_head_across_attributes[attributes::size] = {level_config::edge_weight, level_config::edge_weight, 1.0f, 1.0f};
    // inline const Vector2 mack_head_left_offset = Vector2{0.0f, 0.0f};
    // inline const Vector2 mack_head_right_offset = Vector2{0.0f, 0.0f};
    inline const float test_decoration_attributes[attributes::size] =  {level_config::edge_weight * 2.0f, level_config::edge_weight * 2.0f, 1.0f, 1.0f}; // TODO update values (3/02)
    inline const float gargoyle_decoration_attributes[attributes::size] = {level_config::edge_weight * 0.75f, level_config::edge_weight * 1.75f, 1.0f, 1.0f};
    inline const float table_attributes[attributes::size] = {level_config::edge_weight * 2.0f, level_config::edge_weight * 2.0f, 1.0f, 1.0f};
    // inline const float npc_dog_across_attributes[attributes::size] = {level_config::edge_weight * 2.0f, level_config::edge_weight * 0.75f, 1.0f, 1.0f};
    // inline const float npc_dog_head_across_attributes[attributes::size] = {level_config::edge_weight, level_config::edge_weight, 1.0f, 1.0f};
    // inline const Vector2 npc_dog_head_left_offset = Vector2{0.0f, 0.0f};
    // inline const Vector2 npc_dog_head_right_offset = Vector2{0.0f, 0.0f};
    inline const Vector2 dog_move_speed = {level_config::edge_weight, level_config::edge_weight};
    
}
namespace controls_config{
    // controls 
    inline std::vector<int> mouse_controls = std::vector<int>{MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT};
    enum key_press_actions{
        dog_switch = KEY_F,
        shop_open = KEY_S,
        inventory_open = KEY_I,
        menu_open = KEY_TAB,
        quests_open = KEY_Q,
        map_open = KEY_M,
        back = KEY_ESCAPE,
        exit_edit = KEY_E
    };
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
namespace hud_config{
    // TODO change values pending test
    inline unsigned char opacity = 120;
    inline Color green_decoration_highlight = Color {0, 255, 0, opacity};
    inline Color red_decoration_highlight = Color {255, 255, 0, opacity};
    inline Color decoration_grid_highlight = Color{243, 252, 255, opacity};
    inline const int decoration_grid_thickness = 3;
    // hud elements paths 
    inline const char* cursor_edit_progres_wheel = "../sprites/edit_wheel.png";
    inline const char* edit_grid = "../sprites/edit_grid.png";
    
    inline const float edit_wheel_attributes[entity_config::attributes::size] = {35.0f, 35.0f, game_config::hold_duration,  1.0f}; // for now, pending animation play speed implementation , frames is 90

}
#endif
