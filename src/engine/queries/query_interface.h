#ifndef QUERY_INTERFACE_H
#define QUERY_INTERFACE_H

#include "queries.h"

 namespace query_interface{
    template<typename Q, typename T> // q for query, T for type
    inline void subscribe(queries::query_executor<T>& executor, queries::query_handler<Q, T>& handler){
        std::unique_ptr<queries::query_handler_interface<T>> h = std::make_unique<queries::query_handler<Q, T>>(handler);
        executor.subscribe(Q::get_static_type(), std::move(h));
    } 
    
    template<typename Q, typename T> // q for query
    inline void unsubscribe(queries::query_executor<T>& executor, const queries::query_handler<Q, T>& handler){
        (void) handler;
        executor.unsubscribe(Q::get_static_type());
    } 
    template<typename T>
    inline T execute_query(queries::query_executor<T>& executor, const queries::query& query){
        return executor.execute_query(query);
    }
}

#endif
