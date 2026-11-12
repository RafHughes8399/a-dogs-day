#ifndef MENU_FACTORY_H
#define MENU_FACTORY_H
#include <array>
#include <functional>
#include "config.h"
#include "hud_element_builders.hpp"
#include "menus.h"
#include "raylib.h"
namespace factories{
    class menu_factory {
    public:
        ~menu_factory() = default;
        menu_factory()
        :builders_({
            nullptr,
            [](size_t id) -> void {page_builders::build_menu_page(id, hud_config::menu_frame);},
            [](size_t id) -> void {page_builders::build_menu_page(id, hud_config::menu_frame);},
            nullptr,
            [](size_t id) -> void {page_builders::build_menu_page(id, hud_config::menu_frame);},
            [](size_t id) -> void {page_builders::build_menu_page(id, hud_config::menu_frame);},
            [](size_t id) -> void {page_builders::build_menu_page(id, hud_config::menu_frame);}
        }){

        }
        menu_factory(const menu_factory& other) = default;
        menu_factory(menu_factory&& other) = default;

        menu_factory& operator=(const menu_factory& other) = default;
        menu_factory& operator=(menu_factory&& other) = default;

        bool has_root(size_t menu) const;
        void build_menu(size_t menu, size_t root_id);
    private:
        std::array<std::function<void(size_t)>, menus::menu_ids_size> builders_;
    };
}
#endif
