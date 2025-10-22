/** 
 * file for the player class containing 
 * ! add overview here ! 
 *  author: raffa, october 25
 */
#ifndef PLAYER_H
#define PLAYER_H

#include "config.h"
#include "raylib.h"
#include "sprite.h"
namespace player{
    // map from control input to function 
    class controls{
        // TODO implement 
    };

    class hud{

    };
    class inventory{

    };
    // also include at some point hud and inventory
    class player{
        public:
            ~player() = default;
            player() {};
            player(const player& other) = default;
            player(player&& other) = default;
        
            player& operator=(const player& other) = default;
            player& operator=(player&& other) = default;

            void update(float delta);
            void render();

        private:
            //controls control_scheme_;
            //viewport frame_; // what can be seen by the player, essentially a rectangle, maybe better for the world ?
            /** 
             * 
             hud hud_;
             inventory inventory_;
             */
    };

} // namespace player

#endif