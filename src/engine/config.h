
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
namespace interaction_config{
    enum interactions{
        customer_table_sit = 0,
        waiter_table_serve,
        size
    };
    // * ---------------------- DOG INTERACTOR INTERACTIONS ------------------------- // 
    inline std::vector<size_t> waiter_dog_interactor = {
        waiter_table_serve
    };
    inline std::vector<size_t> customer_dog_interactor = {
        customer_table_sit
    };
    // * ------------------- STATION INTERACTEE INTERACTIONS ----------------------------- //
    inline std::vector<size_t> table_interactee = {
        customer_table_sit,
        waiter_table_serve,
    };
}
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
    inline const float edge_weight = 64.0f; // placeholder
    
    inline float world_x = 3072.0f;
    inline float world_y = 2048.0f;
    
    inline float graph_x = 0.0f;
    inline float footpath_overhang = 3 * edge_weight;
    inline float graph_y = -footpath_overhang;
    inline float graph_width = world_x;
    inline float graph_height = world_y + footpath_overhang;
    
    // footpath bounds
    inline float footpath_x = 0.0f;
    inline float footpath_y = graph_y;
    inline float footpath_width = 5.0f * edge_weight;
    inline float footpath_height = graph_height + footpath_overhang;
    // cafe_bounds 
    inline float zone_overlap = 1.0f * edge_weight;
    inline float cafe_x = footpath_x + footpath_width - zone_overlap;
    inline float cafe_y = 0.0f;
    inline float cafe_width = world_x - cafe_x;
    inline float cafe_height = world_y;

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
        decoration,
        stations,
        dogs,
        hud,
        cursor,
        size
    };

    // these indices are shared with the direction-ordered sprite arrays
    // (dog::set_direction_index indexes body_/head_ with a directions value), so
    // an entry here without a matching sprite is a trap for whoever adds an
    // animation next.
    // TODO (25 / 8 / 26) drop 'all' - it was an attempt at an omnidirectional cursor
    // TODO direction, but {1,1} is not a unit vector so it moves ~1.41x too fast
    // TODO when multiplied by move_speed, it has no sprite, and level_graph::
    // TODO position_to_node snaps it identically to 'right'. an entity with no
    // TODO facing wants no direction at all - i.e. no movement_component. see
    // TODO the note above ecs_entities::build_cursor.
    enum directions{
        left = 0,
        right,
        up,
        down,
        all,
        directions_size
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
        right
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
    // * halfway up the cafe, on the seam the two zones share - x in
    // * [cafe_x, footpath_x + footpath_width) sits inside both areas, and
    // * level_config::cafe_x is the one grid column both graphs hold a node
    // * for, so a path can end here from the footpath and start here into the cafe
    inline const Vector2 cafe_entrance = Vector2{
        level_config::cafe_x,
        level_config::cafe_y + level_config::cafe_height * 0.5f
    };
    inline const Vector2 cafe_exit = Vector2Zero();  // TODO (25 / 8 / 26) placeholder, input actual value
    
}
namespace station_config{
    inline const float station_reach = level_config::edge_weight * 0.25f;
}
namespace animation_config{
    // * sheet rows, one enum per dog sprite slot. indices 0 - shared_size are the
    // * same animation on every part, so a whole-body action plays with one index
    // * across all four slots; part-specific rows start at shared_size.
    // * index 0 is what a freshly built sprite shows - animation::current_animation_
    // * starts there - so it is the resting row on every part.
    enum shared{
        idle = 0,
        downward_dog,
        eating,
        shared_size
    };
    namespace tail{
        enum tags{
            idle = shared::idle,
            downward_dog = shared::downward_dog,
            eating = shared::eating,
            wag = shared::shared_size,
            size
        };
    }
    namespace body{
        enum tags{
            idle = shared::idle,
            downward_dog = shared::downward_dog,
            eating = shared::eating,
            walking = shared::shared_size,
            pawing,
            size
        };
    }
    namespace head{
        enum tags{
            idle = shared::idle,
            downward_dog = shared::downward_dog,
            eating = shared::eating,
            pinned_back = shared::shared_size,
            one_up,
            bouncing,
            size
        };
    }
    namespace face{
        enum tags{
            idle = shared::idle,
            downward_dog = shared::downward_dog,
            eating = shared::eating,
            sniffing = shared::shared_size,
            panting,
            licking,
            size
        };
    }
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
        decoration_kind,
        station_kind,
        customer_dog_kind,
        waiter_dog_kind,
        selectable_kinds_size
    };

    enum customers{
        tex = 0,
        customers_size
    };
    enum special_customers{
        garfield = customers_size,
        cumulative_customers_size
    };
    enum waiters{
        gianluca = 0,
        lionel,
        waiters_size
    };
    enum tables{
        dining_table = 0,
        tables_size
    };
    enum counters{
        food_counter = 0,
        counters_size
    };
    enum foods{
        lasagna = 0,
        coffee,
        foods_size
    };
    enum counter_sprite_slots{
        counter_body = 0,
        counter_food,
        counter_sprite_slots_size
    };
    enum dog_sprite_slots{
        dog_head = 0,
        dog_face,
        dog_body,
        dog_tail,
        dog_sprite_slots_size
    };
    enum dog_part_directions{
        dog_part_left = 0,
        dog_part_right,
        dog_part_directions_size
    };

    // * authored in left-facing space; the right-facing sprite mirrors x across
    // * the dog's across width. advances == false anchors the part to the
    // * preceding advancing part, and its offset.x is relative to that anchor.
    struct dog_part{
        const float* attributes;
        Vector2 offset;
        bool advances;
        const char* left_path;
        const char* right_path;
    };

    // file paths
    // * ------------------------ ENVIRONMENT AND CURSOR PATHS --------------------------------------- *//
    inline const char* background_path = "../sprites/background.png" ;
    inline const char* cursor_path = "../sprites/cursor.png";
    inline const char* paw_mark_path = "../sprites/paw_mark.png";
    
    // * ------------------------ PLAYER DOG PATHS  --------------------------------------- *//
    inline const char* khiri_left_path = "../sprites/khiri_left.png";
    inline const char* khiri_right_path = "../sprites/khiri_right.png";
    inline const char* khiri_head_left_path = "../sprites/khiri_head_left.png";
    inline const char* khiri_head_right_path = "../sprites/khiri_head_right.png";
    inline const char* khiri_face_left_path = "../sprites/khiri_face_left.png";
    inline const char* khiri_face_right_path = "../sprites/khiri_face_right.png";
    inline const char* khiri_body_left_path = "../sprites/khiri_body_left.png";
    inline const char* khiri_body_right_path = "../sprites/khiri_body_right.png";
    inline const char* khiri_tail_left_path = "../sprites/khiri_tail_left.png";
    inline const char* khiri_tail_right_path = "../sprites/khiri_tail_right.png";

    inline const char* khiri_left_outline_path = "../sprites/khiri_left_outline.png";
    inline const char* khiri_right_outline_path = "../sprites/khiri_right_outline.png";


    inline const char* mack_left_path = "../sprites/mack_left.png";
    inline const char* mack_right_path = "../sprites/mack_right.png";
    inline const char* mack_head_left_path = "../sprites/mack_head_left.png";
    inline const char* mack_head_right_path = "../sprites/mack_head_right.png";
    inline const char* mack_face_left_path = "../sprites/mack_face_left.png";
    inline const char* mack_face_right_path = "../sprites/mack_face_right.png";
    inline const char* mack_body_left_path = "../sprites/mack_body_left.png";
    inline const char* mack_body_right_path = "../sprites/mack_body_right.png";
    inline const char* mack_tail_left_path = "../sprites/mack_tail_left.png";
    inline const char* mack_tail_right_path = "../sprites/mack_tail_right.png";

    inline const char* mack_left_outline_path = "../sprites/mack_left_outline.png";
    inline const char* mack_right_outline_path = "../sprites/mack_right_outline.png";
    // * ------------------------ WAITER DOG PATHS --------------------------------------- *//
    inline const char* gianluca_left_path = "../sprites/gianluca-left.png";
    inline const char* gianluca_right_path = "../sprites/gianluca-right.png";
    inline const char* gianluca_head_left_path = "../sprites/gianluca_head_left.png";
    inline const char* gianluca_head_right_path = "../sprites/gianluca_head_right.png";
    inline const char* gianluca_face_left_path = "../sprites/gianluca_face_left.png";
    inline const char* gianluca_face_right_path = "../sprites/gianluca_face_right.png";
    inline const char* gianluca_body_left_path = "../sprites/gianluca_body_left.png";
    inline const char* gianluca_body_right_path = "../sprites/gianluca_body_right.png";
    inline const char* gianluca_tail_left_path = "../sprites/gianluca_tail_left.png";
    inline const char* gianluca_tail_right_path = "../sprites/gianluca_tail_right.png";
    inline const char* lionel_left_path = "../sprites/lionel-left.png";
    inline const char* lionel_right_path = "../sprites/lionel-right.png";
    inline const char* lionel_head_left_path = "../sprites/lionel_head_left.png";
    inline const char* lionel_head_right_path = "../sprites/lionel_head_right.png";
    inline const char* lionel_face_left_path = "../sprites/lionel_face_left.png";
    inline const char* lionel_face_right_path = "../sprites/lionel_face_right.png";
    inline const char* lionel_body_left_path = "../sprites/lionel_body_left.png";
    inline const char* lionel_body_right_path = "../sprites/lionel_body_right.png";
    inline const char* lionel_tail_left_path = "../sprites/lionel_tail_left.png";
    inline const char* lionel_tail_right_path = "../sprites/lionel_tail_right.png";
    // * ------------------------ DECORATION PATHS --------------------------------------- *//
    
    inline const char* dog_painting_decoration_path = "../sprites/one-dog-goes-this-way.png";
    inline const char* gargoyle_void_decoration_path = "../sprites/gargoyle-void.png";
    inline const char* gargoyle_sick_of_it_decoration_path = "../sprites/gargoyle_sick_of_it.png";
    inline const char* poker_table_decoration_path = "../sprites/dog-poker.png";
    inline const char* test_decoration_path ="../sprites/test_decoration.png";
    
    // * ------------------------ STATION PATHS --------------------------------------- *//
    inline const char* dining_table_station_path = "../sprtes/dining-table.png";
    inline const char* food_counter_station_path = "";
    
    
    // * ------------------------ FOOD PATHS --------------------------------------- *//
    inline const char* lasagna_food_path = "../sprites/lasagna.png";
    inline const char* coffee_food_path = "../sprites/coffee.png";
    
    // sprite attributes, stored as an array of four numbers [frame width, frame height, frames, animations]
    enum attributes{
        frame_width = 0,
        frame_height,
        frames,
        animations,
        size
    };
    
    inline const float background_attributes[attributes::size] = {3840.0f, 2160.0f, 1.0f, 1.0f};
    inline const float cursor_attributes[attributes::size] = {25.0f, 25.0f, 1.0f, 2.0f}; 
    inline const float paw_mark_attributes[attributes::size] =  {20.0f, 20.0f, 81.0f, 1.0f};
    inline const float khiri_across_attributes[attributes::size] =  {level_config::edge_weight * 2.0f, level_config::edge_weight * 0.75f, 1.0f, 1.0f}; // TODO update values (25 / 8 / 26)
    inline const float mack_across_attributes[attributes::size] =  {level_config::edge_weight * 2.0f, level_config::edge_weight * 0.75f, 1.0f, 1.0f}; // TODO update values (25 / 8 / 26)
    inline const float gianluca_attributes[attributes::size] =  {level_config::edge_weight * 2.25f, level_config::edge_weight * 0.91f, 1.0f, 1.0f};
    inline const float lionel_attributes[attributes::size] =  {level_config::edge_weight * 1.75f, level_config::edge_weight * 0.75f, 1.0f, 1.0f};

    // TODO (06 / 09 / 26) placeholder splits - advancing widths sum to the matching
    // across width, but the proportions are guesses pending the part art
    inline const float khiri_head_attributes[attributes::size] = {level_config::edge_weight * 0.60f, level_config::edge_weight * 0.75f, 1.0f, static_cast<float>(animation_config::head::size)};
    inline const float khiri_face_attributes[attributes::size] = {level_config::edge_weight * 0.35f, level_config::edge_weight * 0.30f, 1.0f, static_cast<float>(animation_config::face::size)};
    inline const float khiri_body_attributes[attributes::size] = {level_config::edge_weight * 1.00f, level_config::edge_weight * 0.75f, 1.0f, static_cast<float>(animation_config::body::size)};
    inline const float khiri_tail_attributes[attributes::size] = {level_config::edge_weight * 0.40f, level_config::edge_weight * 0.50f, 1.0f, static_cast<float>(animation_config::tail::size)};

    inline const float mack_head_attributes[attributes::size] = {level_config::edge_weight * 0.60f, level_config::edge_weight * 0.75f, 1.0f, static_cast<float>(animation_config::head::size)};
    inline const float mack_face_attributes[attributes::size] = {level_config::edge_weight * 0.35f, level_config::edge_weight * 0.30f, 1.0f, static_cast<float>(animation_config::face::size)};
    inline const float mack_body_attributes[attributes::size] = {level_config::edge_weight * 1.00f, level_config::edge_weight * 0.75f, 1.0f, static_cast<float>(animation_config::body::size)};
    inline const float mack_tail_attributes[attributes::size] = {level_config::edge_weight * 0.40f, level_config::edge_weight * 0.50f, 1.0f, static_cast<float>(animation_config::tail::size)};

    inline const float gianluca_head_attributes[attributes::size] = {level_config::edge_weight * 0.70f, level_config::edge_weight * 0.91f, 1.0f, static_cast<float>(animation_config::head::size)};
    inline const float gianluca_face_attributes[attributes::size] = {level_config::edge_weight * 0.40f, level_config::edge_weight * 0.35f, 1.0f, static_cast<float>(animation_config::face::size)};
    inline const float gianluca_body_attributes[attributes::size] = {level_config::edge_weight * 1.10f, level_config::edge_weight * 0.91f, 1.0f, static_cast<float>(animation_config::body::size)};
    inline const float gianluca_tail_attributes[attributes::size] = {level_config::edge_weight * 0.45f, level_config::edge_weight * 0.60f, 1.0f, static_cast<float>(animation_config::tail::size)};

    inline const float lionel_head_attributes[attributes::size] = {level_config::edge_weight * 0.50f, level_config::edge_weight * 0.75f, 1.0f, static_cast<float>(animation_config::head::size)};
    inline const float lionel_face_attributes[attributes::size] = {level_config::edge_weight * 0.30f, level_config::edge_weight * 0.30f, 1.0f, static_cast<float>(animation_config::face::size)};
    inline const float lionel_body_attributes[attributes::size] = {level_config::edge_weight * 0.90f, level_config::edge_weight * 0.75f, 1.0f, static_cast<float>(animation_config::body::size)};
    inline const float lionel_tail_attributes[attributes::size] = {level_config::edge_weight * 0.35f, level_config::edge_weight * 0.50f, 1.0f, static_cast<float>(animation_config::tail::size)};

    inline const dog_part khiri_parts[dog_sprite_slots_size] = {
        {khiri_head_attributes, Vector2{0.0f, 0.0f}, true, khiri_head_left_path, khiri_head_right_path},
        {khiri_face_attributes, Vector2{level_config::edge_weight * 0.15f, level_config::edge_weight * 0.15f}, false, khiri_face_left_path, khiri_face_right_path},
        {khiri_body_attributes, Vector2{0.0f, 0.0f}, true, khiri_body_left_path, khiri_body_right_path},
        {khiri_tail_attributes, Vector2{0.0f, level_config::edge_weight * 0.15f}, true, khiri_tail_left_path, khiri_tail_right_path}};

    inline const dog_part mack_parts[dog_sprite_slots_size] = {
        {mack_head_attributes, Vector2{0.0f, 0.0f}, true, mack_head_left_path, mack_head_right_path},
        {mack_face_attributes, Vector2{level_config::edge_weight * 0.15f, level_config::edge_weight * 0.15f}, false, mack_face_left_path, mack_face_right_path},
        {mack_body_attributes, Vector2{0.0f, 0.0f}, true, mack_body_left_path, mack_body_right_path},
        {mack_tail_attributes, Vector2{0.0f, level_config::edge_weight * 0.15f}, true, mack_tail_left_path, mack_tail_right_path}};

    inline const dog_part gianluca_parts[dog_sprite_slots_size] = {
        {gianluca_head_attributes, Vector2{0.0f, 0.0f}, true, gianluca_head_left_path, gianluca_head_right_path},
        {gianluca_face_attributes, Vector2{level_config::edge_weight * 0.18f, level_config::edge_weight * 0.18f}, false, gianluca_face_left_path, gianluca_face_right_path},
        {gianluca_body_attributes, Vector2{0.0f, 0.0f}, true, gianluca_body_left_path, gianluca_body_right_path},
        {gianluca_tail_attributes, Vector2{0.0f, level_config::edge_weight * 0.18f}, true, gianluca_tail_left_path, gianluca_tail_right_path}};

    inline const dog_part lionel_parts[dog_sprite_slots_size] = {
        {lionel_head_attributes, Vector2{0.0f, 0.0f}, true, lionel_head_left_path, lionel_head_right_path},
        {lionel_face_attributes, Vector2{level_config::edge_weight * 0.12f, level_config::edge_weight * 0.15f}, false, lionel_face_left_path, lionel_face_right_path},
        {lionel_body_attributes, Vector2{0.0f, 0.0f}, true, lionel_body_left_path, lionel_body_right_path},
        {lionel_tail_attributes, Vector2{0.0f, level_config::edge_weight * 0.15f}, true, lionel_tail_left_path, lionel_tail_right_path}};
    inline const float test_decoration_attributes[attributes::size] =  {level_config::edge_weight * 2.0f, level_config::edge_weight * 2.0f, 1.0f, 1.0f}; // TODO update values (25 / 8 / 26)
    inline const float gargoyle_decoration_attributes[attributes::size] = {40.0f, 70.0f, 1.0f, 1.0f};
    inline const float poker_table_attributes[attributes::size] = {level_config::edge_weight * 5, level_config::edge_weight * 3, 1.0f, 1.0f}; // TODO update values (24/08/26)
    inline const float dog_painting_attributes[attributes::size] = {level_config::edge_weight * 2, level_config::edge_weight * 2.25f, 1.0f, 1.0f}; // TODO update values (24/08/26)
    
    inline const float table_attributes[attributes::size] = {level_config::edge_weight * 2.0f, level_config::edge_weight * 2.0f, 1.0f, 1.0f};
    inline const float dining_table_attributes[attributes::size] = {level_config::edge_weight * 1.5f, level_config::edge_weight * 1.5f, 1.0f, 1.0f};

    inline const float food_counter_attributes[attributes::size] = {level_config::edge_weight * 2.0f, level_config::edge_weight * 2.0f, 1.0f, 1.0f};
    inline const float dishwasher_attributes[attributes::size] = {level_config::edge_weight * 2.0f, level_config::edge_weight * 2.0f, 1.0f, 1.0f};
    inline const float stove_attributes[attributes::size] = {level_config::edge_weight * 2.0f, level_config::edge_weight * 2.0f, 1.0f, 1.0f};
    // food is a small one-tile entity; it reuses the test_decoration texture for now.
    inline const float test_food_attributes[attributes::size] = {level_config::edge_weight, level_config::edge_weight, 1.0f, 1.0f};
    inline const float lasagna_attributes[attributes::size] = {level_config::edge_weight, level_config::edge_weight, 1.0f, 1.0f};
    inline const float coffee_attributes[attributes::size] = {level_config::edge_weight, level_config::edge_weight, 1.0f, 1.0f};
    // * legacy entities::station capacity. the ECS stations size their capacity
    // * off the slot offset lists below instead.
    inline const size_t food_counter_capacity = 3;
    inline const Vector2 station_slot_left  = Vector2Zero();
    inline const Vector2 station_slot_right = Vector2Zero(); 
    inline const Vector2 station_slot_up    = Vector2Zero();
    inline const Vector2 station_slot_down  = Vector2Zero(); 

    inline const float station_reach = level_config::edge_weight * 0.25f;
    // where stored food is drawn relative to the counter origin.
    inline const Vector2 food_draw_offset = {level_config::edge_weight * 0.5f, level_config::edge_weight * 0.5f};
    inline const int dog_eating_duration = game_config::frames * 10;
}
namespace dog_config{
    inline const Vector2 dog_move_speed = {level_config::edge_weight, level_config::edge_weight};
    inline const float customer_spawn_interval = 20.0f;
    inline const float dog_reach = level_config::edge_weight * 0.3f;

