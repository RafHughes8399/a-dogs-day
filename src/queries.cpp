#include "queries.h"

queries::query_executor queries::global_executor_;
void queries::query_executor::subscribe(int query_key, std::unique_ptr<query_handler_interface> handler){
    // find the id key
    subscriber_map_[query_key] = (std::move(handler));
    return;
}

void queries::query_executor::unsubscribe(int query_key){
    // find the query key
    // erase the handler
    subscriber_map_.erase(query_key);
    return;
}
// this cannot be bool,. this has to be something else 
bool queries::query_executor::execute_query(const query& query){
    return subscriber_map_[query.get_type()]->execute(query);
}