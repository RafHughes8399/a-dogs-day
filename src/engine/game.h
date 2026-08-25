/**
 *  header file for the main game object, the final point of convergence for each system within the game
 * it is directly used in the main executable file
 *  author: raffa, October 25
 */
#ifndef GAME_H
#define GAME_H

#include "debug_log_interface.h"
#include "debug_logger.h"
#include "system.h"
namespace game{
    // owns the systems by value so they are destroyed inside main, while
    // events::global_dispatcher_ is still alive for them to unsubscribe from.
    // member declaration order is the tick order, reversed is the teardown order.
    class game {
        public:
            ~game() = default;
            // resolving every singleton here is what stands them up - and therefore
            // subscribes them - before init() creates the first entity
            game()
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
            game(const game& other) = delete;
            game(game&& other) = delete;

            game& operator=(const game& other) = delete;
            game& operator=(game&& other) = delete;

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