    inline const float waiter_idle_bounds_edges = 3.0f;
    inline const size_t waiter_idle_min_points = 2;
    inline const size_t waiter_idle_max_points = 4;
    inline const float waiter_idle_cooldown_min = 4.0f;
    inline const float waiter_idle_cooldown_max = 5.0f;
    inline const size_t waiter_idle_max_attempts = 3;

    enum waiter_dog_types{
        basic = 0,
        size
    };
    enum customer_dog_types{
        fred = 0,
        john
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
    // ? antiicpating the need for multiple control schemes, one for the home level, and one for the 
    // ? resource collecting levels 
}
namespace hud_config{
    // TODO (25 / 8 / 26) change values pending test
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
    inline const int toggle_key = KEY_J;
    inline const int pause_key = KEY_P;
    inline const float logger_height_ratio = 0.6f;
    inline const float logger_y_position_scalar = 1.0f - logger_height_ratio;
    inline const int backdrop_opacity = 102;
    inline const Color backdrop = Color{28, 28, 28, backdrop_opacity};
    inline const Color text = Color{245, 240, 225, 255};
    // max_messages lines at line_height must fit inside the backdrop, which is
    // logger_height_ratio of the screen less padding_y top and bottom
    inline const int font_size = 20;
    inline const int line_height = 30;
    inline const int padding_x = 18;
    inline const int padding_y = 16;
    inline const size_t max_messages = 20;
}
#endif
