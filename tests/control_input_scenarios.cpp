#include <catch2/catch_test_macros.hpp>

#include "ecs_test_game.h"
#include "test_game.h"
#include "component.h"
#include "events.h"
#include "events_interface.h"
#include "menus.h"
#include "system.h"

// -----------------------------------------------------------------------------
// control_input_system - the bindings an entity holds, the actions they reach.
//
// raylib 5.5 exposes no way to inject key or button state, so these enter through
// simulate_input, which is what check_inputs calls once IsKeyPressed and friends
// have said a binding fired. Everything past that point - the {key_, action_}
// lookup and the action itself - is the real path. The device query is the one
// thing not covered here, and cannot be without a window taking real OS input.
// -----------------------------------------------------------------------------

namespace {

    // RAII, because REQUIRE throws - an unsubscribe after the assertion would be
    // skipped on failure and leak the handler into the next scenario
    template<typename E>
    class listener{
        public:
            listener(std::function<void(const E&)> on_event)
            : handler_(std::move(on_event)){
                event_interface::subscribe<E>(handler_);
            }
            ~listener(){
                event_interface::unsubscribe<E>(handler_);
            }
            listener(const listener&) = delete;
            listener(listener&&) = delete;
            listener& operator=(const listener&) = delete;
            listener& operator=(listener&&) = delete;
        private:
            events::event_handler<E> handler_;
    };

    // the actions queue rather than execute, so nothing is observable until the
    // dispatcher is pumped
    void flush(){
        events::global_dispatcher_.process_events(0.0f);
    }

    systems::control_input_system& controls(){
        return systems::control_input_system::get_instance();
    }

    systems::selection_system& selection(){
        return systems::selection_system::get_instance();
    }

    game_config::input key_press_of(int key){
        return game_config::input{key, game_config::key_press};
    }
    game_config::input key_hold_of(int key){
        return game_config::input{key, game_config::key_hold};
    }
    game_config::input mouse_press_of(int button){
        return game_config::input{button, game_config::mouse_press};
    }

    // menus::menu_graph::menu_ids is private, so the ids are mirrored here. if
    // the enum in menus.h gains or reorders a menu, these go stale with it
    enum test_menu_ids{
        blank_menu = 0,
        pause_menu = 1,
        tab_menu = 2,
        inventory_menu = 3,
        map_menu = 4,
        shop_menu = 5,
        quest_menu = 6
    };

    // the whole path a key takes to a menu: control map -> queued key_press ->
    // menu_graph's handler
    void press(int key, size_t id = 0){
        controls().simulate_input(key_press_of(key), id);
        flush();
    }

} // namespace

SCENARIO("the control map covers every binding the config hands out", "[ecs][controls]"){
    GIVEN("a fresh ecs world"){
        testing::ecs_test_game game;

        THEN("every binding in player_controls has an action"){
            for(const auto& binding : game_config::player_controls){
                REQUIRE(controls().is_bound(binding.key_, binding.action_));
            }
        }
        THEN("every binding in cursor_controls has an action"){
            for(const auto& binding : game_config::cursor_controls){
                REQUIRE(controls().is_bound(binding.key_, binding.action_));
            }
        }
        THEN("a binding is matched on the action as well as the key"){
            // KEY_UP is bound as a hold, so the same key as a press is a miss
            REQUIRE(controls().is_bound(controls_config::key_hold_actions::move_up, game_config::key_hold));
            REQUIRE_FALSE(controls().is_bound(controls_config::key_hold_actions::move_up, game_config::key_press));
        }
        THEN("the edit bindings are deliberately unbound"){
            REQUIRE_FALSE(controls().is_bound(controls_config::key_hold_actions::edit_mode, game_config::key_hold));
            REQUIRE_FALSE(controls().is_bound(controls_config::key_press_actions::exit_edit, game_config::key_press));
        }
    }
}

