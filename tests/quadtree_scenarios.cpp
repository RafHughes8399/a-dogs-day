#include <catch2/catch_test_macros.hpp>

#include "component.h"
#include "config.h"
#include "events.h"
#include "events_interface.h"
#include "quadtree.h"
#include "raylib.h"
#include "raymath.h"

namespace{
    constexpr float k_entity_size = 16.0f;

    // A small, easy-to-reason-about world. depth=1 means insert() builds the
    // 4 children on the first insert and places every object directly into a
    // leaf (or the root, if it straddles the centre) - no further
    // subdivision to reason about.
    raglib::bounding_box_2 test_world(){
        return raglib::bounding_box_2{Vector2{0.0f, 0.0f}, Vector2{400.0f, 400.0f}};
    }

    class collision_component_guard{
        public:
            collision_component_guard(){ component_helpers::clear_all_components(); }
            ~collision_component_guard(){ component_helpers::clear_all_components(); }
            collision_component_guard(const collision_component_guard&) = delete;
            collision_component_guard(collision_component_guard&&) = delete;
            collision_component_guard& operator=(const collision_component_guard&) = delete;
            collision_component_guard& operator=(collision_component_guard&&) = delete;
    };

    constexpr float k_interaction_reach = 8.0f;

    // collision only - neither side of the interaction pair
    void insert_collidable(tree::ecs_quadtree& tree, size_t entity_id, Vector2 position){
        std::vector<hitbox::hitbox> hitboxes{
            hitbox::hitbox(Rectangle{position.x, position.y, k_entity_size, k_entity_size})};
        component_helpers::register_collision_component(entity_id,
            component_builders::build_collision_component(
                component_builders::build_hitbox_component(hitboxes, 0)));

        auto* collision = component_managers::collision_manager_.get_component(entity_id);
        tree.insert(entity_id, collision->get_hitbox_component().get_hitbox());
    }

    // collision plus interactable - a station. check_interaction looks for
    // interactors, so one of these is never the answer, only ever the asker.
    void insert_interactable(tree::ecs_quadtree& tree, size_t entity_id, Vector2 position,
        float reach = k_interaction_reach){
        std::vector<hitbox::hitbox> hitboxes{
            hitbox::hitbox(Rectangle{position.x, position.y, k_entity_size, k_entity_size})};
        component_helpers::register_collision_component(entity_id,
            component_builders::build_collision_component(
                component_builders::build_hitbox_component(hitboxes, 0)));
        std::array<std::optional<Vector2>, DIRECTIONS> slot_offsets{};
        component_helpers::register_interactable_component(entity_id,
            component_builders::build_interactable_component(reach, slot_offsets));

        auto* collision = component_managers::collision_manager_.get_component(entity_id);
        tree.insert(entity_id, collision->get_hitbox_component().get_hitbox());
    }

    // collision plus interactor - a dog, and the only thing check_interaction
    // reports. its interaction box is the hitbox grown by reach on every side,
    // so a k_entity_size entity at p answers to boxes overlapping
    // [p - reach, p + k_entity_size + reach]
    void insert_interactor(tree::ecs_quadtree& tree, size_t entity_id, Vector2 position,
        float reach = k_interaction_reach){
        std::vector<hitbox::hitbox> hitboxes{
            hitbox::hitbox(Rectangle{position.x, position.y, k_entity_size, k_entity_size})};
        component_helpers::register_collision_component(entity_id,
            component_builders::build_collision_component(
                component_builders::build_hitbox_component(hitboxes, 0)));
        component_helpers::register_interactor_component(entity_id,
            component_builders::build_interactor_component(reach));

        auto* collision = component_managers::collision_manager_.get_component(entity_id);
        tree.insert(entity_id, collision->get_hitbox_component().get_hitbox());
    }
}

