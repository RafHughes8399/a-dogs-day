#include "maitre_d.h"

void maitre_d::interface::register_table(size_t table_id, Vector2 position){
    maitre_d::maitre_d::get_instance().register_table(table_id, position);
}

void maitre_d::interface::register_customer(size_t customer_id){
    maitre_d::maitre_d::get_instance().register_customer(customer_id);
}

void maitre_d::interface::request_table_for_customer(size_t customer_id){
    maitre_d::maitre_d::get_instance().request_table_for_customer(customer_id);
}
