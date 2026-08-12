# Interaction components — dog ↔ station in the ECS

## Context

The `ecs_refactor` branch has components and systems for position, movement, collision,
rendering, selection and input. Right-click already sends the selected player dog to a
clicked position ([control_input_system.cpp:208](src/engine/systems/control_input_system.cpp:208)),
but nothing happens when it gets there. The next milestone is **sending a player dog to a
station and having the arrival become an observable, testable fact**.

Two placeholders exist for this and are both empty:

- `components::interaction_component` — no data, no members ([component.h:162](src/engine/components/component.h:162))
- `systems::interaction_system::update` — `(void) delta;` ([interaction_system.cpp:4](src/engine/systems/interaction_system.cpp:4))

The legacy path being replaced is `customer_dog::walking_to_table` firing
`dog_reached_station` on arrival ([customer_dog.cpp:29](src/entities/dogs/customer_dog.cpp:29)),
which `level` turns into `station::enter` ([level.cpp:326](src/engine/level.cpp:326)) against
`station::interacting_dog_ids_` ([station.cpp:53-72](src/entities/stations/station.cpp:53)).

**Chosen design:** geometric detection using **interaction boxes that are separate from
collision hitboxes** (the hitbox/hurtbox split), with **asymmetric components** —
stations *offer* interaction, actors *do* interaction.

---

## The geometry — get this right first

Everything else follows from these numbers. All verified.

| Thing | Value | Source |
|---|---|---|
| `edge_weight` (grid cell) | `64.0f` | [config.h:90](src/engine/config.h:90) |
| station hitbox (table / counter / dishwasher) | `128 × 128`, top-left anchored at position | [config.h:300-302](src/engine/config.h:300), [hitbox_builders.cpp:4](src/entities/hitbox_builders.cpp:4) |
| dog hitbox | `128 × 48`, top-left anchored | [config.h:288](src/engine/config.h:288) |
| interaction slots today | `left = P.x − 64`, `right = P.x + 128` | [station.cpp:86](src/entities/stations/station.cpp:86) |
| overlap test | strict `<` / `>` — touching edges do **not** collide | `rshapes.c:2248` |

### Why the collision hitboxes cannot be the trigger

```
station box            x ∈ [P.x,       P.x + 128)
dog at LEFT slot       x ∈ [P.x - 64,  P.x + 64)    -> overlaps [P.x, P.x+64)     ✔
dog at RIGHT slot      x ∈ [P.x + 128, P.x + 256)   -> P.x+128 < P.x+128 is FALSE ✘
```

Reusing the collision hitboxes gives interaction from the left only. The root cause is
that **the dog hitbox is `2 × edge_weight` wide but anchored top-left**, so it is not
centred on the cell the dog stands in. Inflating the station box by a buffer fixes the
right slot and immediately breaks the left: at buffer `64` the station reaches
`x ∈ [P.x − 64, P.x + 192)`, and a dog a whole extra cell out at `P.x − 128`
(box `[P.x − 128, P.x)`) still overlaps.

### Why separate interaction boxes fix it

Anchor the **actor's** interaction box to the cell it occupies (`edge_weight` square at
its position) rather than to its 128-wide sprite hitbox, and inflate the **station's**
interaction box by a buffer `b`:

```
actor  interaction box   x ∈ [pos.x,      pos.x + 64)
station interaction box  x ∈ [P.x - b,    P.x + 128 + b)     (same in y)
```

Work the three cases:

```
LEFT  slot   pos.x = P.x - 64    -> [P.x-64,  P.x)      overlaps iff  b > 0
RIGHT slot   pos.x = P.x + 128   -> [P.x+128, P.x+192)  overlaps iff  b > 0
OVER-REACH   pos.x = P.x - 128   -> [P.x-128, P.x-64)   overlaps iff  b > 64
```