SCENARIO("a point collision check reports the entity sitting under a position",
        "[quadtree][ecs][collision]"){
    GIVEN("two collidable entities in opposite quadrants of a 400x400 world"){
        collision_component_guard components;
        tree::ecs_quadtree tree(test_world(), 1);

        const size_t asker_id = 1;
        const size_t target_id = 2;
        insert_collidable(tree, asker_id, Vector2{50.0f, 50.0f});
        insert_collidable(tree, target_id, Vector2{300.0f, 300.0f});

        REQUIRE(tree.size() == 2);
        REQUIRE(tree.depth_of(asker_id) == 1);
        REQUIRE(tree.depth_of(target_id) == 1);

        WHEN("a position inside the target's hitbox is checked"){
            auto hit = tree.check_collision(asker_id, Vector2{308.0f, 308.0f});

            THEN("the target's id comes back"){
                REQUIRE(hit == static_cast<int>(target_id));
            }
        }

        WHEN("a position in an occupied quadrant but clear of every hitbox is checked"){
            auto hit = tree.check_collision(asker_id, Vector2{10.0f, 10.0f});

            THEN("nothing is reported"){
                REQUIRE(hit == game_config::empty_entity);
            }
        }

        WHEN("a position in an empty quadrant is checked"){
            auto hit = tree.check_collision(asker_id, Vector2{50.0f, 300.0f});

            THEN("nothing is reported"){
                REQUIRE(hit == game_config::empty_entity);
            }
        }
    }
}

SCENARIO("a box collision check reports the entity a rectangle overlaps",
        "[quadtree][ecs][collision]"){
    GIVEN("two collidable entities in opposite quadrants of a 400x400 world"){
        collision_component_guard components;
        tree::ecs_quadtree tree(test_world(), 1);

        const size_t asker_id = 1;
        const size_t target_id = 2;
        insert_collidable(tree, asker_id, Vector2{50.0f, 50.0f});
        insert_collidable(tree, target_id, Vector2{300.0f, 300.0f});

        REQUIRE(tree.size() == 2);

        WHEN("a box partially overlapping the target's hitbox is checked"){
            auto hit = tree.check_collision(asker_id,
                Rectangle{295.0f, 295.0f, k_entity_size, k_entity_size});

            THEN("the target's id comes back"){
                REQUIRE(hit == static_cast<int>(target_id));
            }
        }

        WHEN("a box in an occupied quadrant but clear of every hitbox is checked"){
            auto hit = tree.check_collision(asker_id,
                Rectangle{100.0f, 100.0f, k_entity_size, k_entity_size});

            THEN("nothing is reported"){
                REQUIRE(hit == game_config::empty_entity);
            }
        }

        WHEN("a box in an empty quadrant is checked"){
            auto hit = tree.check_collision(asker_id,
                Rectangle{50.0f, 300.0f, k_entity_size, k_entity_size});

            THEN("nothing is reported"){
                REQUIRE(hit == game_config::empty_entity);
            }
        }
    }
}

SCENARIO("a collision check never reports the entity doing the asking",
        "[quadtree][ecs][collision]"){
    GIVEN("two collidable entities stacked on the same spot"){
        collision_component_guard components;
        tree::ecs_quadtree tree(test_world(), 1);

        const size_t asker_id = 1;
        const size_t target_id = 2;
        insert_collidable(tree, asker_id, Vector2{300.0f, 300.0f});
        insert_collidable(tree, target_id, Vector2{300.0f, 300.0f});

        REQUIRE(tree.size() == 2);

        WHEN("the asker checks a position inside its own hitbox"){
            auto found = tree.check_collision(asker_id, Vector2{308.0f, 308.0f});

            THEN("it gets the other entity, not itself"){
                REQUIRE(found == static_cast<int>(target_id));
            }
        }

        WHEN("the asker checks a box overlapping its own hitbox"){
            auto found = tree.check_collision(asker_id,
                Rectangle{295.0f, 295.0f, k_entity_size, k_entity_size});

            THEN("it gets the other entity, not itself"){
                REQUIRE(found == static_cast<int>(target_id));
            }
        }
    }

    GIVEN("a single collidable entity alone in the tree"){
        collision_component_guard components;
        tree::ecs_quadtree tree(test_world(), 1);

        const size_t asker_id = 1;
        insert_collidable(tree, asker_id, Vector2{300.0f, 300.0f});

        WHEN("it checks a position inside its own hitbox"){
            auto found = tree.check_collision(asker_id, Vector2{308.0f, 308.0f});

            THEN("nothing is reported"){
                REQUIRE(found == game_config::empty_entity);
            }
        }
    }
}

