#ifndef HUD_SYSTEMS_H
#define HUD_SYSTEMS_H

#include <array>
#include <functional>

#include "component.h"
#include "component_managers.hpp"
#include "config.h"
#include "events.h"
#include "events_interface.h"
#include "quadtree.h"
namespace hud_systems{
    class hud_spatial_system {
    public:
        static hud_spatial_system& get_instance(){
            static hud_spatial_system instance;
            return instance;
        }
        ~hud_spatial_system(){
            event_interface::unsubscribe<events::create_hud_element>(create_hud_element_handler_);
            event_interface::unsubscribe<events::remove_hud_element>(remove_hud_element_handler_);
            event_interface::unsubscribe<events::move_entity>(move_entity_handler_);
        }
        hud_spatial_system(const hud_spatial_system& other) = delete;
        hud_spatial_system(hud_spatial_system&& other) = delete;

        hud_spatial_system& operator=(const hud_spatial_system& other) = delete;
        hud_spatial_system& operator=(hud_spatial_system&& other) = delete;

        void on_created_hud_element(const events::create_hud_element& event);
        void on_removed_hud_element(const events::remove_hud_element& event);
        void on_moved_entity(const events::move_entity& event);
        void track(size_t hud_id);
        void untrack(size_t hud_id);
        bool is_tracked(size_t hud_id);
        size_t tracked_count();
        int check_collision_with(Vector2 position);
        void clear();
    private:
        hud_spatial_system(raglib::bounding_box_2 world_bounds = raglib::bounding_box_2{
            Vector2{level_config::graph_x, level_config::graph_y},
            Vector2{level_config::graph_x + level_config::graph_width,
                level_config::footpath_y + level_config::footpath_height}})
        : create_hud_element_handler_([this](const events::create_hud_element& event) -> void{on_created_hud_element(event);}),
          remove_hud_element_handler_([this](const events::remove_hud_element& event) -> void{on_removed_hud_element(event);}),
          move_entity_handler_([this](const events::move_entity& event) -> void{on_moved_entity(event);}),
          elements_(world_bounds, MAX_DEPTH, component_managers::hud_collision_manager_){
            event_interface::subscribe<events::create_hud_element>(create_hud_element_handler_);
            event_interface::subscribe<events::remove_hud_element>(remove_hud_element_handler_);
            event_interface::subscribe<events::move_entity>(move_entity_handler_);
        }
        void move_hud(components::hud_component& hud, Vector2 delta);

        events::event_handler<events::create_hud_element> create_hud_element_handler_;
        events::event_handler<events::remove_hud_element> remove_hud_element_handler_;
        events::event_handler<events::move_entity> move_entity_handler_;
        tree::ecs_quadtree elements_;
    };

    // rendering huyd elements
    class hud_rendering_system {
    public:
        static hud_rendering_system& get_instance(){
            static hud_rendering_system instance;
            return instance;
        }
        ~hud_rendering_system() = default;
        hud_rendering_system(const hud_rendering_system& other) = delete;
        hud_rendering_system(hud_rendering_system&& other) = delete;
        
        hud_rendering_system& operator=(const hud_rendering_system& other) = delete;
        hud_rendering_system& operator=(hud_rendering_system&& other) = delete;
        void render(int frame);
        private:
            hud_rendering_system() = default;
    };
    class click_system {
    public:
        static click_system& get_instance(){
            static click_system instance;
            return instance;
        }
        ~click_system() = default;
        click_system(const click_system& other) = delete;
        click_system(click_system&& other) = delete;

        click_system& operator=(const click_system& other) = delete;
        click_system& operator=(click_system&& other) = delete;

        bool click(Vector2 position);
    private:
        click_system()
        : defined_clicks_(){
            defined_clicks_[hud_config::click_actions::select_recipe] = select_recipe;
            defined_clicks_[hud_config::click_actions::change_page] = change_page;
            defined_clicks_[hud_config::click_actions::move_decoration] = move_decoration;
            defined_clicks_[hud_config::click_actions::sell_decoration] = sell_decoration;
            defined_clicks_[hud_config::click_actions::store_decoration] = store_decoration;
        }

        static void select_recipe(size_t button, int payload);
        static void change_page(size_t button, int payload);
        static void move_decoration(size_t button, int payload);
        static void sell_decoration(size_t button, int payload);
        static void store_decoration(size_t button, int payload);
        std::array<std::function<void(size_t, int)>, hud_config::click_actions::size> defined_clicks_;
    };
    // storing hud elements 
    // processing hud behaviours
    // and then the hud as a whole that has the instances of the 
    class hud {
    public:
        static hud& get_instance(){
            static hud instance;
            return instance;
        }
        ~hud(){
            event_interface::unsubscribe<events::entity_selected>(entity_selected_handler_);
            event_interface::unsubscribe<events::entity_deselected>(entity_deselected_handler_);
        }
        hud(const hud& other) = delete;
        hud(hud&& other) = delete;
        
        hud& operator=(const hud& other) = delete;
        hud& operator=(hud&& other) = delete;
        
        void init();
        void update(float delta_time);
        void render(int frame);
        void on_entity_selected(const events::entity_selected& event);
        void on_entity_deselected(const events::entity_deselected& event);

    private:
        hud()
        : entity_selected_handler_([this](const events::entity_selected& event) -> void{on_entity_selected(event);}),
          entity_deselected_handler_([this](const events::entity_deselected& event) -> void{on_entity_deselected(event);}),
          hud_rendering_(hud_rendering_system::get_instance()),
          clicks_(click_system::get_instance()){
            event_interface::subscribe<events::entity_selected>(entity_selected_handler_);
            event_interface::subscribe<events::entity_deselected>(entity_deselected_handler_);
        }

        events::event_handler<events::entity_selected> entity_selected_handler_;
        events::event_handler<events::entity_deselected> entity_deselected_handler_;
        hud_rendering_system& hud_rendering_;
        click_system& clicks_;
    };
}
#endif
