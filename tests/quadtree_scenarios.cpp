// Scenarios for tree::quadtree itself: does it place entities in the right
// node, and does it keep tracking them correctly once they move - in
// particular the way dogs actually move.
//
// dog::update_path (src/entities/dogs/dog.cpp move_toward_current_waypoint)
// advances position_ directly and returns status_codes::moved; it never calls
// entity::move() and so never fires a move_entity event. The quadtree catches
// that relocation itself, inside its own update(): if a "moved" object no
// longer fits its current node's bounds, it is extracted and reinserted from
// the root. That is the path tested here as "movement along a path". A
// separate scenario covers the other relocation route - entity::move() firing
// move_entity, which on_move_event listens for - since it's the same
// mechanism a few other movers (e.g. the player dogs) rely on.
//
// These tests build tree::quadtree directly with minimal test-double entities
// (a single fixed hitbox, no sprites) rather than going through test_game/
// level, for two reasons: level never exposes its internal quadtree for
// inspection, and quadtree's public surface (size/height/num_nodes/
// object_in_node/on_is_colliding_query/extract) is already rich enough to
// verify placement and post-move behaviour without it.
#include <catch2/catch_test_macros.hpp>

#include "entities.h"
#include "events.h"
#include "events_interface.h"
#include "quadtree.h"
#include "raylib.h"
#include "raymath.h"

namespace{
    constexpr float k_entity_size = 16.0f;

    body::body make_single_hitbox_body(Vector2 position){
        std::vector<hitbox::hitbox> hitboxes{
            hitbox::hitbox(Rectangle{position.x, position.y, k_entity_size, k_entity_size})};
        std::vector<sprite::sprite> sprites{};
        return body::body(hitboxes, sprites, 0);
    }

    // A stationary sentinel, used purely as a fixed collision partner.
    class fixed_entity : public entities::entity{
        public:
            fixed_entity(Vector2 position, int id)
            : entity(make_single_hitbox_body(position), position, id, "qt_test_fixed"){}
    };

    // Moves at a constant velocity every update() and reports
    // status_codes::moved - the same contract dog::update_path fulfils.
    class path_walker : public entities::entity{
        public:
            path_walker(Vector2 position, int id, Vector2 velocity)
            : entity(make_single_hitbox_body(position), position, id, "qt_test_walker"),
            velocity_(velocity){}

            int update(float delta, int frame) override{
                (void) frame;
                move_without_event(Vector2Add(position_, Vector2Scale(velocity_, delta)));
                return entities::status_codes::moved;
            }
        private:
            Vector2 velocity_;
    };

    // Reports itself dead on every update() - exercises the quadtree's
    // dead-entity removal/graveyard branch.
    class dying_entity : public entities::entity{
        public:
            dying_entity(Vector2 position, int id)
            : entity(make_single_hitbox_body(position), position, id, "qt_test_dying"){}

            int update(float delta, int frame) override{
                (void) delta;
                (void) frame;
                return entities::status_codes::dead;
            }
    };

    // A small, easy-to-reason-about world. depth=1 means insert() builds the
    // 4 children on the first insert and places every object directly into a
    // leaf (or the root, if it straddles the centre) - no further
    // subdivision to reason about.
    raglib::bounding_box_2 test_world(){
        return raglib::bounding_box_2{Vector2{0.0f, 0.0f}, Vector2{400.0f, 400.0f}};
    }
    // Quadrant bounds matching tree::quadtree::build_children's layout for
    // test_world() (centre at {200, 200}).
    raglib::bounding_box_2 bottom_left_quadrant(){
        return raglib::bounding_box_2{Vector2{0.0f, 0.0f}, Vector2{200.0f, 200.0f}};
    }
    raglib::bounding_box_2 top_right_quadrant(){
        return raglib::bounding_box_2{Vector2{200.0f, 200.0f}, Vector2{400.0f, 400.0f}};
    }
}

using tree::quadtree;

