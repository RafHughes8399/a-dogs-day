#ifndef HUD_H
#define HUD_H


#include "events.h"
#include "items.h"
#include "sprite.h"
#include <memory>
#include <vector>
#include <string>

namespace hud{

    class hud_element{
        public:
            virtual ~hud_element() = default;
            hud_element(Vector2 position, Rectangle outline, sprite::sprite sprite)
            : outline_(outline), position_(position), sprite_(sprite) {};

            hud_element(const hud_element& other) = default;
            hud_element(hud_element&& other) = default;
            hud_element& operator=(const hud_element& other) = default;
            hud_element& operator=(hud_element&& other) = default;

            Rectangle get_outline();
            sprite::sprite& get_sprite();
            Vector2 get_position();


        protected:
            Rectangle outline_;
            sprite::sprite sprite_;
            Vector2 position_;

    };
    class button : hud_element {
        public:
            ~button() = default;
            button(Vector2 position, Rectangle outline, sprite::sprite sprite)
            : hud_element(position, outline, sprite), menu_interact_handler_([this](const events::interact_menu& event) -> void {on_menu_interact(event);}){};

            
            button(const button& other) = default;
            button(button&& other) = default;
            button& operator=(const button& other) = default;
            button& operator=(button&& other) = default;

            void subscribe();
            void unsubscribe();
            void on_menu_interact(const events::interact_menu& event);

            void press_button();
        private:
            events::event_handler<events::interact_menu> menu_interact_handler_;
    };

    class hud{
        public:
            ~hud() = default;
            hud()
            : elements_(){};

            hud(const hud& other) = delete;
            hud(hud&& other) = default;
            hud& operator=(const hud& other) =delete;
            hud& operator=(hud&& other) = default;

            void buttons_subscribe();
            void buttons_unsubscribe();

        private:
            std::vector<std::unique_ptr<hud_element>> elements_;

    };

    class hud_builder{
        hud build_player_hud();
        hud build_pause_menu_hud();
        button build_button();
        std::unique_ptr<hud_element> build_item_hud_element(items::item&  item);
        // .....
    };
    extern hud_builder h_builder_;

}
#endif 