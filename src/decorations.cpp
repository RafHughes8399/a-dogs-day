#include "entities.h"

// ------------------------ decorations -----------------------------------// 


void entities::decoration::on_moved_cursor(const events::moved_cursor& event){
    move(event.get_position());
}

 void entities::decoration::subscribe_to_cursor(){
    // store the "start position"
    pre_move_position_ = position_;
    event_interface::subscribe<events::moved_cursor>(moved_cursor_handler);
}
void entities::decoration::unsubscribe_from_cursor(){
    event_interface::unsubscribe<events::moved_cursor>(moved_cursor_handler);
    // this is the "end position"
    auto post_move_position = position_;
    auto width = hitboxes_[sprites_.index()].get_box().width;
    auto height = hitboxes_[sprites_.index()].get_box().height;

    auto pre_move_rectangle = Rectangle{pre_move_position_.x, pre_move_position_.y, width, height};
    auto post_move_rectangle = Rectangle{pre_move_position_.x, pre_move_position_.y, width, height};
    std::unique_ptr<events::event> move_decoration = std::make_unique<events::moved_decoration>(pre_move_rectangle, post_move_rectangle, id_);
    // create a move_in_graph_event

    // pass in the two rectangles 
}

// ------------------------------ builds -------------------------------- //

std::unique_ptr<entities::entity> entities::entity_builder::build_test_decoration(Vector2 position, int id){
    // load the sprite and the hitbox

    auto sprite = sprite::sprite(
        textures::textures_.get_texture(textures::test_decoration, assets_config::test_decoration_path),
        assets_config::test_decoration_attributes[assets_config::attributes::frame_width],
        assets_config::test_decoration_attributes[assets_config::attributes::frame_height],
        assets_config::test_decoration_attributes[assets_config::attributes::frames],
        assets_config::test_decoration_attributes[assets_config::attributes::animations]
    );
    auto hitbox = hitbox::h_builder_.build_test_decoration_hitbox(position);
    std::vector<sprite::sprite> sprites = {sprite};
    std::vector<hitbox::hitbox> hitboxes = {hitbox};

    return std::make_unique<entities::decoration>(sprites, hitboxes, position, id);
}
