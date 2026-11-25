#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <algorithm>

#include "ecs_test_game.h"
#include "component.h"
#include "component_managers.hpp"
#include "events.h"
#include "events_interface.h"
#include "hud_systems/hud_systems.hpp"
#include "items.h"
#include "menus.h"
#include "system.h"

namespace {
    const Vector2 cursor_spot{900.0f, 900.0f};

    void emit(size_t category, size_t id, size_t button = static_cast<size_t>(game_config::empty_entity)){
        events::item_from_inventory command{category, id, button};
        event_interface::execute_event(command);
    }

    components::carrier_component* carrier_of(size_t cursor){
        return component_managers::entity_carrier_manager_.get_component(cursor);
    }

    size_t item_button_on(size_t page, size_t index){
        size_t seen = 0;
        for(auto element : component_managers::hud_manager_.get_component(page)->get_elements()){
            auto* clickable = component_managers::hud_clickable_manager_.get_component(element);
            if(clickable == nullptr or clickable->get_action() != hud_config::click_actions::select_item){ continue; }
            if(seen == index){ return element; }
            ++seen;
        }
        return static_cast<size_t>(game_config::empty_entity);
    }

    bool page_has(size_t page, size_t element){
        auto& elements = component_managers::hud_manager_.get_component(page)->get_elements();
        return std::find(elements.begin(), elements.end(), element) != elements.end();
    }
}

SCENARIO("an inventory item command builds the item and hands it to the cursor", "[inventory][creation]"){
    GIVEN("a world with a cursor that carries nothing"){
        testing::ecs_test_game game;
        auto& input = systems::control_input_system::get_instance();
        systems::entity_lifespan_system::get_instance();
        auto cursor = game.create_cursor();
        systems::movement_system::get_instance().update_position(cursor, cursor_spot);
        REQUIRE(carrier_of(cursor) != nullptr);
        REQUIRE_FALSE(carrier_of(cursor)->is_carrying());

        WHEN("a decoration is requested from the inventory"){
            emit(entity_config::decoration_category, entity_config::decorations::gargoyle);

            THEN("the cursor carries a new gargoyle at the cursor position"){
                REQUIRE(carrier_of(cursor)->is_carrying());
                auto item = *carrier_of(cursor)->get_carried_entity();
                auto* ref = component_managers::entity_item_manager_.get_component(item);
                REQUIRE(ref != nullptr);
                REQUIRE(ref->get_ref() == items::item_ref{entity_config::decoration_category, entity_config::decorations::gargoyle});
                auto* position = component_managers::entity_positional_manager_.get_component(item);
                REQUIRE(position != nullptr);
                REQUIRE(position->get_position().x == cursor_spot.x);
                REQUIRE(position->get_position().y == cursor_spot.y);
            }
            THEN("the controls enter the placing state"){
                REQUIRE(input.get_control_state() == game_config::placing_from_inventory);
            }
            AND_WHEN("a second item is requested while the first is still carried"){
                auto first = *carrier_of(cursor)->get_carried_entity();
                emit(entity_config::decoration_category, entity_config::decorations::pavlov);

                THEN("the first item stays carried and nothing new is built"){
                    REQUIRE(*carrier_of(cursor)->get_carried_entity() == first);
                    REQUIRE(component_managers::entity_item_manager_.get_component(first)->get_ref().get_id()
                        == entity_config::decorations::gargoyle);
                }
            }
        }
        WHEN("a station of any kind is requested"){
            auto station = static_cast<size_t>(GENERATE(range(0, static_cast<int>(entity_config::stations_size))));
            emit(entity_config::station_category, station);

            THEN("the cursor carries a station of that kind"){
                REQUIRE(carrier_of(cursor)->is_carrying());
                auto item = *carrier_of(cursor)->get_carried_entity();
                REQUIRE(component_managers::entity_item_manager_.get_component(item)->get_ref()
                    == items::item_ref{entity_config::station_category, station});
            }
        }
        WHEN("an item with no definition is requested"){
            emit(entity_config::decoration_category, entity_config::decorations_size);
            emit(entity_config::waiter_category, 0);

            THEN("nothing is built, carried, or switched"){
                REQUIRE_FALSE(carrier_of(cursor)->is_carrying());
                REQUIRE(input.get_control_state() == game_config::default_controls);
            }
        }
        WHEN("the control input system is cleared after a request"){
            emit(entity_config::decoration_category, entity_config::decorations::gargoyle);
            input.clear();

            THEN("the control state is back to default"){
                REQUIRE(input.get_control_state() == game_config::default_controls);
            }
        }
    }
}

