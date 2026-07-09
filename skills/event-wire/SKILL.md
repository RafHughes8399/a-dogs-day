---
name: event-wire
description: Create or modify a C++ event wire using this codebase's existing event system. Use when the user describes a trigger scenario that should emit an event, queue or execute it, and have one or more listeners subscribe, unsubscribe, and handle the typed event payload.
---

# Event Wire

## Overview

Turn a natural-language trigger description into the repo's existing event pattern: event class, emitter, queued event pointer, listener subscription lifecycle, and typed handler.

## Workflow

1. Parse the request into four facts before editing:
   - Trigger: the exact scenario that should emit the event.
   - Event payload: the data the event must carry.
   - Emitter owner: the object or system that knows the trigger happened.
   - Listener owner: the object or system that should react.

2. Inspect the codebase before deciding the shape. Prefer these anchors:
   - `src/systems/events.h` for `events::ids`, event classes, payload getters, and `events::event_handler`.
   - `src/systems/events_interface.h` for `event_interface::subscribe`, `unsubscribe`, `queue_event`, and `execute_event`.
   - Nearby emitter and listener examples in the same domain as the requested behavior.

3. If any of these are unclear, stop and ask a focused question before editing:
   - Which owner should emit the event.
   - Which owner should listen.
   - Whether the event is a fact that happened or a command requesting work.
   - Which payload fields are required.
   - Whether the event should be queued, delayed, or executed immediately.

4. Implement the smallest matching event wire:
   - Add a new `events::ids` value and update `ids::size` if needed.
   - Add an event class deriving from `events::event`.
   - Store payload fields as private immutable members when practical.
   - Add public getters for payload fields.
   - Add `static int get_static_type()` returning the matching id.
   - In the emitter, create `std::unique_ptr<events::event>` with `std::make_unique<events::<event_name>>(...)`.
   - Queue it with `event_interface::queue_event(event_ptr)` unless the verified local pattern requires immediate execution or delay.
   - In each listener, add an `events::event_handler<events::<event_name>>` member.
   - Initialize that member with a lambda that calls a typed handler.
   - Subscribe with `event_interface::subscribe<events::<event_name>>(handler_)`.
   - Unsubscribe with `event_interface::unsubscribe<events::<event_name>>(handler_)`.
   - Define the handler as `on_<event_name>_event(const events::<event_name>& event)` unless the local class already uses a different naming pattern.

5. Verify build impact with the narrowest useful check available for the touched files.

## Guardrails

- Do not invent event payload fields. Use only fields stated by the user or proven necessary from code.
- Do not invent emitter or listener ownership. Infer ownership only when the current code makes it clear.
- Match existing naming, constructor, getter, subscribe, and unsubscribe style before adding new style.
- Prefer a typed handler like `on_send_customer_to_queue_event(const events::send_customer_to_queue& event)`. Use generic `on_event(const events::event&)` only when the target class already uses that strategy pattern.
- Keep event classes as data carriers. Put behavior in the emitter or listener.
- Preserve lifecycle symmetry. Any new subscription should have a matching unsubscribe path for that owner.
- Keep changes scoped to the event class, emitter, listener, and build files only when required.
- If the requested behavior is really a synchronous answer, use the query system instead of the event system.

## Repo Examples

- Event class and payload shape: `events::send_customer_to_queue` in `src/systems/events.h`.
- Event facade: `event_interface::queue_event` in `src/systems/events_interface.h`.
- Emitter example: `maitre_d::send_dog_to_table` queues `events::send_customer_to_queue`.
- Listener example: `level` owns `send_customer_to_queue_handler_`, subscribes and unsubscribes it, then handles `on_send_customer_to_queue_event`.
