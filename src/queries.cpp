#include "queries.h"

void queries::query_executor::subscribe(int query_key, std::unique_ptr<query_handler_interface> handler){
    // find the id key
    subscriber_map_[query_key].push_back(std::move(handler));
    return;
}

void queries::query_executor::unsubscribe(int query_key, const int handler){
    // find the query key
    // erase the handler
    auto handlers = subscriber_map_[query_key];
    return;
}

void queries::query_executor::execute_query(const query& query){
    // get the query key
    // go through the subs
    // call query
    auto handlers = subscriber_map_[query.get_type()];
    for(auto & h : handlers){
        h->execute(query);
    }
}