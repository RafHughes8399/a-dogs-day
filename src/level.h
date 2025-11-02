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
#include <utility>
#include <vector>

#include "config.h"
#include "quadtree.h"
#include "raglib.h"
#include "texture.h"


namespace level{
    class level_graph{
        private:
            enum nodes{
                corner = 1,
                perimeter = 2,
                interior = 3
            };
            struct node{
                Vector2 position_;
                bool operator==(const node& other){
                    return Vector2Equals(position_, other.position_);
                }
                bool operator<(const node& other){

                };
            };
            struct edge{
                node* destination_;
                float weight_;

                bool operator==(const edge& other){
                    return destination_ == other.destination_ && weight_ == other.weight_;
                }
            };

            std::vector<edge> build_corner_edges(int row, int column);
            std::vector<edge> build_interior_edges(int row, int column);
            std::vector<edge> build_perimeter_edges(int row, int column);
            void build_nodes(int level_x, int level_y);
            void build_edges();
            int categorise_node(int row, int column);
            
            int num_rows_;
            int row_length_;
            std::vector<std::pair<node, std::vector<edge>>> graph_;
        public:
            ~level_graph() = default;
            level_graph(int level_x, int level_y)
            : graph_({}), num_rows_(level_y / dimensions_config::edge_weight), row_length_(level_x / dimensions_config::edge_weight){
                build_nodes(level_x, level_y);
                build_edges();
                std::cout << "num row: " << num_rows_ << std::endl;
                std::cout << "row_length: " << row_length_<< std::endl;
                    // columns is x, rows is y
            }
            level_graph(const level_graph& other) = default;
            level_graph(level_graph&& other) = default;
            
            
            level_graph& operator=(const level_graph& other) = default;
            level_graph& operator=(level_graph&& other) = default;
    

            bool find_path(Vector2 start, Vector2 end);
            
            int num_nodes();
            int num_edges();
            int num_edges_from(node & node);

            node& position_to_node(Vector2 position);

            std::vector<edge> edges();
            std::vector<edge> edges_from_node(node& node);
            std::vector<node> nodes();

            void insert_node(Vector2 position);
            void insert_edge(int source_num, node& destination, float weight);
            void render(Rectangle frame);

    };
    class level{
        public :
        ~level(){
            event_interface::unsubscribe<events::left_mouse_down>(left_mouse_handler_);
            event_interface::unsubscribe<events::right_mouse_click>(right_mouse_handler_);
            }
            level(sprite::sprite sprite, Rectangle frame, Vector2 dimensions)
            : background_(sprite), view_frame_(frame), dimensions_(dimensions), graph_(level_graph(static_cast<int>(dimensions.x), static_cast<int>(dimensions.y))),
            level_entities_(tree::quadree(raglib::bounding_box_2{Vector2Zero(), dimensions})),
            left_mouse_handler_([this](const events::left_mouse_down& event) -> void{on_left_mouse_event(event);}),
            right_mouse_handler_([this](const events::right_mouse_click& event) -> void{on_right_mouse_event(event);})
            {
                event_interface::subscribe<events::left_mouse_down>(left_mouse_handler_);
                event_interface::subscribe<events::right_mouse_click>(right_mouse_handler_);
            };
            level(const level& other) = default;
            level(level&& other) = default;
            
            level& operator=(const level& other) = default;
            level& operator=(level&& other) = default;

            void update(float delta);
            void render();

            void add_entity(std::unique_ptr<entities::entity> entity);
            int entity_id();
            int num_entities();

            void on_left_mouse_event(const events::left_mouse_down& event);
            void on_right_mouse_event(const events::right_mouse_click& event);
        private :
            level_graph graph_;
            sprite::sprite background_;
            tree::quadree level_entities_;
            Rectangle view_frame_;
            Vector2 dimensions_;


            // event handlers
            events::event_handler<events::left_mouse_down> left_mouse_handler_;
            events::event_handler<events::right_mouse_click> right_mouse_handler_;
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