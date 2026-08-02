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
    // * the composition root for the ECS side: owns every system by value, so
    // * they are constructed and destroyed deterministically inside main rather
    // * than during static teardown. That matters because each system
    // * unsubscribes its handlers in its destructor and events::global_dispatcher_
    // * is a global - a singleton system could outlive the dispatcher it
    // * unsubscribes from.
    // *
    // * member declaration order IS the tick order, and reverse declaration
    // * order is the teardown order. Keep the two in step:
    // *   lifecycle -> input -> npc -> movement -> spatial -> collision
    // *   -> interaction -> rendering
    // * spatial refreshes the index after movement writes positions, so
    // * collision and interaction read a current index.
    class ecs_game {
        public:
            ~ecs_game() = default;
            ecs_game(sprite::sprite background, Rectangle view_frame)
                : frame_count_(0), lifecycle_(), input_(), npc_(), movement_(),
                spatial_(), collision_(), interaction_(),
                rendering_(background, view_frame){}
            // systems subscribe handlers in their constructors, so they are
            // non-copyable and non-movable - and so is anything holding them.
            ecs_game(const ecs_game& other) = delete;
            ecs_game(ecs_game&& other) = delete;

            ecs_game& operator=(const ecs_game& other) = delete;
            ecs_game& operator=(ecs_game&& other) = delete;

            // * two-phase on purpose. The constructor only stands the systems
            // * up; init populates the world. Separate because building entities
            // * loads textures (raylib must already be running) and because the
            // * builders write into managers the systems observe, so every
            // * system has to exist first. Also makes a level reload / new game
            // * a matter of calling init again.
            void init();

            void update(float delta_time);
            void render(float delta_time);
            void debug(float delta_time);
        private:
            int frame_count_;

            systems::entity_lifecycle_system lifecycle_;
            systems::control_input_system input_;
            systems::npc_system npc_;
            systems::movement_system movement_;
            systems::spatial_system spatial_;
            systems::collision_system collision_;
            systems::interaction_system interaction_;
            systems::rendering_system rendering_;
    };
}
#endif
