#ifndef TEST_GAME_H
#define TEST_GAME_H

// -----------------------------------------------------------------------------
// test_game - the "testing interface" / composition root for logic tests.
//
// Owns fresh instances of the game systems needed to drive logic scenarios
// (no rendering, no player/controls/menus). Each Catch2 SCENARIO declares its
// own `test_game` on the stack, so every test starts from a clean world and
// RAII teardown unsubscribes handlers between scenarios.
//   design: docs/superpowers/specs/2026-07-05-test-game-catch2-design.md
//
// The public methods below ARE the interface: `build_*`/`insert_*` set up
// state, `customer_arrives()` (etc.) trigger flows, `tick*()` advances the
// simulation, and the accessors read state back for REQUIRE assertions.
//
// NOTE: these are STUBS - signatures only. Bodies live in test_game.cpp and
// currently throw "not implemented"; fill them in as you go.
// -----------------------------------------------------------------------------

#include <functional>
#include <memory>
#include <optional>

#include "raylib.h"    // Vector2
#include "config.h"    // level_config::draw_layers, dog-type ids
#include "entities.h"  // entities::entity, entities::e_builder, customer_dog...
#include "level.h"     // level::level, level::level_builder
#include "maitre_d.h"  // maitre_d::maitre_d  (owned once de-singletonised)
#include "expediter.h" // expediter::expediter (owned once de-singletonised)

namespace testing{

    class test_game{
        public:
            // ---------------- lifecycle ("create game") ----------------
            // InitWindow(FLAG_WINDOW_HIDDEN) once, build a fresh level_, and
            // (after the de-singleton refactor) construct maitre_d_/expediter_.
            test_game();
            // Drain the shared event queue, then destroy members so their RAII
            // destructors unsubscribe from events::global_dispatcher_.
            ~test_game();

            test_game(const test_game&) = delete;
            test_game(test_game&&) = delete;
            test_game& operator=(const test_game&) = delete;
            test_game& operator=(test_game&&) = delete;

            // ---------------- simulation clock ----------------
            // Advance the sim `frames` step(s): process_events ->
            // maitre_d_.update(delta) -> expediter_.process_serving_jobs(). No render.
            void tick(float delta, int frames = 1);
            // tick() in a loop up to max_frames, stopping early when predicate()
            // is true. Returns whether the predicate became true (the "await a
            // condition" mechanism - avoids hardcoded frame counts).
            bool tick_until(const std::function<bool()>& predicate, int max_frames);

            // ---------------- build actions ----------------
            // Thin wrappers over entities::e_builder. These CONSTRUCT an entity
            // but do NOT add it to the level - compose with insert_entity(), e.g.
            //   insert_entity(build_customer_dog(1, pos), level_config::dogs);
            std::unique_ptr<entities::entity> build_mack(int id, Vector2 position);  // player dog
            std::unique_ptr<entities::entity> build_khiri(int id, Vector2 position); // player dog
            std::unique_ptr<entities::entity> build_table(int id, Vector2 position);        // station
            std::unique_ptr<entities::entity> build_food_counter(int id, Vector2 position); // station
            std::unique_ptr<entities::entity> build_dishwasher(int id, Vector2 position);  // station
            std::unique_ptr<entities::food> build_test_food(int id, Vector2 position);      // food entity
            std::unique_ptr<entities::entity> build_customer_dog(
                int id, Vector2 position,
                std::optional<Vector2> destination = std::nullopt,
                int dog_type = cafe_config::customer_dog_type);
            std::unique_ptr<entities::entity> build_waiter_dog(
                int id, int dog_type, Vector2 position,
                std::optional<Vector2> destination = std::nullopt);
            std::unique_ptr<entities::entity> build_dishwasher_dog(int id, Vector2 position);

            // ---------------- insert actions ----------------
            // Add an already-built entity to the level on the given draw layer.
            void insert_entity(std::unique_ptr<entities::entity> entity, size_t layer);

            // Convenience build+insert helpers (mirror the spec's example). These
            // bypass the normal spawn-via-event flow for deterministic setup.
            void insert_customer_dog(int id, Vector2 position,
                                     std::optional<Vector2> destination = std::nullopt);
            void insert_waiter_dog(int id, int dog_type, Vector2 position,
                                   std::optional<Vector2> destination = std::nullopt);
            void insert_dishwasher_dog(int id, Vector2 position);
            void insert_dishwasher(int id, Vector2 position);   // station
            void insert_table(int id, Vector2 position);
            void insert_food_counter(int id, Vector2 position);

            // ---------------- event triggers ----------------
            // Fire the same path the debug 'L' key fires
            // (maitre_d_.request_customer_arrival()). No synthetic OS input.
            void customer_arrives();
            // Fire dog_reached_station for the given customer/table - the "customer
            // requests an order on seating" signal the expediter listens for.
            void request_order(size_t customer_id, size_t table_id, Vector2 table_position);
            // Remove an entity from the level (fires remove_entity).
            void remove_entity(int id);
            // Reposition an entity in place (fires move_entity), standing in for
            // the player dragging a decoration - the cursor/carry flow needs
            // input the harness deliberately does not simulate.
            void move_entity(int id, Vector2 position);
            // Fire clear_table for a table, the fact the maitre d' emits once a
            // customer leaves. Lets a clearing job be driven without running the
            // whole seat -> eat -> leave cycle first.
            void request_clear_table(int table_id);

            // ---------------- inspection accessors ----------------
            // Look up an entity by id in the level; nullptr if absent.
            entities::entity* find_entity(int id);
            // Total entities currently in the level (public level API passthrough).
            int num_entities();
            // The level's view frame - the thing move_view_frame actually moves.
            Rectangle view_frame();
            // Cafe-system tracking counts (passthroughs to expediter_/maitre_d_),
            // for asserting registration and removal in scenarios.
            int num_waiters();
            int num_counters();
            int num_tables();          // maitre d' table count
            int num_customers();
            int num_expediter_tables(); // expediter table count (distinct system)
            int num_dishwashers();
            int num_serving_jobs();
            int num_clearing_jobs();
            int num_tracked_customers(); // maitre d' pointer tracking, not queue length
            expediter::serving_job_status first_serving_job_status();
            // Live pointers to the first tracked waiter / counter, for driving
            // availability in scenarios (nullptr if none). Not owned.
            entities::waiter_dog* first_waiter();
            entities::food_counter* first_counter();
            entities::dishwasher* first_dishwasher();
            // First customer the maitre d' is tracking. The arrival flow builds
            // the dog via an event, so the test never chose its id - this is how
            // a scenario gets hold of a customer it did not insert itself.
            entities::customer_dog* first_customer();
            // Typed accessors (throw if id is not that dog type / not present).
            entities::customer_dog& get_customer_dog(int id);
            entities::waiter_dog& get_waiter_dog(int id);
            // Typed level lookups; nullptr when absent or the wrong kind.
            entities::table* find_table(int id);
            entities::dishwasher* find_dishwasher(int id);

        private:
            std::unique_ptr<level::level> level_;
            // Owned BY VALUE so per-scenario RAII cleans up their event
            // subscriptions when the test_game goes out of scope (spec
            // Decisions 3 & 4). Construction order: level_, then maitre_d_,
            // then expediter_; destruction unsubscribes in reverse.
            maitre_d::maitre_d   maitre_d_;
            expediter::expediter expediter_;
    };

} // namespace testing

#endif // TEST_GAME_H
