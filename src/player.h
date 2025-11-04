/** 
 * file for the player class containing 
 * ! add overview here ! 
 *  author: raffa, october 25
 */
#ifndef PLAYER_H
#define PLAYER_H

#include "config.h"
#include "events.h"
#include "events_interface.h"
#include "raylib.h"
#include "sprite.h"
namespace player{
    enum mouse{
        left_mouse = 0,
        right_mouse = 1
    };

    class hud{

    };
    class inventory{

    };
    // also include at some point hud and inventory
    class player{
        public:
            ~player() = default;
            player()
            : mouse_position_(GetMousePosition()), mouse_controls_(controls_config::mouse_controls), key_controls_(){};
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
            Vector2 mouse_position_;
            std::vector<int> mouse_controls_; 
            std::vector<int> key_controls_; 
            
    };

} // namespace player

#endif