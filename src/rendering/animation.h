/**
 *  file that handles animations from spritesheets
 *  
 *  author: raffa October 25
 */
#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>

#include "raglib.h"
#include "raylib.h"

namespace animation{
    class animation{
        public:
            ~animation() = default;
            animation(float frame_width, float frame_height, int row, int frames, int play_speed = 1)
            : frame_(Rectangle{0.0f, frame_height * static_cast<float>(row), frame_width, frame_height}),
            row_(row), frames_(frames), play_speed_(play_speed){
            }
            animation(const animation& other) = default;
            animation(animation&& other)= default;
                
            animation& operator=(const animation& other) = delete;
            animation& operator=(animation&& other) = delete;

            bool playing();
            const Rectangle& get_frame() const;
            int get_current_frame(); 
            int get_row();

            void goto_frame(const int frame);
            void next_frame(bool wrap = true);

            void advance(int frame);
            void pause();
            void play(bool repeat = true);

            int num_frames();
            int get_play_speed();
        private:
        // the frame of the sprite sheet
        Rectangle frame_;
        // the row this animation occupies on the sheet
        const int row_;
        // num frames in the animation (columns)
        const int frames_;
        int play_speed_;

        int current_frame_ = 0;
        bool is_playing_ = false;
        bool repeat_ = true;

    };
} // namespace name
namespace animation_builders{
    animation::animation build_animation(float frame_width, float frame_height, int row,
        int frames, int play_speed);

    animation::animation build_dog_idle_animation(float frame_width, float frame_height);
    animation::animation build_dog_downward_dog_animation(float frame_width, float frame_height);
    animation::animation build_dog_eating_animation(float frame_width, float frame_height);
    animation::animation build_dog_walking_animation(float frame_width, float frame_height);
    animation::animation build_dog_sitting_animation(float frame_width, float frame_height);
    animation::animation build_dog_interacting_animation(float frame_width, float frame_height);
    animation::animation build_dog_carrying_animation(float frame_width, float frame_height);

    animation::animation build_dog_head_pinned_back_animation(float frame_width, float frame_height);
    animation::animation build_dog_head_one_up_animation(float frame_width, float frame_height);
    animation::animation build_dog_head_bouncing_animation(float frame_width, float frame_height);

    animation::animation build_dog_face_sniffing_animation(float frame_width, float frame_height);
    animation::animation build_dog_face_panting_animation(float frame_width, float frame_height);
    animation::animation build_dog_face_licking_animation(float frame_width, float frame_height);

    animation::animation build_dog_body_pawing_animation(float frame_width, float frame_height);

    animation::animation build_dog_tail_wag_animation(float frame_width, float frame_height);

    std::vector<animation::animation> build_shared_dog_animations(float frame_width, float frame_height);
    std::vector<animation::animation> build_dog_head_animations(float frame_width, float frame_height);
    std::vector<animation::animation> build_dog_face_animations(float frame_width, float frame_height);
    std::vector<animation::animation> build_dog_body_animations(float frame_width, float frame_height);
    std::vector<animation::animation> build_dog_tail_animations(float frame_width, float frame_height);
    std::vector<animation::animation> build_uniform_animations(float frame_width, float frame_height,
        int rows, int frames);
}
#endif
