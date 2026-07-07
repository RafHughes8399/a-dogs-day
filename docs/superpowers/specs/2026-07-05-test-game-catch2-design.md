# test_game + Catch2 simulation harness — Design

## Goal

Add a Catch2-based test suite that can drive game logic (dog state machines, queueing, order flow) through manual simulation scenarios, without rendering and without relying on real OS keyboard input. Prove correctness of things like "customer dog queues, gets assigned a table, and paths there correctly" with `REQUIRE`-style assertions instead of manual playtesting.

## Current state (facts gathered before designing)

- No test infrastructure exists in the repo today: no Catch2, no `tests/` directory, no `ctest`/`add_test` wiring.
- `expediter` and `maitre_d` are Meyer's singletons (private constructor, `static X& get_instance()`, function-local static). `get_instance()` is called from exactly two places: [src/engine/game.h:23-24](../../../src/engine/game.h) and the already-dead `src/systems/maitre_d_interface.cpp`. Nothing else references them directly — all other interaction happens through the event system.
- The shared event bus, `events::global_dispatcher_` ([src/systems/events.cpp:5](../../../src/systems/events.cpp)), is a plain `extern` global, always alive for the process lifetime. It is not owned by `expediter`/`maitre_d`/`level` — everything subscribes to it directly via `event_interface::subscribe<E>(...)`.
- `level` already follows correct RAII subscribe/unsubscribe discipline (constructor subscribes, destructor unsubscribes — [src/engine/level.h:137-144](../../../src/engine/level.h)), as does `customer_dog` ([src/entities/entities.h:546-547](../../../src/entities/entities.h)). `expediter` and `maitre_d` do not: both have `~X() = default` and never unsubscribe.
- Debug-only test-triggers like the L-key customer arrival are wired directly to raylib's real input polling (`if(IsKeyPressed(KEY_L))` in [src/systems/maitre_d.cpp:126](../../../src/systems/maitre_d.cpp)). There is no way to inject a synthetic keypress into raylib from a test process; the only viable path is to call the same underlying method the key would call (`maitre_d::request_customer_arrival()`, already public — [src/systems/maitre_d.h:202](../../../src/systems/maitre_d.h)).
- Building a real `customer_dog` goes through `entities::entity_builder::build_customer_dog` ([src/entities/dogs.cpp:523-618](../../../src/entities/dogs.cpp)), whose active (non-`#if 0`'d) code path calls `textures::textures_.get_texture(...)` → raylib `LoadTexture()` ([src/entities/dogs.cpp:567-568](../../../src/entities/dogs.cpp), [src/rendering/texture.cpp:16-22](../../../src/rendering/texture.cpp)), which requires a real GL context from `InitWindow()`. There is no texture-free path for building a real dog today.
- A dog's current path (`current_path_`, `move_paths_` — [src/entities/entities.h:303-304](../../../src/entities/entities.h)) and current state (`state_` on `stateful_npc_dog`) are protected, with no public accessor of any kind today.

## Decisions made during brainstorming

1. **Debug/key-triggered actions**: the testing interface calls the underlying method directly (e.g. `fire_debug_arrival()` → `maitre_d_.request_customer_arrival()`). No attempt to simulate real OS-level key input.
2. **GL context**: `test_game` calls `InitWindow()` with `SetConfigFlags(FLAG_WINDOW_HIDDEN)` once, so production entity-building code (including texture loads) runs completely unmodified. Tests are not pure headless unit tests — they need a display/GPU available wherever they run.
3. **Singletons**: `expediter` and `maitre_d` stop being singletons. They become plain, constructible/destructible objects, owned directly by `game` (production) and `test_game` (tests) — mirroring the pattern `game` already uses for `level_`/`player_`. Blast radius is small (2 call sites, see above).
4. **Event dispatcher / cross-scenario isolation**: rather than adding a `reset_for_test()` to the dispatcher, give `expediter` and `maitre_d` proper RAII destructors that unsubscribe their handlers — the same pattern `level` and `customer_dog` already follow. Because `test_game` owns these by value and constructs a fresh instance per `SCENARIO`, normal C++ destruction at end of scope naturally leaves the shared dispatcher with zero stale subscribers between scenarios. The one residual gap — events already sitting in the dispatcher's queue but not yet processed at scenario teardown — is handled by draining the queue in `test_game`'s destructor/teardown, not by a broader dispatcher reset.
5. **Introspection for assertions**: add narrow, `DOG_DAYS_TESTING`-guarded public accessors directly to the relevant production classes (e.g. `stateful_npc_dog::get_state_name()`, a getter for `current_path_`), rather than going fully black-box (asserting only on final position/emitted events) or using a friend-class accessor. Each state subclass gets a small `name()` override to back `get_state_name()`.

## Architecture

`test_game` is a **new, dedicated composition root**, separate from `game`, not a stripped-down reuse of it. It owns fresh instances of only the systems needed for logic scenarios:

```cpp
class test_game{
    public:
        test_game(); // InitWindow(hidden), constructs level_/maitre_d_/expediter_
        ~test_game(); // drains dispatcher queue, then destroys members (RAII unsubscribe fires)

        void tick(float delta, int frames = 1);
        void tick_until(std::function<bool()> predicate, int max_frames);

        // command methods (the "testing interface")
        void insert_customer_dog(int id, Vector2 position);
        void fire_debug_arrival();

        // assertion accessors
        entities::customer_dog& get_dog(int id);

    private:
        level::level level_;
        maitre_d::maitre_d maitre_d_;
        expediter::expediter expediter_;
};
```

`tick()` replicates `game::update`'s system order minus player/controls/menus/rendering: `process_events → maitre_d_.update(delta) → expediter_.process_orders()`. `tick_until()` calls `tick()` in a loop up to `max_frames`, checking `predicate()` each time — this is the "await a condition" mechanism, avoiding hardcoded frame counts for event-driven flows.

There is no separate "testing interface" class — the public methods on `test_game` are the interface. `insert_customer_dog` calls `entities::e_builder.build_customer_dog(...)` (same as [src/engine/level.cpp:226](../../../src/engine/level.cpp)) and `level_.add_entity(...)` directly, bypassing the normal spawn-via-event flow for deterministic test setup.

## Production-code changes required

1. `src/systems/expediter.h` / `.cpp`: make constructor public, remove `get_instance()`, add a real destructor that calls `event_interface::unsubscribe<E>(...)` for all 3 remaining handlers (`registered_waiter_handler_`, `registered_food_counter_handler_`, `dog_reached_table_handler_`).
2. `src/systems/maitre_d.h` / `.cpp`: same treatment — public constructor, remove `get_instance()`, add destructor unsubscribing all its handlers.
3. `src/engine/game.h`: change `maitre_d_`/`expediter_` from singleton references to owned value members, constructed directly in `game`'s constructor init list instead of via `get_instance()`.
4. Delete the now-fully-dead `src/systems/maitre_d_interface.cpp` and its declarations in `maitre_d.h` (its only caller was `get_instance()`, which no longer exists) — confirm with user before deleting since it wasn't explicitly in scope of this design; flag it rather than silently removing.
5. `src/entities/entities.h`: add `#ifdef DOG_DAYS_TESTING` guarded accessors to `stateful_npc_dog` (`get_state_name()`) and `dog` (`get_current_path()`, returning `current_path_`). Add a small `name()` virtual to each state class (`customer_dog_state` hierarchy at minimum; `waiter_dog_state` hierarchy can follow the same pattern later if/when waiter scenarios are written).

## Build wiring

- Add Catch2 via `FetchContent`, following the same pattern already used for raylib in `CMakeLists.txt` (`find_package` QUIET, fetch if not found).
- New CMake executable target `tests`, linking the existing static libs (`level`, `maitre_d`, `expediter`, `entities`, `events`, `dog_actions`, `graph`, raylib, etc. — the same set `dog-days`/`game` already link).
- A `DOG_DAYS_TESTING` compile definition scoped to the `tests` target only (via `target_compile_definitions`), so the test-only accessors added in step 5 above never compile into the shipped `dog-days` binary.

## Example scenario

```cpp
SCENARIO("customer dog queues and is assigned a table"){
    test_game game;
    GIVEN("a customer dog is inserted at the entrance"){
        game.insert_customer_dog(/*id=*/1, entry_position);
        WHEN("a debug arrival is fired"){
            game.fire_debug_arrival();
            game.tick_until([&]{ return game.get_dog(1).get_state_name() != std::string("default_state"); }, 60);
            THEN("the dog has a real path toward a queue slot or table"){
                REQUIRE_FALSE(game.get_dog(1).get_current_path().empty());
            }
        }
    }
}
```

## Explicitly out of scope

- Simulating real OS-level keyboard/mouse input.
- Testing `player`, `controls`, `menus`, or anything rendering-related.
- Waiter dog scenarios — all 8 waiter leaf states are still no-op stubs (per `adogsday_current_work` memory), so there's nothing meaningful to simulate there yet.
- A general dispatcher-wide `reset_for_test()` — deliberately avoided in favor of RAII (see Decision 4).
