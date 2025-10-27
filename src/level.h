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

#include <map>
#include <utility>
#include <vector>

#include "config.h"
#include "quadtree.h"
#include "raglib.h"
#include "texture.h"


namespace level{
    class level_graph{
        private:
            struct node{
                // grid position
                const Vector2 position_;
                const int row_;
                const int col_;
                std::vector<int> entity_ids_; // maybe pointers instead, on va voir (27.10)
            };
            struct edge{
                const node& src_;
                const node& dst_;
                const float weight_;
            };
            
            std::map<node, std::vector<edge>> node_edge_map;
        public:
            ~level_graph() = default;
            level_graph() {};
            level_graph(const level_graph& other) = default;
            level_graph(level_graph&& other) = default;
            
            
            level_graph& operator=(const level_graph& other) = default;
            level_graph& operator=(level_graph&& other) = default;
    
            node& find_entity(int entity_id);
            void insert_entity(Vector2 position, raglib::bounding_box_2 bounds, int entity_id);
            
            void insert_node(Vector2 position, int row, int col);
            void insert_edge(node & source, node & destination);
    
            void path_to_entity(int entity_id); // unsure what the return type would be exactly

    };
    class level{
        public :
        ~level(){
            event_interface::unsubscribe<events::left_mouse_down>(left_mouse_handler_);
            event_interface::unsubscribe<events::right_mouse_click>(right_mouse_handler_);
            }
            level(sprite::sprite sprite, Rectangle frame, Vector2 dimensions)
            : background_(sprite), view_frame_(frame), dimensions_(dimensions), 
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