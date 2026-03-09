#ifndef HUD_H
#define HUD_H

#include "config.h"
#include "events.h"
#include "items.h"
#include "sprite.h"
#include "texture.h"
#include <memory>
#include <vector>
#include <string>

namespace hud{
    // some habve a sprite, some are just a draw rectangle / draw line
    // here's some thinking, a hud elemetn will need to respond to events

    // previously I did a event strategy pattern, I dont mind that idea
    // can have multiple strategies
    // TODO event strategies
    
    class hud_element{
        public:
        // --------------------------- event handle strategies --------------------------------------//
        // allows for a hud element to have its own event handlder (or multiple)
        // parent handles construction, subscription and unsubscription, no it can;t because its tempalted
        class event_strategy{
            public:
                virtual ~event_strategy() = default;
                event_strategy(std::unique_ptr<events::event_handler_interface> handler)
                : handler_(std::move(handler)){
                }
                event_strategy(const event_strategy& other) = delete;
                event_strategy(event_strategy&& other) = default;

                event_strategy& operator=(const event_strategy& other) = delete;
                event_strategy& operator=(event_strategy&& other) = default;
                virtual void on_event(const events::event& event) = 0;
                virtual void unsubscribe() = 0;
                virtual void subscribe() = 0;
            protected:
                std::unique_ptr<events::event_handler_interface> handler_;
        };
        class empty_handler_strategy : public event_strategy{
            public:
                ~empty_handler_strategy() = default;
                empty_handler_strategy()
                : event_strategy(std::make_unique<events::event_handler<events::empty_event>>([this](const events::empty_event& event) -> void {on_event(event);})){};
                void on_event(const events::event& event) override{
                    // do nothing
                }
                void unsubscribe() override{
                    // do nothing
                }
                void subscribe() override{
                    // do nothing
                }
            private:
                std::unique_ptr<events::event_handler<events::empty_event>> handler_;
        };
        class edit_wheel_strategy : public event_strategy{
            public:
                ~edit_wheel_strategy(){
                    unsubscribe();
                }
                edit_wheel_strategy(sprite::sprite* sprite, Vector2* position)
                : event_strategy(std::make_unique<events::event_handler<events::edit_hold>>([this](const events::edit_hold& event) -> void {on_event(event);})), 
                position_(position), sprite_(sprite){
                    subscribe();
                }
                void on_event(const events::event& event) override{
                    const events::edit_hold& edit_event = static_cast<const events::edit_hold&>(event);
                    if(position_){
                        position_->x = edit_event.get_position().x;
                        position_->y = edit_event.get_position().y;
                    }
                    sprite_->get_animation().goto_frame(edit_event.get_edit_progress());
                }
                void unsubscribe() override{
                    auto* handler_cast = static_cast<events::event_handler<events::edit_hold>*>(handler_.get());
                    event_interface::unsubscribe<events::edit_hold>(*handler_cast);
                }
                void subscribe() override{
                    auto* handler_cast = static_cast<events::event_handler<events::edit_hold>*>(handler_.get());
                    event_interface::subscribe<events::edit_hold>(*handler_cast);
                }
            private:
                sprite::sprite* const sprite_;
                Vector2* const position_;
        };
        // ------------------------------- event hand strategies --------------------------------------//
        // ------------------ draw strateiges --------------------------------------- //
        class draw_strategy{
            public:
            virtual ~draw_strategy() = default;
            draw_strategy(Vector2 position = Vector2Zero()) : position_(position) {};
            draw_strategy(const draw_strategy& other) = default;
            draw_strategy(draw_strategy&& other) = default;

            draw_strategy& operator=(const draw_strategy& other) = default;
            draw_strategy& operator=(draw_strategy&& other) = default;

            virtual void draw() = 0;
            void set_position(Vector2 position) { position_ = position; }
            Vector2 get_position() { return position_; }
            Vector2* get_position_ptr() {return &position_;}
            
