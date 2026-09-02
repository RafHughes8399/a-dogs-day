# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

An idle cafe-management game built in C++20 with raylib, themed around the author's dogs. The player runs a cafe staffed and visited by dogs: customer dogs queue, get seated, order, get served by waiter dogs, eat, and leave.

The codebase is mid-refactor from an inheritance-based entity hierarchy with `maitre_d`/`expediter` orchestrators to an ECS. **The ECS is what the shipped binary runs**; the legacy half still compiles and is still covered by tests, but nothing in the live call graph reaches it. Read the Layering section before assuming which half you are working in.

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

## Code style

- Prefer `class` over `struct` unless explicitly told otherwise, including for component-style types (e.g. the ECS components in `src/engine/components/`) — these are expected to carry member functions and encapsulated state, not stay plain data aggregates.
- **Do not write comments unless explicitly asked to.** Write the code and nothing else. This applies everywhere — implementation, headers, and test files alike. The existing `// *` blocks throughout the codebase (`component.h`, `ecs_builders.cpp`, `system.h`) are the author's own design notes recording decisions they reasoned through; they are not a density to match, and adding to them buries the notes that matter. If something genuinely can't be understood from the code, say so in your response instead of committing a comment about it.

## Architecture

### Layering

**The ECS is the live game.** An ECS refactor has replaced the old object hierarchy on the shipped path. `game::game` (`src/engine/game.h`) holds nine system references and no `level`, no `maitre_d`, no `expediter`:

```
dog_days.cpp (main)
  -> game::game                 frame loop: update/render/debug
       process_events, then tick in member-declaration order:
         systems::entity_lifespan_system   id allocation, create/destroy
         systems::control_input_system     keyboard/mouse -> commands
         systems::npc_system               owns dbs::customer_arrival_system, dbs::waiter_idling_system
         systems::movement_system          paths, positions, owns the level_graphs
         systems::spatial_system           owns the tree::ecs_quadtree
         systems::collision_system         stub
         systems::interaction_system       stub
       render: systems::rendering_system   owns the render_layer::ecs_layers
```

`systems::selection_system` is the ninth member and is never ticked — `control_input_system` drives it directly.

Every system is a Meyers singleton (`get_instance()`), non-copyable because it subscribes event handlers in its constructor and unsubscribes in its destructor. There is **no system base class** — `void update(float delta)` is a convention, not an interface. Entities are bare `size_t` ids allocated by `entity_lifespan_system::next_id()`; ids are recycled on destroy with no generation counter. `game_config::empty_entity` (`-1`, an `int`) is the sentinel, so id comparisons cast.

Components live in one header, `src/engine/components/component.h`, namespace `components`. Storage is one global `component_managers::component_manager<C>` per type — a `std::unordered_map<size_t, C>` keyed by entity id. There is no base component class, no runtime component type id, and no `get_component_by_type`: every call site names a specific manager and null-checks the result. Adding a component type means editing the `extern` block plus the three hand-maintained per-type loops in `component_helpers.cpp` (`unregister_all_components`, `num_registered_components`, `clear_all_components`) and `ecs_test_game::total_components`.

Entities are assembled by free `build_*` functions in `namespace ecs_entities` (`src/entities/ecs_builders.cpp`, declared in `entity.h`), each taking an already-allocated id and calling `component_helpers::add_*_component`. There is no archetype or descriptor — an entity kind *is* its sequence of `add_*` calls.

**The legacy layer still compiles and is still tested, but nothing in the live call graph reaches it.** `level::level`, `maitre_d::maitre_d`, `expediter::expediter`, `player`, `controls`, `menus`, the `entities::` class hierarchy and the entity-owning `tree::quadtree` are constructed only by `testing::test_game` (`tests/test_game.h`). Treat that half as reference material for behavior being ported, not as code the game runs. When adding cafe/service behavior, it goes in the ECS.

### Communication: events (async/observer) vs queries (sync/request-response)

Two parallel, near-identical infrastructures — events in `src/engine/events/`, queries in `src/engine/queries/` — used for different directions of data flow:

- **Events** (`events.h`, `events_interface.h`): fire red-and-forget facts or commands. `events.h` is a pure aggregator over `event_core.h` plus per-domain headers (`dog_events.h`, `entity_events.h`, `system_events.h`, `decoration_events.h`, `input_events.h`, `debug_events.h`). `events::event` subclasses are pure data carriers with a `static int get_static_type()`; `events::event_handler<E>` wraps a typed lambda; `event_interface::subscribe/unsubscribe/queue_event/execute_event` is the facade over the single global `events::global_dispatcher_`. Events can be queued (processed next `process_events(delta)` tick) or executed immediately, and can carry a delay. **The lifecycle events are executed, not queued** — `create_entity`, `remove_entity` and `move_entity` all fire synchronously so the spatial index never lags the world by a frame. `create_entity` fires *after* the builder has registered components; `remove_entity` fires *before* they are unregistered, so a handler can still read them.
- **Queries** (`queries.h`, `query_interface.h`): synchronous request/response when a caller needs a value back (`is_colliding`, `path`, `can_place_decoration`). `queries::query_executor<T>` is templated on the *return type* (there are three global executors: `bool_executor_`, `int_executor_`, `path_executor_`), unlike events which dispatch by event type only.