**Any `b` in `(0, 64]` admits exactly the adjacent cells and nothing further, on all four
sides, symmetrically.** Half an edge — `b = 32` — sits in the middle of that window and is
the value to use. This is the original "buffer of maybe half an edge" instinct; it only
works once the actor's box stops being its collision hitbox.

Add `interaction_config::station_buffer = level_config::edge_weight * 0.5f` and
`interaction_config::actor_reach` (the actor cell size, `edge_weight`) to `config.h`.

### Derive the boxes, do not cache them

`collision_component` caches hitbox rects and pays for it — `movement_system::update_position`
has to walk every variant and re-`update()` it on each write
([movement_system.cpp:129-135](src/engine/systems/movement_system.cpp:129)). Interaction boxes
should store **extents only** and be computed from `position_component` at read time. There is
then no sync path and no way for the box to lag the position.

---

## Components

Two components, not one. A station *offers* slots; an actor *occupies* one. A single symmetric
`other_entity_` id caps a station at one occupant, which contradicts `station::capacity_` and
`worked::enter`'s capacity check ([station.cpp:57-60](src/entities/stations/station.cpp:57)).
The split also makes dog↔dog binding structurally impossible — dogs never carry
`interactable_component`.

```cpp
// components::interactable_component  - stations offer interaction
class interactable_component {
public:
    interactable_component(Vector2 extents, float buffer, size_t capacity);

    Rectangle get_interaction_box(Vector2 position) const;   // inflated by buffer_
    bool has_free_slot() const;
    bool is_occupied_by(size_t actor_id) const;
    bool occupy(size_t actor_id);                            // false when full or duplicate
    void release(size_t actor_id);
    const std::vector<size_t>& get_occupants() const;
    size_t get_capacity() const;
private:
    Vector2 extents_;
    float buffer_;
    size_t capacity_;
    std::vector<size_t> occupants_;
};

// components::interacting_component  - actors do interaction
class interacting_component {
public:
    explicit interacting_component(Vector2 extents);

    Rectangle get_interaction_box(Vector2 position) const;   // the cell the actor stands in
    bool is_interacting() const { return target_.has_value(); }
    std::optional<size_t> get_target() const;
    void bind(size_t target);
    void unbind();
private:
    Vector2 extents_;
    std::optional<size_t> target_;
};
```

`occupy`/`release` are a direct port of `station::worked::enter` / `worked::leave`
([station.cpp:53-72](src/entities/stations/station.cpp:53)) — duplicate check, then capacity
check, then push. The unworked/worked state machine does not come across; "is anyone here" is
`not occupants_.empty()`, which is what the two states were encoding.

`interaction_component` and everything named after it is deleted:
`build_interaction_component` ([component_builders.cpp:24](src/engine/components/component_builders.cpp:24)),
`interaction_manager_` ([component_managers.cpp:19](src/engine/components/component_managers.cpp:19)),
the register/unregister helpers, and the entries in `unregister_all_components`,
`num_registered_components` and `clear_all_components` ([component.h:453-494](src/engine/components/component.h:453)).
Two new managers replace it in all four parallel blocks.

---

## The system

### Drive-by binding, and the one-line fix

Pure geometric detection binds a dog that merely *walks past* a station — it would fire
`dog_reached_station` and consume a slot for a few frames. Gate binding on the actor being
**stationary**: `movement_manager_[actor].get_paths().empty()`. A dog in transit always has a
path; a dog that arrived does not. Existing data, no new state, and it removes the entire
drive-by class without giving up the geometric trigger.

### `interaction_system::update`

