#ifndef ENTITY_CONFIG_H
#define ENTITY_CONFIG_H

#include "animation_config.h"
#include "cafe_config.h"
#include "enum_config.h"
#include "raylib.h"
#include "raymath.h"

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

    // * authored in left-facing space; the right-facing sprite mirrors x across
    // * the dog's across width. offset is measured from the left edge of the across width.
    struct dog_part{
        const float* attributes;
        Vector2 offset;
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
    inline const char* khiri_head_left_path = "../sprites/player-dogs/khiri-left-head.png";
    inline const char* khiri_head_right_path = "../sprites/player-dogs/khiri-right-head.png";
    inline const char* khiri_face_left_path = "../sprites/player-dogs/khiri-left-face.png";
    inline const char* khiri_face_right_path = "../sprites/player-dogs/khiri-right-face.png";
    inline const char* khiri_body_left_path = "../sprites/player-dogs/khiri-left-body.png";
    inline const char* khiri_body_right_path = "../sprites/player-dogs/khiri-right-body.png";
    inline const char* khiri_tail_left_path = "../sprites/player-dogs/khiri-left-tail.png";
    inline const char* khiri_tail_right_path = "../sprites/player-dogs/khiri-right-tail.png";

    inline const char* khiri_left_outline_path = "../sprites/khiri_left_outline.png";
    inline const char* khiri_right_outline_path = "../sprites/khiri_right_outline.png";

    inline const char* mack_left_path = "../sprites/mack_left.png";
    inline const char* mack_right_path = "../sprites/mack_right.png";
    inline const char* mack_head_left_path = "../sprites/player-dogs/mack-left-head.png";
    inline const char* mack_head_right_path = "../sprites/player-dogs/mack-right-head.png";
    inline const char* mack_face_left_path = "../sprites/player-dogs/mack-left-face.png";
    inline const char* mack_face_right_path = "../sprites/player-dogs/mack-right-face.png";
    inline const char* mack_body_left_path = "../sprites/player-dogs/mack-left-body.png";
    inline const char* mack_body_right_path = "../sprites/player-dogs/mack-right-body.png";
    inline const char* mack_tail_left_path = "../sprites/player-dogs/mack-left-tail.png";
    inline const char* mack_tail_right_path = "../sprites/player-dogs/mack-right-tail.png";

    inline const char* mack_left_outline_path = "../sprites/mack_left_outline.png";
    inline const char* mack_right_outline_path = "../sprites/mack_right_outline.png";
    // * ------------------------ WAITER DOG PATHS --------------------------------------- *//
    inline const char* gianluca_left_path = "../sprites/gianluca-left.png";
    inline const char* gianluca_right_path = "../sprites/gianluca-right.png";
    inline const char* gianluca_head_left_path = "../sprites/player-dogs/gianluca-left-head.png";
    inline const char* gianluca_head_right_path = "../sprites/player-dogs/gianluca-right-head.png";
    inline const char* gianluca_face_left_path = "../sprites/player-dogs/gianluca-left-face.png";
    inline const char* gianluca_face_right_path = "../sprites/player-dogs/gianluca-right-face.png";
    inline const char* gianluca_body_left_path = "../sprites/player-dogs/gianluca-left-body.png";
    inline const char* gianluca_body_right_path = "../sprites/player-dogs/gianluca-right-body.png";
    inline const char* gianluca_tail_left_path = "../sprites/player-dogs/gianluca-left-tail.png";
    inline const char* gianluca_tail_right_path = "../sprites/player-dogs/gianluca-right-tail.png";
    inline const char* lionel_left_path = "../sprites/lionel-left.png";
    inline const char* lionel_right_path = "../sprites/lionel-right.png";
    inline const char* lionel_head_left_path = "../sprites/player-dogs/lionel-left-head.png";
    inline const char* lionel_head_right_path = "../sprites/player-dogs/lionel-right-head.png";
    inline const char* lionel_face_left_path = "../sprites/player-dogs/lionel-left-face.png";
    inline const char* lionel_face_right_path = "../sprites/player-dogs/lionel-right-face.png";
    inline const char* lionel_body_left_path = "../sprites/player-dogs/lionel-left-body.png";
    inline const char* lionel_body_right_path = "../sprites/player-dogs/lionel-right-body.png";
    inline const char* lionel_tail_left_path = "../sprites/player-dogs/lionel-left-tail.png";
    inline const char* lionel_tail_right_path = "../sprites/player-dogs/lionel-right-tail.png";
    // * ------------------------ DECORATION PATHS --------------------------------------- *//

    inline const char* dog_painting_decoration_path = "../sprites/one-dog-goes-this-way.png";
    inline const char* frangipani_decoration_path = "../sprites/frangipani.png";
    inline const char* gargoyle_void_decoration_path = "../sprites/gargoyle-void.png";
    inline const char* gargoyle_sick_of_it_decoration_path = "../sprites/gargoyle_sick_of_it.png";
    inline const char* pavlov_path = "../sprites/pavlov.png";
    inline const char* poker_table_decoration_path = "../sprites/dog-poker.png";
    inline const char* test_decoration_path ="../sprites/test_decoration.png";

    // * ------------------------ STATION PATHS --------------------------------------- *//
    inline const char* dining_table_station_path = "../sprites/dining-table.png";
    inline const char* tiled_table_station_path ="../sprites/tiled-table.png";
    inline const char* tiled_table_test_path ="../sprites/tiled-table-test.png";
    inline const char* tiled_table_orange_path = "../sprites/tiled-table_733a02.png";
    inline const char* tiled_table_red_path = "../sprites/tiled-table_630802.png";
    inline const char* tiled_table_green_path = "../sprites/tiled-table_1a2f03.png";
    inline const char* tiled_table_blue_path = "../sprites/tiled-table_032b59.png";
    inline const char* food_counter_station_path = "";
    inline const char* stove_station_path = "../sprites/stove.png";
    inline const char* oven_station_path = "../sprites/oven.png";
    inline const char* espresso_station_path = "../sprites/espresso-station.png";
    inline const char* mini_fridge_station_path = "../sprites/mini-fridge.png";
    inline const char* bush_station_path = "../sprites/bush.png";
    inline const char* chopping_board_station_path = "../sprites/chopping-board.png";

    // * ------------------------ FOOD PATHS --------------------------------------- *//
    inline const char* lasagna_food_path = "../sprites/lasagna.png";
    inline const char* espresso_food_path = "../sprites/espresso.png";
    inline const char* cutlets_food_path = "../sprites/cutlets.png";
    inline const char* mutton_food_path = "../sprites/mutton.png"; // TODO art not made yet
    inline const char* muttoff_food_path = "../sprites/muttoff.png"; // TODO art not made yet
    inline const char* prozac_food_path = "../sprites/prozac.png"; // TODO art not made yet
    inline const char* escargot_food_path = "../sprites/escargot.png"; // TODO art not made yet

    inline const float station_width = level_config::edge_weight * 1.5f;
    inline const float station_height = level_config::edge_weight * 1.5f;
    inline const float food_width = level_config::edge_weight * 0.6f;
    inline const float food_height = level_config::edge_weight * 0.6f;

    inline const float background_attributes[attributes::size] = {level_config::world_x, level_config::world_y, 1.0f, 1.0f};
    inline const float cursor_attributes[attributes::size] = {25.0f, 25.0f, 1.0f, 2.0f};
    inline const float paw_mark_attributes[attributes::size] =  {20.0f, 20.0f, 81.0f, 1.0f};
    inline const float khiri_across_attributes[attributes::size] =  {level_config::edge_weight * 2.0f, level_config::edge_weight * 0.75f, 1.0f, 1.0f}; // TODO update values (25 / 8 / 26)
    inline const float mack_across_attributes[attributes::size] =  {level_config::edge_weight * 2.0f, level_config::edge_weight * 0.75f, 1.0f, 1.0f}; // TODO update values (25 / 8 / 26)
    inline const float gianluca_attributes[attributes::size] =  {level_config::edge_weight * 2.25f, level_config::edge_weight * 0.91f, 1.0f, 1.0f};
    inline const float lionel_attributes[attributes::size] =  {level_config::edge_weight * 1.75f, level_config::edge_weight * 0.75f, 1.0f, 1.0f};

    // TODO (06 / 09 / 26) placeholder splits - advancing widths sum to the matching
    // across width, but the proportions are guesses pending the part art
    inline const float khiri_head_attributes[attributes::size] = {35.0f, 62.0f, 1.0f, static_cast<float>(animation_config::head::size)};
    inline const float khiri_face_attributes[attributes::size] = {level_config::edge_weight * 0.35f, level_config::edge_weight * 0.30f, 1.0f, static_cast<float>(animation_config::face::size)};
    inline const float khiri_body_attributes[attributes::size] = {88.0f, 48.0f, 1.0f, static_cast<float>(animation_config::body::size)};
    inline const float khiri_tail_attributes[attributes::size] = {18.0f, 48.0f, 1.0f, static_cast<float>(animation_config::tail::size)};

    inline const float mack_head_attributes[attributes::size] = {30.0f, 48.0f, 1.0f, static_cast<float>(animation_config::head::size)};
    inline const float mack_face_attributes[attributes::size] = {level_config::edge_weight * 0.35f, level_config::edge_weight * 0.30f, 1.0f, static_cast<float>(animation_config::face::size)};
    inline const float mack_body_attributes[attributes::size] = {92.0f, 48.0f, 1.0f, static_cast<float>(animation_config::body::size)};
    inline const float mack_tail_attributes[attributes::size] = {16.0f, 48.0f, 1.0f, static_cast<float>(animation_config::tail::size)};

    inline const float gianluca_head_attributes[attributes::size] = {27.0f, 58.0f, 1.0f, static_cast<float>(animation_config::head::size)};
    inline const float gianluca_face_attributes[attributes::size] = {level_config::edge_weight * 0.40f, level_config::edge_weight * 0.35f, 1.0f, static_cast<float>(animation_config::face::size)};
    inline const float gianluca_body_attributes[attributes::size] = {117.0f, 58.0f, 1.0f, static_cast<float>(animation_config::body::size)};
    inline const float gianluca_tail_attributes[attributes::size] = {4.0f, 58.0f, 1.0f, static_cast<float>(animation_config::tail::size)};

    inline const float lionel_head_attributes[attributes::size] = {35.0f, 48.0f, 1.0f, static_cast<float>(animation_config::head::size)};
    inline const float lionel_face_attributes[attributes::size] = {level_config::edge_weight * 0.30f, level_config::edge_weight * 0.30f, 1.0f, static_cast<float>(animation_config::face::size)};
    inline const float lionel_body_attributes[attributes::size] = {90.0f, 48.0f, 1.0f, static_cast<float>(animation_config::body::size)};
    inline const float lionel_tail_attributes[attributes::size] = {7.0f, 48.0f, 1.0f, static_cast<float>(animation_config::tail::size)};

    inline const dog_part khiri_parts[dog_sprite_slots_size] = {
        {khiri_body_attributes, Vector2{26.0f, 0.0f}, khiri_body_left_path, khiri_body_right_path},
        {khiri_tail_attributes, Vector2{110.0f, 0.0f}, khiri_tail_left_path, khiri_tail_right_path},
        {khiri_head_attributes, Vector2{0.0f, -14.0f}, khiri_head_left_path, khiri_head_right_path},
        {khiri_face_attributes, Vector2{level_config::edge_weight * 0.15f, level_config::edge_weight * 0.15f}, khiri_face_left_path, khiri_face_right_path}};

    inline const dog_part mack_parts[dog_sprite_slots_size] = {
        {mack_body_attributes, Vector2{21.0f, 0.0f}, mack_body_left_path, mack_body_right_path},
        {mack_tail_attributes, Vector2{112.0f, 0.0f}, mack_tail_left_path, mack_tail_right_path},
        {mack_head_attributes, Vector2{0.0f, 0.0f}, mack_head_left_path, mack_head_right_path},
        {mack_face_attributes, Vector2{level_config::edge_weight * 0.15f, level_config::edge_weight * 0.15f}, mack_face_left_path, mack_face_right_path}};

    inline const dog_part gianluca_parts[dog_sprite_slots_size] = {
        {gianluca_body_attributes, Vector2{0.0f, 0.0f}, gianluca_body_left_path, gianluca_body_right_path},
        {gianluca_tail_attributes, Vector2{140.0f, 0.0f}, gianluca_tail_left_path, gianluca_tail_right_path},
        {gianluca_head_attributes, Vector2{0.0f, 0.0f}, gianluca_head_left_path, gianluca_head_right_path},
        {gianluca_face_attributes, Vector2{level_config::edge_weight * 0.18f, level_config::edge_weight * 0.18f}, gianluca_face_left_path, gianluca_face_right_path}};

    inline const dog_part lionel_parts[dog_sprite_slots_size] = {
        {lionel_body_attributes, Vector2{16.0f, 0.0f}, lionel_body_left_path, lionel_body_right_path},
        {lionel_tail_attributes, Vector2{105.0f, 0.0f}, lionel_tail_left_path, lionel_tail_right_path},
        {lionel_head_attributes, Vector2{0.0f, 0.0f}, lionel_head_left_path, lionel_head_right_path},
        {lionel_face_attributes, Vector2{level_config::edge_weight * 0.12f, level_config::edge_weight * 0.15f}, lionel_face_left_path, lionel_face_right_path}};

    inline const float test_decoration_attributes[attributes::size] =  {level_config::edge_weight * 2.0f, level_config::edge_weight * 2.0f, 1.0f, 1.0f}; // TODO update values (25 / 8 / 26)
    inline const float gargoyle_decoration_attributes[attributes::size] = {40.0f, level_config::edge_weight * 1.5f, 1.0f, 1.0f};
    inline const float pavlov_attributes[attributes::size] = {level_config::edge_weight * 1.5f, level_config::edge_weight * 2.75f, 1.0f, 1.0f}; // TODO update values (24/08/26)
    inline const float poker_table_attributes[attributes::size] = {level_config::edge_weight * 5, level_config::edge_weight * 3, 1.0f, 1.0f}; // TODO update values (24/08/26)
    inline const float dog_painting_attributes[attributes::size] = {level_config::edge_weight * 2, level_config::edge_weight * 2.25f, 1.0f, 1.0f}; // TODO update values (24/08/26)
    inline const float frangipani_attributes[attributes::size] = {level_config::edge_weight * 1.5f, level_config::edge_weight * 2.5f, 3.0f, 1.0f}; // TODO update values once exported

    inline const float table_attributes[attributes::size] = {station_width, station_height, 1.0f, 1.0f};
    inline const float dining_table_attributes[attributes::size] = {station_width, station_height, 1.0f, 1.0f};
    inline const float tiled_table_attributes[attributes::size] = {station_width, station_height, 1.0f, 1.0f};

    inline const float food_counter_attributes[attributes::size] = {station_width, station_height, 1.0f, 1.0f};
    inline const float dishwasher_attributes[attributes::size] = {station_width, station_height, 1.0f, 1.0f};
    inline const float stove_attributes[attributes::size] = {station_width, station_height, 1.0f, 1.0f};
    inline const float oven_attributes[attributes::size] = {station_width, station_height, 1.0f, 1.0f};
    inline const float espresso_station_attributes[attributes::size] = {station_width, station_height, 1.0f, 1.0f};
    inline const float mini_fridge_attributes[attributes::size] = {station_width, station_height, 1.0f, 1.0f};
    inline const float bush_attributes[attributes::size] = {station_width, station_height, 1.0f, 1.0f};
    inline const float chopping_board_attributes[attributes::size] = {station_width, station_height, 1.0f, 1.0f};
    // food is a small one-tile entity; it reuses the test_decoration texture for now.
    inline const float test_food_attributes[attributes::size] = {level_config::edge_weight, level_config::edge_weight, 1.0f, 1.0f};
    inline const float lasagna_attributes[attributes::size] = {42.0f, 42.0f, 1.0f, 1.0f};
    inline const float espresso_attributes[attributes::size] = {42.0f, 42.0f, 1.0f, 1.0f};
    inline const float cutlets_attributes[attributes::size] = {42.0f, 42.0f, 1.0f, 1.0f};
    inline const float mutton_attributes[attributes::size] = {42.0f, 42.0f, 1.0f, 1.0f};
    inline const float muttoff_attributes[attributes::size] = {42.0f, 42.0f, 1.0f, 1.0f};
    inline const float prozac_attributes[attributes::size] = {42.0f, 42.0f, 1.0f, 1.0f};
    inline const float escargot_attributes[attributes::size] = {42.0f, 42.0f, 1.0f, 1.0f};

    inline const Vector2 station_slot_left  = Vector2Zero();
    inline const Vector2 station_slot_right = Vector2Zero();

    inline const float station_reach = level_config::edge_weight * 0.5f;
    // where stored food is drawn relative to the counter origin.
    inline const Vector2 food_draw_offset = {level_config::edge_weight * 0.5f, level_config::edge_weight * 0.5f};
    inline const Vector2 food_carry_offset = {level_config::edge_weight * 0.25f, 0.0f};
}
#endif
