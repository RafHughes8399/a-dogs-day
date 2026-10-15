#include <catch2/catch_test_macros.hpp>

#include <vector>

#include "ecs_test_game.h"
#include "component_helpers.hpp"
#include "component_managers.hpp"
#include "config.h"
#include "hud_element_builders.hpp"
#include "hud_systems/hud_systems.hpp"
#include "sprite.h"

namespace {

    const float base_width = 2 * hud_config::nine_sprite_corner_width + hud_config::nine_sprite_centre_width;
    const float base_height = 2 * hud_config::nine_sprite_corner_height + hud_config::nine_sprite_centre_height;

    const std::vector<float> factors = {1.5f, 2.0f, 2.25f};

    Rectangle scaled_frame(float width_factor, float height_factor){
        return Rectangle{0.0f, 0.0f, base_width * width_factor, base_height * height_factor};
    }

    float centre_width(const sprite::nine_sprite& nine){
        return hud_config::nine_sprite_centre_width * nine.get_scale().x;
    }

    float centre_height(const sprite::nine_sprite& nine){
        return hud_config::nine_sprite_centre_height * nine.get_scale().y;
    }

    bool same_colour(Color left, Color right){
        return left.r == right.r and left.g == right.g and left.b == right.b and left.a == right.a;
    }
}

SCENARIO("a nine sprite at its base dimensions is unscaled", "[hud]"){
    GIVEN("a nine sprite whose frame is two corners plus one centre"){
        testing::ecs_test_game game;
        auto nine = sprite::nine_sprite(scaled_frame(1.0f, 1.0f), WHITE);

        THEN("the frame is kept as given"){
            REQUIRE(nine.get_frame().width == base_width);
            REQUIRE(nine.get_frame().height == base_height);
        }
        THEN("the centre is drawn at its own size"){
            REQUIRE(nine.get_scale().x == 1.0f);
            REQUIRE(nine.get_scale().y == 1.0f);
            REQUIRE(centre_width(nine) == hud_config::nine_sprite_centre_width);
            REQUIRE(centre_height(nine) == hud_config::nine_sprite_centre_height);
        }
    }
}

SCENARIO("a nine sprite stretches its centre to fill a larger frame", "[hud]"){
    GIVEN("frames scaled uniformly from the base dimensions"){
        testing::ecs_test_game game;

        THEN("the frame is the base dimensions times the factor"){
            for(auto factor : factors){
                auto nine = sprite::nine_sprite(scaled_frame(factor, factor), WHITE);
                CAPTURE(factor);
                REQUIRE(nine.get_frame().width == base_width * factor);
                REQUIRE(nine.get_frame().height == base_height * factor);
            }
        }
        THEN("the corners keep their size and the centre takes up the rest"){
            for(auto factor : factors){
                auto nine = sprite::nine_sprite(scaled_frame(factor, factor), WHITE);
                CAPTURE(factor);
                REQUIRE(2 * hud_config::nine_sprite_corner_width + centre_width(nine) == base_width * factor);
                REQUIRE(2 * hud_config::nine_sprite_corner_height + centre_height(nine) == base_height * factor);
            }
        }
        THEN("the centre grows by more than the factor, since the corners do not grow"){
            for(auto factor : factors){
                auto nine = sprite::nine_sprite(scaled_frame(factor, factor), WHITE);
                CAPTURE(factor);
                REQUIRE(nine.get_scale().x > factor);
                REQUIRE(nine.get_scale().y > factor);
            }
        }
        THEN("a square frame scales both axes alike"){
            for(auto factor : factors){
                auto nine = sprite::nine_sprite(scaled_frame(factor, factor), WHITE);
                CAPTURE(factor);
                REQUIRE(nine.get_scale().x == nine.get_scale().y);
            }
        }
    }

    GIVEN("a frame scaled differently on each axis"){
        testing::ecs_test_game game;
        auto nine = sprite::nine_sprite(scaled_frame(1.5f, 2.25f), WHITE);

        THEN("each axis is scaled from its own dimension"){
            REQUIRE(2 * hud_config::nine_sprite_corner_width + centre_width(nine) == base_width * 1.5f);
            REQUIRE(2 * hud_config::nine_sprite_corner_height + centre_height(nine) == base_height * 2.25f);
            REQUIRE(nine.get_scale().x < nine.get_scale().y);
        }
    }
}