SCENARIO("a simulated key press runs its action", "[ecs][controls]"){
    GIVEN("a player entity and a listener on key_press"){
        testing::ecs_test_game game;
        auto cursor_id = game.create_cursor();
        auto player_id = game.create_player(cursor_id);

        std::vector<int> announced;
        listener<events::key_press> keys([&announced](const events::key_press& event) -> void{
            announced.push_back(event.get_key());
        });

        WHEN("each menu key is pressed"){
            const std::vector<int> menu_keys = {
                controls_config::key_press_actions::shop_open,
                controls_config::key_press_actions::inventory_open,
                controls_config::key_press_actions::menu_open,
                controls_config::key_press_actions::quests_open,
                controls_config::key_press_actions::map_open,
                controls_config::key_press_actions::back
            };
            for(const auto& key : menu_keys){
                controls().simulate_input(key_press_of(key), player_id);
            }
            flush();

            THEN("each one announces itself, in order, exactly once"){
                REQUIRE(announced == menu_keys);
            }
        }

        WHEN("a key with no binding is pressed"){
            controls().simulate_input(key_press_of(KEY_Z), player_id);
            flush();

            THEN("nothing is announced"){
                REQUIRE(announced.empty());
            }
        }

        WHEN("a bound key arrives under the wrong action"){
            // move_up is a hold binding - as a press it matches nothing
            controls().simulate_input(key_press_of(controls_config::key_hold_actions::move_up), player_id);
            flush();

            THEN("nothing is announced"){
                REQUIRE(announced.empty());
            }
        }
    }
}

SCENARIO("switching dogs flips the selection and announces it", "[ecs][controls]"){
    GIVEN("a player entity and a listener on selected_dog"){
        testing::ecs_test_game game;
        auto cursor_id = game.create_cursor();
        auto player_id = game.create_player(cursor_id);

        std::vector<size_t> selected;
        listener<events::selected_dog> dogs([&selected](const events::selected_dog& event) -> void{
            selected.push_back(event.get_id());
        });

        THEN("the selection starts on mack"){
            REQUIRE(controls().get_selected_dog() == level_config::mack_id);
        }

        WHEN("the switch key is pressed"){
            controls().simulate_input(key_press_of(controls_config::key_press_actions::dog_switch), player_id);
            flush();

            THEN("the other dog is selected and announced"){
                REQUIRE(controls().get_selected_dog() == level_config::khiri_id);
                REQUIRE(selected == std::vector<size_t>{level_config::khiri_id});
            }

            WHEN("it is pressed again"){
                controls().simulate_input(key_press_of(controls_config::key_press_actions::dog_switch), player_id);
                flush();

                THEN("the selection flips back"){
                    REQUIRE(controls().get_selected_dog() == level_config::mack_id);
                    REQUIRE(selected == std::vector<size_t>{level_config::khiri_id, level_config::mack_id});
                }
            }
        }
    }
}

SCENARIO("a held direction key moves the view frame that way", "[ecs][controls]"){
    GIVEN("a player entity and a listener on move_view_frame"){
        testing::ecs_test_game game;
        auto cursor_id = game.create_cursor();
        auto player_id = game.create_player(cursor_id);

        std::vector<Vector2> moves;
        listener<events::move_view_frame> frame([&moves](const events::move_view_frame& event) -> void{
            moves.push_back(event.get_delta());
        });

        WHEN("up is held for a frame"){
            controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_up), player_id, 1.0f);
            flush();

            THEN("the frame is asked to move up by a full second of travel"){
                REQUIRE(moves.size() == 1);
                REQUIRE(moves[0].x == 0.0f);
                REQUIRE(moves[0].y == -level_config::frame_move.y);
            }
        }

        WHEN("each direction is held"){
            controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_left), player_id, 1.0f);
            controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_right), player_id, 1.0f);
            controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_down), player_id, 1.0f);
            flush();

            THEN("each moves along its own axis, in its own direction"){
                REQUIRE(moves.size() == 3);
                REQUIRE(moves[0].x == -level_config::frame_move.x);
                REQUIRE(moves[1].x == level_config::frame_move.x);
                REQUIRE(moves[2].y == level_config::frame_move.y);
            }
        }

        WHEN("the same key is held for half a frame"){
            controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_right), player_id, 0.5f);
            flush();

            THEN("the move is scaled by delta"){
                REQUIRE(moves.size() == 1);
                REQUIRE(moves[0].x == level_config::frame_move.x * 0.5f);
            }
        }
    }
}

