#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <optional>
#include <vector>

#include "ecs_test_game.h"
#include "component.h"
#include "component_managers.hpp"
#include "config.h"
#include "events.h"
#include "hud_systems/hud_systems.hpp"
#include "items.h"
#include "menus.h"
#include "system.h"

namespace {
    std::optional<size_t> category_button_of(size_t page, size_t category){
        auto* hud = component_managers::hud_manager_.get_component(page);
        if(hud == nullptr){ return std::nullopt; }
        for(auto element : hud->get_elements()){
            auto* clickable = component_managers::hud_clickable_manager_.get_component(element);
            if(clickable == nullptr or clickable->get_action() != hud_config::click_actions::change_category){ continue; }
            auto* choice = std::get_if<components::category_choice>(&clickable->get_payload());
            if(choice != nullptr and choice->get_category() == category){ return element; }
        }
        return std::nullopt;
    }

    Vector2 centre_of(size_t element){
        auto box = component_managers::hud_collision_manager_.get_component(element)
            ->get_hitbox_component().get_hitbox().get_box();
        return Vector2{box.x + box.width * 0.5f, box.y + box.height * 0.5f};
    }

    bool page_active(size_t page){
        auto* hud = component_managers::hud_manager_.get_component(page);
        return hud != nullptr and hud->is_active();
    }

    const size_t decoration_group = entity_config::decoration_category;
    const size_t station_group = entity_config::station_category;
    const size_t inventory_menu = menus::menu_ids::inventory;
}

SCENARIO("a menu node keeps its pages in separate groups", "[menu][category]"){
    GIVEN("a menu graph with pages in two groups of the inventory node"){
        menus::menu_graph graph;
        graph.add_root(inventory_menu, 10, 0);
        graph.add_root(inventory_menu, 11, 0);
        graph.add_root(inventory_menu, 20, 1);
        auto& node = graph.get_node(inventory_menu);

        THEN("each group lists only its own pages"){
            REQUIRE(node.get_roots(0) == std::vector<size_t>{10, 11});
            REQUIRE(node.get_roots(1) == std::vector<size_t>{20});
            REQUIRE(node.get_roots(2).empty());
            REQUIRE(node.get_all_roots() == std::vector<size_t>{10, 11, 20});
        }
        THEN("the node starts on the first page of the first group"){
            REQUIRE(node.get_group() == 0);
            REQUIRE(node.get_active_root() == std::optional<size_t>{10});
        }
        WHEN("the page is turned inside a group"){
            node.next_root();

            THEN("only that group's page moved"){
                REQUIRE(node.get_active_root() == std::optional<size_t>{11});
                node.set_group(1);
                REQUIRE(node.get_active_root() == std::optional<size_t>{20});
                node.next_root();
                REQUIRE(node.get_active_root() == std::optional<size_t>{20});
                node.set_group(0);
                REQUIRE(node.get_active_root() == std::optional<size_t>{11});
            }
        }
        WHEN("the node is switched to a group with no pages"){
            node.set_group(3);

            THEN("it has no active root and turning pages does nothing"){
                REQUIRE_FALSE(node.get_active_root().has_value());
                node.next_root();
                node.previous_root();
                REQUIRE_FALSE(node.get_active_root().has_value());
            }
        }
        WHEN("the graph is reset"){
            graph.reset();

            THEN("every group is empty and the node is back on group zero"){
                REQUIRE(node.get_all_roots().empty());
                REQUIRE(node.get_group() == 0);
                REQUIRE_FALSE(node.get_active_root().has_value());
            }
        }
    }
}

SCENARIO("every decoration definition carries the decoration category", "[items][category]"){
    GIVEN("the item encyclopedia"){
        auto& encyclopedia = items::item_encyclopedia::get_instance();

        THEN("each decoration is filed under decorations"){
            for(size_t decoration = 0; decoration < entity_config::decorations_size; ++decoration){
                REQUIRE(encyclopedia.definition_of(decoration_group, decoration).get_category() == decoration_group);
            }
        }
    }
}

