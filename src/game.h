/**
 *  header file for the main game object, the final point of convergence for each system within the game
 * it is directly used in the main executable file
 *  author: raffa, October 25
 */
#ifndef GAME_H
#define GAME_H

#include "level.h"
#include "player.h"
#include <iostream>
namespace game{
    class game {
        public:
            ~game() = default;
            game(level::level& level, player::player& player)
                : level_(level), player_(player), frame_count_(0){};
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
            int frame_count_;
            level::level& level_;
            player::player& player_;
    };
}
#endif