SCENARIO("a left click selects the entity under the cursor", "[ecs][controls][selection]"){
    // left_click reads GetMousePosition() itself and the device cannot be
    // driven from a test, so the dog is placed AT whatever the mouse reports
    // rather than the mouse being moved onto the dog. the click is asked for by
    // the cursor, which sits on that same position - the query has to see past
    // itself to find the dog underneath
    GIVEN("a player dog sitting under the cursor"){
        testing::ecs_test_game game;
        auto cursor_id = game.create_cursor();
        auto khiri_id = game.create_khiri();
        game.move_entity(khiri_id, GetMousePosition());

        REQUIRE(selection().selected() == game_config::empty_entity);

        WHEN("the left button is pressed"){
            controls().simulate_input(mouse_press_of(MOUSE_BUTTON_LEFT), cursor_id);

            THEN("the dog becomes the selected entity"){
                REQUIRE(selection().selected() == static_cast<int>(khiri_id));
            }
            THEN("its selectable component agrees"){
                auto* selectable = component_managers::selectable_manager_.get_component(khiri_id);
                REQUIRE(selectable != nullptr);
                REQUIRE(selectable->is_selected());
            }
        }
    }

    GIVEN("a selected dog and empty space under the cursor"){
        testing::ecs_test_game game;
        auto cursor_id = game.create_cursor();
        auto khiri_id = game.create_khiri();
        game.move_entity(khiri_id, GetMousePosition());
        controls().simulate_input(mouse_press_of(MOUSE_BUTTON_LEFT), cursor_id);
        REQUIRE(selection().selected() == static_cast<int>(khiri_id));

        WHEN("the dog moves away and the left button is pressed again"){
            game.move_entity(khiri_id, Vector2{level_config::edge_weight * 12,
                                               level_config::edge_weight * 12});
            controls().simulate_input(mouse_press_of(MOUSE_BUTTON_LEFT), cursor_id);

            THEN("the selection is cleared"){
                REQUIRE(selection().selected() == game_config::empty_entity);
            }
            THEN("the component is no longer selected"){
                auto* selectable = component_managers::selectable_manager_.get_component(khiri_id);
                REQUIRE(selectable != nullptr);
                REQUIRE_FALSE(selectable->is_selected());
            }
        }
    }
}

SCENARIO("a right click paths the selected player dog to the clicked position",
         "[ecs][controls][movement]"){
    GIVEN("a selected player dog and a listener on create_path_to"){
        testing::ecs_test_game game;
        auto cursor_id = game.create_cursor();
        auto khiri_id = game.create_khiri();
        game.move_entity(khiri_id, GetMousePosition());
        controls().simulate_input(mouse_press_of(MOUSE_BUTTON_LEFT), cursor_id);
        REQUIRE(selection().selected() == static_cast<int>(khiri_id));

        std::vector<size_t> path_for;
        std::vector<Vector2> path_to;
        listener<events::create_path_to> paths([&](const events::create_path_to& event) -> void{
            path_for.push_back(event.get_id());
            path_to.push_back(event.get_destination());
        });

        WHEN("the right button is pressed"){
            controls().simulate_input(mouse_press_of(MOUSE_BUTTON_RIGHT), cursor_id);
            flush();

            THEN("one path is requested, for the selected dog"){
                REQUIRE(path_for.size() == 1);
                REQUIRE(path_for[0] == khiri_id);
            }
            THEN("it is aimed at the clicked position"){
                REQUIRE(path_to.size() == 1);
                REQUIRE(path_to[0].x == GetMousePosition().x);
                REQUIRE(path_to[0].y == GetMousePosition().y);
            }
        }

        WHEN("nothing is selected and the right button is pressed"){
            selection().deselect();
            REQUIRE(selection().selected() == game_config::empty_entity);
            controls().simulate_input(mouse_press_of(MOUSE_BUTTON_RIGHT), cursor_id);
            flush();

            THEN("no path is requested"){
                REQUIRE(path_for.empty());
            }
        }

        WHEN("a mouse button with no binding is pressed"){
            controls().simulate_input(mouse_press_of(MOUSE_BUTTON_MIDDLE), cursor_id);
            flush();

            THEN("no path is requested and the selection is untouched"){
                REQUIRE(path_for.empty());
                REQUIRE(selection().selected() == static_cast<int>(khiri_id));
            }
        }
    }
}

