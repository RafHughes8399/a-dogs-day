#ifndef ANIMATION_CONFIG_H
#define ANIMATION_CONFIG_H

namespace animation_config{
    inline constexpr int default_play_speed = 6;

    inline constexpr int idle_frames = 4;
    inline constexpr int idle_play_speed = 10;
    inline constexpr int downward_dog_frames = 6;
    inline constexpr int downward_dog_play_speed = default_play_speed;
    inline constexpr int eating_frames = 6;
    inline constexpr int eating_play_speed = default_play_speed;
    inline constexpr int walking_frames = 6;
    inline constexpr int walking_play_speed = default_play_speed;
    inline constexpr int sitting_frames = 3;
    inline constexpr int sitting_play_speed = 10;
    inline constexpr int interacting_frames = 5;
    inline constexpr int interacting_play_speed = default_play_speed;
    inline constexpr int carrying_frames = 6;
    inline constexpr int carrying_play_speed = default_play_speed;

    inline constexpr int head_pinned_back_frames = 2;
    inline constexpr int head_pinned_back_play_speed = 10;
    inline constexpr int head_one_up_frames = 2;
    inline constexpr int head_one_up_play_speed = 10;
    inline constexpr int head_bouncing_frames = 4;
    inline constexpr int head_bouncing_play_speed = default_play_speed;

    inline constexpr int face_sniffing_frames = 4;
    inline constexpr int face_sniffing_play_speed = default_play_speed;
    inline constexpr int face_panting_frames = 6;
    inline constexpr int face_panting_play_speed = 5;
    inline constexpr int face_licking_frames = 5;
    inline constexpr int face_licking_play_speed = default_play_speed;

    inline constexpr int body_pawing_frames = 4;
    inline constexpr int body_pawing_play_speed = default_play_speed;

    inline constexpr int tail_wag_frames = 4;
    inline constexpr int tail_wag_play_speed = 5;

    inline constexpr int static_frames = 1;
    inline constexpr int static_play_speed = 1;
}
#endif