SCENARIO("a nine sprite keeps the tint it was built with", "[hud]"){
    GIVEN("nine sprites built with different tints"){
        testing::ecs_test_game game;
        const Color translucent = Color{12, 34, 56, 78};

        auto white = sprite::nine_sprite(scaled_frame(1.0f, 1.0f), WHITE);
        auto red = sprite::nine_sprite(scaled_frame(1.0f, 1.0f), RED);
        auto faded = sprite::nine_sprite(scaled_frame(1.0f, 1.0f), translucent);

        THEN("each holds its own tint, alpha included"){
            REQUIRE(same_colour(white.get_tint(), WHITE));
            REQUIRE(same_colour(red.get_tint(), RED));
            REQUIRE(same_colour(faded.get_tint(), translucent));
        }
        THEN("one tint does not leak into another"){
            REQUIRE_FALSE(same_colour(white.get_tint(), red.get_tint()));
            REQUIRE_FALSE(same_colour(red.get_tint(), faded.get_tint()));
        }
    }

    GIVEN("a tinted nine sprite in a scaled frame"){
        testing::ecs_test_game game;

        THEN("scaling leaves the tint alone"){
            for(auto factor : factors){
                auto nine = sprite::nine_sprite(scaled_frame(factor, factor), RED);
                CAPTURE(factor);
                REQUIRE(same_colour(nine.get_tint(), RED));
            }
        }
    }
}

namespace {
    void register_hud_box(size_t hud_id, Vector2 position){
        std::vector<hitbox::hitbox> hitboxes{
            hitbox::hitbox(Rectangle{position.x, position.y, 16.0f, 16.0f})};
        component_helpers::register_hud_collision_component(hud_id,
            component_builders::build_hud_collision_component(
                component_builders::build_hud_hitbox_component(hitboxes, 0)));
    }
}

SCENARIO("the hud spatial system indexes hud boxes apart from the world", "[hud][spatial]"){
    GIVEN("a hud element with a hud collision box"){
        testing::ecs_test_game game;
        auto& hud_spatial = hud_systems::hud_spatial_system::get_instance();
        const size_t hud_id = 40;
        const Vector2 position{level_config::graph_x + 32.0f, level_config::graph_y + 32.0f};
        register_hud_box(hud_id, position);

        WHEN("it is tracked"){
            hud_spatial.track(hud_id);

            THEN("the hud tree holds it"){
                REQUIRE(hud_spatial.is_tracked(hud_id));
                REQUIRE(hud_spatial.tracked_count() == 1);
            }
            THEN("a position inside its box still misses, because it has no active owner"){
                REQUIRE(hud_spatial.check_collision_with(Vector2Add(position, Vector2{8.0f, 8.0f}))
                    == game_config::empty_entity);
            }
            THEN("a position outside its box hits nothing"){
                REQUIRE(hud_spatial.check_collision_with(Vector2Subtract(position, Vector2{8.0f, 8.0f}))
                    == game_config::empty_entity);
            }
            THEN("the world spatial index does not see it"){
                REQUIRE_FALSE(game.is_tracked(hud_id));
            }
        }

        WHEN("it is tracked then untracked"){
            hud_spatial.track(hud_id);
            hud_spatial.untrack(hud_id);

            THEN("the hud tree is empty and the box no longer hits"){
                REQUIRE_FALSE(hud_spatial.is_tracked(hud_id));
                REQUIRE(hud_spatial.check_collision_with(Vector2Add(position, Vector2{8.0f, 8.0f}))
                    == game_config::empty_entity);
            }
        }
    }

    GIVEN("an id with only an entity collision box"){
        testing::ecs_test_game game;
        auto& hud_spatial = hud_systems::hud_spatial_system::get_instance();
        const size_t entity_id = 41;
        const Vector2 position{level_config::graph_x + 32.0f, level_config::graph_y + 32.0f};
        std::vector<hitbox::hitbox> hitboxes{
            hitbox::hitbox(Rectangle{position.x, position.y, 16.0f, 16.0f})};
        component_helpers::register_entity_collision_component(entity_id,
            component_builders::build_entity_collision_component(
                component_builders::build_entity_hitbox_component(hitboxes, 0)));

        WHEN("tracking it is attempted"){
            hud_spatial.track(entity_id);

            THEN("the hud tree ignores it"){
                REQUIRE_FALSE(hud_spatial.is_tracked(entity_id));
                REQUIRE(hud_spatial.tracked_count() == 0);
            }
        }
    }
}