SCENARIO("an inventory item command hides the open inventory", "[inventory][creation][menu]"){
    GIVEN("an open inventory"){
        testing::ecs_test_game game;
        hud_systems::hud::get_instance();
        hud_systems::hud_spatial_system::get_instance();
        systems::control_input_system::get_instance();
        systems::entity_lifespan_system::get_instance();
        systems::rendering_system::get_instance().move_camera(Vector2{300.0f, 2000.0f});
        auto& navigation = systems::menu_navigation_system::get_instance();
        systems::inventory_system::get_instance().init();
        auto cursor = game.create_cursor();
        navigation.on_key_press(events::key_press{game_config::key_press_actions::inventory_open});
        auto head = navigation.pages_of(menus::menu_ids::inventory, entity_config::decoration_category)[0];
        REQUIRE(component_managers::hud_manager_.get_component(head)->is_active());

        WHEN("a decoration is requested from the inventory"){
            emit(entity_config::decoration_category, entity_config::decorations::gargoyle);

            THEN("the inventory page is hidden and the cursor carries the item"){
                REQUIRE_FALSE(component_managers::hud_manager_.get_component(head)->is_active());
                REQUIRE(carrier_of(cursor)->is_carrying());
            }
        }
    }
}

SCENARIO("while placing from the inventory only left click, escape and the camera keys are bound", "[inventory][creation][controls]"){
    GIVEN("the control input system"){
        auto& input = systems::control_input_system::get_instance();

        THEN("the placing state binds left click, escape and the four camera keys and nothing else"){
            REQUIRE(input.bound_count(game_config::placing_from_inventory) == 6);
            REQUIRE(input.is_bound(game_config::key_hold_actions::move_up, game_config::key_hold, game_config::placing_from_inventory));
            REQUIRE(input.is_bound(game_config::key_hold_actions::move_down, game_config::key_hold, game_config::placing_from_inventory));
            REQUIRE(input.is_bound(game_config::key_hold_actions::move_left, game_config::key_hold, game_config::placing_from_inventory));
            REQUIRE(input.is_bound(game_config::key_hold_actions::move_right, game_config::key_hold, game_config::placing_from_inventory));
            REQUIRE(input.is_bound(MOUSE_BUTTON_LEFT, game_config::mouse_press, game_config::placing_from_inventory));
            REQUIRE(input.is_bound(game_config::key_press_actions::back, game_config::key_press, game_config::placing_from_inventory));
            REQUIRE_FALSE(input.is_bound(MOUSE_BUTTON_RIGHT, game_config::mouse_press, game_config::placing_from_inventory));
            REQUIRE_FALSE(input.is_bound(game_config::key_press_actions::inventory_open, game_config::key_press, game_config::placing_from_inventory));
        }
        THEN("the default state keeps its full set"){
            REQUIRE(input.is_bound(MOUSE_BUTTON_RIGHT, game_config::mouse_press));
            REQUIRE(input.is_bound(game_config::key_press_actions::inventory_open, game_config::key_press));
            REQUIRE(input.bound_count() > 2);
        }
    }
}

