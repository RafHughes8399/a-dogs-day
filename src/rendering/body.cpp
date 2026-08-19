#include "body.h"
#include <algorithm>
size_t body::body::get_index(){
    return index_;
}
void body::body::set_index(size_t index){
    index_ = index;
}
size_t body::body::num_sprites(){
    return sprites_.size();
}
hitbox::hitbox& body::body::get_hitbox(){
    return hitboxes_[index_];
}
sprite::sprite& body::body::get_sprite(){
    return sprites_[index_];
}
std::vector<hitbox::hitbox> body::body::get_hitboxes(){
    return hitboxes_;
}
std::vector<sprite::sprite> body::body::get_sprites(){
    return sprites_;
}

void body::body::render(Vector2 position, int frame){
    if(index_ >= sprites_.size() or index_ >= hitboxes_.size()) return;
    sprites_[index_].render(position, frame);

    // temp for debug purposes
    auto box = hitboxes_[index_].get_box();
    // draw the hitbox at its actual place 
    DrawRectangleLines(static_cast<int>(box.x), static_cast<int>(box.y),
        static_cast<int>(box.width), static_cast<int>(box.height), GREEN);
}

void body::body::update_hitboxes(Vector2 new_position){
    std::for_each(hitboxes_.begin(), hitboxes_.end(), [new_position](hitbox::hitbox& h) -> void {h.update(new_position);});

}
