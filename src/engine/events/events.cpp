/** implementation file for events namespace */
#include "events.h"
#include <iostream>
// defining the dispatcher
events::event_dispatcher events::global_dispatcher_;


void events::event_dispatcher::subscribe(int event_key, std::unique_ptr<event_handler_interface>& handler_value){
    auto event = subscriber_map_.find(event_key);
    // if the event does not exist yet in subscribes, add it as a key
    if(event == subscriber_map_.end()){
        subscriber_map_.insert({event_key, std::vector<std::unique_ptr<event_handler_interface>>{}});
        subscriber_map_[event_key].push_back(std::move(handler_value));
    }
    // otherwise insert handler, provided there are no duplicates
    else{
        event->second.push_back(std::move(handler_value));
    }
    return;
}
void events::event_dispatcher::unsubscribe(int event_key, size_t handler_id){
    // matched on handler identity - matching on get_type() removed every
    // listener for the event, not just this one
    auto subscribers = subscriber_map_.find(event_key);
    if(subscribers == subscriber_map_.end()){
        return;
    }
    auto & handlers = subscribers->second;
    auto new_end = std::remove_if(handlers.begin(), handlers.end(),
                    [handler_id](auto & h) -> bool {
                        return handler_id == h->get_handler_id();
                    });
    handlers.erase(new_end, handlers.end());
    return;
}
void events::event_dispatcher::execute_event(const event& event){
    //for all handlers of the event, execute the event
    auto key = event.get_type();
    auto subscribers = subscriber_map_.find(key);
    if(subscribers == subscriber_map_.end()){
        return;
    }
    for(auto& handler : subscribers->second){
        handler->execute(event);
    }
    
}

void events::event_dispatcher::queue_event(std::unique_ptr<event>& event){
    // add an event to the queue
    event_queue_.push(std::move(event));
    return;
}

void events::event_dispatcher::add_delayed_event(std::unique_ptr<event>& event){
    delayed_events_.push_back(std::move(event));
    return;
}
void events::event_dispatcher::process_events(float delta){
    // execute events within the queue
    
    // pop is a void remove
    // front is a non-void inspect
    while(not event_queue_.empty()){
        auto&  event = event_queue_.front();
        if(not event->is_handled()){
            execute_event(*event.get());
        }
        event_queue_.pop();
    }
    // then check delayed events
    for(auto it = delayed_events_.begin(); it != delayed_events_.end();){
        // true if there is no longer a delay
        if((*it)->update_delay(delta)){
            auto event = std::move(*it);
            it = delayed_events_.erase(it);
            event_queue_.push(std::move(event));
        }
        else{
            ++it;
        }
    }
    return; 
}
