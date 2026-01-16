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

    class hud{

    };
    class inventory{

    };
    // also include at some point hud and inventory
    class player{
        public:
        enum key_press_actions{
            dog_switch = KEY_F,
            shop_open = KEY_S,
            inventory_open = KEY_I,
            menu_open = KEY_TAB,
            quests_open = KEY_Q,
            map_open = KEY_M,

        };
        enum key_hold_actions{
            //edit_mode = KEY_E,
            move_down = KEY_DOWN,
            move_up  = KEY_UP,
            move_left = KEY_LEFT,
            move_right = KEY_RIGHT
        };
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

            };
            player(const player& other) = default;
            player(player&& other) = default;
        
            player& operator=(const player& other) = default;
            player& operator=(player&& other) = default;

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
             inventory inventory_;
            */

            void setup_control_maps();
            events::event_handler<events::selected_dog> select_dog_handler_;            
            int selected_dog_;
            
            std::vector<int> mouse_controls_; 
            std::map<int, std::function<void(float)>> key_hold_controls_;
            std::map<int, std::function<void()>> key_press_controls_; 
            Vector2 mouse_position_;
            
    };

} // namespace player

#endif