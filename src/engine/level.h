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
#include <unordered_map>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

#include "config.h"
#include "events.h"
#include "quadtree.h"
#include "queries.h"
#include "query_interface.h"
#include "raglib.h"
#include "render_layer.h"
#include "texture.h"
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
                    return destination_ == other.destination_ && std::fabs(weight_ - other.weight_) <= 0.0001f;
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
    class level{
        public :
        ~level(){
            event_interface::unsubscribe<events::left_mouse_click>(left_mouse_click_handler_);
            event_interface::unsubscribe<events::move_view_frame>(move_view_frame_handler_);
	            event_interface::unsubscribe<events::right_mouse_click>(right_mouse_click_handler_);
		            event_interface::unsubscribe<events::build_customer_dog>(build_customer_dog_handler_);
		            event_interface::unsubscribe<events::send_dog_to_position>(send_dog_to_position_handler_);
		            event_interface::unsubscribe<events::send_dog_to_furniture>(send_dog_to_furniture_handler_);
		            event_interface::unsubscribe<events::order_served>(order_served_handler_);
		            event_interface::unsubscribe<events::remove_entity>(removed_entity_handler_);
		            }
            level(sprite::sprite sprite, Rectangle frame, Vector2 dimensions)
            : left_mouse_click_handler_([this](const events::left_mouse_click& event) -> void{on_left_mouse_click_event(event);}),
            move_view_frame_handler_([this](const events::move_view_frame& event) -> void{on_move_view_frame_event(event);}),
            right_mouse_click_handler_([this](const events::right_mouse_click& event) -> void{on_right_mouse_event(event);}),
		            build_customer_dog_handler_([this](const events::build_customer_dog& event) -> void{on_build_customer_dog_event(event);}),
		            send_dog_to_position_handler_([this](const events::send_dog_to_position& event) -> void{on_send_dog_to_position_event(event);}),
		            send_dog_to_furniture_handler_([this](const events::send_dog_to_furniture& event) -> void{on_send_dog_to_furniture(event);}),
		            order_served_handler_([this](const events::order_served& event) -> void{on_order_served_event(event);}),
		            removed_entity_handler_([this](const events::remove_entity& event) -> void{on_removed_entity(event);}),
		            graph_(level_graph(static_cast<int>(dimensions.x), static_cast<int>(dimensions.y))),
            view_frame_(frame), background_(sprite), id_entity_map_({}),
            next_entity_id_(0),
            level_entities_(tree::quadtree(raglib::bounding_box_2{Vector2Zero(), dimensions}))
            {
                event_interface::subscribe<events::left_mouse_click>(left_mouse_click_handler_);
                event_interface::subscribe<events::move_view_frame>(move_view_frame_handler_);
	                event_interface::subscribe<events::right_mouse_click>(right_mouse_click_handler_);
		                event_interface::subscribe<events::build_customer_dog>(build_customer_dog_handler_);
		                event_interface::subscribe<events::send_dog_to_position>(send_dog_to_position_handler_);
		                event_interface::subscribe<events::send_dog_to_furniture>(send_dog_to_furniture_handler_);
		                event_interface::subscribe<events::order_served>(order_served_handler_);
		                event_interface::subscribe<events::remove_entity>(removed_entity_handler_);
		            }
            level(const level& other) = delete;
            level(level&& other) = delete;
            
            level& operator=(const level& other) = delete;
            level& operator=(level&& other) = delete;

            int entity_id();
            int num_entities();
            void add_entity(std::unique_ptr<entities::entity> entity, size_t layer);
            // Look up a registered entity by id; nullptr if none.
            entities::entity* get_entity(int id);

            void on_left_mouse_click_event(const events::left_mouse_click& event);
            void on_move_view_frame_event(const events::move_view_frame& event);
            void on_right_mouse_event(const events::right_mouse_click& event);

		    void on_build_customer_dog_event(const events::build_customer_dog& event);
		    void on_send_dog_to_position_event(const events::send_dog_to_position& event);
            void on_order_served_event(const events::order_served& event);
		    void on_send_dog_to_furniture(const events::send_dog_to_furniture& event);
            void on_removed_entity(const events::remove_entity& event);
            void render(int frame);
            void update(float delta, int frame);
        private :
            struct void_entity_record{
                std::unique_ptr<entities::entity> entity;
                size_t layer;
            };

            void add_void_entity(std::unique_ptr<entities::entity> entity, size_t layer);
            void update_void_entities(float delta, int frame);
            bool is_inside_screen(entities::entity& entity) const;
            void move_void_entity_toward_screen(entities::entity& entity, int frame);
            void insert_void_entity(void_entity_record record);

            // event handlers
            events::event_handler<events::left_mouse_click> left_mouse_click_handler_;
            events::event_handler<events::move_view_frame> move_view_frame_handler_;
            events::event_handler<events::right_mouse_click> right_mouse_click_handler_;

		    events::event_handler<events::build_customer_dog> build_customer_dog_handler_;
		    events::event_handler<events::send_dog_to_position> send_dog_to_position_handler_;
		    events::event_handler<events::send_dog_to_furniture> send_dog_to_furniture_handler_;
		    events::event_handler<events::order_served> order_served_handler_;

            events::event_handler<events::remove_entity> removed_entity_handler_;
            
            level_graph graph_;
            
            Rectangle view_frame_;
            sprite::sprite background_;
            std::unordered_map<size_t, entities::entity*> id_entity_map_;
            render_layer::layer render_layers_[level_config::size];
            int next_entity_id_;
            tree::quadtree level_entities_;
            std::vector<void_entity_record> void_entities_;
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

            std::unique_ptr<level> build_main_level();
    };
}

#endif
