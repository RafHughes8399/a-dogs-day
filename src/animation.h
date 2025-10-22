/**
 *  file that handles animations from spritesheets
 *  - more info -
 *  author: raffa October 25
 */
#ifndef ANIMATION_H
#define ANIMATION_H

#include "raglib.h"
#include "raylib.h"

namespace animation{
    class animation{
        public:
            ~animation() = default;
            animation(float frame_width, float frame_height, int frames, int animations)
            : frame_(Rectangle{0.0f, 0.0f, frame_width, frame_height}), frames_(frames), animations_(animations){
            }
            animation(const animation& other) = default;
            animation(animation&& other)= default;
                
            animation& operator=(const animation& other) = default;
            animation& operator=(animation&& other) = default;
            const Rectangle& get_frame() const;
            int get_current_frame(); 
            int get_current_animation();
            bool playing();
            void next_frame(bool wrap = true);
            void next_animation();

            void goto_frame(const int frame);
            void goto_animation(const int animation);

            void play();
            void pause();

            const int num_frames();
            const int num_animations();
        private:
        // the frame of the sprite sheet
        Rectangle frame_;
        // num animations in the sheet (rows)
        const int animations_; 
        // num frames in the animation (columns)
        const int frames_;
        
        // default frame is the origin upon construction 
        int current_frame_ = 0;
        int current_animation_ = 0;
        bool is_playing_ = false;

    };
    // TODO: 3D model animation
    // class model_animation{}
} // namespace name
#endif