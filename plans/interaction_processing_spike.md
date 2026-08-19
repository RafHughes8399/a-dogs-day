# Spike — processing an interaction once detected

Research only. No implementation, no decision taken. Companion to
[interaction_component_plan.md](plans/interaction_component_plan.md), which settles *detection*
(separate interaction boxes, asymmetric `interactable_component` / `interactor_component`) and
stops at the moment the pair binds.

The open question: **the boxes overlap and the ids are stored — now what runs?**

---

## 0. The question splits three ways

Every system surveyed below separates the same three concerns, and most of the confusion in the
current plan comes from them being collapsed into one system:

| | Question | Who answers it in a shipped game |
|---|---|---|
| **Arbitration** | *which* actor uses *which* object | a planner / job giver / utility scorer — in this repo, `maitre_d` and `expediter` |
| **Arrival** | is the actor now in a position to use it | proximity / navigation completion |
| **Execution** | what happens, over how long, and how it aborts | a sequenced behaviour owned by the agent, the object, or both |

The plan's `interaction_system` proposes to do all three: it scans for *any* nearby interactable
(arbitration), tests overlap (arrival), and emits a fact (a stand-in for execution). The literature
is close to unanimous that these should not be the same pass.

---

## 1. The strongest throughline: **intent precedes geometry**

This is the finding that most directly contradicts the current design, and it recurs in every
production system I looked at.