SCENARIO("a box straddling a quadrant split still finds what it overlaps",
        "[quadtree][ecs][collision]"){
    GIVEN("an entity sitting just inside the top-right quadrant"){
        collision_component_guard components;
        tree::ecs_quadtree tree(test_world(), 1);

        const size_t asker_id = 1;
        const size_t target_id = 2;
        insert_collidable(tree, asker_id, Vector2{50.0f, 50.0f});
        insert_collidable(tree, target_id, Vector2{205.0f, 205.0f});

        WHEN("a box crossing the centre split overlaps it"){
            // spans [195,215) on both axes - fits in no single child, so a
            // containment-gated descent would never look inside one
            auto found = tree.check_collision(asker_id,
                Rectangle{195.0f, 195.0f, 20.0f, 20.0f});

            THEN("the target is still found"){
                REQUIRE(found == static_cast<int>(target_id));
            }
        }
    }
}


SCENARIO("an interaction check reports the interactor a rectangle reaches",
        "[quadtree][ecs][interaction]"){
    GIVEN("a station asking, and an interactor in the opposite quadrant of a 400x400 world"){
        collision_component_guard components;
        tree::ecs_quadtree tree(test_world(), 1);

        const size_t asker_id = 1;
        const size_t target_id = 2;
        insert_interactable(tree, asker_id, Vector2{50.0f, 50.0f});
        insert_interactor(tree, target_id, Vector2{300.0f, 300.0f});

        REQUIRE(tree.size() == 2);

        WHEN("a box overlapping the interactor's hitbox is checked"){
            auto found = tree.check_interaction(asker_id,
                Rectangle{295.0f, 295.0f, k_entity_size, k_entity_size});

            THEN("the interactor's id comes back"){
                REQUIRE(found == static_cast<int>(target_id));
            }
        }

        WHEN("a box that clears the hitbox but lands inside the interactor's reach is checked"){
            // [290,294] misses the hitbox at [300,316] but meets the interaction
            // box at [292,324] - both sides bring their own reach
            auto found = tree.check_interaction(asker_id, Rectangle{290.0f, 290.0f, 4.0f, 4.0f});

            THEN("the interactor's id comes back"){
                REQUIRE(found == static_cast<int>(target_id));
            }
        }

        WHEN("a box just outside the interactor's reach is checked"){
            // [280,284] stops short of the interaction box at [292,324]
            auto found = tree.check_interaction(asker_id, Rectangle{280.0f, 280.0f, 4.0f, 4.0f});

            THEN("nothing is reported"){
                REQUIRE(found == game_config::empty_entity);
            }
        }

        WHEN("a box in an empty quadrant is checked"){
            auto found = tree.check_interaction(asker_id,
                Rectangle{50.0f, 300.0f, k_entity_size, k_entity_size});

            THEN("nothing is reported"){
                REQUIRE(found == game_config::empty_entity);
            }
        }
    }
}

SCENARIO("an interaction check reports only interactors, never other interactables",
        "[quadtree][ecs][interaction]"){
    GIVEN("a station asking, with a second station and a collision-only entity to find"){
        collision_component_guard components;
        tree::ecs_quadtree tree(test_world(), 1);

        const size_t asker_id = 1;
        const size_t station_id = 2;
        const size_t inert_id = 3;
        insert_interactable(tree, asker_id, Vector2{50.0f, 50.0f});
        insert_interactable(tree, station_id, Vector2{300.0f, 300.0f});
        insert_collidable(tree, inert_id, Vector2{340.0f, 340.0f});

        REQUIRE(tree.size() == 3);

        WHEN("a box sitting squarely on the second station is checked"){
            auto found = tree.check_interaction(asker_id,
                Rectangle{295.0f, 295.0f, k_entity_size, k_entity_size});

            THEN("nothing is reported - two stations do not occupy each other"){
                REQUIRE(found == game_config::empty_entity);
            }
        }

        WHEN("a box sitting squarely on the collision-only entity is checked"){
            auto found = tree.check_interaction(asker_id,
                Rectangle{335.0f, 335.0f, k_entity_size, k_entity_size});

            THEN("nothing is reported"){
                REQUIRE(found == game_config::empty_entity);
            }
        }

        WHEN("the same boxes are checked for collision instead"){
            THEN("the collision check still sees both"){
                REQUIRE(tree.check_collision(asker_id,
                    Rectangle{295.0f, 295.0f, k_entity_size, k_entity_size})
                    == static_cast<int>(station_id));
                REQUIRE(tree.check_collision(asker_id,
                    Rectangle{335.0f, 335.0f, k_entity_size, k_entity_size})
                    == static_cast<int>(inert_id));
            }
        }

        WHEN("an interactor joins them in the same quadrant"){
            const size_t dog_id = 4;
            insert_interactor(tree, dog_id, Vector2{370.0f, 370.0f});

            THEN("a box reaching all three reports only the interactor"){
                auto found = tree.check_interaction(asker_id,
                    Rectangle{295.0f, 295.0f, 95.0f, 95.0f});
                REQUIRE(found == static_cast<int>(dog_id));
            }
        }
    }
}

