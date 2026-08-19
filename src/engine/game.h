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
#include "system.h"
namespace game{
    class game {
        public:
            ~game() = default;
            game(level::level& level, player::player& player, menus::menu_graph& menu, player::controls& controls)
                : frame_count_(0), level_(level), maitre_d_(), expediter_(),
                logger_(debug::logger::get_instance()), menus_(menu), controls_(controls), player_(player){}
            // game now owns maitre_d_/expediter_ by value; those are non-copyable
            // and non-movable, so game is too. (dog_days constructs it in place.)
            game(const game& other) = delete;
            game(game&& other) = delete;

            game& operator=(const game& other) = delete;
            game& operator=(game&& other) = delete;

            void update(float delta_time);
            void render(float delta_time);
            void debug(float delta_time);
        private:
            void run_debug_behaviours();
            
            int frame_count_;
            
            level::level& level_;
            maitre_d::maitre_d maitre_d_;
            expediter::expediter expediter_;
            debug::logger& logger_;
            menus::menu_graph& menus_;
            player::controls& controls_;
            player::player& player_;
    };

    // TODO: RENAME AFTER REFACTOR IS COMPLETE - replaces game once level is gone.
    // owns the systems by value so they are destroyed inside main, while
    // events::global_dispatcher_ is still alive for them to unsubscribe from.
    // member declaration order is the tick order, reversed is the teardown order.
    class ecs_game {
        public:
            ~ecs_game() = default;
            // resolving every singleton here is what stands them up - and therefore
            // subscribes them - before init() creates the first entity
            ecs_game()
                : frame_count_(0),
                lifespan_(systems::entity_lifespan_system::get_instance()),
                input_(systems::control_input_system::get_instance()),
                npc_(systems::npc_system::get_instance()),
                movement_(systems::movement_system::get_instance()),
                spatial_(systems::spatial_system::get_instance()),
                collision_(systems::collision_system::get_instance()),
                interaction_(systems::interaction_system::get_instance()),
                rendering_(systems::rendering_system::get_instance()),
                selection_(systems::selection_system::get_instance()){}
            // systems subscribe handlers in their constructors, so they are
            // non-copyable and non-movable - and so is anything holding them.
            ecs_game(const ecs_game& other) = delete;
            ecs_game(ecs_game&& other) = delete;

            ecs_game& operator=(const ecs_game& other) = delete;
            ecs_game& operator=(ecs_game&& other) = delete;

            // ctor stands the systems up, init populates the world - separate so
            // raylib is running before textures load, and re-runnable for reloads
            void init();

            void update(float delta_time);
            void render(float delta_time);
            void debug(float delta_time);
        private:
            int frame_count_;

            systems::entity_lifespan_system& lifespan_;
            systems::control_input_system& input_;
            systems::npc_system& npc_;
            systems::movement_system& movement_;
            systems::spatial_system& spatial_;
            systems::collision_system& collision_;
            systems::interaction_system& interaction_;
            systems::rendering_system& rendering_;
            systems::selection_system& selection_;
    };
}
#endif
