#include "animation.h"


// TODO update the rectangle's position as well
const Rectangle& animation::animation::get_frame() const{
    return frame_;
}

int animation::animation::get_current_frame(){
    return current_frame_;
}
int animation::animation::get_current_animation(){
    return current_animation_;
}
bool animation::animation::playing(){
    return is_playing_;
}
void animation::animation::next_frame(bool wrap){
    // check bounds, increment, adjust rectangle, loop if wrap 
    if(current_frame_ < frames_ - 1){
        current_frame_++;
        frame_.x += frame_.width;
    }
    else if(wrap){
            current_frame_ = 0;
            frame_.x = 0.0;
    }
}
void animation::animation::next_animation(){
    // check bounds, increment, adjust rectangle
    // 0 to animations -1
    if(current_animation_ < animations_  - 1){
        current_animation_++;
    
        frame_.y += frame_.height;
    }
}

void animation::animation::goto_frame(const int frame){
    if(frame < frames_){
        current_frame_ = frame;
        frame_.x = frame_.width * frame;
    }
}

void animation::animation::goto_animation(const int animation){
    if(animation < animations_){
        current_animation_ = animation;
        frame_.y = frame_.height * animation;
    }
}

const int animation::animation::num_animations(){
    return animations_;
}

const int animation::animation::num_frames(){
    return frames_;
}
void animation::animation::play(){
    is_playing_ = true;
}
void animation::animation::pause(){
    is_playing_ = false;
}