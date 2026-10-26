#ifndef ANIMATION_CONFIG_H
#define ANIMATION_CONFIG_H

namespace animation_config{
    // * sheet rows, one enum per dog sprite slot. indices 0 - shared_size are the
    // * same animation on every part, so a whole-body action plays with one index
    // * across all four slots; part-specific rows start at shared_size.
    // * index 0 is what a freshly built sprite shows - a sprite's active animation
    // * starts there - so it is the resting row on every part.
    enum shared{
        idle = 0,
        downward_dog,
        eating,
        walking,
        sitting,
        interacting,
        carrying,
        shared_size
    };
    namespace tail{
        enum tags{
            idle = shared::idle,
            downward_dog = shared::downward_dog,
            eating = shared::eating,
            walking = shared::walking,
            sitting = shared::sitting,
            interacting = shared::interacting,
            carrying = shared::carrying,
            wag = shared::shared_size,
            size
        };
    }
    namespace body{
        enum tags{
            idle = shared::idle,
            downward_dog = shared::downward_dog,
            eating = shared::eating,
            walking = shared::walking,
            sitting = shared::sitting,
            interacting = shared::interacting,
            carrying = shared::carrying,
            pawing = shared::shared_size,
            size
        };
    }
    namespace head{
        enum tags{
            idle = shared::idle,
            downward_dog = shared::downward_dog,
            eating = shared::eating,
            walking = shared::walking,
            sitting = shared::sitting,
            interacting = shared::interacting,
            carrying = shared::carrying,
            pinned_back = shared::shared_size,
            one_up,
            bouncing,
            size
        };
    }
    namespace face{
        enum tags{
            idle = shared::idle,
            downward_dog = shared::downward_dog,
            eating = shared::eating,
            walking = shared::walking,
            sitting = shared::sitting,
            interacting = shared::interacting,
            carrying = shared::carrying,
            sniffing = shared::shared_size,
            panting,
            licking,
            size
        };
    }
    namespace station{
        enum tags{
            idle = 0,
            active,
            size
        };
    }

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
