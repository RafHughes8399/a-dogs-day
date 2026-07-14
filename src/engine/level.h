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
#include "graph.h"
#include "quadtree.h"
#include "queries.h"
#include "query_interface.h"
#include "raglib.h"
#include "render_layer.h"
#include "texture.h"
#include "raylib.h"

namespace level{
    class level{
        public :
        ~level(){
            event_interface::unsubscribe<events::left_mouse_click>(left_mouse_click_handler_);
            event_interface::unsubscribe<events::move_view_frame>(move_view_frame_handler_);
	            event_interface::unsubscribe<events::right_mouse_click>(right_mouse_click_handler_);
		            event_interface::unsubscribe<events::build_customer_dog>(build_customer_dog_handler_);
		            event_interface::unsubscribe<events::send_dog_to_position>(send_dog_to_position_handler_);
		            event_interface::unsubscribe<events::send_dog_to_station>(send_dog_to_station_handler_);
		            event_interface::unsubscribe<events::order_served>(order_served_handler_);
		            event_interface::unsubscribe<events::remove_entity>(removed_entity_handler_);
		            event_interface::unsubscribe<events::dog_reached_station>(dog_reached_station_handler_);
		            }
            level(sprite::sprite sprite, Rectangle frame, Vector2 dimensions)
            : left_mouse_click_handler_([this](const events::left_mouse_click& event) -> void{on_left_mouse_click_event(event);}),
            move_view_frame_handler_([this](const events::move_view_frame& event) -> void{on_move_view_frame_event(event);}),
            right_mouse_click_handler_([this](const events::right_mouse_click& event) -> void{on_right_mouse_event(event);}),
		            build_customer_dog_handler_([this](const events::build_customer_dog& event) -> void{on_build_customer_dog_event(event);}),
		            send_dog_to_position_handler_([this](const events::send_dog_to_position& event) -> void{on_send_dog_to_position_event(event);}),
		            send_dog_to_station_handler_([this](const events::send_dog_to_station& event) -> void{on_send_dog_to_station(event);}),
		            order_served_handler_([this](const events::order_served& event) -> void{on_order_served_event(event);}),
		            removed_entity_handler_([this](const events::remove_entity& event) -> void{on_removed_entity(event);}),
		            dog_reached_station_handler_([this](const events::dog_reached_station& event) -> void{on_dog_reached_station_event(event);}),
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
		                event_interface::subscribe<events::send_dog_to_station>(send_dog_to_station_handler_);
		                event_interface::subscribe<events::order_served>(order_served_handler_);
		                event_interface::subscribe<events::remove_entity>(removed_entity_handler_);
		                event_interface::subscribe<events::dog_reached_station>(dog_reached_station_handler_);
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
		    void on_send_dog_to_station(const events::send_dog_to_station& event);
            void on_removed_entity(const events::remove_entity& event);
            // dog_reached_station already names the station a dog arrived at
            // (the dog's own traveling state resolves and carries that id), so
            // this is a direct id lookup - no scanning every station's position.
            void on_dog_reached_station_event(const events::dog_reached_station& event);
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
		    events::event_handler<events::send_dog_to_station> send_dog_to_station_handler_;
		    events::event_handler<events::order_served> order_served_handler_;

            events::event_handler<events::remove_entity> removed_entity_handler_;
            events::event_handler<events::dog_reached_station> dog_reached_station_handler_;

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
