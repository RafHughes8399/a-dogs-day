/** interface to interact with the events.h */
#ifndef EVENT_INTERFACE_H
#define EVENT_INTERFACE_H
#include "events.h"

// facade pattern
// clean method interface for events to be used throughout the project
namespace event_interface{

    template<typename E> // e for event
    inline void subscribe(const events::event_handler<E>& handler){
        std::unique_ptr<events::event_handler_interface> h = std::make_unique<events::event_handler<E>>(handler);
        events::global_dispatcher_.subscribe(E::get_static_type(), h);
    } 
    
    template<typename E> // e for event
    inline void unsubscribe(const events::event_handler<E>& handler){
        events::global_dispatcher_.unsubscribe(E::get_static_type(), handler.get_type());
    } 
    
    
    inline void execute_event(const events::event& event){
        events::global_dispatcher_.execute_event(event);
    }
    
    inline void queue_event(std::unique_ptr<events::event>& event){
        events::global_dispatcher_.queue_event(event);
    }
    
    inline void add_delayed_event(std::unique_ptr<events::event>& event){
        events::global_dispatcher_.add_delayed_event(event);
    }
}
#endif