#ifndef QUERY_INTERFACE_H
#define QUERY_INTERFACE_H

#include "queries.h"

 namespace query_interface{
    template<typename Q>
    inline void subscribe(queries::query_handler<Q>& handler){
        std::unique_ptr<queries::query_handler_interface> h = std::make_unique<queries::query_handler<Q>>(handler);
        queries::executor_.subscribe(Q::get_static_type(), std::move(h));
    }

    template<typename Q>
    inline void unsubscribe(){
        queries::executor_.unsubscribe(Q::get_static_type());
    }
    inline queries::query_result execute_query(const queries::query& query){
        return queries::executor_.execute_query(query);
    }
}

#endif
