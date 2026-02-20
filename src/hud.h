#ifndef HUD_H
#define HUD_H

#include "config.h"
#include "events.h"
#include "items.h"
#include "sprite.h"
#include <memory>
#include <vector>
#include <string>

namespace hud{
    // some habve a sprite, some are just a draw rectangle / draw line
    class hud_element{
        public:
        class draw_strategy{
            public:
            virtual ~draw_strategy() = default;
            draw_strategy() = default;
            draw_strategy(const draw_strategy& other) = default;
            draw_strategy(draw_strategy&& other) = default;

            draw_strategy& operator=(const draw_strategy& other) = default;
            draw_strategy& operator=(draw_strategy&& other) = default;

            virtual void draw(Vector2 position) = 0;
        };
        class sprite_draw : public draw_strategy{
            public:
            virtual ~sprite_draw() = default;
            sprite_draw(sprite::sprite& sprite)
            : draw_strategy(), sprite_(sprite){};

            sprite_draw(const sprite_draw& other) = default;
            sprite_draw(sprite_draw&& other) = default;

            sprite_draw& operator=(const sprite_draw& other) = default;
            sprite_draw& operator=(sprite_draw&& other) = default;

            void draw(Vector2 position) override;
            private:
            sprite::sprite sprite_;
        };
        
        class rectangle_draw : public draw_strategy{
            public:
                virtual ~rectangle_draw() = default;
                rectangle_draw(Rectangle rectangle, Color colour)
                : draw_strategy(), rectangle_(rectangle), colour_(colour){};

                rectangle_draw(const rectangle_draw& other) = default;
                rectangle_draw(rectangle_draw&& other) = default;

                rectangle_draw& operator=(const rectangle_draw& other) = default;
                rectangle_draw& operator=(rectangle_draw&& other) = default;
                void draw(Vector2 position) override;
            private:
                Rectangle rectangle_;
                Color colour_; // damn american spelling
            };
        class grid_draw : public draw_strategy{
            public:
                virtual ~grid_draw() = default;
                grid_draw()
                : draw_strategy(){};

                grid_draw(const grid_draw& other) = default;
                grid_draw(grid_draw&& other) = default;

                grid_draw& operator=(const grid_draw& other) = default;
                grid_draw& operator=(grid_draw&& other) = default;

                void draw(Vector2 position) override;
            private:

        };
            virtual ~hud_element() = default;
            hud_element(Vector2 position, Rectangle outline, std::unique_ptr<draw_strategy> draw_strat)
            : outline_(outline), position_(position), draw_strategy_(std::move(draw_strat)) {};

            hud_element(const hud_element& other) = default;
            hud_element(hud_element&& other) = default;
            hud_element& operator=(const hud_element& other) = default;
            hud_element& operator=(hud_element&& other) = default;

            Rectangle get_outline();
            sprite::sprite& get_sprite();
            Vector2 get_position();

            void draw();

        protected:
            Rectangle outline_;
            std::unique_ptr<draw_strategy> draw_strategy_;
            Vector2 position_;

    };
    class button : hud_element {
        public:
            ~button() = default;
            button(Vector2 position, Rectangle outline, std::unique_ptr<draw_strategy> sprite_draw)
            : hud_element(position, outline, std::move(sprite_draw)), menu_interact_handler_([this](const events::interact_menu& event) -> void {on_menu_interact(event);}){};

            
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
        // builder picks the strategy
        std::unique_ptr<hud_element> build_item_hud_element(items::item&  item);
        // .....
    };
    extern hud_builder h_builder_;

}
#endif 