SCENARIO("the inventory builds a page set for every category", "[hud][inventory][category]"){
    GIVEN("a world with the hud systems standing and the camera panned"){
        testing::ecs_test_game game;
        hud_systems::hud::get_instance();
        hud_systems::hud_spatial_system::get_instance();
        auto& rendering = systems::rendering_system::get_instance();
        auto& navigation = systems::menu_navigation_system::get_instance();
        rendering.move_camera(Vector2{300.0f, 2000.0f});

        WHEN("the inventory is initialised"){
            systems::inventory_system::get_instance().init();

            THEN("each category has exactly one head page"){
                for(size_t category = 0; category < entity_config::item_categories_size; ++category){
                    REQUIRE(navigation.pages_of(inventory_menu, category).size() == 1);
                }
                REQUIRE(navigation.all_pages_of(inventory_menu).size() == entity_config::item_categories_size);
            }
            THEN("every head page carries one button per category, in order"){
                for(size_t group = 0; group < entity_config::item_categories_size; ++group){
                    auto head = navigation.pages_of(inventory_menu, group)[0];
                    for(size_t category = 0; category < entity_config::item_categories_size; ++category){
                        REQUIRE(category_button_of(head, category).has_value());
                    }
                }
            }
            THEN("the category buttons sit in the side strip left of the page"){
                auto head = navigation.pages_of(inventory_menu, decoration_group)[0];
                auto origin = component_managers::entity_positional_manager_.get_component(head)->get_position();
                for(size_t category = 0; category < entity_config::item_categories_size; ++category){
                    auto button = *category_button_of(head, category);
                    auto box = component_managers::hud_collision_manager_.get_component(button)
                        ->get_hitbox_component().get_hitbox().get_box();
                    REQUIRE(box.x + box.width <= origin.x);
                    REQUIRE(box.y >= origin.y);
                }
            }
            THEN("a camera pan re-pins the pages of every category"){
                rendering.move_camera(Vector2{120.0f, 80.0f});
                auto anchor = GetScreenToWorld2D(hud_config::inventory_page_anchor, rendering.get_camera());
                for(auto page : navigation.all_pages_of(inventory_menu)){
                    auto position = component_managers::entity_positional_manager_.get_component(page)->get_position();
                    REQUIRE(position.x == Catch::Approx(anchor.x));
                    REQUIRE(position.y == Catch::Approx(anchor.y));
                }
            }
        }
    }
}

SCENARIO("a stored decoration lands on the decoration pages only", "[hud][inventory][category]"){
    GIVEN("an initialised inventory"){
        testing::ecs_test_game game;
        hud_systems::hud::get_instance();
        hud_systems::hud_spatial_system::get_instance();
        systems::rendering_system::get_instance().move_camera(Vector2{300.0f, 2000.0f});
        auto& inventory = systems::inventory_system::get_instance();
        auto& navigation = systems::menu_navigation_system::get_instance();
        inventory.init();

        WHEN("a decoration is stored"){
            inventory.on_stored_item(events::store_item{entity_config::decoration_category, entity_config::decorations::gargoyle});

            THEN("the decoration head holds it and the other categories stay empty"){
                for(size_t category = 0; category < entity_config::item_categories_size; ++category){
                    auto head = navigation.pages_of(inventory_menu, category)[0];
                    auto* storage = component_managers::hud_inventory_storage_manager_.get_component(head);
                    REQUIRE(storage != nullptr);
                    REQUIRE(storage->size() == (category == decoration_group ? 1u : 0u));
                }
            }
        }
    }
}

