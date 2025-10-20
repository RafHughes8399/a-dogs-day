/**
 *  header file for the main game object, the final point of convergence for each system within the game
 * it is directly used in the main executable file
 *  author: raffa, October 25
 */
#ifndef GAME_H
#define GAME_H

#include "level.h"

namespace game{
    class game {
        public:
            ~game() = default;
            game() = default;
            game(const game& other) = default;
            game(game&& other) = default;

            game& operator=(const game& other) = default;
            game& operator=(game&& other) = default;

            void update(float delta_time);
            void render(float delta_time);
            void debug(float delta_time);
        private:
            // the world
            // the player controller
    };
}
#endif