#include "entities.h"

// ------------------------ decorations -----------------------------------// 


void entities::decoration::on_moved_cursor(const events::moved_cursor& event){
    move(event.get_position());
    // and let the hud_element know too
}

void entities::decoration::subscribe_to_cursor(){
    event_interface::subscribe<events::moved_cursor>(moved_cursor_handler);
}
    
void entities::decoration::pick_up(){
    // store the "start position"
    pre_move_position_ = position_;
    subscribe_to_cursor();
    // make the hud element subscribe 
}
void entities::decoration::place_down(){
    // query the grid, can it be placed there
    // TODO create the query and check the answer
    round_position(); // so the decoration fits on a node
    std::unique_ptr<queries::query> can_place_decoration = std::make_unique<queries::can_place_decoration>(hitboxes_[sprites_.index()].get_box(), id_);
    bool can_place = query_interface::execute_query(queries::bool_executor_, *can_place_decoration);
    if(can_place){
        std::cout << "can place " << std::endl;
        unsubscribe_from_cursor();
        
        // update the post move position after it has been rounded
        post_move_position_ = position_;
        auto width = hitboxes_[sprites_.index()].get_box().width;
        auto height = hitboxes_[sprites_.index()].get_box().height;
        
        auto pre_move_rectangle = Rectangle{pre_move_position_.x, pre_move_position_.y, width, height};
        auto post_move_rectangle = Rectangle{post_move_position_.x, post_move_position_.y, width, height};
        
        // create a move_in_graph_event, pass in the two rectangles 
        std::unique_ptr<events::event> move_decoration = std::make_unique<events::moved_decoration>(pre_move_rectangle, post_move_rectangle, id_);
        event_interface::queue_event(move_decoration);
    }
    else{
        std::cout << "can't place " << std::endl;
        // TODO visual indication for the player maybe ? 
    }
}
void entities::decoration::unsubscribe_from_cursor(){
    event_interface::unsubscribe<events::moved_cursor>(moved_cursor_handler);
}

void entities::decoration::round_position(){
    auto rounded_position = Vector2 {
        std::round(position_.x / level_config::edge_weight) * level_config::edge_weight,
        std::round(position_.y / level_config::edge_weight) * level_config::edge_weight
    };
    move(rounded_position);
}
// ------------------------------ builds -------------------------------- //

std::unique_ptr<entities::entity> entities::entity_builder::build_test_decoration(Vector2 position, int id){
    // load the sprite and the hitbox

    auto sprite = sprite::sprite(
        textures::textures_.get_texture(textures::test_decoration, entity_config::test_decoration_path),
        entity_config::test_decoration_attributes[entity_config::attributes::frame_width],
        entity_config::test_decoration_attributes[entity_config::attributes::frame_height],
        entity_config::test_decoration_attributes[entity_config::attributes::frames],
        entity_config::test_decoration_attributes[entity_config::attributes::animations]
    );
    auto hitbox = hitbox::h_builder_.build_test_decoration_hitbox(position);
    std::vector<sprite::sprite> sprites = {sprite};
    std::vector<hitbox::hitbox> hitboxes = {hitbox};

    return std::make_unique<entities::decoration>(sprites, hitboxes, position, id);
}
