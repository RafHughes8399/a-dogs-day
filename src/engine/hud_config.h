#ifndef HUD_CONFIG_H
#define HUD_CONFIG_H

#include "cafe_config.h"
#include "entity_config.h"
#include "enum_config.h"
#include "game_config.h"
#include "raylib.h"

namespace hud_config{
    inline unsigned char opacity = 120;
    inline Color green_decoration_highlight = Color {0, 255, 0, opacity};
    inline Color red_decoration_highlight = Color {255, 255, 0, opacity};
    inline Color decoration_grid_highlight = Color{243, 252, 255, opacity};
    inline const int decoration_grid_thickness = 3;
    // hud elements paths
    inline const char* cursor_edit_progres_wheel = "../sprites/edit_wheel.png";
    inline const char* edit_grid = "../sprites/edit_grid.png";
    inline const char* move_decoration_path = "../sprites/hud/move-decoration.png";
    inline const char* store_decoration_path = "../sprites/hud/store-decoration.png";
    inline const char* sell_decoration_path = "../sprites/hud/sell-decoration.png";
    inline const float edit_wheel_attributes[entity_config::attributes::size] = {35.0f, 35.0f, static_cast<float>(game_config::hold_duration),  1.0f}; // for now, pending animation play speed implementation , frames is 90

    inline const float nine_sprite_corner_width = level_config::edge_weight * 0.5;
    inline const float nine_sprite_corner_height = level_config::edge_weight * 0.5;
    inline const Vector2 nine_sprite_corner = {nine_sprite_corner_width, nine_sprite_corner_height};
    inline const float nine_sprite_centre_width = level_config::edge_weight;
    inline const float nine_sprite_centre_height = level_config::edge_weight;
    inline const Vector2 nine_sprite_centre{nine_sprite_centre_width, nine_sprite_centre_height};

    inline const Vector2 nine_sprite_horizontal = {nine_sprite_centre_width, nine_sprite_corner_height};
    inline const Vector2 nine_sprite_vertical = {nine_sprite_corner_width, nine_sprite_centre_height};

    inline const Vector2 top_left_corner_offset = {};
    inline const Vector2 top_right_corner_offset = {};
    inline const Vector2 bottom_left_corner_offset = {};
    inline const Vector2 bottom_right_corner_offset = {};

    inline const float nine_sprite_frames = 1.0f;
    inline const float nine_sprite_animations = 1.0f;

    inline const char* nine_sprite_corner_path = "../sprites/hud/nine_sprite_corner.png";
    inline const char* nine_sprite_centre_path = "../sprites/hud/nine_sprite_centre.png";
    inline const char* nine_sprite_horizontal_path = "../sprites/hud/nine_sprite_horizontal.png";
    inline const char* nine_sprite_vertical_path = "../sprites/hud/nine_sprite_vertical.png";

    inline const float recipe_icon_width = entity_config::food_width * 0.8;
    inline const float recipe_icon_height = recipe_icon_width;
    inline const float reicpe_icon_width_buffer = entity_config::food_width * 0.2;
    inline const float recipe_icon_height_buffer = reicpe_icon_width_buffer;

    inline const int recipe_rows = 2;
    inline const int recipe_columns = 2;
    inline const Rectangle cook_station_hud_sprite = {0.0f, 0.0f,
        recipe_columns * (recipe_icon_width + reicpe_icon_width_buffer),
        recipe_rows * (recipe_icon_height + recipe_icon_height_buffer) };
    inline const Vector2 cook_station_hud_offset = {entity_config::station_width - (entity_config::station_width / 4), entity_config::station_height  * -0.5f};

    inline const float decoration_hud_height_ratio = 0.6f;
    inline const float decoration_hud_corner_scale = 0.5f;
    inline const float decoration_hud_width = nine_sprite_corner_width * 2.0f * decoration_hud_corner_scale;
    inline const float decoration_hud_edge_gap = level_config::edge_weight * 0.0625f;
    inline const float decoration_hud_button_size = 20.0f;
    inline const float decoration_hud_button_buffer = level_config::edge_weight * 0.0625f;
    inline const int decoration_hud_button_count = 3;
    inline const float decoration_hud_min_height = decoration_hud_button_count * decoration_hud_button_size
        + (decoration_hud_button_count + 1) * decoration_hud_button_buffer;
    inline const float decoration_button_attributes[entity_config::attributes::size] = {
        decoration_hud_button_size, decoration_hud_button_size, 1.0f, 1.0f};
}
#endif
