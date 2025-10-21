/**
 *  header file for handling spritesheets for entities, holds the animation details too. 
 *  ! basic overview of how it works !
 *  author: raffa, october 25 
 */
#ifndef SPRITE_H
#define SPRITE_H

#include "animation.h"
namespace sprite{
    class sprite{
        public:
            ~sprite() = default;
            sprite() = default;
            sprite(const sprite& other) = default;
            sprite(sprite&& other) = default;
            
            sprite& operator=(const sprite& other) = default;
            sprite& operator=(sprite&& other) = default;

            void render();
        private:
            // has the texture 
            Texture2D sprite_texture_; // TODO make the texture a singleton instance
            // and the animation
            animation::animation sprite_animation_;
    };
    // potential use for displaying things like shop items
    // i.e all the hats exist on one sprite sheet, then the shop displays them, idk more thought needded
    class spritesheet{
        
    };
}
#endif