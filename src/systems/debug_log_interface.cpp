#include "debug_log_interface.h"

#include "events.h"
#include "events_interface.h"

void debug::log(std::string message){
    auto log_event = events::debug_log(std::move(message));
    event_interface::execute_event(log_event);
}
