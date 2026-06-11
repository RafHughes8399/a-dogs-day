#include "hitbox.h"
#include "config.h"
// ----------------------- global declaration ----------------------- //
hitbox::hitbox_builder hitbox::h_builder_;

// ----------------------- hitbox ----------------------- //
bool hitbox::hitbox::check_collision_box_sub_boxes(const Rectangle& box, const std::vector<Rectangle>& other_boxes){
    for(auto & other_box : other_boxes){
        if(CheckCollisionRecs(box, other_box)) {return true;}
    }
    return false;
}
bool hitbox::hitbox::check_collision_sub_boxes(const std::vector<Rectangle>& boxes, const std::vector<Rectangle> other_boxes){
    // yes this is n^2 but entities won't have more than like 4 sub boxes at most so its chill
    // the input size will never become unruly 
    for(auto & this_box : boxes){
        for(auto & other_box : other_boxes){
            if(CheckCollisionRecs(this_box, other_box)){
                return true;
            } 
        }
    }
    return false;
}
bool hitbox::hitbox::check_collision(const hitbox& other){
    // compare frames
    if(CheckCollisionRecs(box_, other.box_)){
        bool this_empty = sub_boxes_.empty();
        bool other_empty = other.sub_boxes_.empty();
        if(this_empty && other_empty){
            return true;
        }
        else if(this_empty && ! other_empty){
            return check_collision_box_sub_boxes(box_, other.sub_boxes_);
        }
        else if(! this_empty && other_empty){
            return check_collision_box_sub_boxes(other.box_, sub_boxes_);
        }
        else{
            return check_collision_sub_boxes(sub_boxes_, other.sub_boxes_);
        }
    }
    else{
        return false;
    }
    // then compare subboxes
}
const Rectangle& hitbox::hitbox::get_box() const{
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
    return hitbox(Rectangle{position.x, position.y, entity_config::cursor_attributes[entity_config::attributes::frame_width], entity_config::cursor_attributes[entity_config::attributes::frame_height]});
}
hitbox::hitbox hitbox::hitbox_builder::build_paw_mark_hitbox(Vector2 position){
    return hitbox(Rectangle{position.x, position.y, entity_config::paw_mark_attributes[entity_config::attributes::frame_width], entity_config::paw_mark_attributes[entity_config::attributes::frame_height]});
}

hitbox::hitbox hitbox::hitbox_builder::build_player_dog_across_hitbox(Vector2 position){
    return hitbox(Rectangle{position.x, position.y, entity_config::khiri_across_attributes[entity_config::attributes::frame_width], entity_config::khiri_across_attributes[entity_config::attributes::frame_height]});
}
hitbox::hitbox hitbox::hitbox_builder::build_player_dog_down_hitbox(Vector2 position){
    return hitbox(Rectangle{position.x, position.y, entity_config::khiri_down_attributes[entity_config::attributes::frame_width], entity_config::khiri_down_attributes[entity_config::attributes::frame_height]});
}

hitbox::hitbox hitbox::hitbox_builder::build_test_decoration_hitbox(Vector2 position){
    return hitbox(Rectangle{position.x, position.y, entity_config::test_decoration_attributes[entity_config::attributes::frame_width], entity_config::test_decoration_attributes[entity_config::attributes::frame_height]});
}
hitbox::hitbox hitbox::hitbox_builder::build_gargoyle_hitbox(Vector2 position){
    return hitbox(Rectangle{position.x, position.y, entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_width], entity_config::gargoyle_decoration_attributes[entity_config::attributes::frame_height]});
}
hitbox::hitbox hitbox::hitbox_builder::build_table_hitbox(Vector2 position){
    return hitbox(Rectangle{position.x, position.y, entity_config::table_attributes[entity_config::attributes::frame_width], entity_config::table_attributes[entity_config::attributes::frame_height]});
}
