/**
 * cursor and paw_mark entities.
 */
#ifndef CURSOR_ENTITY_H
#define CURSOR_ENTITY_H

#include "entity.h"

namespace entities{
    class cursor : public entity{
        public:
            class state {
                public:
                    virtual ~state() = default;
                    state(){}
                    state(const state& other) = default;
                    state(state&& other) = default;

                    state& operator=(const state& other) = default;
                    state& operator=(state&& other) = default;

                    virtual void create_move_event(cursor& cursor);
                    virtual void left_click(cursor& cursor, entity& other);
                    virtual void right_click(cursor& cursor, entity& other);
            };
            class in_menus : public state{
                // to be implemented
                in_menus()
                : state() {}
            };
            class editing : public state{
                public:
                    editing()
                    : state(){}
                    editing(const editing& other) = default;
                    editing(editing&& other) = default;

                    editing& operator=(const editing& other) = default;
                    editing& operator=(editing&& other) = default;

                    void left_click(cursor& cursor, entity& other) override;
                    void right_click(cursor& cursor, entity& other) override;

                };
                class carrying_decoration : public editing {
                    public:
                    carrying_decoration(entity* carried)
                    : editing(), carried_decoration_(carried){}
                    carrying_decoration(const carrying_decoration& other) = default;
                    carrying_decoration(carrying_decoration&& other) = default;

                    carrying_decoration& operator=(const carrying_decoration& other) = default;
                    carrying_decoration& operator=(carrying_decoration&& other) = default;

                    void left_click(cursor& cursor, entity& other) override;
                    void create_move_event(cursor& cursor) override;
                    private:
                    entity* carried_decoration_;
            };
            class interaction_strategy{
                public:
                    virtual ~interaction_strategy() = default;
                    interaction_strategy(){}
                    interaction_strategy(const interaction_strategy& other) = default;
                    interaction_strategy(interaction_strategy&& other) = default;

                    interaction_strategy& operator=(const interaction_strategy& other) = default;
                    interaction_strategy& operator=(interaction_strategy&& other) = default;

                    virtual void interact(cursor& cursor, entity& other) = 0;
                private:
            };
            class default_strategy : public interaction_strategy{
                public:
                    default_strategy()
                    : interaction_strategy() {}
                    default_strategy(const default_strategy& other) = default;
                    default_strategy(default_strategy&& other) = default;

                    default_strategy& operator=(const default_strategy& other) = default;
                    default_strategy& operator=(default_strategy&& other) = default;
                    void interact(cursor& cursor, entity& other) override;
                private:

            };
            class left_click_strategy : public interaction_strategy{
                public:
                    left_click_strategy()
                    : interaction_strategy() {}
                    left_click_strategy(const left_click_strategy& other) = default;
                    left_click_strategy(left_click_strategy&& other) = default;

                    left_click_strategy& operator=(const left_click_strategy& other) = default;
                    left_click_strategy& operator=(left_click_strategy&& other) = default;

                    void interact(cursor& cursor, entity& other) override;

                    private:
                };

                class right_click_strategy : public interaction_strategy{
                    public:
                    right_click_strategy()
                    : interaction_strategy() {}
                    right_click_strategy(const right_click_strategy& other) = default;
                    right_click_strategy(right_click_strategy&& other) = default;

                    right_click_strategy& operator=(const right_click_strategy& other) = default;
                    right_click_strategy& operator=(right_click_strategy&& other) = default;

                    void interact(cursor& cursor, entity& other) override;
                    private:
                };

                ~cursor() override {
                    event_interface::unsubscribe<events::enter_edit_mode>(enter_edit_mode_handler_);
                    event_interface::unsubscribe<events::exit_edit_mode>(exit_edit_mode_handler_);
                    event_interface::unsubscribe<events::left_mouse_click>(left_mouse_click_handler_);
                    event_interface::unsubscribe<events::move_view_frame>(move_view_frame_handler_);
                    event_interface::unsubscribe<events::right_mouse_click>(right_mouse_click_handler_);
                }
                cursor(body::body body, Vector2 position, int id, std::string debug_id)
                : entity(body, position, id, std::move(debug_id)),
                enter_edit_mode_handler_([this](const events::enter_edit_mode& event) -> void{on_enter_edit_mode_event(event);}),
                exit_edit_mode_handler_([this](const events::exit_edit_mode& event) -> void{on_exit_edit_mode_event(event);}),
                interaction_strategy_(std::make_unique<default_strategy>()),
                left_mouse_click_handler_([this](const events::left_mouse_click& event) -> void{on_left_mouse_click_event(event);}),
                move_view_frame_handler_([this](const events::move_view_frame& event) -> void{on_move_view_frame_event(event);}),
                right_mouse_click_handler_([this](const events::right_mouse_click& event) -> void{on_right_mouse_click_event(event);}),
                state_(std::make_unique<state>()){
                    event_interface::subscribe<events::enter_edit_mode>(enter_edit_mode_handler_);
                    event_interface::subscribe<events::exit_edit_mode>(exit_edit_mode_handler_);
                    event_interface::subscribe<events::left_mouse_click>(left_mouse_click_handler_);
                    event_interface::subscribe<events::move_view_frame>(move_view_frame_handler_);
                    event_interface::subscribe<events::right_mouse_click>(right_mouse_click_handler_);
                }

                cursor(const cursor& other) = delete;
                cursor(cursor&& other) = default;

                cursor& operator=(const cursor& other) = delete;
                cursor& operator=(cursor&& other)  = delete;


                void create_move_event();
                void interact(entity& other) override;
                void on_enter_edit_mode_event(const events::enter_edit_mode& event);
                void on_exit_edit_mode_event(const events::exit_edit_mode& event);
                void on_left_mouse_click_event(const events::left_mouse_click& event);
                void on_move_view_frame_event(const events::move_view_frame& event);
                void on_right_mouse_click_event(const events::right_mouse_click& event);
                int update(float delta, int frame) override;

            private:

                enum animation_tags{
                        base = 0,
                        hover = 1
                };

                events::event_handler<events::enter_edit_mode> enter_edit_mode_handler_;
                events::event_handler<events::exit_edit_mode> exit_edit_mode_handler_;
                std::unique_ptr<interaction_strategy> interaction_strategy_;
                events::event_handler<events::left_mouse_click> left_mouse_click_handler_;
                events::event_handler<events::move_view_frame> move_view_frame_handler_;
                events::event_handler<events::right_mouse_click> right_mouse_click_handler_;
                std::unique_ptr<state> state_;
        };

        class paw_mark : public entity{
        public:
        paw_mark(body::body body, Vector2 position, int id, std::string debug_id)
        : entity(body, position, id, std::move(debug_id)){}
            paw_mark(const paw_mark& other) = default;
            paw_mark(paw_mark&& other) = default;

            paw_mark& operator=(const paw_mark& other) = delete;
            paw_mark& operator=(paw_mark&& other) = delete;

            int update(float delta, int frame) override;
            void interact(entity& other) override;

        private:
    };
}
#endif