```
for (actor_id, interacting) in interacting_manager_:

    actor_position = positional_manager_[actor_id]            # skip if absent
    actor_box      = interacting.get_interaction_box(actor_position)

    # ---------------- already bound: does it still hold? ----------------
    if interacting.is_interacting():
        target      = interacting.get_target()
        interactable = interactable_manager_.get_component(target)

        still_there = interactable != null
                      and overlaps(actor_box,
                                   interactable.get_interaction_box(positional_manager_[target]))

        if still_there: continue

        if interactable: interactable.release(actor_id)
        interacting.unbind()
        queue ended_interaction{actor_id, target}
        continue

    # ---------------- unbound: only settled actors may bind ----------------
    movement = movement_manager_.get_component(actor_id)
    if movement and not movement.get_paths().empty(): continue      # still walking

    for (target_id, interactable) in interactable_manager_:
        if target_id == actor_id:            continue
        if not interactable.has_free_slot(): continue
        if not overlaps(actor_box,
                        interactable.get_interaction_box(positional_manager_[target_id])):
            continue

        interactable.occupy(actor_id)
        interacting.bind(target_id)
        queue dog_reached_station{actor_id, target_id, positional_manager_[target_id]}
        break                                # one slot at a time
```

Unbinding falls out for free: the dog is given a new path → it moves → the boxes stop
overlapping → the next tick releases the slot. No separate "leave" command.

### Why brute force, and not the quadtree

The inner loop is over `interactable_manager_` — stations only, a handful. With ~10 dogs and
~10 stations that is 100 rectangle tests a frame. `ecs_quadtree` indexes **collision hitboxes**,
not interaction boxes, and `check_collision(id, box)` returns only the first hit
([quadtree.h:289](src/engine/structures/quadtree.h:289)) — using it would mean a second spatial
index plus a new collect-all traversal. Revisit if the station count reaches the low hundreds.

### Where it runs

`ecs_game::update` already calls `interaction_.update(delta)` immediately after
`movement_.update(delta)` ([game.cpp:97-113](src/engine/game.cpp:97)), so an arrival is visible
the same frame it happens. **Do not** hook `events::move_entity` — it is executed synchronously
inside `update_position`, once per moving entity per frame, mid position-write
([movement_system.cpp:138](src/engine/systems/movement_system.cpp:138)).

### Teardown on destroy

Nothing currently clears the *other* side's reference when an entity dies —
`unregister_all_components` erases the dead entity's own components only. `interaction_system`
needs an `events::remove_entity` handler, the shape of
`selection_system::on_destroyed_entity` ([system.h:383](src/engine/systems/system.h:383)):

- removed entity was a target → clear `target_` on every actor bound to it
- removed entity was an actor → `release` it from its target

Which means `interaction_system` stops being default-constructible: it gains a handler member,
subscribes in the constructor and unsubscribes in the destructor, per the `event-wire`
conventions in `skills/event-wire/SKILL.md`.

### Events

Reuse `events::dog_reached_station` ([dog_events.h:140](src/engine/events/dog_events.h:140)) —
`level`, `maitre_d` and `expediter` already consume it
([expediter.cpp:449](src/engine/orchestrators/expediter.cpp:449)), so ECS arrivals land on the
existing seam rather than a parallel one. `ended_interaction` is new: add it to the `events::ids`
enum with `ids::size` bumped, following `event-wire`.

The system emits facts and stores ids. It does not call behaviour on the target — that is
`station::interact(entity&)` ([station.cpp:15](src/entities/stations/station.cpp:15)), the
pattern this refactor removes. Components store; systems act; orchestrators decide.

---

## Files to touch