SCENARIO("the selection does not leak between scenarios", "[ecs][controls]"){
    GIVEN("a world where the dog was switched"){
        {
            testing::ecs_test_game game;
            auto cursor_id = game.create_cursor();
            controls().simulate_input(key_press_of(controls_config::key_press_actions::dog_switch), cursor_id);
            flush();
            REQUIRE(controls().get_selected_dog() == level_config::khiri_id);
        }

        WHEN("a new world is built"){
            testing::ecs_test_game game;

            THEN("the selection is back on mack"){
                REQUIRE(controls().get_selected_dog() == level_config::mack_id);
            }
        }
    }
}

// -----------------------------------------------------------------------------
// arrow keys -> the level's view frame.
//
// These use test_game rather than ecs_test_game because level::level is what
// consumes move_view_frame. The ECS rendering_system holds a view_frame_ of its
// own but subscribes to nothing, so there is no ECS-side frame to assert on yet.
//
// The test window is 1x1, so the clamp in level::on_move_view_frame_event allows
// travel right up to world_x - 1 / world_y - 1.
// -----------------------------------------------------------------------------

SCENARIO("a held arrow key moves the level's view frame by the delta", "[controls][view_frame]"){
    GIVEN("a level with the frame at the origin"){
        testing::test_game game;

        THEN("it starts at the origin"){
            REQUIRE(game.view_frame().x == 0.0f);
            REQUIRE(game.view_frame().y == 0.0f);
        }

        WHEN("right is held for a full second"){
            controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_right), 0, 1.0f);
            flush();

            THEN("the frame has travelled one second of frame_move, on x only"){
                REQUIRE(game.view_frame().x == level_config::frame_move.x);
                REQUIRE(game.view_frame().y == 0.0f);
            }
        }

        WHEN("down is held for a full second"){
            controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_down), 0, 1.0f);
            flush();

            THEN("the frame has travelled on y only"){
                REQUIRE(game.view_frame().y == level_config::frame_move.y);
                REQUIRE(game.view_frame().x == 0.0f);
            }
        }

        WHEN("right is held for half a second"){
            controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_right), 0, 0.5f);
            flush();

            THEN("the frame travels half as far"){
                REQUIRE(game.view_frame().x == level_config::frame_move.x * 0.5f);
            }
        }

        WHEN("right is held over several frames"){
            for(int frame = 0; frame < 3; ++frame){
                controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_right), 0, 0.5f);
            }
            flush();

            THEN("the travel accumulates"){
                REQUIRE(game.view_frame().x == level_config::frame_move.x * 1.5f);
            }
        }

        WHEN("right then left are held for the same duration"){
            controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_right), 0, 1.0f);
            controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_left), 0, 1.0f);
            flush();

            THEN("the frame is back where it started"){
                REQUIRE(game.view_frame().x == 0.0f);
            }
        }

        WHEN("left is held at the origin"){
            controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_left), 0, 1.0f);
            flush();

            THEN("the frame clamps rather than going negative"){
                REQUIRE(game.view_frame().x == 0.0f);
            }
        }

        WHEN("up is held at the origin"){
            controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_up), 0, 1.0f);
            flush();

            THEN("the frame clamps rather than going negative"){
                REQUIRE(game.view_frame().y == 0.0f);
            }
        }

        WHEN("up is held after moving down"){
            controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_down), 0, 2.0f);
            controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_up), 0, 1.0f);
            flush();

            THEN("it comes back by exactly one second of travel"){
                REQUIRE(game.view_frame().y == level_config::frame_move.y);
            }
        }

        WHEN("the frame is driven past the far edge of the world"){
            for(int frame = 0; frame < 100; ++frame){
                controls().simulate_input(key_hold_of(controls_config::key_hold_actions::move_right), 0, 1.0f);
            }
            flush();

            THEN("it clamps to the world's width less the screen"){
                REQUIRE(game.view_frame().x == level_config::world_x - GetScreenWidth());
            }
        }
    }
}

