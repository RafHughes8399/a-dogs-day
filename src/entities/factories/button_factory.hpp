#ifndef BUTTON_FACTORY_H
#define BUTTON_FACTORY_H
#include <array>
#include <functional>
#include "config.h"
#include "hud_element_builders.hpp"
#include "raylib.h"
namespace factories{
    class button_factory {
    public:
        ~button_factory() = default;
        button_factory()
        :builders_({
            [](size_t id, Vector2 position, size_t) -> void{button_builders::build_move_decoration_button(id, position);},
            [](size_t id, Vector2 position, size_t) -> void{button_builders::build_sell_decoration_button(id, position);},
            [](size_t id, Vector2 position, size_t) -> void{button_builders::build_to_storage_decoration_button(id, position);},
            [](size_t id, Vector2 position, size_t subject) -> void{button_builders::build_inventory_item_button(id, position, subject);},
            [](size_t id, Vector2 position, size_t) -> void{button_builders::build_page_navigation_button(id, position, -1);},
            [](size_t id, Vector2 position, size_t) -> void{button_builders::build_page_navigation_button(id, position, 1);}
        }){

        }
        button_factory(const button_factory& other) = default;
        button_factory(button_factory&& other) = default;

        button_factory& operator=(const button_factory& other) = default;
        button_factory& operator=(button_factory&& other) = default;

        void build_button(size_t kind, size_t id, Vector2 position, size_t subject);
    private:
        std::array<std::function<void(size_t, Vector2, size_t)>, hud_config::button_kinds_size> builders_;
    };
}
#endif
