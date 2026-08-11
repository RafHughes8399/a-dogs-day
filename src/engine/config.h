
/**
 * inline definitions for certain values like sprite dimensions, world attributes
 * key values, etc
 */
#ifndef CONFIG_H
#define CONFIG_H

#include "raylib.h"
#include <cstddef>
#include <raymath.h>
#include <vector>
namespace type_config{
    // * an ordered run of world positions to walk through. Lives here rather
    // * than inside any one class so the graph, the query layer, the movement
    // * component and the dogs all name the same type.
    using path = std::vector<Vector2>;
}
namespace game_config {
    // * keyboard and mouse actions are separate enums, and `control`/`mouse_input`
    // * are separate structs, so a keyboard binding cannot carry a mouse action
    // * (or vice versa). While both lived in one enum, {KEY_F, mouse_press} was
    // * a legal value that nothing would catch.
    enum control_action{
        key_press = 0,
        key_hold = 1,
        key_release = 2
    };
    enum mouse_action{
        mouse_press = 3,
        mouse_hold = 4,
        mouse_released = 5,
        mouse_up = 6
    };
    enum control_input{
        dog_switch = KEY_F,
        shop_open = KEY_S,
        inventory_open = KEY_I,
        menu_open = KEY_TAB,
        quests_open = KEY_Q,
        map_open = KEY_M,
        back = KEY_ESCAPE,
        move_down = KEY_DOWN,
        move_up  = KEY_UP,
        move_left = KEY_LEFT,
        move_right = KEY_RIGHT
    };
    struct input{
        int key_;
        int action_;
    };
    inline std::vector<input> player_controls = {
        {KEY_LEFT, key_hold},
        {KEY_RIGHT, key_hold},
        {KEY_DOWN, key_hold},
        {KEY_UP, key_hold}
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
}
namespace player_config{
    inline const int max_bones = 999999;
    inline const int max_level = 50;
}
namespace feature_flag_config{
    inline const bool automatic_arrivals = false;
}
namespace level_config{
    // world dimensions
    inline const float screen_width = static_cast<float>(game_config::window_width);
    inline const float screen_height = static_cast<float>(game_config::window_height);
    inline float world_x = 2048.0f;
    inline float world_y = 2048.0f;
    
    // for the main level graph
    inline const float edge_weight = 64.0f; // placeholder

    inline const int screen_edges_x = static_cast<int>(screen_width / edge_weight);
    inline const int screen_edges_y = static_cast<int>(screen_height / edge_weight);

    inline const size_t mack_id = 0;
    inline const size_t khiri_id = 1;

    // the same spawns level_builder::build_main_level places them at
    inline const Vector2 mack_start = Vector2{edge_weight * 7.0f,
                                              edge_weight * 4.0f};
    inline const Vector2 khiri_start = Vector2{edge_weight * 4.0f,
                                               edge_weight * 3.5f};

    inline const Vector2 frame_move = Vector2{375, 375};
    inline const float void_move = edge_weight * 0.125;
    enum draw_layers{
        background = 0,
        decoration = 1,
        stations = 2,
        dogs = 3,
        hud = 4,
        cursor = 5,
        size = 6
    };

    // these indices are shared with the direction-ordered sprite arrays
    // (dog::set_direction_index indexes body_/head_ with a directions value), so
    // an entry here without a matching sprite is a trap for whoever adds an
    // animation next.
    // TODO drop 'all' - it was an attempt at an omnidirectional cursor
    // TODO direction, but {1,1} is not a unit vector so it moves ~1.41x too fast
    // TODO when multiplied by move_speed, it has no sprite, and level_graph::
    // TODO position_to_node snaps it identically to 'right'. an entity with no
    // TODO facing wants no direction at all - i.e. no movement_component. see
    // TODO the note above ecs_entities::build_cursor.
    enum directions{
        left = 0,
        right = 1,
        up = 2,
        down = 3,
        all = 4,
        directions_size = 5
    };
    inline const Vector2 direction_scalars[directions::directions_size] = {
        Vector2{-1, 0}, // left  (index 0)
        Vector2{1, 0},  // right (index 1)
        Vector2{0, -1}, // up    (index 2)
        Vector2{0, 1} ,  // down  (index 3)
        Vector2{1,1}    // all   (index 4) - see TODO above, no sprite for this
    };

}
namespace graph_config{
    inline const int empty_node = -1;
}
namespace cafe_config{
    enum queue_sides{
        left = 0,
        right = 1
    };
    inline const int queue_width_edges = 3;
    inline const int queue_x_edges = 1;
    inline const int queue_y_buffer_edges = 1;
    inline const int queue_y_edges = level_config::screen_edges_y - (2 * queue_y_buffer_edges);
    inline const float queue_gap_edges = 2.0f;
    inline const float queue_arrival_s = 120.0f;
    inline const float queue_left_window_s = 30.0f;
    inline const int queue_left_trigger = 3;
    // How long a customer stays in the eating state before leaving (seconds).
    inline const float eating_duration_s = 10.0f;
    // How long a pickup/placement animation holds a dog still (seconds). One
    // value for all four until one of them needs to differ.
    inline const float animation_duration_s = 0.5f;
    inline const int customer_dog_type = 0;
    inline const Vector2 queue_dir = Vector2{0.0f, 1.0f};
    inline const Vector2 customer_spawn_positions[2] = {
        Vector2{queue_x_edges * level_config::edge_weight, 0 - (2.0f * level_config::edge_weight)},
        Vector2{queue_x_edges * level_config::edge_weight, level_config::screen_height + (2.0f * level_config::edge_weight)}
    };
    inline const float queue_width = queue_width_edges * level_config::edge_weight;
    inline const float queue_height = level_config::screen_height;

