#include "config.h"
#include "entities.h"
#include "hitbox.h"
#include "texture.h"
#include "queries.h"
#include "query_interface.h"
#include <cassert>
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

    return std::make_unique<entities::decoration>(body, position, id, next_debug_id(entity_config::decoration_debug_id_prefix));
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

    return std::make_unique<entities::decoration>(body, position, id, next_debug_id(entity_config::decoration_debug_id_prefix));
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

    return std::make_unique<entities::table>(body, position, id, next_debug_id(entity_config::table_debug_id_prefix));
}

std::unique_ptr<entities::entity> entities::entity_builder::build_food_counter(Vector2 position, int id){
    auto food_counter_sprite = sprite::sprite(
        textures::textures_.get_texture(textures::test_decoration, entity_config::test_decoration_path),
        entity_config::food_counter_attributes[entity_config::attributes::frame_width],
        entity_config::food_counter_attributes[entity_config::attributes::frame_height],
        entity_config::food_counter_attributes[entity_config::attributes::frames],
        entity_config::food_counter_attributes[entity_config::attributes::animations],
        Vector2Zero(),
        BLUE
    );

    auto hitbox = hitbox::h_builder_.build_food_counter_hitbox(position);
    std::vector<sprite::sprite> sprites = {food_counter_sprite};
    std::vector<hitbox::hitbox> hitboxes = {hitbox};
    auto body = body::body(hitboxes, sprites);

    return std::make_unique<entities::food_counter>(body, position, id, next_debug_id(entity_config::food_counter_debug_id_prefix));
}

std::unique_ptr<entities::food> entities::entity_builder::build_test_food(Vector2 position, int id){
    auto food_sprite = sprite::sprite(
        textures::textures_.get_texture(textures::test_decoration, entity_config::test_decoration_path),
        entity_config::test_food_attributes[entity_config::attributes::frame_width],
        entity_config::test_food_attributes[entity_config::attributes::frame_height],
        entity_config::test_food_attributes[entity_config::attributes::frames],
        entity_config::test_food_attributes[entity_config::attributes::animations],
        Vector2Zero(),
        RED
    );

    auto hitbox = hitbox::h_builder_.build_food_hitbox(position);
    std::vector<sprite::sprite> sprites = {food_sprite};
    std::vector<hitbox::hitbox> hitboxes = {hitbox};
    auto body = body::body(hitboxes, sprites);

    return std::make_unique<entities::food>(body, position, id, next_debug_id(entity_config::food_debug_id_prefix));
}

// ------------------------ food counter storage -----------------------------------//
bool entities::food_counter::store(std::unique_ptr<food> item){
    if(stored_food_.size() >= max_capacity_){
        return false;
    }
    stored_food_.push_back(std::move(item));
    return true;
}

std::unique_ptr<entities::food> entities::food_counter::take(){
    // precondition: the counter is not empty (callers guard with !is_empty()).
    assert(!stored_food_.empty() && "food_counter::take() called on an empty counter");
    auto item = std::move(stored_food_.back());
    stored_food_.pop_back();
    return item;
}

bool entities::food_counter::is_empty() const{
    return stored_food_.empty();
}

size_t entities::food_counter::current_capacity() const{
    return stored_food_.size();
}

size_t entities::food_counter::max_capacity() const{
    return max_capacity_;
}

entities::food_counter::counter_status entities::food_counter::status() const{
    if(stored_food_.empty()){
        return counter_status::empty;
    }
    if(stored_food_.size() >= max_capacity_){
        return counter_status::full;
    }
    return counter_status::has_food;
}

void entities::food_counter::reserve(){
    ++reserved_;
}
void entities::food_counter::release_reservation(){
    if(reserved_ > 0){
        --reserved_;
    }
}
size_t entities::food_counter::reserved() const{
    return reserved_;
}
size_t entities::food_counter::available_capacity() const{
    return current_capacity() > reserved_ ? current_capacity() - reserved_ : 0;
}
bool entities::food_counter::has_available_food() const{
    return available_capacity() > 0;
}

void entities::food_counter::render(Vector2 draw_position, int frame){
    entity::render(draw_position, frame);
    if(! stored_food_.empty()){
        Vector2 food_position = Vector2{
            draw_position.x + entity_config::food_draw_offset.x,
            draw_position.y + entity_config::food_draw_offset.y
        };
        stored_food_.front()->render(food_position, frame);
    }
}

void entities::station::update_interaction_positions(){
    // Defensive: keep interaction nodes on the map. A station near an edge could
    // otherwise produce an off-map interaction position that snaps to a bogus node.
    const float max_x = level_config::world_x - level_config::edge_weight;
    const float max_y = level_config::world_y - level_config::edge_weight;
    auto clamp_position = [max_x, max_y](Vector2 p) -> Vector2 {
        return Vector2{
            std::max(0.0f, std::min(p.x, max_x)),
            std::max(0.0f, std::min(p.y, max_y))
        };
    };
    interaction_positions_ = interaction_positions{
        clamp_position(Vector2{position_.x - level_config::edge_weight, position_.y}),
        clamp_position(Vector2{position_.x + (2.0f * level_config::edge_weight), position_.y})
    };
}
entities::station::interaction_positions entities::station::get_interaction_positions() const{
    return interaction_positions_;
}

void entities::table::place_down(){
    decoration::place_down();
    update_interaction_positions();
    std::unique_ptr<events::event> registered_table = std::make_unique<events::registered_table>(this);
    event_interface::queue_event(registered_table);
}

void entities::food_counter::place_down(){
    decoration::place_down();
    // Recompute the flanking interaction nodes from the (possibly moved)
    // position; the expediter reads them live off this pointer via the event.
    update_interaction_positions();
    std::unique_ptr<events::event> registered_food_counter = std::make_unique<events::registered_food_counter>(
        this);
    event_interface::queue_event(registered_food_counter);
}
