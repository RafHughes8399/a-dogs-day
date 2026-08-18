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
            // the real ecs_entities::build_cursor, not a stand-in
            size_t create_cursor(size_t layer = level_config::draw_layers::cursor);
            size_t create_khiri(size_t layer = level_config::draw_layers::dogs);
            size_t create_mack(size_t layer = level_config::draw_layers::dogs);
            size_t create_customer_dog(Vector2 position, size_t layer = level_config::draw_layers::dogs);
            size_t create_waiter_dog(Vector2 position, size_t layer = level_config::draw_layers::dogs);
            size_t create_test_decoration(Vector2 position, size_t layer = level_config::draw_layers::decoration);
            size_t create_table(Vector2 position, size_t layer = level_config::draw_layers::stations);
            size_t create_food_counter(Vector2 position, size_t layer = level_config::draw_layers::stations);
            size_t create_dishwasher(Vector2 position, size_t layer = level_config::draw_layers::stations);
            size_t create_stove(Vector2 position, size_t layer = level_config::draw_layers::stations);
            size_t create_food(Vector2 position, size_t layer = level_config::draw_layers::decoration);

            void tick(float delta);
            bool tick_until(std::function<bool()> predicate, int max_frames, float delta = 0.016f);
            void path_to(size_t entity_id, Vector2 destination,
                std::optional<size_t> destination_entity = std::nullopt);
            void remove(size_t entity_id);
            // the single legal position write - moves the hitbox and reindexes
            void move_entity(size_t entity_id, Vector2 position);

            // ---------------- inspection accessors ----------------
            size_t layer_size(size_t layer);
            bool layer_contains(size_t layer, size_t entity_id);
            Rectangle view_frame();
            // how many of the nine managers hold a component for this entity
            size_t num_components(size_t entity_id);
            bool has_position(size_t entity_id);
            bool has_renderable(size_t entity_id);
            bool has_controls(size_t entity_id);
            bool has_collision(size_t entity_id);
            bool has_mouse_input(size_t entity_id);
            bool has_movement(size_t entity_id);
            bool has_path(size_t entity_id);
            float facing_x_of(size_t entity_id);
            bool has_selectable(size_t entity_id);
            bool has_interactor(size_t entity_id);
            bool has_interactable(size_t entity_id);
            size_t selectable_kind_of(size_t entity_id);
            // total across every manager, for asserting a clean world
            size_t total_components();

            // ---------------- spatial accessors ----------------
            bool is_tracked(size_t entity_id);
            size_t tracked_count();
            int node_depth_of(size_t entity_id);
            bool node_bounds_of(size_t entity_id, raglib::bounding_box_2& bounds);
            Rectangle hitbox_of(size_t entity_id);

            // ---------------- graph accessors ----------------
            int graph_occupant_at(Vector2 position);
            size_t graph_occupied_node_count();
            bool graph_marks(size_t entity_id, Rectangle footprint);

        private:
            // the systems are singletons that outlive every scenario - the ctor
            // and dtor wipe their storage instead of relying on RAII
            systems::entity_lifespan_system& lifespan_;
            systems::rendering_system& rendering_;
            systems::spatial_system& spatial_;
            systems::movement_system& movement_;
    };

} // namespace testing

#endif // ECS_TEST_GAME_H