SCENARIO("a quadtree places a newly inserted entity into the correct quadrant", "[quadtree][insert]"){
    GIVEN("an empty quadtree over a 400x400 world"){
        quadtree tree(test_world(), 1);

        WHEN("an entity fully inside the top-right quadrant is inserted"){
            auto entity = std::make_unique<fixed_entity>(Vector2{300.0f, 300.0f}, 1);
            auto* entity_ptr = entity.get();
            tree.insert(std::move(entity));

            THEN("the tree holds one object across a root + 4 children layout"){
                REQUIRE(tree.size() == 1);
                REQUIRE(tree.num_nodes() == 5);
                REQUIRE(tree.height() == 1);
                REQUIRE_FALSE(tree.is_leaf());
            }
            THEN("the object's rectangle belongs to the top-right quadrant only"){
                auto obj_rect = entity_ptr->get_hitbox().get_box();
                auto top_right = top_right_quadrant();
                auto bottom_left = bottom_left_quadrant();
                REQUIRE(tree.object_in_node(top_right, obj_rect));
                REQUIRE_FALSE(tree.object_in_node(bottom_left, obj_rect));
            }
        }
    }
}

SCENARIO("an entity straddling the centre stays at the parent level", "[quadtree][insert][boundary]"){
    GIVEN("an empty quadtree over a 400x400 world"){
        quadtree tree(test_world(), 1);

        WHEN("an entity whose rectangle crosses both quadrant boundaries is inserted"){
            // rect spans [194, 210) on both axes - straddles the x=200 and
            // y=200 splits, so it cannot fit entirely inside any one child.
            auto entity = std::make_unique<fixed_entity>(Vector2{194.0f, 194.0f}, 1);
            auto* entity_ptr = entity.get();
            tree.insert(std::move(entity));

            THEN("children still get built, but the object is not lost or duplicated"){
                REQUIRE(tree.size() == 1);
                REQUIRE(tree.num_nodes() == 5);
            }
            THEN("the object belongs to the root's bounds but no single quadrant"){
                auto obj_rect = entity_ptr->get_hitbox().get_box();
                auto world = test_world();
                auto top_right = top_right_quadrant();
                auto bottom_left = bottom_left_quadrant();
                REQUIRE(tree.object_in_node(world, obj_rect));
                REQUIRE_FALSE(tree.object_in_node(top_right, obj_rect));
                REQUIRE_FALSE(tree.object_in_node(bottom_left, obj_rect));
            }
        }
    }
}

SCENARIO("a path-following entity that crosses into a new quadrant is still tracked correctly",
        "[quadtree][update][movement]"){
    GIVEN("a walker in the bottom-left quadrant and a sentinel in the top-right quadrant"){
        quadtree tree(test_world(), 1);

        const int walker_id = 1;
        const int sentinel_id = 2;
        // One update() with delta=1.0 and velocity {100,100} carries the
        // walker from (150,150) to exactly (250,250), landing squarely on
        // the sentinel - deterministic, no need to iterate several frames.
        tree.insert(std::make_unique<path_walker>(Vector2{150.0f, 150.0f}, walker_id, Vector2{100.0f, 100.0f}));
        tree.insert(std::make_unique<fixed_entity>(Vector2{250.0f, 250.0f}, sentinel_id));
        REQUIRE(tree.size() == 2);

        WHEN("the tree is updated once, moving the walker across the centre"){
            std::vector<std::unique_ptr<entities::entity>> graveyard;
            auto to_remove = tree.update(1.0f, 0, graveyard);

            THEN("no entities died and none were lost or duplicated"){
                REQUIRE(to_remove.empty());
                REQUIRE(graveyard.empty());
                REQUIRE(tree.size() == 2);
            }
            THEN("the walker's rectangle now belongs to the top-right quadrant, not the bottom-left"){
                // Keep the extracted unique_ptr alive in a named variable -
                // extract() returning a temporary would destroy the entity
                // at end of statement, leaving a dangling raw pointer.
                auto extracted = tree.extract(static_cast<size_t>(walker_id));
                auto* walker = dynamic_cast<path_walker*>(extracted.get());
                REQUIRE(walker != nullptr);
                auto walker_rect = walker->get_hitbox().get_box();
                auto top_right = top_right_quadrant();
                auto bottom_left = bottom_left_quadrant();
                REQUIRE(tree.object_in_node(top_right, walker_rect));
                REQUIRE_FALSE(tree.object_in_node(bottom_left, walker_rect));
            }
            THEN("a collision query for the walker finds the sentinel it landed on"){
                hitbox::hitbox walker_box{Rectangle{250.0f, 250.0f, k_entity_size, k_entity_size}};
                queries::is_colliding_query query{walker_box, walker_id};
                REQUIRE(tree.on_is_colliding_query(query));
            }
        }
    }
}

