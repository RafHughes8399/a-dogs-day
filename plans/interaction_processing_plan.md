# Interaction processing — outline

## Context

`systems::interaction_system::update` is `(void) delta;`
([interaction_system.cpp:4-6](src/engine/systems/interaction_system.cpp:4)), ticked every frame
([game.cpp:78](src/engine/game.cpp:78)), doing nothing. Arbitration already works —
`send_customer_to_table` claims the table, sets the target, issues the path
([customer_arrival_system.cpp:85-102](src/engine/systems/customer_arrival_system.cpp:85)) — so a
customer walks to a reserved table and nothing happens on arrival.

Shape: participation lists on the two components; `interaction` is a live pair of entity ids; the
system holds live pairs plus an index-parallel list of `std::function<void(size_t, size_t, float)>`
behaviours. Each update, intersect the pair's two participation lists and call the behaviour at every
shared index.

---

## 1. `config.h` — new namespace

```cpp
namespace interaction_config{
    enum behaviours{
        sit_at_table = 0,
        behaviours_size
    };
    inline const float seating_duration = /* tune */;
}
```

Follows the `entity_config::selectable_kinds` convention ([config.h:254-261](src/engine/config.h:254)).
`behaviours_size` sizes the behaviour vector, as `events::ids::size` does.

## 2. `component.h` — field additions

`components::interactor_component` ([component.h:149-167](src/engine/components/component.h:149)) and
`components::interactable_component` ([component.h:96-146](src/engine/components/component.h:96)) each gain:

```cpp
    const std::vector<size_t>& get_interactions() const;
private:
    std::vector<size_t> interactions_;
```

Kept ascending so the match is `std::set_intersection` over two sorted vectors. `<algorithm>` and
`<vector>` already included ([component.h:4-13](src/engine/components/component.h:4)).

Thread one parameter through the existing chain:

| Layer | Site |
|---|---|
| builder | `build_interactor_component` / `build_interactable_component` ([component.h:444-446](src/engine/components/component.h:444), [component_builders.cpp:23-30](src/engine/components/component_builders.cpp:23)) |
| helper | `add_interactor_component` / `add_interactable_component` ([component.h:476-479](src/engine/components/component.h:476), [component_helpers.cpp:57-66](src/engine/components/component_helpers.cpp:57)) |
| entity | `build_dog` ([ecs_builders.cpp:29-49](src/entities/ecs_builders.cpp:29)) and `build_station` ([ecs_builders.cpp:182-192](src/entities/ecs_builders.cpp:182)); decls at [entity.h:95,132](src/entities/entity.h:95) |

`build_dog`'s six callers ([ecs_builders.cpp:58,71,79,94,103,111](src/entities/ecs_builders.cpp:58)) and
`build_station`'s four ([:194,206,218,225](src/entities/ecs_builders.cpp:194)) each pass their own list.
Slice 1: `build_customer_dog` and `build_table` pass `{sit_at_table}`, everything else `{}`.

## 3. New `src/engine/systems/interaction.h` / `.cpp`

Add the `.cpp` to the `systems` library in `CMakeLists.txt`.

```cpp
namespace interactions{
class interaction{
public:
    ~interaction() = default;
    interaction(size_t interactor_id, size_t interactable_id);
    interaction(const interaction& other) = default;
    interaction(interaction&& other) = default;
    interaction& operator=(const interaction& other) = default;
    interaction& operator=(interaction&& other) = default;

    size_t get_interactor() const;
    size_t get_interactable() const;
private:
    size_t interactor_id_;
    size_t interactable_id_;
};
}
```

Ids not pointers, as `expediter`'s job types do and for the same reason
([expediter.h:6-13](src/engine/orchestrators/expediter.h:6)).

## 4. `systems::interaction_system` — field additions

Currently stateless ([system.h:200-218](src/engine/systems/system.h:200)). Gains:

```cpp
    void add_interaction(size_t interactor_id, size_t interactable_id);
    void register_behaviour(size_t index, std::function<void(size_t, size_t, float)> behaviour);
    void clear();
    void on_destroyed_entity(const events::remove_entity& event);
private:
    std::vector<interactions::interaction> interactions_;
    std::vector<std::function<void(size_t, size_t, float)>> behaviours_;
    events::event_handler<events::remove_entity> remove_entity_handler_;
```