namespace {
    size_t create_hud_owner(){
        return systems::entity_lifespan_system::get_instance().create([](size_t id){
            auto base = sprite::nine_sprite(scaled_frame(1.0f, 1.0f), WHITE);
            component_helpers::add_hud_component(id, base);
        }, level_config::draw_layers::stations);
    }

    size_t create_boxed_element(size_t owner, Vector2 position){
        return systems::entity_lifespan_system::get_instance().create_hud_element(owner,
            [position](size_t id){ register_hud_box(id, position); });
    }

    const std::vector<size_t>& elements_of(size_t owner){
        return component_managers::hud_manager_.get_component(owner)->get_elements();
    }
}

SCENARIO("a hud element is created against an owner with a hud", "[hud][lifespan]"){
    GIVEN("an owner entity with a hud component"){
        testing::ecs_test_game game;
        auto& lifespan = systems::entity_lifespan_system::get_instance();
        auto& hud_spatial = hud_systems::hud_spatial_system::get_instance();
        auto owner = create_hud_owner();
        const Vector2 position{level_config::graph_x + 32.0f, level_config::graph_y + 32.0f};

        WHEN("an element with a hud box is created for it"){
            auto element = create_boxed_element(owner, position);

            THEN("the element gets its own id"){
                REQUIRE(element != owner);
                REQUIRE(element != static_cast<size_t>(game_config::empty_entity));
            }
            THEN("the owner lists it"){
                REQUIRE(elements_of(owner) == std::vector<size_t>{element});
            }
            THEN("the element points back at the owner"){
                auto* owner_component = component_managers::hud_owner_manager_.get_component(element);
                REQUIRE(owner_component != nullptr);
                REQUIRE(owner_component->get_owner() == owner);
            }
            THEN("the hud tree holds it and the world tree does not"){
                REQUIRE(hud_spatial.is_tracked(element));
                REQUIRE_FALSE(game.is_tracked(element));
            }
        }

        WHEN("an element is created then destroyed"){
            auto element = create_boxed_element(owner, position);
            lifespan.destroy_hud_element(element);

            THEN("the owner no longer lists it"){
                REQUIRE(elements_of(owner).empty());
            }
            THEN("the hud tree drops it"){
                REQUIRE_FALSE(hud_spatial.is_tracked(element));
                REQUIRE(hud_spatial.tracked_count() == 0);
            }
            THEN("none of its hud components remain"){
                REQUIRE(component_helpers::num_registered_hud_components(element) == 0);
            }
            THEN("the owner is untouched"){
                REQUIRE(component_managers::hud_manager_.get_component(owner) != nullptr);
            }
        }
    }

    GIVEN("an owner entity with no hud component"){
        testing::ecs_test_game game;
        auto& lifespan = systems::entity_lifespan_system::get_instance();
        auto owner = game.create_empty(level_config::draw_layers::stations);

        WHEN("an element is created for it"){
            bool built = false;
            auto element = lifespan.create_hud_element(owner, [&built](size_t){ built = true; });

            THEN("nothing is built and the empty sentinel comes back"){
                REQUIRE_FALSE(built);
                REQUIRE(element == static_cast<size_t>(game_config::empty_entity));
                REQUIRE(component_managers::hud_owner_manager_.size() == 0);
            }
        }
    }
}

