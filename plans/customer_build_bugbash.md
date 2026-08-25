# Customer build + footpath pathing — bug bash

## Context

Review of the `ecs_customer_arrival` branch work that builds customer dogs into the world
and walks them along the footpath. Scope is the dog factory and everything it touches:

- `dog_factory` — marble bag, spawn/destination pair, path event
  ([dog_factory.hpp](src/entities/factories/dog_factory.hpp), [dog_factory.cpp](src/entities/factories/dog_factory.cpp))
- The spawn tick in `entity_lifespan_system`
  ([entity_lifespan_system.cpp:26-39](src/engine/systems/entity_lifespan_system.cpp:26))
- The two-graph split (footpath / cafe) in `movement_system`
  ([system.h:276](src/engine/systems/system.h:276), [movement_system.cpp:190-196](src/engine/systems/movement_system.cpp:190))
- The uncommitted `build_player_dog` → `build_dog` rename in
  [ecs_builders.cpp](src/entities/ecs_builders.cpp) and [entity.h](src/entities/entity.h)

**Verdict:** the shape is right — marble bag, spawn/destination pair, path event on build —
but it does not currently work. Three blockers, two logic bugs, four design concerns.

**Status at time of review:** build red, a spawned customer would never move, and would crash
inside the first minute if it did.

---

## Method

Two throwaway probes, both since deleted:

1. **Geometry** — constructed the real `graph::level_graph` footpath and cafe from
   `level_config`, linked against `graph.lib`, and asked them directly for
   `position_in_area` / `find_path` on the factory's four hardcoded positions.
2. **Marble bag** — copied `refresh_dogs` and `pick_dog` verbatim into a standalone TU and
   drew 22 marbles, reporting bag size and builder occupancy per draw.

Output from both is quoted inline below.

---

## Blockers

### 1. Build is broken — `build_customer_dog` declaration replaced, not added

[entity.h:100](src/entities/entity.h:100) (uncommitted) changed the declaration from 2-param
to 5-param:

```diff
-    void build_customer_dog(size_t id, Vector2 position);
+    void build_customer_dog(size_t id, Vector2 position,
+    std::vector<sprite::sprite> sprites, size_t kind, float reach);
```

But [ecs_builders.cpp:94](src/entities/ecs_builders.cpp:94) still defines the 2-param version:

```
ecs_builders.cpp:94  error: out-of-line definition of 'build_customer_dog'
                     does not match any declaration in namespace 'ecs_entities'
```

[ecs_test_game.cpp:72](tests/ecs_test_game.cpp:72) also calls the 2-param form, so the tests
break the same way. The declaration needs to be an **overload set**, not a replacement.

### 2. The new 5-param `build_customer_dog` is a byte-identical copy of `build_dog`

[ecs_builders.cpp:51-70](src/entities/ecs_builders.cpp:51) is line-for-line the same body as
[build_dog at :30-49](src/entities/ecs_builders.cpp:30) — same components, same order, same
arguments. Nothing calls it. It is dead code, and it is the thing that forced the declaration
change in blocker 1. Deleting it fixes both.

### 3. The customer is given a destination outside every graph — it never gets a path

`pick_destination()` hard-returns `destination_positions_[0]`
([dog_factory.cpp:10](src/entities/factories/dog_factory.cpp:10)). Built the two real
`level_graph`s from `level_config` and asked them directly:

```
footpath  x[    0.0,  320.0)  y[ -192.0, 2048.0)
cafe      x[  256.0, 3072.0)  y[    0.0, 2048.0)

spawn_[0]          (   64.0, -128.0)  footpath=yes cafe=NO
spawn_[1]          (  192.0, 2176.0)  footpath=NO  cafe=NO   <-- IN NEITHER GRAPH
destination_[0]    (   64.0, 2176.0)  footpath=NO  cafe=NO   <-- IN NEITHER GRAPH
destination_[1]    (  192.0, -128.0)  footpath=yes cafe=NO

find_path(spawn_[0] -> destination_[0]) on footpath: 0 nodes <-- NO PATH
find_path(spawn_[0] -> destination_[1]) on footpath: 3 nodes
```

**Root cause:** both `y = 2176` positions come from `(cafe_y + cafe_height) + 2 * edge_weight`
([dog_factory.hpp:28,30](src/entities/factories/dog_factory.hpp:28)). That assumes the footpath
overhangs the world at the bottom the way it does at the top — but
[config.h:92-100](src/engine/config.h:92) only extends it upward:

```cpp
graph_y         = -3 * edge_weight;              // -192, overhang above
footpath_height = graph_height;                  //  2240
// footpath bottom = -192 + 2240 = 2048          //  no overhang below
```

This is **not** fixed by swapping to `destination_[1]` — that is `y = -128`, the *same* end the
dog spawns at. A dog walking the footpath top-to-bottom needs the footpath to actually extend
past `y = 2048`.

