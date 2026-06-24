#include "config.h"
#include "entities.h"
#include "hitbox.h"
#include "texture.h"
#include "queries.h"
#include "query_interface.h"
#include <iostream>
// ------------------------ decorations -----------------------------------// 


void entities::decoration::on_moved_cursor(const events::moved_cursor& event){
    move(event.get_position());
    // and let the hud_element know too
    return;
}

void entities::decoration::subscribe_to_cursor(){
    event_interface::subscribe<events::moved_cursor>(moved_cursor_handler);
}
    
void entities::decoration::pick_up(){
    // store the "start position"
    std::cout << "[decoration pick up] " << get_debug_id() << " subscribe to cursor" << std::endl;
    pre_move_position_ = position_;
    subscribe_to_cursor();
    // make the hud element subscribe 
}
bool entities::decoration::can_place_down(){
    Vector2 rounded_position = round_position();
    Rectangle box = body_.get_hitbox().get_box();

    
    Vector2 rounded_position_difference = Vector2Subtract(rounded_position, position_);
    box.x += rounded_position_difference.x;
    box.y += rounded_position_difference.y;



    std::unique_ptr<queries::query> can_place_decoration = std::make_unique<queries::can_place_decoration>(box, id_);
    return query_interface::execute_query(queries::bool_executor_, *can_place_decoration);
}
void entities::decoration::place_down(){
    Vector2 rounded_position = round_position();

    move(rounded_position);
    unsubscribe_from_cursor();
        
    // update the post move position after it has been rounded
    post_move_position_ = position_;
    auto width = body_.get_hitbox().get_box().width;
    auto height = body_.get_hitbox().get_box().height;
        
    auto pre_move_rectangle = Rectangle{pre_move_position_.x, pre_move_position_.y, width, height};
    auto post_move_rectangle = Rectangle{post_move_position_.x, post_move_position_.y, width, height};
        
    // create a move_in_graph_event, pass in the two rectangles 
    std::unique_ptr<events::event> move_decoration = std::make_unique<events::moved_decoration>(pre_move_rectangle, post_move_rectangle, id_);
    event_interface::queue_event(move_decoration);
}
void entities::decoration::unsubscribe_from_cursor(){
    event_interface::unsubscribe<events::moved_cursor>(moved_cursor_handler);
}

Vector2 entities::decoration::round_position(){
    auto rounded_position = Vector2 {
        std::round(position_.x / level_config::edge_weight) * level_config::edge_weight,
        std::round(position_.y / level_config::edge_weight) * level_config::edge_weight
    };
    //move(rounded_position);
    return rounded_position;
}

// ------------------------ stations -----------------------------------// 
entities::station::station_type entities::station::get_station_type(){
    return type_;
}

void entities::station::interact(entity& other){
    (void) other;
    return;
}

// ------------------------ tables -----------------------------------// 
bool entities::table::can_accept_dog(){
    return state_ == table_state::available;
}

bool entities::table::reserve_for(int dog_id){
    if(! can_accept_dog()){
        return false;
    }

    state_ = table_state::reserved;
    assigned_dog_id_ = dog_id;
    return true;
}

void entities::table::occupy(){
    state_ = table_state::occupied;
}

void entities::table::clear(){
    state_ = table_state::available;
    assigned_dog_id_ = level_config::empty_node;
}

entities::table::table_state entities::table::get_state(){
    return state_;
}

int entities::table::get_assigned_dog_id(){
    return assigned_dog_id_;
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
    auto body = body::body(hitboxes, sprites);

    return std::make_unique<entities::decoration>(body, position, id, next_debug_id("dec_"));
}

std::unique_ptr<entities::entity> entities::entity_builder::build_gargoyle(Vector2 position, int id){
    auto gargoyle_void = sprite::sprite(
        textures::textures_.get_texture(textures::gargoyle_void, entity_config::gargoyle_void_decoration_path),
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_width],
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_height],
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::frames],
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::animations]
    );
    auto gargoyle_sick_of_it = sprite::sprite(
        textures::textures_.get_texture(textures::gargoyle_sick_of_it, entity_config::gargoyle_void_decoration_path),
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_width],
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_height],
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::frames],
        entity_config::gargoyle_decoration_attributes[entity_config::attributes::animations]
    );

    auto hitbox = hitbox::h_builder_.build_gargoyle_hitbox(position);

    std::vector<sprite::sprite> sprites = {gargoyle_void, gargoyle_sick_of_it};
    std::vector<hitbox::hitbox> hitboxes = {hitbox, hitbox};
    auto body = body::body(hitboxes, sprites);

    return std::make_unique<entities::decoration>(body, position, id, next_debug_id("dec_"));
}

std::unique_ptr<entities::entity> entities::entity_builder::build_table(Vector2 position, int id){
    auto table_sprite = sprite::sprite(
        textures::textures_.get_texture(textures::test_decoration, entity_config::test_decoration_path),
        entity_config::table_attributes[entity_config::attributes::frame_width],
        entity_config::table_attributes[entity_config::attributes::frame_height],
        entity_config::table_attributes[entity_config::attributes::frames],
        entity_config::table_attributes[entity_config::attributes::animations]
    );

    auto hitbox = hitbox::h_builder_.build_table_hitbox(position);
    std::vector<sprite::sprite> sprites = {table_sprite};
    std::vector<hitbox::hitbox> hitboxes = {hitbox};
    auto body = body::body(hitboxes, sprites);

    return std::make_unique<entities::table>(body, position, id, next_debug_id("table_"));
}

Vector2 entities::table::get_interaction_position() const{
    auto x = position_.x - level_config::edge_weight;
    int y_edges_floor = std::floor(position_.y / level_config::edge_weight);
    int y = position_.y + (level_config::edge_weight * y_edges_floor);
    return Vector2{x, static_cast<float>(y)};
}