- `behaviours_` sized `interaction_config::behaviours_size` and populated **in the private constructor**
  ([system.h:214](src/engine/systems/system.h:214)), not `game::init()` — `ecs_test_game` never calls
  `init()` and would see an empty table.
- Subscribing means `~interaction_system() = default;` ([system.h:207](src/engine/systems/system.h:207))
  becomes a real destructor that unsubscribes. Follow `skills/event-wire/SKILL.md` and the shape at
  [system.h:493-510](src/engine/systems/system.h:493).
- Add `interaction_system::get_instance().clear()` to `systems::clear_all_systems`
  ([system.h:555-563](src/engine/systems/system.h:555)) — absent today because the system held no state.
  `clear()` empties `interactions_`, leaves `behaviours_`.

## 5. `update` — pseudocode

```
update(delta):

    pair up:
        for each (id, interactor) in interactor_manager_:
            target = interactor.get_target()
            if no target: continue
            if pair (id, target) already in interactions_: continue
            interactable = interactable_manager_.get_component(target)
            if null: continue
            if not overlap(interactor box at id, interactable box at target): continue
            interactions_.emplace_back(id, target)

    drop dead:
        for i in interactions_ descending:
            interactor   = interactor_manager_.get_component(interactions_[i].get_interactor())
            interactable = interactable_manager_.get_component(interactions_[i].get_interactable())
            dead = interactor == null
                or interactable == null
                or interactor.get_target() != interactions_[i].get_interactable()
                or interactions_[i].get_interactor() not in interactable.get_interactors()
            if dead: erase interactions_[i]

    run:
        for i in 0 .. interactions_.size():
            a = interactions_[i].get_interactor()
            b = interactions_[i].get_interactable()
            shared = set_intersection(interactor.get_interactions(), interactable.get_interactions())
            for index in shared:
                behaviours_[index](a, b, delta)
```

Notes:

- **Index-based loop, deferred erase.** `component_manager` wraps `std::unordered_map`
  ([component.h:411](src/engine/components/component.h:411)); a behaviour that destroys an entity fires
  `remove_entity`, whose handler erases from `interactions_` — the vector being iterated. Never erase
  inside the run pass; collect and apply after.
- **Termination falls out of the void return.** A behaviour has no way to signal completion, so it ends
  itself by calling `interactor->stop_interacting()` and `interactable->release(id)`
  ([component.h:127-129](src/engine/components/component.h:127), [:163-164](src/engine/components/component.h:163))
  — the same pair `unregister_interactor_component` performs
  ([component_helpers.cpp:192-201](src/engine/components/component_helpers.cpp:192)). The drop pass
  removes it next tick. No status enum: the claim is the state.
- **Elapsed time lives in the behaviour**, e.g. a map keyed by interactor id captured by the lambda,
  erased on release. Keeps `interaction` a plain pair.
- **Overlap** uses `interactor->get_interaction_box(dog_hitbox)` vs
  `interactable->get_interaction_box(station_hitbox)`, both already implemented
  ([interactor_component.cpp:3-8](src/engine/components/interactor_component.cpp:3),
  [interactable_component.cpp:7-12](src/engine/components/interactable_component.cpp:7)). Fetch hitboxes
  as `spatial_system::bounds_for` does ([spatial_system.cpp:4-8](src/engine/systems/spatial_system.cpp:4)).
- **Do not use `spatial_system::check_interactions_with`**
  ([spatial_system.cpp:45-47](src/engine/systems/spatial_system.cpp:45)) — it tests against
  `interactor_bounds_for` ([quadtree.cpp:776](src/engine/structures/quadtree.cpp:776)), so it finds
  interactors, and returns only the first hit.
- **`on_destroyed_entity`** drops any pair naming that id. `destroy` executes `remove_entity` *before*
  `unregister_all_components` ([entity_lifespan_system.cpp:17-24](src/engine/systems/entity_lifespan_system.cpp:17)),
  so components are still readable there.
