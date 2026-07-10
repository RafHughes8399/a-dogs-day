#include "config.h"
#include "entities.h"
#include "hitbox.h"
#include "texture.h"
#include "queries.h"
#include "query_interface.h"
#include <cmath>
// ------------------------ decorations -----------------------------------//


bool entities::decoration::can_place_down(){
    Vector2 rounded_position = round_position();
    Rectangle box = body_.get_hitbox().get_box();

    Vector2 rounded_position_difference = Vector2Subtract(rounded_position, position_);
    box.x += rounded_position_difference.x;
    box.y += rounded_position_difference.y;

    std::unique_ptr<queries::query> can_place_decoration = std::make_unique<queries::can_place_decoration>(box, id_);
    return query_interface::execute_query(queries::bool_executor_, *can_place_decoration);
}

void entities::decoration::on_moved_cursor(const events::moved_cursor& event){
    move(event.get_position());
    // and let the hud_element know too
    return;
}

void entities::decoration::pick_up(){
    // store the "start position"
    pre_move_position_ = position_;
    subscribe_to_cursor();
    // make the hud element subscribe
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

Vector2 entities::decoration::round_position(){
    auto rounded_position = Vector2 {
        std::round(position_.x / level_config::edge_weight) * level_config::edge_weight,
        std::round(position_.y / level_config::edge_weight) * level_config::edge_weight
    };
    //move(rounded_position);
    return rounded_position;
}

void entities::decoration::subscribe_to_cursor(){
    event_interface::subscribe<events::moved_cursor>(moved_cursor_handler);
}

void entities::decoration::unsubscribe_from_cursor(){
    event_interface::unsubscribe<events::moved_cursor>(moved_cursor_handler);
}

