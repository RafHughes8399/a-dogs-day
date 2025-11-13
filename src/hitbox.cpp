#include "hitbox.h"
#include <iostream>
// ----------------------- global declaration ----------------------- //
hitbox::hitbox_builder hitbox::h_builder_;

// ----------------------- hitbox ----------------------- //
bool hitbox::hitbox::check_collision(const hitbox& other){
    // compare frames
    if(CheckCollisionRecs(box_, other.box_)){
        for(auto & this_box : sub_boxes_){
            for(auto & other_box : other.sub_boxes_){
                if(CheckCollisionRecs(this_box, other_box)){
                    return true;
                } 
            }
        }
    }
    else{
        return false;
    }
    // then compare subboxes
}
Rectangle& hitbox::hitbox::get_box(){
    return box_;
}

Rectangle hitbox::hitbox::get_sub_box(size_t index){
    return sub_boxes_[index];
}
std::vector<Rectangle> hitbox::hitbox::get_sub_boxes(){
    return sub_boxes_;
}

void hitbox::hitbox::update(Vector2 new_position){
    box_.x = new_position.x;
    box_.y = new_position.y;
    
    std::for_each(sub_boxes_.begin(), sub_boxes_.end(), [new_position](auto & sub_box ) -> void {
        sub_box.x = new_position.x;
        sub_box.y = new_position.y;
    });
}
// ----------------------- hitbox builder ----------------------- //
hitbox::hitbox hitbox::hitbox_builder::build_cursor_hitbox(Vector2 position){
    return hitbox(Rectangle{position.x, position.y, assets_config::cursor_attributes[assets_config::attributes::frame_width], assets_config::cursor_attributes[assets_config::attributes::frame_height]});
}
hitbox::hitbox hitbox::hitbox_builder::build_paw_mark_hitbox(Vector2 position){
    return hitbox(Rectangle{position.x, position.y, assets_config::paw_mark_attributes[assets_config::attributes::frame_width], assets_config::paw_mark_attributes[assets_config::attributes::frame_height]});
}
hitbox::hitbox hitbox::hitbox_builder::build_player_dog_hitbox(Vector2 position){
    return hitbox(Rectangle{position.x, position.y, assets_config::khiri_attributes[assets_config::attributes::frame_width], assets_config::khiri_attributes[assets_config::attributes::frame_height]});
}