#ifndef MENU_H
#define MENU_H

#include <memory>
#include <map>
#include <vector>

#include "events.h"
#include "items.h"
#include "hud.h"
#include "raylib.h"
#include "sprite.h"
namespace menus{
    class menu{
        public:
            virtual ~menu() = default;
            // ! baasic implementation, currently (23.01), for testing menu navigation
            // ! proper implementation will follow 
            menu(Rectangle box, hud::hud components)
            :box_(box), components_(std::move(components)) {};
            menu(const menu& other) = delete;
            menu(menu&& other) = default;
            menu& operator=(const menu& other) = delete;
            menu& operator=(menu&& other) = default;

            virtual void render();
            void subscribe_hud();
            void unsubscribe_hud();
        
        protected:
            hud::hud components_; // holds the hud elements and buttons
            Rectangle box_;
    };  
    class item_menu : public menu{
        public:
            item_menu();
            item_menu(const item_menu& other) = default;
            item_menu(item_menu&& other) = default;
            item_menu& operator=(const item_menu& other) = default;
            item_menu& operator=(item_menu&& other) = default;

            void render() override;
        private:
            std::vector<std::unique_ptr<items::item>> items_;
            

    };
    class menu_graph{
        enum menu_ids{
            blank = 0,
            pause = 1,
            tab = 2,
            inventory = 3,
            map = 4,
            shop = 5,
            quest = 6
        };
        private:
            struct node {
                std::unique_ptr<menu> menu_;
                size_t id_;
                bool operator==(const node& other){
                    return id_ == other.id_;
                }
                bool operator<=(const node& other){
                    return id_ <= other.id_;
                }
                bool operator>=(const node& other){
                    return id_ >= other.id_;
                }
            };
            struct edge {
                int key_;
                node* destination_menu_;
            };

            edge build_edge(node* dst, int key);
            node build_node(std::unique_ptr<menu> menu, size_t id);
            void build_graph();
        public:
        // need to sub and unsub
            ~menu_graph(){
                event_interface::unsubscribe<events::interact_menu>(menu_interact_handler_);
                event_interface::unsubscribe<events::key_press>(key_event_handler_);
            }
            menu_graph()
            : current_(0), graph_(), key_event_handler_([this](const events::key_press& event) -> void {on_key_press_event(event);}),
            menu_interact_handler_([this](const events::interact_menu& event) -> void {on_menu_interact_event(event);}){
                build_graph();
                event_interface::subscribe<events::interact_menu>(menu_interact_handler_);
                event_interface::subscribe<events::key_press>(key_event_handler_);
            }
            menu_graph(const menu_graph& other) = default;
            menu_graph(menu_graph&& other) = default;
            
            menu_graph& operator=(const menu_graph& other) = default;
            menu_graph& operator=(menu_graph&& other) = default;

            // needs to listen to key presses
            
            // update and draw
            int update(float delta);

            void on_key_press_event(const events::key_press& event);
            void on_menu_interact_event(const events::interact_menu& event);
            void render();


            events::event_handler<events::key_press> key_event_handler_;
            events::event_handler<events::interact_menu> menu_interact_handler_;
            size_t current_;
            std::vector<std::pair<node, std::vector<edge>>> graph_;
    };

    class menu_builder{
        public:
            std::unique_ptr<menu> build_blank_menu();
            std::unique_ptr<menu> build_pause_menu();
            std::unique_ptr<menu> build_tab_menu();
            std::unique_ptr<menu> build_shop_menu();
            std::unique_ptr<menu> build_map_menu();
            std::unique_ptr<menu> build_quest_menu();
            std::unique_ptr<menu> build_inventory_menu();
            menu_graph build_menus();
        private:
    };

    extern menu_builder m_builder_;
    
}
#endif