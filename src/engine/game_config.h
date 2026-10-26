#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include "raylib.h"
#include <vector>

namespace game_config {
    // * keyboard and mouse actions are separate enums, and `control`/`mouse_input`
    // * are separate structs, so a keyboard binding cannot carry a mouse action
    // * (or vice versa). While both lived in one enum, {KEY_F, mouse_press} was
    // * a legal value that nothing would catch.
    enum control_action{
        key_press = 0,
        key_hold,
        key_release
    };
    enum mouse_action{
        mouse_press = 3,
        mouse_hold,
        mouse_released,
        mouse_up
    };

    struct input{
        int key_;
        int action_;
    };
    inline const int counter_start_stock = 2;
    inline std::vector<input> player_controls = {
        {KEY_LEFT, key_hold},
        {KEY_RIGHT, key_hold},
        {KEY_DOWN, key_hold},
        {KEY_UP, key_hold},
        {KEY_J, key_press}
    };
    // * the cursor's bindings. Note there is no entry for pointer movement -
    // * that is not a binding, it is ambient device state, and holding a
    // * mouse_input_component at all is what makes an entity mouse-positioned.
    inline std::vector<input> cursor_controls = {
        {MOUSE_BUTTON_LEFT, mouse_press},
        {MOUSE_BUTTON_RIGHT, mouse_press}
    };
    inline const int window_width = 1920;
    inline const int window_height = 1080;
    inline const int frames = 60;
    inline const int twenty_seconds = frames * 60; // 20 seconds in frames
    inline const int hold_duration = static_cast<int>(frames * 1.2f); // 1.2 seconds in frames
    /** Frames to ignore edit-hold after exiting edit (prevents instant re-enter). */
    inline const int edit_cooldown = static_cast<int>(frames * 0.5);
    inline const int empty_entity = -1;

    inline const int cook_speed = 2;
}
namespace controls_config{
    enum key_press_actions{
        dog_switch = KEY_F,
        shop_open = KEY_S,
        inventory_open = KEY_I,
        menu_open = KEY_TAB,
        quests_open = KEY_Q,
        map_open = KEY_M,
        back = KEY_ESCAPE,
        exit_edit = KEY_E,
        debug_toggle = KEY_J
    };
    enum key_hold_actions{
        edit_mode = KEY_E,
        move_down = KEY_DOWN,
        move_up  = KEY_UP,
        move_left = KEY_LEFT,
        move_right = KEY_RIGHT
    };
}
#endif