Use the **`event-wire` skill** (`skills/event-wire/SKILL.md`) when adding a new event wire — it encodes the exact naming/lifecycle conventions (typed handler members, subscribe in constructor, unsubscribe in destructor, `events::ids` enum + `ids::size`).

Event/query naming convention worth knowing: events are named as either **facts that already happened** (`registered_table`, `customer_dog_left`, `dog_reached_station`) or **commands** (`send_dog_to_position`, `send_waiter_to_table`) — comments in `events.h` mark cafe-domain events as "Cafe-domain fact" vs "Cafe-domain command". Keep that distinction when adding new ones.

### Components (the live model)

`src/engine/components/component.h`, namespace `components`, one global manager each:

| Component | Holds |
|---|---|
| `position_component` | `Vector2 position_` |
| `movement_component` | `std::queue<path::path> paths_`, move speed, direction scalar |
| `collision_component` | a `hitbox_component` — a vector of hitboxes plus the active index |
| `renderable_component` | a vector of `sprite_component`, each a sprite vector plus active index |
| `selectable_component` | `size_t kind_` (`entity_config::selectable_kinds`), `bool is_selected_` |
| `interactable_component` | reach, per-direction approach offsets, and the interactor claiming each slot |
| `interactor_component` | reach, `std::optional<size_t> target_` |
| `storage_component` | an `item_stack::item_stack` (counters and stoves only) |
| `key_input_component` / `mouse_input_component` | bound `game_config::input`s |
| `state_machine_component` | **empty stub** — no members, no `.cpp`, not in `CMakeLists.txt` |

`interactable_component` / `interactor_component` are a two-way claim, not a proximity test: `claim`/`release` on one side pair with `interact_with`/`stop_interacting` on the other, and the only place both halves are reconciled is `component_helpers::unregister_interact*_component`. The claim is taken by the orchestrator *before* the dog walks over (`dbs::customer_arrival_system::send_customer_to_table`), so a table promised to a customer still crossing the cafe already reads as taken.

### Legacy entity hierarchy (test-only; being ported)

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

Every concrete legacy entity has a matching `*_builder.cpp` implementing factory methods declared on `entities::entity_builder` (`src/entities/entity_builder.h`) — the single global instance is `entities::e_builder`. This is the *legacy* builder path; new entity kinds go through `ecs_entities::build_*` in `src/entities/ecs_builders.cpp` instead.

### State pattern (legacy), and one consistent shape

Used pervasively on the legacy side. `customer_dog_state`, `waiter_dog_state`, `station::station_state`, `cursor::state`, `player_dog::state`, `player::state`, `debug::logger::state` all follow the same shape: an abstract base with pure-virtual behavior methods taking the owning object by reference (never storing it), owned via `std::unique_ptr<State>` on the owner, swapped with a `set_state`/`set_*` method. `customer_dog_traveling_state` / `waiter_dog_traveling_state` factor out the "walking somewhere, react `on_path_finished`" shape shared by multiple states (template-method-ish: `on_arrived` is the hook subclasses implement). The ECS has no equivalent yet — `state_machine_component` is an empty stub — so ECS-side behavior currently lives in the systems, and multi-step sequences (waiter: counter → food → table → serve → return) have no home.

### Config namespaces (`src/engine/config.h`)

All tunable constants live in `config.h`, grouped by domain namespace rather than by file: `type_config`, `game_config`, `player_config`, `feature_flag_config`, `level_config` (world dimensions, draw-layer enum, direction enum/scalars), `graph_config`, `cafe_config` (queue geometry/timing — note several values are computed at namespace-scope via immediately-invoked lambdas, e.g. `left_queue_positions`), `station_config`, `entity_config` (debug-id prefixes, sprite file paths, per-entity sprite `attributes` arrays, `selectable_kinds`, interaction reach and slot offsets), `dog_config`, `controls_config` (raylib key bindings), `hud_config`, `debug_logger_config`. When adding a new tunable, put it in the namespace matching its domain rather than creating a new one.

### Other systems worth knowing about

