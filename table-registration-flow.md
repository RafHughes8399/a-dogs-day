# Table Registration Flow

## Goal

Let the `maitre_d` know which table entities exist and where they are, so it can assign queued customer dogs to real table positions.

## Current Problem

Customer dogs can now reach the queue head, but `maitre_d::assign_tables()` exits early because `tables_` is empty. `maitre_d::register_table()` is currently a stub, and the level-created table entities are not wired into the cafe orchestration layer.

## Simple First Slice

Wire table creation only:

1. When the level adds a table entity, emit `events::registered_table`.
2. Update `events::registered_table` to carry:
   - `table_id`
   - `position`
3. Update `maitre_d::register_table(table_id, position)` to upsert a `table_record`.
4. Store new records as available:
   - `table_id`
   - `position`
   - `is_free = true`
   - `customer_id = empty_id`

## Later Slices

Table movement:

1. Emit a table-specific move/update event when a table moves.
2. Have `maitre_d` update the matching `table_record.position`.
3. If no matching record exists, create one.

Table removal:

1. Add a table removal event once level removal is properly defined.
2. Have `maitre_d` erase the matching table record.

## Notes

Avoid listening directly to generic decoration movement unless the event can be safely identified as a table. A table-specific event keeps `maitre_d` independent from general decoration behavior.