- Frame position is already right: `interaction_` ticks after `movement_`
  ([game.cpp:75-78](src/engine/game.cpp:75)), and `update_position` moves the hitbox and executes
  `move_entity` synchronously ([movement_system.cpp:243-263](src/engine/systems/movement_system.cpp:243)).

## 6. `sit_at_table` — two things to settle before writing it

**Despawn race.** `npc_` ticks before `interaction_` ([game.cpp:74](src/engine/game.cpp:74) vs
[:78](src/engine/game.cpp:78)), and `customer_cleanup` destroys any customer that is not interacting
with an empty path queue ([customer_arrival_system.cpp:110-115](src/engine/systems/customer_arrival_system.cpp:110)).
Releasing on frame N despawns on frame N+1 with no walk-out. Give the behaviour a departure path
before it releases, or accept the instant despawn for slice 1.

**Right slot overlaps by ~3.2px**, against ~131px on the left. From `edge_weight` 64
([config.h:86](src/engine/config.h:86)), table hitbox 128×128 ([config.h:384](src/engine/config.h:384)
via [hitbox_builders.cpp:60-64](src/entities/hitbox_builders.cpp:60)), dog hitbox 128×48
([config.h:367](src/engine/config.h:367)), `station_reach` 16 ([config.h:238](src/engine/config.h:238)),
`dog_reach` 19.2 ([config.h:414](src/engine/config.h:414)), right offset `width + 0.5·edge_weight`
([component_helpers.cpp:99-101](src/engine/components/component_helpers.cpp:99)):

```
station  [P.x - 16,    P.x + 144)
dog      [P.x + 140.8, P.x + 307.2)
```

`create_path_to_entity` then snaps to a graph node
([movement_system.cpp:139-150](src/engine/systems/movement_system.cpp:139)), which can move it further.
Test the right slot explicitly; raise `station_reach` only if it fails.

## 7. Harness

`ecs_test_game::tick` runs only `process_events` and `movement_.update`
([ecs_test_game.cpp:117-120](tests/ecs_test_game.cpp:117)). Add a `systems::interaction_system&` member
beside `spatial_` ([ecs_test_game.cpp:13](tests/ecs_test_game.cpp:13),
[ecs_test_game.h:107-113](tests/ecs_test_game.h:107)) and call `interaction_.update(delta)` last.
Do **not** add `npc_.update` — it spawns and destroys on a timer
([customer_arrival_system.cpp:122-129](src/engine/systems/customer_arrival_system.cpp:122)) and would
make every existing scenario nondeterministic; drive claims with a `seat()`-style helper as
[customer_arrival_scenarios.cpp:26-33](tests/customer_arrival_scenarios.cpp:26) does.

`DOG_DAYS_TESTING` accessors (convention at [system.h:271-287](src/engine/systems/system.h:271)):
`interaction_count()`, `has_interaction(interactor, interactable)`, surfaced through `ecs_test_game`
beside `has_interactor` / `has_interactable` ([ecs_test_game.h:85-88](tests/ecs_test_game.h:85)).

New `tests/interaction_scenarios.cpp`, registered in the `tests` target.

```bash
cmake --build build && ./build/tests
```

## 8. Refresh CLAUDE.md — done

Corrected the layering diagram (nine ECS systems, no `level`; legacy is test-only), the
`src/systems/components/` and `src/systems/` paths, the dishwasher and `items.h` stub entries, and
added sections for the live component set, the `tree::ecs_quadtree`, the executed-not-queued
lifecycle events, and the two test harnesses.

## 9. Deferred

- **Claim vs occupy** — `interactable_component::interactors_` stays two-state
  ([component.h:145](src/engine/components/component.h:145)). Restructure occupation later.
- **No broad-phase** — pairs only form from an arbitrated target, so nothing fires between two entities
  nobody sent to each other.
- **One producer of targets** — only `customer_arrival_system`
  ([customer_arrival_system.cpp:89](src/engine/systems/customer_arrival_system.cpp:89)); player
  right-click issues a path without naming one
  ([control_input_system.cpp:203-249](src/engine/systems/control_input_system.cpp:203)).
- **`state_machine_component` stays empty** ([component.h:335](src/engine/components/component.h:335)),
  so multi-step behaviour (counter → food → table → serve → return) has no home yet.