SCENARIO("an entity reported dead during update() is removed into the graveyard",
        "[quadtree][update][removal]"){
    GIVEN("a quadtree holding one entity that reports itself dead every update"){
        quadtree tree(test_world(), 1);
        const int id = 7;
        tree.insert(std::make_unique<dying_entity>(Vector2{50.0f, 50.0f}, id));
        REQUIRE(tree.size() == 1);

        WHEN("the tree is updated"){
            std::vector<std::unique_ptr<entities::entity>> graveyard;
            auto to_remove = tree.update(1.0f / 60.0f, 0, graveyard);

            THEN("the entity's id is reported for removal and it lands in the graveyard"){
                REQUIRE(to_remove.size() == 1);
                REQUIRE(to_remove.front() == id);
                REQUIRE(graveyard.size() == 1);
                REQUIRE(graveyard.front()->get_id() == id);
            }
            THEN("the tree no longer holds it"){
                REQUIRE(tree.size() == 0);
                REQUIRE(tree.extract(static_cast<size_t>(id)) == nullptr);
            }
        }
    }
}

SCENARIO("erasing an entity does not disturb a sibling in a different quadrant",
        "[quadtree][erase]"){
    GIVEN("two entities in different quadrants"){
        quadtree tree(test_world(), 1);
        const int a_id = 3;
        const int b_id = 4;
        tree.insert(std::make_unique<fixed_entity>(Vector2{50.0f, 50.0f}, a_id));
        tree.insert(std::make_unique<fixed_entity>(Vector2{250.0f, 250.0f}, b_id));
        REQUIRE(tree.size() == 2);

        WHEN("one of them is erased"){
            tree.erase(static_cast<size_t>(a_id));

            THEN("only the erased entity is gone"){
                REQUIRE(tree.size() == 1);
                REQUIRE(tree.extract(static_cast<size_t>(a_id)) == nullptr);
                REQUIRE(tree.extract(static_cast<size_t>(b_id)) != nullptr);
            }
        }
    }
}

SCENARIO("explicit move() relocates an entity via the move_entity event, like on_move_event expects",
        "[quadtree][move_event]"){
    GIVEN("an entity inserted into the bottom-left quadrant"){
        quadtree tree(test_world(), 1);
        const int id = 9;
        auto entity = std::make_unique<fixed_entity>(Vector2{50.0f, 50.0f}, id);
        auto* entity_ptr = entity.get();
        tree.insert(std::move(entity));

        WHEN("the entity moves into the top-right quadrant via entity::move()"){
            entity_ptr->move(Vector2{250.0f, 250.0f});
            // move() only queues move_entity; drain it so on_move_event runs.
            events::global_dispatcher_.process_events(0.0f);

            THEN("the tree still holds exactly one object, now in the top-right quadrant"){
                REQUIRE(tree.size() == 1);
                auto obj_rect = entity_ptr->get_hitbox().get_box();
                auto top_right = top_right_quadrant();
                auto bottom_left = bottom_left_quadrant();
                REQUIRE(tree.object_in_node(top_right, obj_rect));
                REQUIRE_FALSE(tree.object_in_node(bottom_left, obj_rect));
            }
        }
    }
}
