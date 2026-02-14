/**
 * level is a more appropriate name I feel. A level is essentially a container that manages all of the entities
 * it organises the quadtree that stores the entities and handles other information about the level, perhaps any goals, 
 * the structure of rooms, the map, etc
 * 
 * perhaps a requirement to define two types of levels, the main hub and then exploration levels, but that can 
 * come later
 * 
 * author: raffa, October 25
 */
#ifndef LEVEL_H
#define LEVEL_H

#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

#include "config.h"
#include "quadtree.h"
#include "raglib.h"
#include "render_layer.h"
#include "texture.h"


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
                int decoration_ = -1; // -1 means empty, there is no decoration, any other number refers to the id of the decoration stored within
                bool operator==(const edge& other){
                    return destination_ == other.destination_ && weight_ == other.weight_;
                }
            };
            // builder and pathfinding methods
            bool is_node_closer(int current_id, int next_id, int end_id);
            std::vector<edge> build_corner_edges(int row, int column);
            std::vector<edge> build_interior_edges(int row, int column);
            std::vector<edge> build_perimeter_edges(int row, int column);

            node* lowest_f_score(std::vector<node*>& nodes, std::map<int, float>& f_scores);
            float manhattan_distance_heurisitic(Vector2 a, Vector2 b);
            
            int categorise_node(int row, int column);
            
            void build_nodes(int level_x, int level_y);
            void build_edges();
            void update_decoration(Rectangle rectangle, int id = -1);
            std::vector<int> bfs(int start_id, int end_id);
            std::vector<Vector2> make_position_path(std::vector<Vector2>& position_path, std::vector<int>& visited, int start_id, int end_id);
            
            // fields
            events::event_handler<events::moved_decoration> moved_decoration_handler_;
            events::event_handler<events::placed_decoration> placed_decoration_handler_;
            int num_rows_;
            int row_length_;
            std::vector<std::pair<node, std::vector<edge>>> graph_;
        
        public:
            ~level_graph(){
                event_interface::unsubscribe<events::moved_decoration>(moved_decoration_handler_);
                event_interface::unsubscribe<events::placed_decoration>(placed_decoration_handler_);
            }
            level_graph(int level_x, int level_y)
            : graph_({}), num_rows_(level_y / level_config::edge_weight), row_length_(level_x / level_config::edge_weight),
            moved_decoration_handler_([this](const events::moved_decoration& event) -> void{on_moved_decoration(event);}),
            placed_decoration_handler_([this](const events::placed_decoration& event) -> void{on_placed_decoration(event);}){
                // ! columns is x, rows is y
                build_nodes(level_x, level_y);
                build_edges();
                event_interface::subscribe<events::moved_decoration>(moved_decoration_handler_);
                event_interface::subscribe<events::placed_decoration>(placed_decoration_handler_);
            }
            level_graph(const level_graph& other) = default;
            level_graph(level_graph&& other) = default;
            
            
            level_graph& operator=(const level_graph& other) = default;
            level_graph& operator=(level_graph&& other) = default;
    

            
            int num_nodes();
            int num_edges();
            int num_edges_from(node & node);
            
            int position_to_node(Vector2 position);
            int position_to_node(Vector2 position, Vector2 direction);
            
            node* id_to_node(int id);

            std::vector<edge> edges();
            std::vector<edge> edges_from_node(node& node);
            std::vector<node> nodes();
            
            std::vector<Vector2> find_path(Vector2 start, Vector2 end, Vector2 direction);
            
            void insert_node(int id, Vector2 position);
            void insert_edge(int source_num, node& destination, float weight);
            void on_moved_decoration(const events::moved_decoration& event);
            void on_placed_decoration(const events::placed_decoration& event);
            void render(Rectangle frame);
    };
    class level{
        public :
        ~level(){
            event_interface::unsubscribe<events::left_mouse_click>(left_mouse_click_handler_);
            event_interface::unsubscribe<events::move_view_frame>(move_view_frame_handler_);
            event_interface::unsubscribe<events::right_mouse_click>(right_mouse_click_handler_);
            }
            level(sprite::sprite sprite, Rectangle frame, Vector2 dimensions)
            : background_(sprite), view_frame_(frame), dimensions_(dimensions), graph_(level_graph(static_cast<int>(dimensions.x), static_cast<int>(dimensions.y))),
            level_entities_(tree::quadtree(raglib::bounding_box_2{Vector2Zero(), dimensions})), id_entity_map_({}),
            left_mouse_click_handler_([this](const events::left_mouse_click& event) -> void{on_left_mouse_click_event(event);}),
            move_view_frame_handler_([this](const events::move_view_frame& event) -> void{on_move_view_frame_event(event);}),
            right_mouse_click_handler_([this](const events::right_mouse_click& event) -> void{on_right_mouse_event(event);})
            {
                event_interface::subscribe<events::left_mouse_click>(left_mouse_click_handler_);
                event_interface::subscribe<events::move_view_frame>(move_view_frame_handler_);
                event_interface::subscribe<events::right_mouse_click>(right_mouse_click_handler_);
            };
            level(const level& other) = default;
            level(level&& other) = default;
            
            level& operator=(const level& other) = default;
            level& operator=(level&& other) = default;

            void update(float delta);
            void render();

            void add_entity(std::unique_ptr<entities::entity> entity, size_t layer);
            int entity_id();
            int num_entities();

            void on_left_mouse_click_event(const events::left_mouse_click& event);
            void on_move_view_frame_event(const events::move_view_frame& event);
            void on_right_mouse_event(const events::right_mouse_click& event);
        private :
            // event handlers
            events::event_handler<events::left_mouse_click> left_mouse_click_handler_;
            events::event_handler<events::move_view_frame> move_view_frame_handler_;
            events::event_handler<events::right_mouse_click> right_mouse_click_handler_;

            level_graph graph_;
            
            Rectangle view_frame_;
            sprite::sprite background_;
            std::map<int, entities::entity*> id_entity_map_;
            render_layer::layer render_layers_[level_config::size];
            tree::quadtree level_entities_;
            Vector2 dimensions_;


    };
        // self explanatory, a class to construct leveks, outline functions that build levels generating enetities, specifying background,
    // maybe the level map graph, and the tileset too
    class level_builder {
        public :
            ~level_builder()  = default;
            level_builder() = default;
            level_builder(const level_builder& other) = default;
            level_builder(level_builder&& other) = default;

            level_builder& operator=(const level_builder& other) = default;
            level_builder& operator=(level_builder&& other) = default;

            level build_main_level();
    };
}

#endif