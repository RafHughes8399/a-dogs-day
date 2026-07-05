/**
 *  header file for the main game object, the final point of convergence for each system within the game
 * it is directly used in the main executable file
 *  author: raffa, October 25
 */
#ifndef GAME_H
#define GAME_H

#include "debug_log_interface.h"
#include "debug_logger.h"
#include  "menus.h"
#include "debug_log_interface.h"
#include "debug_logger.h"
#include "expediter.h"
#include "level.h"
#include "maitre_d.h"
#include "player.h"
namespace game{
    class game {
        public:
            ~game() = default;
            game(level::level& level, player::player& player, menus::menu_graph& menu, player::controls& controls)
                : frame_count_(0), level_(level), maitre_d_(maitre_d::maitre_d::get_instance()),
                expediter_(expediter::expediter::get_instance()),
                logger_(debug::logger::get_instance()), menus_(menu), controls_(controls), player_(player){}
            game(const game& other) = default;
            game(game&& other) = default;

            game& operator=(const game& other) = delete;
            game& operator=(game&& other) = delete;

            void update(float delta_time);
            void render(float delta_time);
            void debug(float delta_time);
        private:
            void run_debug_behaviours();
            
            int frame_count_;
            
            level::level& level_;
            maitre_d::maitre_d& maitre_d_;
            expediter::expediter& expediter_;
            debug::logger& logger_;
            menus::menu_graph& menus_;
            player::controls& controls_;
            player::player& player_;
    };
}
#endif
