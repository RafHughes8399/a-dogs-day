#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include "enum_config.h"
#include "raylib.h"
#include <vector>

namespace game_config {
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
#endif