// -----------------------------------------------------------------------------
// menu keys -> menu_graph navigation.
//
// Driven end to end: simulate_input runs the control map's action, which queues
// a key_press, which menu_graph's own handler picks up on the flush. Nothing
// about the transition is faked.
// -----------------------------------------------------------------------------

SCENARIO("menu keys navigate the menu graph", "[controls][menus]"){
    GIVEN("a fresh ecs world and a menu graph listening"){
        testing::ecs_test_game game;
        menus::menu_graph menus;

        THEN("it starts on the blank menu"){
            REQUIRE(menus.current_ == blank_menu);
        }

        WHEN("each menu key is pressed from blank"){
            const std::vector<std::pair<int, size_t>> routes = {
                {controls_config::key_press_actions::menu_open,      tab_menu},
                {controls_config::key_press_actions::shop_open,      shop_menu},
                {controls_config::key_press_actions::quests_open,    quest_menu},
                {controls_config::key_press_actions::inventory_open, inventory_menu},
                {controls_config::key_press_actions::map_open,       map_menu},
                {controls_config::key_press_actions::back,           pause_menu}
            };

            THEN("each opens its own menu, and back returns to blank"){
                for(const auto& route : routes){
                    press(route.first);
                    REQUIRE(menus.current_ == route.second);

                    press(controls_config::key_press_actions::back);
                    REQUIRE(menus.current_ == blank_menu);
                }
            }
        }

        WHEN("a menu is open"){
            press(controls_config::key_press_actions::shop_open);
            REQUIRE(menus.current_ == shop_menu);

            THEN("another menu's key does nothing - the only edge out is back"){
                press(controls_config::key_press_actions::map_open);
                REQUIRE(menus.current_ == shop_menu);

                press(controls_config::key_press_actions::inventory_open);
                REQUIRE(menus.current_ == shop_menu);
            }
            THEN("its own key does not re-enter it either"){
                press(controls_config::key_press_actions::shop_open);
                REQUIRE(menus.current_ == shop_menu);
            }
            THEN("back leaves to blank, and a second back opens pause"){
                press(controls_config::key_press_actions::back);
                REQUIRE(menus.current_ == blank_menu);

                press(controls_config::key_press_actions::back);
                REQUIRE(menus.current_ == pause_menu);
            }
        }

        WHEN("a key with no edge is pressed from blank"){
            press(controls_config::key_press_actions::dog_switch);

            THEN("the menu does not move"){
                REQUIRE(menus.current_ == blank_menu);
            }
        }

        WHEN("menus are opened and closed repeatedly"){
            press(controls_config::key_press_actions::menu_open);
            press(controls_config::key_press_actions::back);
            press(controls_config::key_press_actions::map_open);
            press(controls_config::key_press_actions::back);

            THEN("the graph ends where it started"){
                REQUIRE(menus.current_ == blank_menu);
            }
        }
    }
}
