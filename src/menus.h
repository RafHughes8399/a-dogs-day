#ifndef MENU_H
#define MENU_H

#include <memory>

#include "items.h"
#include "raylib.h"
#include "sprite.h"
namespace menus{
    class menu{
        public:
            virtual ~menu() = default;
            menu();
            menu(const menu& other) = default;
            menu(menu&& other) = default;
            menu& operator=(const menu& other) = default;
            menu& operator=(menu&& other) = default;
        private:
            Vector2 position_;
            Rectangle box_;

            // buttons
            // hud elements



    };  
    class item_menu : public menu{

    };
    class menu_graph{
        public:
        private:
    };

    class menu_builder{
        public:
            std::unique_ptr<menu> build_pause_menu();

        private:
    };

    extern menu_builder m_builder_;
    
}
#endif