SCENARIO("destroying an owner destroys its hud elements", "[hud][lifespan]"){
    GIVEN("an owner with two boxed hud elements"){
        testing::ecs_test_game game;
        auto& lifespan = systems::entity_lifespan_system::get_instance();
        auto& hud_spatial = hud_systems::hud_spatial_system::get_instance();
        auto owner = create_hud_owner();
        auto first = create_boxed_element(owner,
            Vector2{level_config::graph_x + 32.0f, level_config::graph_y + 32.0f});
        auto second = create_boxed_element(owner,
            Vector2{level_config::graph_x + 96.0f, level_config::graph_y + 32.0f});
        REQUIRE(hud_spatial.tracked_count() == 2);

        WHEN("the owner is destroyed"){
            lifespan.destroy(owner);

            THEN("both elements leave the hud tree"){
                REQUIRE(hud_spatial.tracked_count() == 0);
            }
            THEN("neither element keeps any hud component"){
                REQUIRE(component_helpers::num_registered_hud_components(first) == 0);
                REQUIRE(component_helpers::num_registered_hud_components(second) == 0);
            }
            THEN("the owner's hud component is gone"){
                REQUIRE(component_managers::hud_manager_.get_component(owner) == nullptr);
            }
            THEN("the world is left empty"){
                REQUIRE(game.total_components() == 0);
            }
        }
    }
}

SCENARIO("a cooking station is built with a recipe button per recipe", "[hud][button]"){
    GIVEN("a stove, which has four recipes"){
        testing::ecs_test_game game;
        auto& lifespan = systems::entity_lifespan_system::get_instance();
        auto& hud_spatial = hud_systems::hud_spatial_system::get_instance();
        const Vector2 position{level_config::graph_x + 64.0f, level_config::graph_y + 64.0f};
        auto stove = lifespan.create_stove(position);
        const auto& buttons = elements_of(stove);

        THEN("it owns four buttons, all in the hud tree"){
            REQUIRE(buttons.size() == 4);
            REQUIRE(hud_spatial.tracked_count() == 4);
        }
        THEN("each button selects the recipe at its own index"){
            for(size_t index = 0; index < buttons.size(); ++index){
                auto* clickable = component_managers::hud_clickable_manager_.get_component(buttons[index]);
                CAPTURE(index);
                REQUIRE(clickable != nullptr);
                REQUIRE(clickable->get_action() == hud_config::click_actions::select_recipe);
                REQUIRE(clickable->get_payload() == static_cast<int>(index));
            }
        }
        THEN("each button has a position, a renderable and a hud box"){
            for(auto button : buttons){
                REQUIRE(component_managers::hud_positional_manager_.get_component(button) != nullptr);
                REQUIRE(component_managers::hud_renderable_manager_.get_component(button) != nullptr);
                REQUIRE(component_managers::hud_collision_manager_.get_component(button) != nullptr);
            }
        }
        THEN("the buttons fill a two by two grid from the hud's top left"){
            auto origin = Vector2Add(position, hud_config::cook_station_hud_offset);
            auto step_x = hud_config::recipe_icon_width + hud_config::reicpe_icon_width_buffer;
            auto step_y = hud_config::recipe_icon_height + hud_config::recipe_icon_height_buffer;
            auto first = Vector2Add(origin, Vector2{hud_config::reicpe_icon_width_buffer * 0.5f,
                hud_config::recipe_icon_height_buffer * 0.5f});
            std::vector<Vector2> expected{
                first,
                Vector2Add(first, Vector2{step_x, 0.0f}),
                Vector2Add(first, Vector2{0.0f, step_y}),
                Vector2Add(first, Vector2{step_x, step_y})};
            for(size_t index = 0; index < buttons.size(); ++index){
                auto actual = component_managers::hud_positional_manager_.get_component(buttons[index])->get_position();
                CAPTURE(index);
                REQUIRE(Vector2Equals(actual, expected[index]));
            }
        }
        THEN("a button is only hit once its stove is selected"){
            auto box = component_managers::hud_collision_manager_.get_component(buttons[3])
                ->get_hitbox_component().get_hitbox().get_box();
            auto centre = Vector2{box.x + box.width * 0.5f, box.y + box.height * 0.5f};
            REQUIRE(hud_spatial.check_collision_with(centre) == game_config::empty_entity);
            systems::selection_system::get_instance().select(stove);
            REQUIRE(hud_spatial.check_collision_with(centre) == static_cast<int>(buttons[3]));
        }

        WHEN("the stove is destroyed"){
            lifespan.destroy_cooking_station(stove);

            THEN("its buttons go with it"){
                REQUIRE(hud_spatial.tracked_count() == 0);
                REQUIRE(component_managers::hud_owner_manager_.size() == 0);
                REQUIRE(component_managers::hud_clickable_manager_.size() == 0);
            }
        }
    }

    GIVEN("a chopping board, which has more recipes than one page holds"){
        testing::ecs_test_game game;
        auto& lifespan = systems::entity_lifespan_system::get_instance();
        auto board = lifespan.create_chopping_board(
            Vector2{level_config::graph_x + 64.0f, level_config::graph_y + 64.0f});

        THEN("only the first page of buttons is built"){
            REQUIRE(station_config::chopping_board_recipes.size() > 4);
            REQUIRE(elements_of(board).size() == 4);
        }
    }
}