| File | Change |
|---|---|
| [src/engine/config.h](src/engine/config.h) | new `interaction_config` namespace: station buffer, actor extents |
| [src/engine/components/component.h](src/engine/components/component.h) | delete `interaction_component`; add both new classes; two managers, builders, register/unregister helpers; update the three blanket helpers |
| [src/engine/components/component_builders.cpp](src/engine/components/component_builders.cpp) | replace `build_interaction_component` with the two builders |
| [src/engine/components/component_managers.cpp](src/engine/components/component_managers.cpp) | replace `interaction_manager_` with two instances |
| [src/engine/components/component_accessors.cpp](src/engine/components/component_accessors.cpp) | `get_interaction_box`, `occupy`, `release`, `bind`, `unbind` |
| [src/engine/systems/system.h](src/engine/systems/system.h) | `interaction_system` gains a `remove_entity` handler, private ctor with subscribe, dtor with unsubscribe |
| [src/engine/systems/interaction_system.cpp](src/engine/systems/interaction_system.cpp) | the update loop and the destroy handler |
| [src/engine/events/event_core.h](src/engine/events/event_core.h), `dog_events.h` | `ended_interaction` id + class |
| [src/entities/ecs_builders.cpp](src/entities/ecs_builders.cpp) | `build_station_components:62` registers `interactable`; `build_dog_components:12` registers `interacting` |
| [tests/ecs_test_game.h](tests/ecs_test_game.h)/`.cpp` | see below |
| [CMakeLists.txt](CMakeLists.txt) | register `tests/interaction_scenarios.cpp` in the `tests` target (line ~259) |

---

## Verification

**The harness does not tick the interaction system.** `ecs_test_game::tick` runs
`process_events` then `movement_.update(delta)` and nothing else
([ecs_test_game.cpp:110-113](tests/ecs_test_game.cpp:110)). It must gain
`interaction_.update(delta)` in the same order the real loop uses, plus an
`interaction_` member. `has_interaction` ([ecs_test_game.h:74](tests/ecs_test_game.h:74))
splits into `has_interactable` / `has_interacting`, and `num_components` /
`total_components` counts shift from nine managers to ten — existing assertions in
`ecs_scenarios.cpp` will need updating.

```bash
cmake --build build --target tests
```

```bash
./build/tests "[interaction]"
```

New `tests/interaction_scenarios.cpp`, one scenario per case, `tick_until(predicate, max_frames)`
rather than fixed frame counts:

- dog placed at the **left** slot of a table binds
- dog placed at the **right** slot binds — the case bare collision-hitbox overlap fails
- dog placed **two cells** out does not bind (the `b ≤ 64` bound)
- dog **walking through** a slot with a live path does not bind
- dog right-clicked onto a table binds once the path completes
- second dog is refused when the table is at capacity
- re-pathing the dog away releases the slot and emits `ended_interaction`
- removing the table clears the dog's `target_`; removing the dog releases the slot
- dog sent to a plain decoration (no `interactable_component`) never binds

In-game: right-click a table with a player dog selected and watch the debug overlay
(`/` to toggle, `P` to pause) for the arrival line.

---

## Deferred

- **Directionality.** The buffer admits interaction from above and below as well as left and
  right, where `station::update_interaction_positions` encodes left/right only. That affects
  sprite facing and seating. Deferred until a station actually reacts to arrival; revisiting
  means either named slots with positions, or a non-uniform buffer (`b > 0` in x, `b = 0` in y).
- **Multi-occupant stations.** `capacity_` is carried and enforced, but every station is
  capacity 1 today ([stations.h:57](src/entities/stations/stations.h:57)), so nothing exercises
  `> 1` yet.

## Prerequisite bug — fix or accept before dogs stand at stations

`level_graph::update_entity` writes node occupancy unconditionally
([graph.cpp:451-458](src/engine/structures/graph.cpp:451)), and
`movement_system::on_moved_entity` re-stamps it on every move
([movement_system.cpp:70-74](src/engine/systems/movement_system.cpp:70)). Because the dog
*collision* hitbox is `128` wide on a `64` grid, a dog standing at a station's left slot spans
the station's own node column, overwrites the station's occupancy with its own id, and blanks it
to `empty_node` on departure — leaving the station's footprint walkable.

This is independent of the interaction design (separate interaction boxes mean interaction no
longer *depends* on that overlap), but it is a pre-existing bug that this feature makes routine,
because the whole point is to park dogs at stations. Worth its own fix: either an occupancy guard
in `update_entity`, or a dog hitbox that fits one cell.