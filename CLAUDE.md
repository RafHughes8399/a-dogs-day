# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

An idle cafe-management game built in C++20 with raylib, themed around the author's dogs. The player runs a cafe staffed and visited by dogs: customer dogs queue, get seated, order, get served by waiter dogs, eat, and leave; a maitre d' and an expediter system orchestrate the whole flow.

## Build / test commands

The project **must be compiled with Clang**, not GCC: `CMakeLists.txt` sets `MY_COMPILE_OPTIONS` with Clang-only warning flags (`-Wconditional-uninitialized`, `-Wloop-analysis`, `-Wmove`, `-Wself-assign`, `-Wdangling`, `-Wsuggest-destructor-override`, `-Winconsistent-missing-destructor-override`, `-Wreturn-std-move`, `-Wduplicate-enum`, etc.) that GCC rejects outright. CI (`.github/workflows/test-runner.yaml`) builds with `CC=clang CXX=clang++`; match that locally.

```sh
# configure (first time / after CMakeLists.txt changes)
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug   # ensure CC/CXX point at clang/clang++

# build everything (game + tests)
cmake --build build

# build only the test binary
cmake --build build --target tests

# run the full test suite
./build/tests

# run a single scenario/tag (Catch2 tag filtering)
./build/tests "[station]"
./build/tests -c "food counter"

# run with CI-style reporters
./build/tests --reporter console::out=- --reporter JUnit::out=test-results.xml
```

`compile_commands.json` is generated into `build/` (`CMAKE_EXPORT_COMPILE_COMMANDS ON`). clangd is configured via the root `.clangd` file (`CompileFlags: CompilationDatabase: build`) rather than a copy/symlink at the repo root.

Test sources live in `tests/*.cpp` and are registered explicitly in `CMakeLists.txt`'s `tests` executable — a new scenario file must be added there to be compiled in. Tests link Catch2 and the game's static libs, and compile with `DOG_DAYS_TESTING` defined (see "Testing accessors" below), but deliberately skip `MY_COMPILE_OPTIONS` so the strict warnings don't fire inside Catch2/raylib headers.

## Architecture

### Layering

```
dog_days.cpp (main)
  -> game::game            orchestrates the frame loop: update/render/debug
       -> level::level      owns all entities, the spatial index, and the pathing graph
       -> maitre_d::maitre_d   cafe/queue/table orchestration (owns no entities)
       -> expediter::expediter service/order orchestration (owns no entities)
       -> player::player, player::controls, menus::menu_graph
```

`level` is the only thing that owns entities (in a `tree::quadtree` plus per-draw-layer `render_layer::layer`s). `maitre_d` and `expediter` are orchestration/domain-logic systems that never own entities — they track **non-owning raw pointers** (e.g. `entities::table*`, `entities::waiter_dog*`) registered/deregistered via events, and command `level` to actually mutate the world. This split (world ownership vs. domain orchestration) is the central architectural boundary; when adding cafe/service behavior, ask whether it's "world state" (belongs in `level`/the entity) or "business logic" (belongs in `maitre_d`/`expediter`).

### Communication: events (async/observer) vs queries (sync/request-response)

Two parallel, near-identical infrastructures in `src/systems/`, used for different directions of data flow:

- **Events** (`events.h`, `events_interface.h`): fire red-and-forget facts or commands. `events::event` subclasses are pure data carriers with a `static int get_static_type()`; `events::event_handler<E>` wraps a typed lambda; `event_interface::subscribe/unsubscribe/queue_event/execute_event` is the facade over the single global `events::global_dispatcher_`. Events can be queued (processed next `process_events(delta)` tick) or executed immediately, and can carry a delay.
- **Queries** (`queries.h`, `query_interface.h`): synchronous request/response when a caller needs a value back (`is_colliding`, `path`, `can_place_decoration`). `queries::query_executor<T>` is templated on the *return type* (there are three global executors: `bool_executor_`, `int_executor_`, `path_executor_`), unlike events which dispatch by event type only.

Use the **`event-wire` skill** (`skills/event-wire/SKILL.md`) when adding a new event wire — it encodes the exact naming/lifecycle conventions (typed handler members, subscribe in constructor, unsubscribe in destructor, `events::ids` enum + `ids::size`).

Event/query naming convention worth knowing: events are named as either **facts that already happened** (`registered_table`, `customer_dog_left`, `dog_reached_station`) or **commands** (`send_dog_to_position`, `send_waiter_to_table`) — comments in `events.h` mark cafe-domain events as "Cafe-domain fact" vs "Cafe-domain command". Keep that distinction when adding new ones.

### Entity hierarchy

```
entities::entity  (base: body/hitbox/sprite, position, id, debug_id)
  -> dog
       -> player_dog          (state pattern: selected/unselected; owns cosmetics/outlines)
       -> npc_dog              (adds pathing-driven update)
            -> stateful_npc_dog<Derived, StateBase>   (CRTP-ish mixin; forwards to state_)
                 -> customer_dog   (customer_dog_state: default_state/walking_to_table/seated/eating/leaving)
                 -> waiter_dog     (waiter_dog_state: idle/serving; carries held_food_)
            -> dishwasher_dog      (plain npc_dog, no state machine yet)
  -> decoration                (placeable; subscribes to cursor while being carried)
       -> station                (station_state: unworked/worked; enter/leave/is_interacting by dog id)
            -> table              (table_state: available/reserved/occupied)
            -> food_counter        (FILO food stack with reservation counting)
            -> dishwasher          (stub: no .cpp, not registered in CMakeLists, not wired to any system)
  -> cursor                    (state: in_menus/editing/carrying_decoration; interaction_strategy: default/left_click/right_click)
  -> food
```

