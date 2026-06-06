/**
 *  header file for the main game object, the final point of convergence for each system within the game
 * it is directly used in the main executable file
 *  author: raffa, October 25
 */
#ifndef GAME_H
#define GAME_H

#include  "menus.h"
#include "level.h"
#include "player.h"
namespace game{
    class game {
        public:
            ~game() = default;
            game(level::level& level, player::player& player, menus::menu_graph& menu, player::controls& controls)
                : level_(level), player_(player), frame_count_(0), menus_(menu), controls_(controls){}
            game(const game& other) = default;
            game(game&& other) = default;

            game& operator=(const game& other) = delete;
            game& operator=(game&& other) = delete;

            void update(float delta_time);
            void render(float delta_time);
            void debug(float delta_time);
        private:
            
            int frame_count_;
            
            level::level& level_;
            menus::menu_graph& menus_;
            player::controls& controls_;
            player::player& player_;
    };
}
#endif