            protected:
            Vector2 position_;
        };
        class sprite_draw : public draw_strategy{
            public:
            virtual ~sprite_draw() = default;
            sprite_draw(sprite::sprite sprite, Vector2 position = {0, 0})
            : draw_strategy(position), sprite_(sprite){
            };

            sprite_draw(const sprite_draw& other) = default;
            sprite_draw(sprite_draw&& other) = default;

            sprite_draw& operator=(const sprite_draw& other) = delete;
            sprite_draw& operator=(sprite_draw&& other) = delete;

            void draw() override;
            sprite::sprite* get_sprite() { return &sprite_; }

            private:
            sprite::sprite sprite_;
        };
        
        class rectangle_draw : public draw_strategy{
            public:
                virtual ~rectangle_draw() = default;
                rectangle_draw(Rectangle rectangle, Color colour)
                : draw_strategy({rectangle.x, rectangle.y}), rectangle_(rectangle), colour_(colour){};

                rectangle_draw(const rectangle_draw& other) = default;
                rectangle_draw(rectangle_draw&& other) = default;

                rectangle_draw& operator=(const rectangle_draw& other) = default;
                rectangle_draw& operator=(rectangle_draw&& other) = default;
                void draw() override;
            private:
                Rectangle rectangle_;
                Color colour_; // damn american spelling
            };
        class grid_draw : public draw_strategy{
            public:
                virtual ~grid_draw() = default;
                grid_draw(Vector2 position = {0, 0})
                : draw_strategy(position){};

                grid_draw(const grid_draw& other) = default;
                grid_draw(grid_draw&& other) = default;

                grid_draw& operator=(const grid_draw& other) = default;
                grid_draw& operator=(grid_draw&& other) = default;

                void draw() override;
            private:
        };
        // ------------------------ draw strategy --------------------------------------------------- // 
        // ----------------------------- hud element --------------------------------------------- //
            virtual ~hud_element() = default;
            hud_element(Rectangle outline, std::unique_ptr<draw_strategy> draw_strat, std::unique_ptr<event_strategy> event_strat)
            : outline_(outline), draw_strategy_(std::move(draw_strat)), event_handler_strategy_(std::move(event_strat)) {
                if(event_handler_strategy_ && draw_strategy_){
                }
            }

            hud_element(const hud_element& other) = delete;
            hud_element(hud_element&& other) = default;
            hud_element& operator=(const hud_element& other) = delete;
            hud_element& operator=(hud_element&& other) = default;

            Rectangle get_outline();
            Vector2 get_position();

            void draw();
            void set_position(Vector2 position);
        protected:
            Rectangle outline_;
            std::unique_ptr<draw_strategy> draw_strategy_;
            std::unique_ptr<event_strategy> event_handler_strategy_;

    };
    class button : hud_element {
        public:
            ~button() = default;
            // here specify what the draw strat type and event listener types are ? 
            button(Rectangle outline, std::unique_ptr<draw_strategy> sprite_draw, std::unique_ptr<event_strategy> event_handler)
            : hud_element(outline, std::move(sprite_draw), std::move(event_handler)){};

            
            button(const button& other) = delete;
            button(button&& other) = default;
            button& operator=(const button& other) = delete;
            button& operator=(button&& other) = default;

            void subscribe();
            void unsubscribe();
            //void on_menu_interact(const events::interact_menu& event);

            void press_button();
        private:
            //events::event_handler<events::interact_menu> menu_interact_handler_;
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
            
            void add_element(std::unique_ptr<hud_element> element);
            void render();
        private:
            std::vector<std::unique_ptr<hud_element>> elements_;

    };

    class hud_builder{
    public:
        hud build_player_hud();
        hud build_pause_menu_hud();
        button build_button();
        // builder picks the strategy
        std::unique_ptr<hud_element> build_item_hud_element(items::item& item);
        std::unique_ptr<hud_element> build_edit_wheel();
        std::unique_ptr<hud_element> build_decoration_grid();
        std::unique_ptr<hud_element> build_green_highlight();
        std::unique_ptr<hud_element> build_red_highlight();
        // .....
    };
    extern hud_builder h_builder_;

}
#endif 