SCENARIO("selecting an owner activates its hud", "[hud][selection]"){
    GIVEN("two stoves, neither selected"){
        testing::ecs_test_game game;
        auto& lifespan = systems::entity_lifespan_system::get_instance();
        auto& selection = systems::selection_system::get_instance();
        auto first = lifespan.create_stove(Vector2{level_config::graph_x + 64.0f, level_config::graph_y + 64.0f});
        auto second = lifespan.create_stove(Vector2{level_config::graph_x + 256.0f, level_config::graph_y + 64.0f});
        auto is_active = [](size_t owner){
            return component_managers::hud_manager_.get_component(owner)->is_active();
        };

        THEN("both huds start inactive"){
            REQUIRE_FALSE(is_active(first));
            REQUIRE_FALSE(is_active(second));
        }

        WHEN("the first stove is selected"){
            selection.select(first);

            THEN("only its hud is active"){
                REQUIRE(is_active(first));
                REQUIRE_FALSE(is_active(second));
            }
        }

        WHEN("the first stove is selected then deselected"){
            selection.select(first);
            selection.deselect();

            THEN("its hud is inactive again"){
                REQUIRE_FALSE(is_active(first));
            }
        }

        WHEN("selection moves from the first stove to the second"){
            selection.select(first);
            selection.select(second);

            THEN("the hud follows the selection"){
                REQUIRE_FALSE(is_active(first));
                REQUIRE(is_active(second));
            }
        }
    }
}

namespace {
    Vector2 centre_of(size_t hud_id){
        auto box = component_managers::hud_collision_manager_.get_component(hud_id)
            ->get_hitbox_component().get_hitbox().get_box();
        return Vector2{box.x + box.width * 0.5f, box.y + box.height * 0.5f};
    }
}

SCENARIO("clicking a hud button runs its action and keeps the selection", "[hud][click]"){
    GIVEN("a selected stove and the cursor over its third recipe button"){
        testing::ecs_test_game game;
        auto& lifespan = systems::entity_lifespan_system::get_instance();
        auto& selection = systems::selection_system::get_instance();
        auto& controls = systems::control_input_system::get_instance();
        auto stove = lifespan.create_stove(Vector2{level_config::graph_x + 64.0f, level_config::graph_y + 64.0f});
        auto cursor = game.create_cursor();
        selection.select(stove);
        auto button = elements_of(stove)[2];
        game.move_entity(cursor, centre_of(button));
        auto* recipes = component_managers::entity_recipes_manager_.get_component(stove);
        auto third_food = recipes->get_selected_recipes()[2].get_food_id();
        REQUIRE(recipes->get_selected_recipe().get_food_id() != third_food);

        WHEN("the left button is pressed"){
            controls.simulate_input(game_config::input{MOUSE_BUTTON_LEFT, game_config::mouse_press}, cursor);

            THEN("the stove's selected recipe becomes the third one"){
                REQUIRE(recipes->get_selected_recipe().get_food_id() == third_food);
            }
            THEN("the stove stays selected and its hud stays open"){
                REQUIRE(selection.selected() == static_cast<int>(stove));
                REQUIRE(component_managers::hud_manager_.get_component(stove)->is_active());
            }
        }

        WHEN("the right button is pressed"){
            controls.simulate_input(game_config::input{MOUSE_BUTTON_RIGHT, game_config::mouse_press}, cursor);

            THEN("the click is consumed by the hud"){
                REQUIRE(recipes->get_selected_recipe().get_food_id() == third_food);
                REQUIRE(selection.selected() == static_cast<int>(stove));
            }
        }
    }

    GIVEN("a stove whose hud is closed"){
        testing::ecs_test_game game;
        auto& lifespan = systems::entity_lifespan_system::get_instance();
        auto stove = lifespan.create_stove(Vector2{level_config::graph_x + 64.0f, level_config::graph_y + 64.0f});
        auto button = elements_of(stove)[2];
        auto* recipes = component_managers::entity_recipes_manager_.get_component(stove);
        auto first_food = recipes->get_selected_recipe().get_food_id();

        WHEN("a click lands where a button would be"){
            auto consumed = hud_systems::click_system::get_instance().click(centre_of(button));

            THEN("the hud does not consume it and nothing changes"){
                REQUIRE_FALSE(consumed);
                REQUIRE(recipes->get_selected_recipe().get_food_id() == first_food);
            }
        }
    }
}