- **`level::level_graph`** (`src/engine/structures/graph.h`): the walkable node/edge grid backing pathfinding and decoration placement. It's itself both an event listener (`moved_decoration`, `placed_decoration` update walkability) and a query handler (`can_place_decoration`, `path` via BFS). On the ECS path the graphs are owned by `systems::movement_system`.
- **`tree::ecs_quadtree`** (`src/engine/structures/quadtree.h`): the live spatial index. Stores bare `size_t` ids and resolves bounds per-query from `collision_manager_`; it holds no handlers and no id allocation — `systems::spatial_system` owns it and does the subscribing. Its `check_collision` / `check_interaction` return the **first** hit as an `int`, never a set. Note `check_interaction` tests against *interactor* boxes, so it finds interactors, not interactables; it currently has no production callers. The legacy `tree::quadtree` is the one that owns entities and does the all-pairs `identify_collisions` loop.
- **`maitre_d::dog_queue`**: models the two physical waiting lines (left/right) as `queue_lane`s of `queued_dog`, independent of the maitre_d's table-assignment logic. Legacy.
- **Debug logging**: `debug::log(message)` (`debug_log_interface.h`) is a free function used throughout for structured trace logging; it queues an `events::debug_log` event that `debug::logger` (a Meyers singleton, toggled with `/` and pause with `P`) renders as an overlay.
- **Skills in `skills/`**: this repo carries its own Claude Code skills (`design-check`, `event-wire`, `map`, `state-of-play`) that encode project-specific collaboration workflows — check `skills/*/SKILL.md` before assuming generic behavior for design review, event wiring, code mapping, or branch status reporting.

### Testing conventions

- **Two composition roots, one per world.** `tests/ecs_test_game.h`/`.cpp` (`testing::ecs_test_game`) is the ECS one: it holds singleton references and wipes every component manager and system in its constructor and destructor, since the singletons outlive a scenario. `tests/test_game.h`/`.cpp` (`testing::test_game`) is the legacy one, owning a fresh `level_`, `maitre_d_`, `expediter_` per `SCENARIO`. Neither renders.
- **`ecs_test_game::tick` deliberately runs only a subset** — `process_events` then `movement_.update`. It does *not* tick `npc_`, because `customer_arrival_system` spawns and destroys dogs on a timer and would make every scenario nondeterministic; drive claims explicitly with a `seat()`-style helper instead. Adding a system to that tick changes the blast radius for every existing ECS scenario, so re-run the whole suite when you do.
- Accessors/build helpers guarded by `DOG_DAYS_TESTING` (set only on the `tests` target, never the shipped binary) expose otherwise-private state for assertions.
- Prefer `tick_until(predicate, max_frames)` over hardcoded frame counts when a scenario needs to wait for a condition (e.g. a dog reaching a destination).
- One scenario file per domain — ECS: `ecs_scenarios.cpp`, `customer_arrival_scenarios.cpp`, `control_input_scenarios.cpp`, `spatial_scenarios.cpp`, `waiter_idling_scenarios.cpp`, `counter_storage_scenarios.cpp`, `quadtree_scenarios.cpp`, `graph_scenarios.cpp`; legacy: `station_scenarios.cpp`, `food_scenarios.cpp`, `maitre_d_scenarios.cpp`, `expediter_scenarios.cpp`, `decoration_scenarios.cpp`, `player_scenarios.cpp`, `customer_dog_scenarios.cpp`, `waiter_dog_scenarios.cpp`, `dishwasher_dog_scenarios.cpp`, `dishwasher_scenarios.cpp`; plus `harness_scenarios.cpp` for the harness itself. Add new scenarios to the matching file, and register genuinely new files in `CMakeLists.txt`'s `tests` target.

### Known incomplete/stub areas (don't assume these are wired up)

- **`systems::interaction_system` and `systems::collision_system` are `(void) delta;` stubs.** They are ticked every frame and do nothing. Nothing on the ECS path currently tests interaction-box overlap, and there is no ECS equivalent of the legacy `dog_reached_station` arrival fact — a customer walks to its claimed table and nothing happens. `entity_lifespan_system::update`, `spatial_system::update` and `selection_system::update` are also stubs, but deliberately: those systems are event-driven and hold no per-frame work.
- **`components::state_machine_component` is empty** — no members (the `std::vector<state_component>` is commented out), no `.cpp`, not registered in `CMakeLists.txt`. `build_state_machine_component` discards its argument.
- **Only `customer_arrival_system` ever sets an interactor's target.** Player right-click issues a path without naming a target, and `waiter_idling_system` only paths.
- `entities::dishwasher_dog` exists as an entity/builder but has no dedicated orchestration system driving it (unlike `waiter_dog`/`expediter`).
- `items.h`'s shop/inventory system (`item`, `shop_item`, `item_manager`) has a `.cpp` now, but zero consumers anywhere in `src/` or `tests/` — still scaffolding.
- `render_layer::ecs_layer` and `tree::ecs_quadtree` carry `RENAME AFTER REFACTOR IS COMPLETE` TODOs; so does `namespace ecs_entities` in `entity.h`. The `ecs_` prefixes are temporary and go away when the legacy halves are deleted.
- See `table-registration-flow.md` at the repo root for a worked example of the legacy "wire an entity into the cafe orchestration layer via events" pattern, and `plans/` for the in-flight interaction design notes.