SCENARIO("placing an item from the inventory ends in a click or escape and returns to the inventory", "[inventory][creation][controls]"){
    GIVEN("a stored gargoyle picked up from the open inventory"){
        testing::ecs_test_game game;
        hud_systems::hud::get_instance();
        hud_systems::hud_spatial_system::get_instance();
        auto& input = systems::control_input_system::get_instance();
        systems::entity_lifespan_system::get_instance();
        systems::rendering_system::get_instance().move_camera(Vector2{300.0f, 2000.0f});
        auto& navigation = systems::menu_navigation_system::get_instance();
        auto& inventory = systems::inventory_system::get_instance();
        inventory.init();
        auto cursor = game.create_cursor();
        systems::movement_system::get_instance().update_position(cursor, Vector2{1030.0f, 1045.0f});
        inventory.on_stored_decoration(events::store_decoration{entity_config::decoration_category, entity_config::decorations::gargoyle});
        navigation.on_key_press(events::key_press{game_config::key_press_actions::inventory_open});
        auto head = navigation.pages_of(menus::menu_ids::inventory, entity_config::decoration_category)[0];
        auto* storage = component_managers::hud_inventory_storage_manager_.get_component(head);
        REQUIRE(storage->size() == 1);
        emit(entity_config::decoration_category, entity_config::decorations::gargoyle, item_button_on(head, 0));
        auto item = *carrier_of(cursor)->get_carried_entity();
        REQUIRE(storage->size() == 0);
        REQUIRE(input.get_control_state() == game_config::placing_from_inventory);
        REQUIRE_FALSE(component_managers::hud_manager_.get_component(head)->is_active());

        WHEN("left click is pressed"){
            input.simulate_input(game_config::input{MOUSE_BUTTON_LEFT, game_config::mouse_press}, cursor);
            events::global_dispatcher_.process_events(0.0f);

            THEN("the item is placed and stays, snapped to the grid, and the inventory is back without it"){
                REQUIRE_FALSE(carrier_of(cursor)->is_carrying());
                REQUIRE(component_managers::entity_positional_manager_.get_component(item) != nullptr);
                REQUIRE(component_managers::entity_positional_manager_.get_component(item)->get_position().x == 1024.0f);
                REQUIRE(component_managers::entity_positional_manager_.get_component(item)->get_position().y == 1024.0f);
                REQUIRE(input.get_control_state() == game_config::default_controls);
                REQUIRE(component_managers::hud_manager_.get_component(head)->is_active());
                REQUIRE(storage->size() == 0);
            }
        }
        WHEN("escape is pressed"){
            input.simulate_input(game_config::input{game_config::key_press_actions::back, game_config::key_press}, cursor);
            events::global_dispatcher_.process_events(0.0f);

            THEN("the item leaves the map, goes back into the inventory, and the inventory is back"){
                REQUIRE_FALSE(carrier_of(cursor)->is_carrying());
                REQUIRE(component_managers::entity_positional_manager_.get_component(item) == nullptr);
                REQUIRE(input.get_control_state() == game_config::default_controls);
                REQUIRE(component_managers::hud_manager_.get_component(head)->is_active());
                REQUIRE(storage->size() == 1);
            }
        }
        WHEN("a binding outside the placing state is pressed"){
            input.simulate_input(game_config::input{game_config::key_press_actions::inventory_open, game_config::key_press}, cursor);
            events::global_dispatcher_.process_events(0.0f);

            THEN("nothing happens and the item is still carried"){
                REQUIRE(carrier_of(cursor)->is_carrying());
                REQUIRE(input.get_control_state() == game_config::placing_from_inventory);
                REQUIRE_FALSE(component_managers::hud_manager_.get_component(head)->is_active());
            }
        }
    }
}