SCENARIO("clicking a category button switches the visible inventory pages", "[hud][inventory][category]"){
    GIVEN("an open inventory showing the decoration pages"){
        testing::ecs_test_game game;
        hud_systems::hud::get_instance();
        hud_systems::hud_spatial_system::get_instance();
        systems::rendering_system::get_instance().move_camera(Vector2{300.0f, 2000.0f});
        auto& navigation = systems::menu_navigation_system::get_instance();
        systems::inventory_system::get_instance().init();
        navigation.on_key_press(events::key_press{game_config::key_press_actions::inventory_open});
        auto decoration_head = navigation.pages_of(inventory_menu, decoration_group)[0];
        auto station_head = navigation.pages_of(inventory_menu, station_group)[0];
        REQUIRE(page_active(decoration_head));
        REQUIRE_FALSE(page_active(station_head));

        WHEN("the station category button is clicked"){
            auto button = category_button_of(decoration_head, station_group);
            REQUIRE(button.has_value());
            REQUIRE(hud_systems::click_system::get_instance().click(centre_of(*button)));

            THEN("the station pages replace the decoration pages"){
                REQUIRE(navigation.group_of(inventory_menu) == station_group);
                REQUIRE_FALSE(page_active(decoration_head));
                REQUIRE(page_active(station_head));
            }
            AND_WHEN("the decoration category button on the station page is clicked"){
                auto back = category_button_of(station_head, decoration_group);
                REQUIRE(back.has_value());
                REQUIRE(hud_systems::click_system::get_instance().click(centre_of(*back)));

                THEN("the decoration pages are back"){
                    REQUIRE(navigation.group_of(inventory_menu) == decoration_group);
                    REQUIRE(page_active(decoration_head));
                    REQUIRE_FALSE(page_active(station_head));
                }
            }
        }
    }
}

SCENARIO("switching category while the inventory is closed does not open it", "[hud][inventory][category]"){
    GIVEN("a closed inventory"){
        testing::ecs_test_game game;
        hud_systems::hud::get_instance();
        hud_systems::hud_spatial_system::get_instance();
        systems::rendering_system::get_instance().move_camera(Vector2{300.0f, 2000.0f});
        auto& navigation = systems::menu_navigation_system::get_instance();
        systems::inventory_system::get_instance().init();
        auto decoration_head = navigation.pages_of(inventory_menu, decoration_group)[0];
        auto station_head = navigation.pages_of(inventory_menu, station_group)[0];

        WHEN("the group is changed directly"){
            navigation.set_group(inventory_menu, station_group);

            THEN("no page becomes visible, and opening shows the selected category"){
                REQUIRE_FALSE(page_active(decoration_head));
                REQUIRE_FALSE(page_active(station_head));
                navigation.on_key_press(events::key_press{game_config::key_press_actions::inventory_open});
                REQUIRE(page_active(station_head));
                REQUIRE_FALSE(page_active(decoration_head));
            }
        }
    }
}

SCENARIO("an item reference round trips through its packed form", "[items][category]"){
    GIVEN("references from different categories sharing the same id"){
        items::item_ref decoration{entity_config::decoration_category, 3};
        items::item_ref station{entity_config::station_category, 3};

        THEN("they are different items"){
            REQUIRE_FALSE(decoration == station);
            REQUIRE(decoration.pack() != station.pack());
        }
        THEN("unpacking restores the category and id"){
            auto restored = items::item_ref::unpack(station.pack());
            REQUIRE(restored == station);
            REQUIRE(restored.get_category() == entity_config::station_category);
            REQUIRE(restored.get_id() == 3);
        }
    }
}

SCENARIO("each category has its own catalogue of definitions", "[items][category]"){
    GIVEN("the item encyclopedia"){
        auto& encyclopedia = items::item_encyclopedia::get_instance();

        THEN("the same id means different items in different categories"){
            auto decoration = encyclopedia.definition_of(items::item_ref{entity_config::decoration_category, 0});
            auto station = encyclopedia.definition_of(items::item_ref{entity_config::station_category, 0});
            REQUIRE(decoration.get_name() != station.get_name());
        }
        THEN("every station has a definition filed under the station category"){
            for(size_t station = 0; station < entity_config::stations_size; ++station){
                items::item_ref ref{entity_config::station_category, station};
                REQUIRE(encyclopedia.has_definition(ref));
                REQUIRE(encyclopedia.definition_of(ref).get_category() == station_group);
            }
        }
        THEN("ids past the end of a catalogue have no definition"){
            REQUIRE_FALSE(encyclopedia.has_definition(items::item_ref{entity_config::station_category, entity_config::stations_size}));
            REQUIRE_FALSE(encyclopedia.has_definition(items::item_ref{entity_config::decoration_category, entity_config::decorations_size}));
            REQUIRE_FALSE(encyclopedia.has_definition(items::item_ref{entity_config::item_categories_size, 0}));
        }
    }
}