SCENARIO("a hud follows its owner when the owner moves", "[hud][move]"){
    GIVEN("a selected stove with its recipe buttons"){
        testing::ecs_test_game game;
        auto& lifespan = systems::entity_lifespan_system::get_instance();
        auto& hud_spatial = hud_systems::hud_spatial_system::get_instance();
        const Vector2 start{level_config::graph_x + 64.0f, level_config::graph_y + 64.0f};
        auto stove = lifespan.create_stove(start);
        systems::selection_system::get_instance().select(stove);
        const auto& buttons = elements_of(stove);
        std::vector<Vector2> before;
        for(auto button : buttons){
            before.push_back(component_managers::hud_positional_manager_.get_component(button)->get_position());
        }
        auto old_centre = centre_of(buttons[0]);

        WHEN("the stove is moved"){
            const Vector2 shift{level_config::edge_weight * 4.0f, level_config::edge_weight * 2.0f};
            systems::movement_system::get_instance().update_position(stove, Vector2Add(start, shift));

            THEN("every button moves by the same amount"){
                for(size_t index = 0; index < buttons.size(); ++index){
                    auto after = component_managers::hud_positional_manager_.get_component(buttons[index])->get_position();
                    CAPTURE(index);
                    REQUIRE(Vector2Equals(after, Vector2Add(before[index], shift)));
                }
            }
            THEN("each button's box moves with it"){
                for(auto button : buttons){
                    auto box = component_managers::hud_collision_manager_.get_component(button)
                        ->get_hitbox_component().get_hitbox().get_box();
                    auto position = component_managers::hud_positional_manager_.get_component(button)->get_position();
                    REQUIRE(Vector2Equals(Vector2{box.x, box.y}, position));
                }
            }
            THEN("the hud tree finds the button at its new place and not its old one"){
                REQUIRE(hud_spatial.check_collision_with(centre_of(buttons[0])) == static_cast<int>(buttons[0]));
                REQUIRE(hud_spatial.check_collision_with(old_centre) != static_cast<int>(buttons[0]));
            }
        }
    }
}

SCENARIO("every food has its own recipe button builder", "[hud][button]"){
    GIVEN("a recipe button built for each food"){
        testing::ecs_test_game game;
        const Vector2 position{level_config::graph_x + 32.0f, level_config::graph_y + 32.0f};
        const size_t food_count = static_cast<size_t>(entity_config::foods::steak_leftovers) + 1;

        THEN("each one is a select-recipe button carrying its recipe index"){
            for(size_t food = 0; food < food_count; ++food){
                auto id = 100 + food;
                button_builders::build_recipe_button(food, id, position, static_cast<int>(food));
                auto* clickable = component_managers::hud_clickable_manager_.get_component(id);
                CAPTURE(food);
                REQUIRE(clickable != nullptr);
                REQUIRE(clickable->get_action() == hud_config::click_actions::select_recipe);
                REQUIRE(clickable->get_payload() == static_cast<int>(food));
                REQUIRE(component_managers::hud_renderable_manager_.get_component(id) != nullptr);
                REQUIRE(component_managers::hud_collision_manager_.get_component(id) != nullptr);
            }
        }
    }

    GIVEN("a food id past the end of the foods"){
        testing::ecs_test_game game;
        const size_t unknown = static_cast<size_t>(entity_config::foods::steak_leftovers) + 1;

        WHEN("a recipe button is built for it"){
            button_builders::build_recipe_button(unknown, 200, Vector2Zero(), 0);

            THEN("nothing is built"){
                REQUIRE(component_helpers::num_registered_hud_components(200) == 0);
            }
        }
    }
}
