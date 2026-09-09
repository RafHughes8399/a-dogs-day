#include "animation.h"
#include "config.h"

animation::animation animation_builders::build_animation(float frame_width, float frame_height,
    int row, int frames, int play_speed){
    return animation::animation(frame_width, frame_height, row, frames, play_speed);
}

animation::animation animation_builders::build_dog_idle_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::shared::idle,
        animation_config::idle_frames, animation_config::idle_play_speed);
}
animation::animation animation_builders::build_dog_downward_dog_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::shared::downward_dog,
        animation_config::downward_dog_frames, animation_config::downward_dog_play_speed);
}
animation::animation animation_builders::build_dog_eating_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::shared::eating,
        animation_config::eating_frames, animation_config::eating_play_speed);
}
animation::animation animation_builders::build_dog_walking_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::shared::walking,
        animation_config::walking_frames, animation_config::walking_play_speed);
}
animation::animation animation_builders::build_dog_sitting_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::shared::sitting,
        animation_config::sitting_frames, animation_config::sitting_play_speed);
}
animation::animation animation_builders::build_dog_interacting_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::shared::interacting,
        animation_config::interacting_frames, animation_config::interacting_play_speed);
}
animation::animation animation_builders::build_dog_carrying_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::shared::carrying,
        animation_config::carrying_frames, animation_config::carrying_play_speed);
}

animation::animation animation_builders::build_dog_head_pinned_back_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::head::pinned_back,
        animation_config::head_pinned_back_frames, animation_config::head_pinned_back_play_speed);
}
animation::animation animation_builders::build_dog_head_one_up_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::head::one_up,
        animation_config::head_one_up_frames, animation_config::head_one_up_play_speed);
}
animation::animation animation_builders::build_dog_head_bouncing_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::head::bouncing,
        animation_config::head_bouncing_frames, animation_config::head_bouncing_play_speed);
}

animation::animation animation_builders::build_dog_face_sniffing_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::face::sniffing,
        animation_config::face_sniffing_frames, animation_config::face_sniffing_play_speed);
}
animation::animation animation_builders::build_dog_face_panting_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::face::panting,
        animation_config::face_panting_frames, animation_config::face_panting_play_speed);
}
animation::animation animation_builders::build_dog_face_licking_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::face::licking,
        animation_config::face_licking_frames, animation_config::face_licking_play_speed);
}

animation::animation animation_builders::build_dog_body_pawing_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::body::pawing,
        animation_config::body_pawing_frames, animation_config::body_pawing_play_speed);
}

animation::animation animation_builders::build_dog_tail_wag_animation(float frame_width, float frame_height){
    return build_animation(frame_width, frame_height, animation_config::tail::wag,
        animation_config::tail_wag_frames, animation_config::tail_wag_play_speed);
}

std::vector<animation::animation> animation_builders::build_shared_dog_animations(float frame_width, float frame_height){
    std::vector<animation::animation> animations;
    animations.push_back(build_dog_idle_animation(frame_width, frame_height));
    animations.push_back(build_dog_downward_dog_animation(frame_width, frame_height));
    animations.push_back(build_dog_eating_animation(frame_width, frame_height));
    animations.push_back(build_dog_walking_animation(frame_width, frame_height));
    animations.push_back(build_dog_sitting_animation(frame_width, frame_height));
    animations.push_back(build_dog_interacting_animation(frame_width, frame_height));
    animations.push_back(build_dog_carrying_animation(frame_width, frame_height));
    return animations;
}

std::vector<animation::animation> animation_builders::build_dog_head_animations(float frame_width, float frame_height){
    auto animations = build_shared_dog_animations(frame_width, frame_height);
    animations.push_back(build_dog_head_pinned_back_animation(frame_width, frame_height));
    animations.push_back(build_dog_head_one_up_animation(frame_width, frame_height));
    animations.push_back(build_dog_head_bouncing_animation(frame_width, frame_height));
    return animations;
}

std::vector<animation::animation> animation_builders::build_dog_face_animations(float frame_width, float frame_height){
    auto animations = build_shared_dog_animations(frame_width, frame_height);
    animations.push_back(build_dog_face_sniffing_animation(frame_width, frame_height));
    animations.push_back(build_dog_face_panting_animation(frame_width, frame_height));
    animations.push_back(build_dog_face_licking_animation(frame_width, frame_height));
    return animations;
}

std::vector<animation::animation> animation_builders::build_dog_body_animations(float frame_width, float frame_height){
    auto animations = build_shared_dog_animations(frame_width, frame_height);
    animations.push_back(build_dog_body_pawing_animation(frame_width, frame_height));
    return animations;
}

std::vector<animation::animation> animation_builders::build_dog_tail_animations(float frame_width, float frame_height){
    auto animations = build_shared_dog_animations(frame_width, frame_height);
    animations.push_back(build_dog_tail_wag_animation(frame_width, frame_height));
    return animations;
}

std::vector<animation::animation> animation_builders::build_uniform_animations(float frame_width, float frame_height,
    int rows, int frames){
    std::vector<animation::animation> animations;
    for(int row = 0; row < rows; ++row){
        animations.push_back(build_animation(frame_width, frame_height, row, frames,
            animation_config::static_play_speed));
    }
    return animations;
}
