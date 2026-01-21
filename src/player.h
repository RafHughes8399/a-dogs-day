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

#include <map>
#include <functional>
namespace player{
    enum mouse{
        left_mouse = 0,
        right_mouse = 1
    };

    class inventory{
        public:
        private:
            // active 
            // stashed
            // decorations and hats
    };
    // also include at some point hud and inventory
    class player{
        public:

            ~player(){
                event_interface::unsubscribe<events::selected_dog>(select_dog_handler_);
            }
            player(int selected_dog = level_config::mack_id)
            : mouse_position_(GetMousePosition()), mouse_controls_(controls_config::mouse_controls), key_press_controls_({}),
            key_hold_controls_({}), selected_dog_(selected_dog),
            select_dog_handler_([this](const events::selected_dog& event) -> void{on_selected_dog(event);}){
                event_interface::subscribe(select_dog_handler_);
                setup_control_maps();
                select_dog();
                std::unique_ptr<events::event> level_up_event = std::make_unique<events::level_up>(level_);
                event_interface::queue_event(level_up_event);
            };
            player(const player& other) = default;
            player(player&& other) = default;
        
            player& operator=(const player& other) = default;
            player& operator=(player&& other) = default;

            void back();
            void move(Vector2 direction_scalar, float delta);            
            void open_inventory();
            void open_map();
            void open_menu();
            void open_quests();
            void open_shop();
            
            void select_dog();
            void switch_dog();

            void on_selected_dog(const events::selected_dog& event);
            void update(float delta);
            void render();

        private:
            //controls control_scheme_;
            //viewport frame_; // what can be seen by the player, essentially a rectangle, maybe better for the world ?
            /** 
             * 
             hud hud_;
             */
            
            void setup_control_maps();

            events::event_handler<events::selected_dog> select_dog_handler_;            
            inventory inventory_;
            int bones_ = player_config::max_bones;
            int level_ = player_config::max_level;
            int selected_dog_;
            
            std::vector<int> mouse_controls_; 
            std::map<int, std::function<void(float)>> key_hold_controls_;
            std::map<int, std::function<void()>> key_press_controls_; 
            
            Vector2 mouse_position_;
            
    };

} // namespace player

#endif