    inline const int queue_midpoint_y_edges = level_config::screen_edges_y / 2;
    inline const float queue_midpoint_y = queue_midpoint_y_edges * level_config::edge_weight;
    inline const Vector2 left_queue_head = Vector2{
        queue_x_edges * level_config::edge_weight,
        queue_midpoint_y - level_config::edge_weight
    };
    inline const Vector2 right_queue_head = Vector2{
        queue_x_edges * level_config::edge_weight,
        queue_midpoint_y + level_config::edge_weight
    };
    
    inline const int queue_capacity = queue_y_edges;
    inline const std::vector<Vector2> left_queue_positions = [](){
        auto positions = std::vector<Vector2>{};
        positions.reserve(static_cast<size_t>(queue_capacity));
        for(int index = 0; index < queue_capacity; ++index){
            auto offset = static_cast<float>(index) * queue_gap_edges * level_config::edge_weight;
            positions.push_back(Vector2{left_queue_head.x, left_queue_head.y - offset});
        }
        return positions;
    }();
    inline const std::vector<Vector2> right_queue_positions = [](){
        auto positions = std::vector<Vector2>{};
        positions.reserve(static_cast<size_t>(queue_capacity));
        for(int index = 0; index < queue_capacity; ++index){
            auto offset = static_cast<float>(index) * queue_gap_edges * level_config::edge_weight;
            positions.push_back(Vector2{right_queue_head.x, right_queue_head.y + offset});
        }
        return positions;
    }();
    inline const Rectangle queue_debug_bounds = Rectangle{
        0.0f,
        0.0f,
        queue_width,
        queue_height
    };
    inline const Vector2 cafe_entrance = Vector2Zero();  // TODO placeholder, input actual value 
    inline const Vector2 cafe_exit = Vector2Zero();  // TODO placeholder, input actual value 
    
}
namespace entity_config{
    inline const char* player_dog_debug_id_prefix = "pd_";
    inline const char* customer_dog_debug_id_prefix = "cd_";
    inline const char* npc_dog_debug_id_prefix = "npc_";
    inline const char* cursor_debug_id_prefix = "cursor_";
    inline const char* paw_mark_debug_id_prefix = "paw_";
    inline const char* decoration_debug_id_prefix = "dec_";
    inline const char* table_debug_id_prefix = "tbl_";
    inline const char* food_counter_debug_id_prefix = "fc_";
    inline const char* food_debug_id_prefix = "food_";
    inline const char* waiter_dog_debug_id_prefix = "wd_";
    inline const char* dishwasher_dog_debug_id_prefix = "dwd_";
    inline const char* dishwasher_debug_id_prefix = "dw_";

    enum selectable_kinds{
        player_dog_kind = 0,
        decoration_kind = 1,
        station_kind = 2,
        selectable_kinds_size = 3
    };

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
    inline const float food_counter_attributes[attributes::size] = {level_config::edge_weight * 2.0f, level_config::edge_weight * 2.0f, 1.0f, 1.0f};
    inline const float dishwasher_attributes[attributes::size] = {level_config::edge_weight * 2.0f, level_config::edge_weight * 2.0f, 1.0f, 1.0f};
    // food is a small one-tile entity; it reuses the test_decoration texture for now.
    inline const float test_food_attributes[attributes::size] = {level_config::edge_weight, level_config::edge_weight, 1.0f, 1.0f};
    // how many food a single counter can hold, and where stored food is drawn relative to the counter origin.
    inline const size_t food_counter_capacity = 3;
    inline const Vector2 food_draw_offset = {level_config::edge_weight * 0.5f, level_config::edge_weight * 0.5f};
    // inline const float npc_dog_across_attributes[attributes::size] = {level_config::edge_weight * 2.0f, level_config::edge_weight * 0.75f, 1.0f, 1.0f};
    // inline const float npc_dog_head_across_attributes[attributes::size] = {level_config::edge_weight, level_config::edge_weight, 1.0f, 1.0f};
    // inline const Vector2 npc_dog_head_left_offset = Vector2{0.0f, 0.0f};
    // inline const Vector2 npc_dog_head_right_offset = Vector2{0.0f, 0.0f};
    inline const int dog_eating_duration = game_config::frames * 10;
}
namespace dog_config{
    inline const Vector2 dog_move_speed = {level_config::edge_weight, level_config::edge_weight};
    enum waiter_dog_types{
        basic = 0,
        size = 1
    };
    enum customer_dog_types{
        fred = 0,
        john = 1
    };
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
    
    inline const float edit_wheel_attributes[entity_config::attributes::size] = {35.0f, 35.0f, static_cast<float>(game_config::hold_duration),  1.0f}; // for now, pending animation play speed implementation , frames is 90

}
namespace debug_logger_config{
    inline const int toggle_key = KEY_SLASH;
    inline const int pause_key = KEY_P;
    inline const float logger_height_ratio = 0.6f;
    inline const float logger_y_position_scalar = 1.0f - logger_height_ratio;
    inline const int backdrop_opacity = 102;
    inline const Color backdrop = Color{28, 28, 28, backdrop_opacity};
    inline const Color text = Color{245, 240, 225, 255};
    inline const int font_size = 24;
    inline const int line_height = 36;
    inline const int padding_x = 18;
    inline const int padding_y = 16;
    inline const size_t max_messages = 80;
}
#endif