SCENARIO("an interaction check never reports the entity doing the asking",
        "[quadtree][ecs][interaction]"){
    GIVEN("two interactors stacked on the same spot"){
        collision_component_guard components;
        tree::ecs_quadtree tree(test_world(), 1);

        const size_t asker_id = 1;
        const size_t target_id = 2;
        insert_interactor(tree, asker_id, Vector2{300.0f, 300.0f});
        insert_interactor(tree, target_id, Vector2{300.0f, 300.0f});

        WHEN("the asker checks a box overlapping its own interaction box"){
            auto found = tree.check_interaction(asker_id,
                Rectangle{295.0f, 295.0f, k_entity_size, k_entity_size});

            THEN("it gets the other entity, not itself"){
                REQUIRE(found == static_cast<int>(target_id));
            }
        }
    }

    GIVEN("a single interactor alone in the tree"){
        collision_component_guard components;
        tree::ecs_quadtree tree(test_world(), 1);

        const size_t asker_id = 1;
        insert_interactor(tree, asker_id, Vector2{300.0f, 300.0f});

        WHEN("it checks its own interaction box"){
            auto found = tree.check_interaction(asker_id, Rectangle{292.0f, 292.0f, 32.0f, 32.0f});

            THEN("nothing is reported"){
                REQUIRE(found == game_config::empty_entity);
            }
        }
    }
}

SCENARIO("an interaction check follows an interactor as it moves",
        "[quadtree][ecs][interaction]"){
    GIVEN("an interactor in the top-right quadrant"){
        collision_component_guard components;
        tree::ecs_quadtree tree(test_world(), 1);

        const size_t asker_id = 1;
        const size_t target_id = 2;
        insert_interactable(tree, asker_id, Vector2{50.0f, 50.0f});
        insert_interactor(tree, target_id, Vector2{300.0f, 300.0f});

        auto probe = Rectangle{295.0f, 295.0f, k_entity_size, k_entity_size};
        REQUIRE(tree.check_interaction(asker_id, probe) == static_cast<int>(target_id));

        WHEN("it is moved into the other quadrant"){
            auto* collision = component_managers::collision_manager_.get_component(target_id);
            collision->get_hitbox_component().get_hitbox().update(Vector2{100.0f, 100.0f});
            tree.move(target_id, collision->get_hitbox_component().get_hitbox());

            THEN("the old spot reports nothing"){
                REQUIRE(tree.check_interaction(asker_id, probe) == game_config::empty_entity);
            }
            THEN("the new spot reports it"){
                REQUIRE(tree.check_interaction(asker_id,
                    Rectangle{95.0f, 95.0f, k_entity_size, k_entity_size})
                    == static_cast<int>(target_id));
            }
        }

        WHEN("it is erased"){
            tree.erase(target_id);

            THEN("nothing is reported"){
                REQUIRE(tree.check_interaction(asker_id, probe) == game_config::empty_entity);
            }
        }
    }
}

SCENARIO("an interaction box straddling a quadrant split still finds what it reaches",
        "[quadtree][ecs][interaction]"){
    GIVEN("an interactor sitting just inside the top-right quadrant"){
        collision_component_guard components;
        tree::ecs_quadtree tree(test_world(), 1);

        const size_t asker_id = 1;
        const size_t target_id = 2;
        insert_interactable(tree, asker_id, Vector2{50.0f, 50.0f});
        insert_interactor(tree, target_id, Vector2{205.0f, 205.0f});

        WHEN("a box crossing the centre split reaches it"){
            // spans [195,215) on both axes - fits in no single child, so the
            // descent has to gate on overlap rather than containment
            auto found = tree.check_interaction(asker_id, Rectangle{195.0f, 195.0f, 20.0f, 20.0f});

            THEN("the target is still found"){
                REQUIRE(found == static_cast<int>(target_id));
            }
        }
    }
}
