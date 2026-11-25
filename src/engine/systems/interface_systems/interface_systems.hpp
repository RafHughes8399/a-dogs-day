#ifndef INTERFACE_SYSTEMS_HPP
#define INTERFACE_SYSTEMS_HPP
#include "component.h"
#include "config.h"
#include "events.h"
#include "events_interface.h"
#include "hud_events.h"
#include "menus.h"
#include "menu_factory.hpp"
#include <optional>
#include <vector>

namespace systems{
    class inventory_system {
    public:
        static inventory_system& get_instance(){
            static inventory_system instance;
            return instance;
        }
        ~inventory_system(){
            event_interface::unsubscribe<events::store_decoration>(store_decoration_handler_);
            event_interface::unsubscribe<events::item_from_inventory>(item_from_inventory_handler_);
        }
        inventory_system(const inventory_system& other) = delete;
        inventory_system(inventory_system&& other) = delete;

        inventory_system& operator=(const inventory_system& other) = delete;
        inventory_system& operator=(inventory_system&& other) = delete;

        void on_stored_decoration(const events::store_decoration& event);
        void on_item_from_inventory(const events::item_from_inventory& event);
        void init();
        bool take_item(size_t button);
    private:
        inventory_system()
        : store_decoration_handler_([this](const events::store_decoration& event) -> void {on_stored_decoration(event);}),
          item_from_inventory_handler_([this](const events::item_from_inventory& event) -> void {on_item_from_inventory(event);}){
            event_interface::subscribe<events::store_decoration>(store_decoration_handler_);
            event_interface::subscribe<events::item_from_inventory>(item_from_inventory_handler_);
        }
        bool store_item(const items::item_ref& ref);
        events::event_handler<events::store_decoration> store_decoration_handler_;
        events::event_handler<events::item_from_inventory> item_from_inventory_handler_;
    };
    class menu_navigation_system{
        public:
            static menu_navigation_system& get_instance(){
                static menu_navigation_system instance;
                return instance;
            }
            ~menu_navigation_system(){
                event_interface::unsubscribe<events::key_press>(key_press_handler_);
                event_interface::unsubscribe<events::item_from_inventory>(item_from_inventory_handler_);
            }
            menu_navigation_system(const menu_navigation_system& other) = delete;
            menu_navigation_system(menu_navigation_system&& other) = delete;

            menu_navigation_system& operator=(const menu_navigation_system& other) = delete;
            menu_navigation_system& operator=(menu_navigation_system&& other) = delete;

            void init();
            void on_key_press(const events::key_press& event);
            void add_root(size_t menu, size_t root, size_t group = 0);
            void next_page();
            void previous_page();
            const std::vector<size_t>& pages_of(size_t menu, size_t group = 0);
            std::vector<size_t> all_pages_of(size_t menu);
            void set_group(size_t menu, size_t group);
            size_t group_of(size_t menu);
            std::optional<size_t> active_page_of(size_t menu);
            void clear();
            void on_item_from_inventory(const events::item_from_inventory& event);
            void show(size_t menu);
            void hide(size_t menu);
        private:
            menu_navigation_system()
            : key_press_handler_([this](const events::key_press& event) -> void {on_key_press(event);}),
              item_from_inventory_handler_([this](const events::item_from_inventory& event) -> void {on_item_from_inventory(event);}),
              menus_(), factory_(){
                event_interface::subscribe<events::key_press>(key_press_handler_);
                event_interface::subscribe<events::item_from_inventory>(item_from_inventory_handler_);
            }
            events::event_handler<events::key_press> key_press_handler_;
            events::event_handler<events::item_from_inventory> item_from_inventory_handler_;
            menus::menu_graph menus_;
            factories::menu_factory factory_;
    };
    class quest_system{
        public:
            static quest_system& get_instance(){
                static quest_system instance;
                return instance;

            }
            ~quest_system() = default;
            quest_system(const quest_system& other) = delete;
            quest_system(quest_system&& other) = delete;

            quest_system& operator=(const quest_system& other) = default;
            quest_system& operator=(quest_system&& other) = delete;

            void init();
        private:
            quest_system() = default;
    };
    class shop_system{
        public:
            static shop_system& get_instance(){
                static shop_system instance;
                return instance;
            }
            ~shop_system() = default;
            shop_system(const shop_system& other) = delete;
            shop_system(shop_system&& other) = delete;

            shop_system& operator=(const shop_system& other) = delete;
            shop_system& operator=(shop_system&& other) = delete;

            void init();
        private:
            shop_system() = default;
    };
}

#endif
