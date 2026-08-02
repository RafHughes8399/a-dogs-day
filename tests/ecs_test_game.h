#ifndef ECS_TEST_GAME_H
#define ECS_TEST_GAME_H

// -----------------------------------------------------------------------------
// ecs_test_game - composition root for the ECS-side logic tests, the counterpart
// to test_game.
//
// Owns the systems by value so RAII unsubscribes their handlers between
// scenarios, and clears the component managers on the way in and out - those
// are namespace-scope globals, so without that, entities leak from one scenario
// into the next.
// -----------------------------------------------------------------------------

#include <functional>

#include "config.h"
#include "entity.h"
#include "raylib.h"
#include "system.h"

namespace testing{

    class ecs_test_game{
        public:
            ecs_test_game();
            ~ecs_test_game();

            ecs_test_game(const ecs_test_game&) = delete;
            ecs_test_game(ecs_test_game&&) = delete;
            ecs_test_game& operator=(const ecs_test_game&) = delete;
            ecs_test_game& operator=(ecs_test_game&&) = delete;

            // ---------------- build actions ----------------
            // allocate an id, run the builder, announce - the real create path
            template<typename Builder>
            size_t create(Builder build, size_t layer){
                return lifespan_.create(build, layer);
            }
            // a bare entity with no components, for testing the id/layer plumbing
            // without a builder's component choices in the way
            size_t create_empty(size_t layer);
            // an entity carrying a position and a renderable, the two components
            // the render layer needs to draw it
            size_t create_renderable(size_t layer, Vector2 position = Vector2{0.0f, 0.0f});
            size_t create_player(size_t cursor_id);

            void remove(size_t entity_id);

            // ---------------- inspection accessors ----------------
            size_t layer_size(size_t layer);
            bool layer_contains(size_t layer, size_t entity_id);
            // how many of the eight managers hold a component for this entity
            size_t num_components(size_t entity_id);
            bool has_position(size_t entity_id);
            bool has_renderable(size_t entity_id);
            bool has_controls(size_t entity_id);
            // total across every manager, for asserting a clean world
            size_t total_components();

        private:
            systems::entity_lifespan_system lifespan_;
            systems::rendering_system rendering_;
    };

} // namespace testing

#endif // ECS_TEST_GAME_H
