
/** the core of the event system, defines event types, classes for handling events and the primary dispatcher
 * that is responsible for queueing and executing events
 *
 * takes advantage of a obsever-listener pattern, enables classes and components within the codebase to
 * queue events with certain information upon something occuring (like an object moving) that are then executed either
 * immediately or after some specified delay
 *
 * classes can subscribe to certain event types and listen for the execution. All listeners to an event type are
 * notified upon the event executing and react based on their handler
 *
 * Event classes are organised by domain under src/systems/events/ (dog_events,
 * entity_events, decoration_events, input_events, debug_events), with the
 * shared base classes/ids enum/dispatcher in events/event_core.h. This file
 * just aggregates all of them so existing `#include "events.h"` call sites
 * keep working unchanged.
 */

#ifndef EVENTS_H
#define EVENTS_H

#include "events/event_core.h"
#include "events/dog_events.h"
#include "events/entity_events.h"
#include "events/system_events.h"
#include "events/decoration_events.h"
#include "events/input_events.h"
#include "events/debug_events.h"

#endif
