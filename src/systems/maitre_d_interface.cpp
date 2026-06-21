#include "maitre_d.h"

#include <cmath>

void maitre_d::interface::register_table(size_t table_id){
    maitre_d::maitre_d::get_instance().register_table(table_id);
}

void maitre_d::interface::register_customer(size_t customer_id){
    maitre_d::maitre_d::get_instance().register_customer(customer_id);
}

void maitre_d::interface::request_table_for_customer(size_t customer_id){
    maitre_d::maitre_d::get_instance().request_table_for_customer(customer_id);
}

void maitre_d::interface::configure_customer_queue_layout(){
    cafe_config::dog_queue_width = cafe_config::dog_queue_width_edges * level_config::edge_weight;
    cafe_config::dog_queue_height = static_cast<float>(GetScreenHeight());
    cafe_config::dog_queue_start = Vector2{
        cafe_config::dog_queue_column_edges * level_config::edge_weight,
        level_config::edge_weight * cafe_config::dog_queue_vertical_buffer_edges
    };
    cafe_config::dog_queue_available_height = cafe_config::dog_queue_height
        - (2.0f * cafe_config::dog_queue_vertical_buffer_edges * level_config::edge_weight);
    cafe_config::dog_queue_capacity = cafe_config::dog_queue_available_height <= 0.0f
        ? 0
        : static_cast<size_t>(std::floor(
            cafe_config::dog_queue_available_height / (cafe_config::dog_queue_spacing_edges * level_config::edge_weight)));
    cafe_config::dog_queue_debug_bounds = Rectangle{
        0.0f,
        0.0f,
        cafe_config::dog_queue_width,
        cafe_config::dog_queue_height
    };
}