Every concrete entity has a matching `*_builder.cpp` that implements factory methods declared on `entities::entity_builder` (`src/entities/entity_builder.h`) — the single global instance is `entities::e_builder`. Adding a new entity kind means: add the class to its domain header, add a `build_*` declaration to `entity_builder.h`, implement it in a new/matching `*_builder.cpp`, and register the new `.cpp` in `CMakeLists.txt`.

### State pattern is used pervasively, and follows one consistent shape

`customer_dog_state`, `waiter_dog_state`, `station::station_state`, `cursor::state`, `player_dog::state`, `player::state`, `debug::logger::state` all follow the same shape: an abstract base with pure-virtual behavior methods taking the owning object by reference (never storing it), owned via `std::unique_ptr<State>` on the owner, swapped with a `set_state`/`set_*` method. `customer_dog_traveling_state` / `waiter_dog_traveling_state` factor out the "walking somewhere, react `on_path_finished`" shape shared by multiple states (template-method-ish: `on_arrived` is the hook subclasses implement). When adding new dog/station behavior, prefer adding a state class over adding boolean flags to the entity.

### Config namespaces (`src/engine/config.h`)

All tunable constants live in `config.h`, grouped by domain namespace rather than by file: `game_config`, `player_config`, `feature_flag_config`, `level_config` (world dimensions, draw-layer enum, direction enum/scalars), `cafe_config` (queue geometry/timing — note several values are computed at namespace-scope via immediately-invoked lambdas, e.g. `left_queue_positions`), `entity_config` (debug-id prefixes, sprite file paths, per-entity sprite `attributes` arrays), `dog_config`, `controls_config` (raylib key bindings), `hud_config`, `debug_logger_config`. When adding a new tunable, put it in the namespace matching its domain rather than creating a new one.

### Other systems worth knowing about

- **`level::level_graph`** (`src/engine/graph.h`): the walkable node/edge grid backing pathfinding and decoration placement. It's itself both an event listener (`moved_decoration`, `placed_decoration` update walkability) and a query handler (`can_place_decoration`, `path` via BFS) — same dual role appears on `tree::quadtree` (listens to `move_entity`/`interact_entity`, answers `is_colliding_query`).
- **`maitre_d::dog_queue`**: models the two physical waiting lines (left/right) as `queue_lane`s of `queued_dog`, independent of the maitre_d's table-assignment logic.
- **Debug logging**: `debug::log(message)` (`debug_log_interface.h`) is a free function used throughout for structured trace logging; it queues an `events::debug_log` event that `debug::logger` (a Meyers singleton, toggled with `/` and pause with `P`) renders as an overlay.
- **Skills in `skills/`**: this repo carries its own Claude Code skills (`design-check`, `event-wire`, `map`, `state-of-play`) that encode project-specific collaboration workflows — check `skills/*/SKILL.md` before assuming generic behavior for design review, event wiring, code mapping, or branch status reporting.

### Testing conventions

- `tests/test_game.h`/`test_game.cpp` is a hand-rolled composition root (`testing::test_game`) that owns a fresh `level_`, `maitre_d_`, `expediter_` per Catch2 `SCENARIO` — no rendering, no player/controls/menus. RAII teardown unsubscribes event handlers between scenarios, so tests don't leak state into each other via the global event dispatcher.
- Accessors/build helpers guarded by `DOG_DAYS_TESTING` (set only on the `tests` target, never the shipped binary) expose otherwise-private state for assertions.
- Prefer `tick_until(predicate, max_frames)` over hardcoded frame counts when a scenario needs to wait for a condition (e.g. a dog reaching a destination).
- One scenario file per domain (`station_scenarios.cpp`, `food_scenarios.cpp`, `maitre_d_scenarios.cpp`, `expediter_scenarios.cpp`, `decoration_scenarios.cpp`, `player_scenarios.cpp`, `dishwasher_dog_scenarios.cpp`, `harness_scenarios.cpp` for the test harness itself) — add new scenarios to the matching file, and register genuinely new files in `CMakeLists.txt`'s `tests` target.

### Known incomplete/stub areas (don't assume these are wired up)

- `entities::dishwasher` (station) is a stub: no `.cpp`, not registered in `CMakeLists.txt`, not tracked by any orchestration system.
- `entities::dishwasher_dog` exists as an entity/builder but currently has no dedicated orchestration system driving it (unlike `waiter_dog`/`expediter`).
- `items.h`'s shop/inventory system (`item`, `shop_item`, `item_manager`) is scaffolding with many unimplemented `build_*` declarations and no `.cpp`.
- See `table-registration-flow.md` at the repo root for a worked example of the "wire an entity into the cafe orchestration layer via events" pattern.
