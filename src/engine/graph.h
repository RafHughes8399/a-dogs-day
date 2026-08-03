/**
 * level_graph: the walkable node/edge graph backing pathfinding and decoration
 * placement for a level. Split out of level.h; the method definitions live in
 * graph.cpp.
 */
#ifndef GRAPH_H
#define GRAPH_H

#include <cmath>
#include <utility>
#include <vector>

#include "config.h"
#include "events.h"
#include "events_interface.h"
#include "queries.h"
#include "query_interface.h"
#include "raylib.h"

namespace level{
    class level_graph{
        private:
            // node types
            enum nodes{
                corner = 1,
                perimeter = 2,
                interior = 3
            };
            // node and dege definitions
            struct node{
                int id_;
                Vector2 position_;
                int decoration_; // indicates the presence of a decoration at the current node, -1 means there is no decoration
                bool operator==(const node& other) const {
                    return id_ == other.id_;
                }
                bool operator<(const node& other) const {
                    return id_ < other.id_;
                }
            };

            struct edge{
                node* destination_;
                float weight_;
                int decoration_ = level_config::empty_node; // -1 means empty, there is no decoration, any other number refers to the id of the decoration stored within
                bool operator==(const edge& other){
                    return destination_ == other.destination_ and std::fabs(weight_ - other.weight_) <= 0.0001f;
                }
            };
            // builder
            bool check_for_decoration(Rectangle rectanlge, int id);
            bool is_node_closer(int current_id, int next_id, int end_id);
            bool is_node_empty(int node_id);
            bool is_node_occupied(int node_id, int decoration_id);
            std::vector<edge> build_corner_edges(int row, int column);
            std::vector<edge> build_interior_edges(int row, int column);
            std::vector<edge> build_perimeter_edges(int row, int column);


            int categorise_node(int row, int column);

            void build_nodes(int level_x, int level_y);
            void build_edges();
            void update_decoration(Rectangle rectangle, int id = level_config::empty_node);
            std::vector<int> bfs(int start_id, int end_id);
            std::vector<Vector2> make_position_path(std::vector<Vector2>& position_path, std::vector<int>& visited, size_t start_id, size_t end_id);

            // fields
            events::event_handler<events::moved_decoration> moved_decoration_handler_;
            events::event_handler<events::placed_decoration> placed_decoration_handler_;
            int num_rows_;
            int row_length_;

            queries::query_handler<queries::can_place_decoration, bool> can_place_decoration_handler_;
            queries::query_handler<queries::path_query, std::vector<Vector2>> path_query_handler_;
            std::vector<std::pair<node, std::vector<edge>>> graph_;

        public:
            ~level_graph(){
                event_interface::unsubscribe<events::moved_decoration>(moved_decoration_handler_);
                event_interface::unsubscribe<events::placed_decoration>(placed_decoration_handler_);
                query_interface::unsubscribe<queries::can_place_decoration>(queries::bool_executor_, can_place_decoration_handler_);
                query_interface::unsubscribe<queries::path_query>(queries::path_executor_, path_query_handler_);
            }
            level_graph(int level_x, int level_y)
            : moved_decoration_handler_([this](const events::moved_decoration& event) -> void{on_moved_decoration(event);}),
            placed_decoration_handler_([this](const events::placed_decoration& event) -> void{on_placed_decoration(event);}),
            num_rows_(static_cast<int>(level_y / level_config::edge_weight)),
            row_length_(static_cast<int>(level_x / level_config::edge_weight)),
            can_place_decoration_handler_([this](const queries::can_place_decoration& query) -> bool {return can_place_decoration(query);}),
            path_query_handler_([this](const queries::path_query& query) -> std::vector<Vector2> {return get_path(query);}),
            graph_({}){
                // ! columns is x, rows is y
                build_nodes(level_x, level_y);
                build_edges();
                event_interface::subscribe<events::moved_decoration>(moved_decoration_handler_);
                event_interface::subscribe<events::placed_decoration>(placed_decoration_handler_);
                query_interface::subscribe<queries::can_place_decoration>(queries::bool_executor_, can_place_decoration_handler_);
                query_interface::subscribe<queries::path_query>(queries::path_executor_, path_query_handler_);
            }
            level_graph(const level_graph& other) = default;
            level_graph(level_graph&& other) = default;


            level_graph& operator=(const level_graph& other) = delete;
            level_graph& operator=(level_graph&& other) = delete;

            bool can_place_decoration(const queries::can_place_decoration& query);
            std::vector<Vector2> get_path(const queries::path_query& query);

            int position_to_node(Vector2 position);
            int position_to_node(Vector2 position, Vector2 direction);

            node* id_to_node(int id);

            std::vector<Vector2> find_path(Vector2 start, Vector2 end, Vector2 direction);

            void insert_node(int id, Vector2 position);
            void insert_edge(int source_num, node& destination, float weight);
            void on_moved_decoration(const events::moved_decoration& event);
            void on_placed_decoration(const events::placed_decoration& event);
            void render(Rectangle frame);
    };
}

#endif