SCENARIO("an inventory item command takes exactly the clicked button off the active page", "[inventory][creation][take]"){
    GIVEN("an inventory page holding two gargoyles"){
        testing::ecs_test_game game;
        hud_systems::hud::get_instance();
        hud_systems::hud_spatial_system::get_instance();
        systems::control_input_system::get_instance();
        systems::entity_lifespan_system::get_instance();
        systems::rendering_system::get_instance().move_camera(Vector2{300.0f, 2000.0f});
        auto& navigation = systems::menu_navigation_system::get_instance();
        auto& inventory = systems::inventory_system::get_instance();
        inventory.init();
        game.create_cursor();
        const auto gargoyle = entity_config::decorations::gargoyle;
        for(int stored = 0; stored < 2; ++stored){
            inventory.on_stored_decoration(events::store_decoration{entity_config::decoration_category, gargoyle});
        }
        auto head = navigation.pages_of(menus::menu_ids::inventory, entity_config::decoration_category)[0];
        auto* storage = component_managers::hud_inventory_storage_manager_.get_component(head);
        auto first = item_button_on(head, 0);
        auto second = item_button_on(head, 1);
        REQUIRE(storage->size() == 2);

        WHEN("the second gargoyle's button is the one clicked"){
            emit(entity_config::decoration_category, gargoyle, second);

            THEN("that button and its placement are gone and the first remains"){
                REQUIRE(storage->size() == 1);
                REQUIRE(page_has(head, first));
                REQUIRE_FALSE(page_has(head, second));
            }
        }
        WHEN("a button that is not on the page is named"){
            emit(entity_config::decoration_category, gargoyle, 100000);

            THEN("the page is untouched"){
                REQUIRE(storage->size() == 2);
                REQUIRE(page_has(head, first));
                REQUIRE(page_has(head, second));
            }
        }
        WHEN("a navigation button on the page is named"){
            auto category_button = component_managers::hud_manager_.get_component(head)->get_elements()[0];
            emit(entity_config::decoration_category, gargoyle, category_button);

            THEN("no item is taken"){
                REQUIRE(storage->size() == 2);
                REQUIRE(page_has(head, category_button));
            }
        }
    }
}

SCENARIO("the camera keys still pan the view while placing from the inventory", "[inventory][creation][controls]"){
    GIVEN("the placing state and the camera at rest"){
        testing::ecs_test_game game;
        auto& input = systems::control_input_system::get_instance();
        auto& rendering = systems::rendering_system::get_instance();
        input.set_control_state(game_config::placing_from_inventory);
        auto before = rendering.get_camera().target;

        WHEN("a camera key is held"){
            input.simulate_input(game_config::input{game_config::key_hold_actions::move_down, game_config::key_hold}, 0, 1.0f);
            input.simulate_input(game_config::input{game_config::key_hold_actions::move_right, game_config::key_hold}, 0, 1.0f);

            THEN("the camera moves"){
                auto after = rendering.get_camera().target;
                REQUIRE((after.x != before.x or after.y != before.y));
            }
        }
    }
}

SCENARIO("dropping a carried item with the default controls snaps it to the grid", "[controls][snap]"){
    GIVEN("a cursor carrying a decoration at an off grid position"){
        testing::ecs_test_game game;
        auto& input = systems::control_input_system::get_instance();
        auto cursor = game.create_cursor();
        auto decoration = systems::entity_lifespan_system::get_instance().create_decoration(
            entity_config::decorations::gargoyle, Vector2{1030.0f, 1045.0f});
        systems::movement_system::get_instance().update_position(cursor, Vector2{1030.0f, 1045.0f});
        carrier_of(cursor)->set_carried_entity(decoration);

        WHEN("left click is pressed"){
            input.simulate_input(game_config::input{MOUSE_BUTTON_LEFT, game_config::mouse_press}, cursor);

            THEN("the item is dropped on the nearest grid position"){
                REQUIRE_FALSE(carrier_of(cursor)->is_carrying());
                auto position = component_managers::entity_positional_manager_.get_component(decoration)->get_position();
                REQUIRE(position.x == 1024.0f);
                REQUIRE(position.y == 1024.0f);
            }
        }
    }
}
