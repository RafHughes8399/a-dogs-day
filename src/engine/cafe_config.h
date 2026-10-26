#ifndef CAFE_CONFIG_H
#define CAFE_CONFIG_H

#include "game_config.h"
#include "raylib.h"
#include <cstddef>
#include <vector>

namespace level_config{
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

    // world dimensions
    inline const float screen_width = static_cast<float>(game_config::window_width);
    inline const float screen_height = static_cast<float>(game_config::window_height);
    inline const float edge_weight = 64.0f; // placeholder

    inline const size_t num_tiles = 16;
    inline const float tile_width = 768.0f;
    inline const float tile_height = 768.0f;

    inline float world_x = 3072.0f;
    inline float world_y = 3072.0f;
    inline const size_t tile_rows = static_cast<size_t>(world_y / tile_height);
    inline const size_t tile_columns = static_cast<size_t>(world_x / tile_width);

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

    inline const Vector2 direction_scalars[directions::directions_size] = {
        Vector2{-1, 0}, // left  (index 0)
        Vector2{1, 0},  // right (index 1)
        Vector2{0, -1}, // up    (index 2)
        Vector2{0, 1} ,  // down  (index 3)
        Vector2{1,1}    // all   (index 4) - see TODO above, no sprite for this
    };
}
namespace cafe_config{
    inline const int queue_width_edges = 3;
    inline const int queue_x_edges = 1;
    inline const int queue_y_buffer_edges = 1;
    inline const int queue_y_edges = level_config::screen_edges_y - (2 * queue_y_buffer_edges);
    inline const float queue_gap_edges = 2.0f;

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
    // * halfway up the cafe, on the seam the two zones share - x in
    // * [cafe_x, footpath_x + footpath_width) sits inside both areas, and
    // * level_config::cafe_x is the one grid column both graphs hold a node
    // * for, so a path can end here from the footpath and start here into the cafe
    inline const Vector2 cafe_entrance = Vector2{
        level_config::cafe_x,
        level_config::cafe_y + level_config::cafe_height * 0.5f
    };
    inline const Vector2 cafe_exit = Vector2{cafe_entrance.x, -2.0f * level_config::edge_weight};
}
#endif