SCENARIO("a stored station lands on the station pages and leaves the map", "[hud][inventory][category][station]"){
    GIVEN("an initialised inventory and a station of every kind in turn"){
        testing::ecs_test_game game;
        hud_systems::hud::get_instance();
        hud_systems::hud_spatial_system::get_instance();
        systems::rendering_system::get_instance().move_camera(Vector2{300.0f, 2000.0f});
        auto& inventory = systems::inventory_system::get_instance();
        auto& navigation = systems::menu_navigation_system::get_instance();
        auto& lifespan = systems::entity_lifespan_system::get_instance();
        inventory.init();
        auto station = static_cast<size_t>(GENERATE(range(0, static_cast<int>(entity_config::stations_size))));
        auto spot = Vector2{1600.0f, 1024.0f};
        size_t entity = 0;
        if(entity_config::is_table_station(station)){ entity = lifespan.create_table(station, spot); }
        else if(entity_config::is_counter_station(station)){ entity = lifespan.create_counter(station, spot); }
        else{ entity = lifespan.create_cooking_station(station, spot); }

        THEN("the factory tagged it with its station reference"){
            auto* item = component_managers::entity_item_manager_.get_component(entity);
            REQUIRE(item != nullptr);
            REQUIRE(item->get_ref() == items::item_ref{entity_config::station_category, station});
        }
        WHEN("its store button is clicked"){
            auto* entity_hud = component_managers::hud_manager_.get_component(entity);
            entity_hud->set_active(true);
            std::optional<size_t> store_button;
            for(auto element : entity_hud->get_elements()){
                auto* clickable = component_managers::hud_clickable_manager_.get_component(element);
                if(clickable != nullptr and clickable->get_action() == hud_config::click_actions::store_item){ store_button = element; }
            }
            REQUIRE(store_button.has_value());
            auto box = component_managers::hud_collision_manager_.get_component(*store_button)
                ->get_hitbox_component().get_hitbox().get_box();
            REQUIRE(hud_systems::click_system::get_instance().click(Vector2{box.x + box.width * 0.5f, box.y + box.height * 0.5f}));
            events::global_dispatcher_.process_events(0.0f);

            THEN("the station head holds it as a selectable item and the decoration head stays empty"){
                auto station_head = navigation.pages_of(inventory_menu, station_group)[0];
                auto decoration_head = navigation.pages_of(inventory_menu, decoration_group)[0];
                REQUIRE(component_managers::hud_inventory_storage_manager_.get_component(station_head)->size() == 1);
                REQUIRE(component_managers::hud_inventory_storage_manager_.get_component(decoration_head)->size() == 0);

                bool found = false;
                for(auto element : component_managers::hud_manager_.get_component(station_head)->get_elements()){
                    auto* clickable = component_managers::hud_clickable_manager_.get_component(element);
                    if(clickable == nullptr or clickable->get_action() != hud_config::click_actions::select_item){ continue; }
                    auto* choice = std::get_if<components::item_choice>(&clickable->get_payload());
                    if(choice != nullptr and choice->get_ref() == items::item_ref{entity_config::station_category, station}){ found = true; }
                }
                REQUIRE(found);
            }
            THEN("the station is gone from the map"){
                REQUIRE(component_managers::entity_positional_manager_.get_component(entity) == nullptr);
                REQUIRE(component_managers::entity_item_manager_.get_component(entity) == nullptr);
            }
        }
    }
}

SCENARIO("a stored item with no definition is ignored", "[hud][inventory][category]"){
    GIVEN("an initialised inventory and a decoration"){
        testing::ecs_test_game game;
        hud_systems::hud::get_instance();
        hud_systems::hud_spatial_system::get_instance();
        systems::rendering_system::get_instance().move_camera(Vector2{300.0f, 2000.0f});
        auto& inventory = systems::inventory_system::get_instance();
        inventory.init();
        auto& navigation = systems::menu_navigation_system::get_instance();

        WHEN("a store arrives for an id past the end of the catalogue"){
            inventory.on_stored_item(events::store_item{entity_config::station_category, entity_config::stations_size});

            THEN("nothing is stored on any category"){
                for(size_t category = 0; category < entity_config::item_categories_size; ++category){
                    auto head = navigation.pages_of(inventory_menu, category)[0];
                    REQUIRE(component_managers::hud_inventory_storage_manager_.get_component(head)->size() == 0);
                }
            }
        }
    }
}
