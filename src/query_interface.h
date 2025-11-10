#ifndef QUERY_INTERFACE_H
#define QUERY_INTERFACE_H

#include "queries.h"
namespace query_interface{
    template<typename Q> // e for event
    inline void subscribe(const queries::query_handler<Q>& handler){
        std::unique_ptr<queries::query_handler_interface> h = std::make_unique<queries::query_handler<Q>>(handler);
        queries::global_executor_.subscribe(Q::get_static_type(), std::move(h));
    } 
    
    template<typename Q> // e for event
    inline void unsubscribe(const queries::query_handler<Q>& handler){
        queries::global_executor_.unsubscribe(Q::get_static_type());
    } 
    
    inline bool execute_query(const queries::query& event){
        return queries::global_executor_.execute_query(event);
    }
}
#endif