- **Unreal SmartObjects.** An agent calls `FindSmartObjects` (filtered by radius, gameplay tags,
  activity requirements), then `ClaimSmartObject` on the one it chose — which returns a *claim
  handle* and moves the slot from Free to **Claimed**. Only on arrival does it call `Use`, moving
  the slot Claimed → **Occupied** and receiving the Behavior Definition. `Release` returns it to
  Free. Three states, and the claim happens *before* the walk.
  ([Smart Objects Overview, UE 5.8 docs](https://dev.epicgames.com/documentation/unreal-engine/smart-objects-in-unreal-engine---overview))
- **RimWorld.** A `WorkGiver` decides the abstract work; the `JobDriver` "tries to reserve the
  apparel/weapon so no one else touches it while the Job is in progress" — the reservation is taken
  when the job is *issued*, not when the pawn arrives.
  ([RimWorld Mod Guide: Jobs and Work](https://github.com/roxxploxx/RimWorldModGuide/wiki/SHORTTUTORIAL:-Jobs-and-Work),
  [Example Mending Job](https://rimworldwiki.com/wiki/Modding_Tutorials/Code_MendingJob))
- **Minecraft villagers.** A POI block stores `free_tickets`, decremented "for each entity that
  claims" it. A villager claims a bed or workstation from up to 48 blocks away — "line of sight,
  time of day — doesn't matter" — and stores the coordinates in a `minecraft:job_site` memory
  module. It may be nowhere near the block. Proximity (≤2 blocks horizontally) gates only the
  *use*.
  ([Point of Interest, Minecraft Wiki](https://minecraft.wiki/w/Point_of_Interest),
  [Villagers.md reverse-engineering notes](https://gist.github.com/orlp/db1ca6dbb82727c4a939c95694a52b81))

### What this buys, concretely

The plan invents a **stationary gate** (`movement_manager_[actor].get_paths().empty()`) to stop a
dog binding to a station it merely walks past. That gate is a symptom. If the actor already carries
a declared target — because something decided to send it there — then:

```
for (actor_id, interactor) in interactor_manager_:
    target = interactor.get_target()          # already known, set at decision time
    if not target: continue
    ... overlap test against that one target only
```

- drive-by binding becomes **structurally impossible**, not filtered out;
- the inner `for each interactable` loop disappears, along with the plan's brute-force-vs-quadtree
  discussion (§"Why brute force, and not the quadtree");
- the "dog sent to a plain decoration never binds" test case becomes trivially true;
- `interactor_component::target_` stops meaning "currently bound" and starts meaning "intends to
  use" — which is what `interact_with(id)` already reads like.

The cost: something must set the target. Today `control_input_system` right-click only issues a
path. The literature's answer is that issuing a path *is* the tail of a decision that also names
the target — one command, two effects.

### The three-state slot

The plan's `add_interactor` / `remove_interactor` is a two-state slot (free / occupied), claimed on
arrival. UE, RimWorld and Minecraft all use three states, because two dogs pathing to the same
capacity-1 table will both arrive and one must lose *after* walking there. Claimed-vs-Occupied moves
that failure to decision time:

```
Free ──claim(actor)──> Claimed ──use(actor)──> Occupied ──release──> Free
         ▲                                                    │
         └────────────────── release / abort ─────────────────┘
```

---

## 2. Where does the behaviour live — object or agent?

Two schools, and the interesting result is that everyone ends up somewhere in the middle.

### Object-owned (smart objects / affordances)

The Sims: objects "broadcast all the ways they can change a Sim's need"; the Sim weighs the
advertised utility against its own motives — a well-rested Sim damps the bed's energy score, an
exhausted one boosts it. Will Wright traced the idea to SimAnt pheromones. Crucially the object
also *contains the interaction code*.
([GMTK, The Genius AI Behind The Sims](https://gmtk.substack.com/p/the-genius-ai-behind-the-sims);
[Forbus & Wright, *The Sims Under the Hood*, GDC lecture outline](https://users.cs.northwestern.edu/~forbus/c95-gd/lectures/The_Sims_Under_the_Hood_files/outline.htm))

The Forbus/Wright outline classifies interactions as **Normal / Advertised (autonomous) / Manual
(player-selected) / Pushed (high priority, immediate)** — worth noting because this repo already has
that split latent in it: `maitre_d` pushes, the player manually selects, and nothing is advertised
yet. The outline also states a requirement to be "expandable (code not just data)" with "drop-in
components" — and the system built to satisfy it was **EDITH, a visual programming language**. That
is the true cost of full object-owned behaviour.

Gaslamp Games' Clockwork Empires post-mortem is the honest version of the same idea: one generalised
job ("Return Goods") queries objects, each object returns whether it can satisfy the requirement, and
the object's script "throws stuff over the fence back to the job system" — job parameters *and an FSM
script* for executing with that object. The author reports containers "turned out to be really tough
to get working", with the core difficulty being a single task having multiple valid execution paths.
([Smart Objects — or: "Everything I know about AI I stole from the Sims"](https://archive-gaslamp.dredmor.com/2015/04/15/smart-objects-or-everything-i-know-about-ai-i-stole-from-the-sims/))

### Agent-owned (job drivers)

RimWorld: a `Pawn_JobTracker` picks a Job; the `JobDriver` decomposes it into **Toils** — "a
singular element, or subtask, of a Job" — run one after another until complete or cancelled. The
object is inert data; the pawn owns the sequence and the reservation.
([RimWorld Mod Guide: How Pawns Think](https://github.com/roxxploxx/RimWorldModGuide/wiki/SHORTTUTORIAL:-How-Pawns-Think))

Minecraft is the same shape: the villager `Brain` holds memory modules, activities and behaviors;
the POI block holds only a type and a ticket count.

### The convergence

UE's SmartObjects is explicitly the hybrid: the *definition* is data on the object (slots, tags,
Behavior Definition struct), and the *execution* is an agent-side StateTree or Mass behaviour that
receives that struct. Object says **what**; agent says **how**.

That hybrid maps onto the rule already written into the plan — *"components store; systems act;
orchestrators decide"* — and onto the CLAUDE.md boundary between world state and domain
orchestration. It is the position this repo is already standing in.

---

## 3. In an ECS, execution is a *chain of systems* talking through components

### Intent components

The canonical ECS answer to "system A detected something, system B must react" is a marker component
that exists purely to be queried. In the Rust roguelike book, `WantsToMelee` "doesn't do anything
itself — it triggers intent for other systems"; the AI systems likewise write `WantsToApproach` /
`WantsToFlee` and a downstream system consumes them.
([Roguelike Tutorial in Rust, ch. 57 "Better AI"](https://bfnightly.bracketproductions.com/book/chapter_57.html))

The same pattern shows up under the name **event components** in ECS design threads: a collision
system adds collision components, a handler system processes them; "systems in ECS should communicate
via components rather than calling each other directly".
([GameDev.net: ECS — how should systems communicate?](https://gamedev.net/forums/topic/703917-ecs-implementation-how-should-systems-communicate-with-eachother/),
[GameDev.net: Event handling within an ECS](https://gamedev.net/forums/topic/673910-event-handling-within-an-ecs/),
[Rasooli, *Systems interaction in ECS (events)*](https://medium.com/@ben.rasooli/systems-interaction-in-entity-component-system-events-4a050153c8ac))

Overwatch is the widely-cited production instance of this discipline — "an ECS consists of entities
composed of data components, along with systems that operate on those components" — with the
communication rule being the thing that makes parallelism and determinism tractable.
([Ford, *'Overwatch' Gameplay Architecture and Netcode*, GDC 2017 — GDC Vault, paywalled](https://www.gdcvault.com/play/1024001/-Overwatch-Gameplay-Architecture-and);
[Game Developer write-up](https://www.gamedeveloper.com/design/video-how-i-overwatch-s-i-gameplay-architecture-creates-variety))

The forum threads also flag the one real hazard, and it is exactly the case the plan already
identified: the marker approach breaks down "when an entity needs to be deleted before other systems
can read signals".

### Components vs. this repo's event dispatcher

Nystrom draws the distinction that settles this: a queue is for decoupling **in time**, not in scope.
*"If you only want to decouple who receives a message from its sender, patterns like Observer and
Command will take care of this with less complexity."* He lists the costs explicitly — world state
changes between send and receive ("the entity may have been deallocated"), feedback loops that
silently persist because the stack has been unwound, and global queues behaving as hidden global
variables. And: *"treat simplicity as a precious resource."*
([Nystrom, *Game Programming Patterns* — Event Queue](https://gameprogrammingpatterns.com/event-queue.html))

Applied here: `events::dog_reached_station` is a genuine cross-layer seam (`level`, `maitre_d` and
`expediter` all already listen), so keeping it queued is justified. But an ECS-internal handoff from
`interaction_system` to whatever executes the behaviour — same frame, known order — gets nothing from
the queue and inherits all three costs. That handoff wants to be a component.

### Relationships and dangling ids

Flecs exists partly to solve the exact teardown problem the plan hand-rolls. A relationship is a
`(Relationship, Target)` pair stored as a component id; when the target entity is deleted, "all
references to that id are deleted from the storage". The documented argument against storing raw
entity ids in component fields is twofold: they "lack automatic invalidation, creating stale
pointers", and they are invisible to queries, so graph traversal becomes manual iteration.
([Flecs: Relationships](https://www.flecs.dev/flecs/md_docs_2Relationships.html);
[Mertens, *Building games in ECS with entity relationships*](https://ajmmertens.medium.com/building-games-in-ecs-with-entity-relationships-657275ba2c6c))

This repo will not grow a relationship storage layer. But the argument still bites: the plan stores
the pair **twice** — `interactable_component::interactors_` on the station and
`interactor_component::target_` on the actor — so `on_remove_entity` has two mirrors to keep
consistent, in both directions, and any bug is a silent desync. The cheap mitigation is to nominate
**one** side as authoritative and derive the other, or at minimum funnel every mutation through a
single reconcile point rather than letting call sites touch either side.

Minecraft, notably, *does* keep two mirrors — villager memory and POI tickets — and the
reverse-engineering notes describe the resulting desyncs as a known bug class ("a potential
desynchronization between the villager's memory and actual bed state"), with villagers holding
memories of beds that no longer exist until a distance or arrival check clears them.

---

## 4. Lifecycle: how the enter/exit edges get fired

Unity's XR Interaction Toolkit is the closest published precedent to the plan's asymmetric
interactor/interactable split, and its Interaction Manager update loop is worth copying wholesale:

1. ask Interactors for a valid target list (used for both hover and selection);
2. check whether *existing* hover/selection objects are still valid;
3. clear the invalid ones (`OnSelectExiting` → `OnSelectExited`);
4. *then* query for new valid states and enter them (`OnSelectEntering` → `OnSelectEntered`).

Three interaction states — **Hover, Select, Activate** — and every edge has a paired `-ing` / `-ed`
callback so listeners can observe both before and after the state flips.
([XR Interaction Toolkit — Architecture](https://docs.unity3d.com/Packages/com.unity.xr.interaction.toolkit@2.0/manual/architecture.html),
[XRInteractionManager](https://docs.unity3d.com/Packages/com.unity.xr.interaction.toolkit@2.0/api/UnityEngine.XR.Interaction.Toolkit.XRInteractionManager.html))

Two things the plan's loop does not do:

- **all exits before any enters.** The plan releases and binds per-actor inside one pass, so a table
  freed by dog A late in the iteration is visible to dog B only if B happens to come later in the
  manager's order. Frame-order-dependent, and awkward to test.
- **a hover tier.** "In range but not engaged" is a real state here — it is precisely what the plan
  calls drive-by binding. XRI models it as a first-class state rather than filtering it out.

---

## 5. Sequencing the behaviour itself

Once bound, "sitting at a table" is not an instant — it is a sequence with durations and abort
conditions. Four ways the field does it:

- **Toils** (RimWorld): a flat list of small steps with wait conditions, owned by the JobDriver, run
  to completion or cancelled. The simplest thing that handles duration and abort.
- **StateTree** (UE5): "combines the Selectors from behavior trees with States and Transitions from
  state machines"; each state has Tasks and Transitions, and tasks for all active states run from
  root down. Built explicitly as the execution half of SmartObjects, and now Epic's default over
  Behavior Trees.
  ([Overview of State Tree](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-state-tree-in-unreal-engine),
  [Unreal Fest 2023: State Trees and Smart Objects](https://dev.epicgames.com/community/learning/talks-and-demos/mox7/unreal-engine-state-trees-and-smart-objects-data-driven-state-machine-workflows-for-open-world-ai-designs-unreal-fest-2023))
- **Behavior trees**: re-evaluate from the root every tick; clear hierarchy, composites and leaf
  tasks. The relevant contrast is that BTs re-tick while state trees hold a state — which is why a
  long-running "be at the table for 8 seconds" reads better as a state.
  ([Comparison between Behavior Trees and Finite State Machines, arXiv 2405.16137](https://arxiv.org/pdf/2405.16137))
- **Utility / GOAP**: for *choosing*, not executing. Dave Mark's Infinite Axis Utility System scores
  every candidate action; GOAP is "A* pathfinding, except the destination is your desired world
  state". The Sims' advertised affordances are the object-broadcast flavour of utility.
  ([Utility system, Wikipedia](https://en.wikipedia.org/wiki/Utility_system),
  [Golden Syrup Games, GOAP and Utility AI](https://goldensyrupgames.com/blog/2024-05-04-grab-n-throw-utility-goap-ai/))

For this repo the last bullet is a non-need: `maitre_d` and `expediter` *are* the arbitration layer,
and adding utility scoring would duplicate them. The first bullet is the relevant one, and this
codebase is already halfway there — the state pattern is used pervasively (`customer_dog_state`,
`waiter_dog_state`, `station_state`, `cursor::state`), and there is an empty `state_machine_component`
stub at [component.h:287](src/engine/components/component.h:287) waiting for exactly this.

Note also the genre match: this is a management sim, same shape as RimWorld, where the content *is*
multi-step tasks (waiter: counter → pick up food → table → serve → return). That argues for the
agent-owned toil sequence rather than the object-owned script as the eventual form.

---

## 6. Options, mapped onto this codebase

| | Option | Precedent | Fit |
|---|---|---|---|
| **A** | `interaction_system` binds, emits `dog_reached_station`, legacy `level`/`maitre_d`/`expediter` do the rest | the plan as written; the repo's own legacy path | smallest step; the ECS learns nothing; behaviour stays outside it |
| **B** | Bind writes a one-frame `interaction_started` / `interaction_ended` marker component; downstream ECS systems query it. Fact event kept for the legacy seam | `WantsToMelee`; ECS event components; Overwatch | idiomatic; testable by inspecting components rather than trapping events; needs a one-frame clear pass |
| **C** | `interactable_component` carries a behaviour id / tag; the system looks it up and dispatches | UE Behavior Definition; The Sims; Clockwork Empires | data-driven, but needs a registry — and both cited precedents report high cost |
| **D** | Actor carries a job/toil sequence; "use station X" is one step; `state_machine_component` becomes the driver | RimWorld JobDriver; Minecraft Brain | best long-run fit for a management sim; largest step; subsumes B |
| **E** | Spawn a short-lived *interaction entity* holding (actor, target, verb, elapsed) | occasional ECS practice; nearest published cousin is UE's claim handle | makes the interaction inspectable and gives duration a natural home; adds an entity lifetime to manage |

---

## 7. What I would take back to the plan

Three findings that change the current design, in order of how much they change it:

1. **Make the target an input to detection, not an output of it** (§1). It deletes the stationary
   gate, the inner loop over all interactables, and the quadtree question, and makes drive-by binding
   impossible rather than filtered. It requires `control_input_system` / the orchestrators to name a
   target when they issue a path.
2. **Split claim from occupy** (§1). Two-state slots race when two dogs path to one capacity-1 table.
   Free → Claimed → Occupied → Free is what UE, RimWorld and Minecraft all landed on independently.
3. **Order the update as all-exits-then-all-enters** (§4), and consider modelling "in range, not
   engaged" as a hover tier rather than a case to suppress.

Two more that are cheap insurance:

4. Nominate one side of the pair as authoritative, or funnel both through one reconcile point, so
   `on_remove_entity` has one mirror to fix rather than two (§3).
5. Keep `dog_reached_station` queued — it is a genuine cross-layer seam — but do **not** route the
   ECS-internal handoff through the dispatcher; use a component (§3, Nystrom).

Deliberately unresolved: whether execution ends up agent-owned toils (D) or object-supplied
definitions (C). The literature says "both, with the object supplying data only", but nothing in this
repo needs it until a station actually reacts to arrival — which the plan already lists under
Deferred.

---

## Sources

Production architecture
- [Smart Objects in Unreal Engine — Overview](https://dev.epicgames.com/documentation/unreal-engine/smart-objects-in-unreal-engine---overview) and [Quick Start](https://dev.epicgames.com/documentation/en-us/unreal-engine/smart-objects-in-unreal-engine---quick-start)
- [unreal.SmartObjectSubsystem API](https://docs.unrealengine.com/5.0/en-US/PythonAPI/class/SmartObjectSubsystem.html)
- [Overview of State Tree in Unreal Engine](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-state-tree-in-unreal-engine)
- [Unreal Fest 2023 — State Trees and Smart Objects: Data-Driven State Machine Workflows for Open World AI Designs](https://dev.epicgames.com/community/learning/talks-and-demos/mox7/unreal-engine-state-trees-and-smart-objects-data-driven-state-machine-workflows-for-open-world-ai-designs-unreal-fest-2023)
- [Unity XR Interaction Toolkit — Architecture](https://docs.unity3d.com/Packages/com.unity.xr.interaction.toolkit@2.0/manual/architecture.html), [XRInteractionManager API](https://docs.unity3d.com/Packages/com.unity.xr.interaction.toolkit@2.0/api/UnityEngine.XR.Interaction.Toolkit.XRInteractionManager.html), [Unity Learn: Interactors and Interactables](https://learn.unity.com/tutorial/using-interactors-and-interactables-with-the-xr-interaction-toolkit)

Management-sim job / reservation systems
- [RimWorld Mod Guide — How Pawns Think](https://github.com/roxxploxx/RimWorldModGuide/wiki/SHORTTUTORIAL:-How-Pawns-Think) and [Jobs and Work](https://github.com/roxxploxx/RimWorldModGuide/wiki/SHORTTUTORIAL:-Jobs-and-Work)
- [RimWorld Wiki — Example Mending Job](https://rimworldwiki.com/wiki/Modding_Tutorials/Code_MendingJob)
- [Minecraft Wiki — Point of Interest](https://minecraft.wiki/w/Point_of_Interest) and [Point of Interest format](https://minecraft.wiki/w/Point_of_Interest_format)
- [orlp — Villagers.md (1.14 villager mechanics, reverse-engineered)](https://gist.github.com/orlp/db1ca6dbb82727c4a939c95694a52b81)

Smart objects / affordances
- [Forbus & Wright — *The Sims Under the Hood*, GDC lecture outline (Northwestern CS)](https://users.cs.northwestern.edu/~forbus/c95-gd/lectures/The_Sims_Under_the_Hood_files/outline.htm)
- [Mark Brown (GMTK) — The Genius AI Behind The Sims](https://gmtk.substack.com/p/the-genius-ai-behind-the-sims)
- [Gaslamp Games — Smart Objects, or: "Everything I know about AI I stole from the Sims"](https://archive-gaslamp.dredmor.com/2015/04/15/smart-objects-or-everything-i-know-about-ai-i-stole-from-the-sims/)
- [Tirrell — *Dumb People, Smart Objects: The Sims and the Distributed Self* (Philosophy of Computer Games 2012)](https://www.gamephilosophy.org/wp-content/uploads/confmanuscripts/pcg2012/Tirrell%202012%20-Dumb-People-Smart-Objects-The-Sims-and-the-Distributed-Self.pdf)

ECS communication
- [Bracket Productions — Roguelike Tutorial in Rust, ch. 57 "Better AI"](https://bfnightly.bracketproductions.com/book/chapter_57.html) and [ch. 14 "Equipment"](https://bfnightly.bracketproductions.com/chapter_14.html)
- [Flecs — Relationships manual](https://www.flecs.dev/flecs/md_docs_2Relationships.html) and [Designing with Flecs](https://www.flecs.dev/flecs/md_docs_2DesignWithFlecs.html)
- [Sander Mertens — Building games in ECS with entity relationships](https://ajmmertens.medium.com/building-games-in-ecs-with-entity-relationships-657275ba2c6c)
- [GameDev.net — ECS: how should systems communicate?](https://gamedev.net/forums/topic/703917-ecs-implementation-how-should-systems-communicate-with-eachother/) and [Event handling within an ECS](https://gamedev.net/forums/topic/673910-event-handling-within-an-ecs/)
- [Behnam Rasooli — Systems interaction in ECS (events)](https://medium.com/@ben.rasooli/systems-interaction-in-entity-component-system-events-4a050153c8ac)
- [Timothy Ford — *'Overwatch' Gameplay Architecture and Netcode*, GDC 2017 (GDC Vault, paywalled)](https://www.gdcvault.com/play/1024001/-Overwatch-Gameplay-Architecture-and); open write-up: [Game Developer](https://www.gamedeveloper.com/design/video-how-i-overwatch-s-i-gameplay-architecture-creates-variety)

Patterns / decision-making
- [Robert Nystrom — *Game Programming Patterns*, Event Queue](https://gameprogrammingpatterns.com/event-queue.html)
- [Comparison between Behavior Trees and Finite State Machines (arXiv 2405.16137)](https://arxiv.org/pdf/2405.16137)
- [Utility system — Wikipedia (Dave Mark, IAUS, *Behavioral Mathematics for Game AI*)](https://en.wikipedia.org/wiki/Utility_system)
- [Golden Syrup Games — GOAP and Utility AI in Grab n' Throw](https://goldensyrupgames.com/blog/2024-05-04-grab-n-throw-utility-goap-ai/)
