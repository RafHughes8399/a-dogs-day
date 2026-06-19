#include "expediter.h"

void expediter::interface::register_waiter(size_t waiter_id){
    expediter::expediter::get_instance().register_waiter(waiter_id);
}

void expediter::interface::register_food_counter(size_t counter_id, Vector2 position){
    expediter::expediter::get_instance().register_food_counter(counter_id, position);
}

void expediter::interface::request_order_service(size_t order_id, size_t table_id, size_t customer_id, Vector2 table_position){
    expediter::expediter::get_instance().request_order_service(order_id, table_id, customer_id, table_position);
}
