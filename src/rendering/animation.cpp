#include "animation.h"


bool animation::animation::playing(){
    return is_playing_;
}

const Rectangle& animation::animation::get_frame() const{
    return frame_;
}

int animation::animation::get_current_frame(){
    return current_frame_;
}
int animation::animation::get_row(){
    return row_;
}

int animation::animation::num_frames(){
    return frames_;
}

int animation::animation::get_play_speed(){
    return play_speed_;
}
void animation::animation::goto_frame(const int frame){
    if(frame < frames_){
        current_frame_ = frame;
        frame_.x = frame_.width * frame;
    }
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


void animation::animation::advance(int frame){
    if(is_playing_ and play_speed_ > 0 and frame % play_speed_ == 0){
        if(not repeat_ and current_frame_ == frames_ - 1){
            is_playing_ = false;
            return;
        }
        next_frame(repeat_);
    }
}
void animation::animation::play(bool repeat){
    repeat_ = repeat;
    is_playing_ = true;
}
void animation::animation::pause(){
    is_playing_ = false;
}