**Second-order:** `resolve_graph` falls back to `footpath_` for a position in neither graph
([movement_system.cpp:193](src/engine/systems/movement_system.cpp:193)). So the out-of-bounds
destination resolves to footpath, passes the `source_graph != destination_graph` guard (both
footpath), and then fails silently inside `find_path`. An off-world position is indistinguishable
from a footpath position — worth making that case explicit rather than a fallback.

---

## Logic bugs

### 4. `garfield` is in the bag but has no builder → `std::bad_function_call`

`builders_` is sized `cumulative_customers_size` (= 2) but the initializer supplies one lambda
([dog_factory.hpp:24-26](src/entities/factories/dog_factory.hpp:24)), so `builders_[garfield]`
is an empty `std::function`. `refresh_dogs` puts exactly one garfield marble in every bag of 7
([dog_factory.cpp:30](src/entities/factories/dog_factory.cpp:30)). Bag logic run verbatim:

```
pick  dog      bag size  note
0     tex      7
1     garfield 7         <-- builders_[garfield] EMPTY -> bad_function_call
```

At `customer_spawn_interval = 8.0f` ([config.h:344](src/engine/config.h:344)) that is a crash
inside the first minute.

### 5. `refresh_dogs` never clears the bag

Same run, continued:

```
6     tex      14        <-- refresh: bag GREW, old marbles kept
19    tex      21        <-- refresh: bag GREW, old marbles kept
```

`dogs_` only ever grows ([dog_factory.cpp:24-34](src/entities/factories/dog_factory.cpp:24)).
Two consequences beyond the memory: the 6:1 ratio is only honoured for the *first* bag, and
already-drawn marbles get reshuffled back in — so it stops being a marble bag at all.

Related, same function: `if(index_ == dogs_.size() - 1)`
([dog_factory.cpp:15](src/entities/factories/dog_factory.cpp:15)) refreshes when `index_` points
*at* the last element, so that marble is never drawn — should be `dogs_.size()`. It also compares
`int` to `size_t`; if `dogs_` were ever empty, `size() - 1` wraps to `SIZE_MAX`, the guard never
fires, and `dogs_[0]` is UB. Not reachable today since the constructor fills it, but the guard is
the wrong shape.

---

## Design

### 6. `create_customer_dog` bypasses the `create()` template

[system.h:137](src/engine/systems/system.h:137) documents `create()` as "allocate → build →
announce, so nothing can be built without reaching the spatial index and a render layer".
[entity_lifespan_system.cpp:33-39](src/engine/systems/entity_lifespan_system.cpp:33) hand-rolls
those same three steps. Correct today, but it is a second copy of the invariant the template
exists to enforce — this is the one place a future edit can drop the announce and produce an
entity in no graph.

### 7. The factory queues an event

[dog_factory.cpp:50](src/entities/factories/dog_factory.cpp:50) queues `create_path_to` inside
`build_customer_dog`, so the factory both assembles components and issues a movement command.
It works because the event is *queued* while `create_entity` is executed immediately after — but
that ordering is implicit and undocumented. Per the architecture split in `CLAUDE.md`, "walk to
the footpath exit" reads like orchestration; the empty `customer_arrival_system` scaffold at
[system.h:246](src/engine/systems/system.h:246) looks like the intended home.

### 8. `world.hpp` is dead and duplicates `movement_system`

[world.hpp](src/engine/structures/world.hpp) declares `class world` holding `footpath_` + `cafe_`
— the exact pair `movement_system` now holds as two members
([system.h:276-277](src/engine/systems/system.h:276)). Nothing includes it, and it is not in
`CMakeLists.txt`. Either it is the intended home for the two graphs or it should go; having both
is the kind of thing that gets edited in the wrong place later.

### 9. Minor

- `time_since_dog_` ([system.h:159](src/engine/systems/system.h:159)) has no initializer. Not a
  live bug — the singleton is a function-local static, so it is zero-initialised before the
  constructor runs — but it is correct only by accident of storage duration.
- `build_tex` passes `sprites` by copy ([ecs_builders.cpp:119](src/entities/ecs_builders.cpp:119));
  `build_mack` / `build_khiri` both `std::move`.
- `pick_spawn` / `pick_destination` are independent, so once the TODOs are filled in nothing stops
  a dog spawning and terminating at the same end of the footpath.

---

## Suggested order

| # | Fix | Unblocks |
|---|-----|----------|
| 1 | Delete the duplicate 5-param `build_customer_dog`, restore the 2-param declaration | build goes green |
| 2 | Give `garfield` a builder, or drop it from `refresh_dogs` until it has one | no crash on spawn |
| 3 | `dogs_.clear()` at the top of `refresh_dogs`; change guard to `>= dogs_.size()` | bag behaves as a bag |
| 4 | Extend the footpath below `y = 2048`, or derive the spawn/destination pair from `footpath_`'s actual area instead of re-deriving from `cafe_y + cafe_height` | dog actually walks |

Items 6–9 are cleanup and can follow once a customer visibly walks the footpath end to end.
