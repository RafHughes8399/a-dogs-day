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
#include "hud.h"
#include "raylib.h"
#include "sprite.h"

#include <map>
#include <functional>
namespace player{
    enum mouse{
        left_mouse = 0,
        right_mouse = 1
    };
    enum control_states{
        regular = 0,
        editing = 1
        // menu = 2 maybe 
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
            class state {
                public:
                    virtual ~state() = default;
                    state(){};
                    state(const state& other) = default;
                    state(state&& other) = default;
                    
                    state& operator=(const state& other) = default;
                    state& operator=(state&& other) = default;
                    
                    // has the "default " control behaviour
                    virtual void left_click(player& player);
                    virtual void right_click(player& player);
            };
            class editing : public state{
                public:
                    virtual ~editing() = default;
                    editing()
                    : state(){};
                    editing(const editing& other) = default;
                    editing(editing&& other) = default;
                    
                    editing& operator=(const editing& other) = default;
                    editing& operator=(editing&& other) = default;
                    
                    void left_click(player& player) override;
                    //void right_click(player& player) override;
            };
            class carrying_decoration : public editing {
                public:
                    ~carrying_decoration() = default;
                    carrying_decoration()
                    : editing(){};
                    carrying_decoration(const carrying_decoration& other) = default;
                    carrying_decoration(carrying_decoration&& other) = default;
                    
                    carrying_decoration& operator=(const carrying_decoration& other) = default;
                    carrying_decoration& operator=(carrying_decoration&& other) = default;
                    
                    // has the "default " control behaviour
                    void left_click(player& player) override;
                    //void right_click(player& player) override;
            };

            
            ~player(){
                event_interface::unsubscribe<events::selected_dog>(select_dog_handler_);
            }
            player(int selected_dog = level_config::mack_id)
            : mouse_position_(GetMousePosition()), mouse_controls_(controls_config::mouse_controls), key_press_controls_({}),
            key_hold_controls_({}), selected_dog_(selected_dog), state_(std::make_unique<state>()), hud_(hud::h_builder_.build_player_hud()),
            select_dog_handler_([this](const events::selected_dog& event) -> void{on_selected_dog(event);}){
                event_interface::subscribe(select_dog_handler_);
                setup_control_maps();
                select_dog();
                std::unique_ptr<events::event> level_up_event = std::make_unique<events::level_up>(level_);
                //event_interface::queue_event(level_up_event);
            };
            player(const player& other) = delete;
            player(player&& other) = default;
        
            player& operator=(const player& other) = delete;
            player& operator=(player&& other) = delete;

            void back();
            void edit(float delta);
            void exit_edit();
            void move(Vector2 direction_scalar, float delta);            
            void open_inventory();
            void open_map();
            void open_menu();
            void open_quests();
            void open_shop();
            
            void left_click(); 
            void right_click();
            void select_dog();
            void switch_dog();

            void on_selected_dog(const events::selected_dog& event);
            void update(float delta);
            void render();

        private:
            //controls control_scheme_;
            //viewport frame_; // what can be seen by the player, essentially a rectangle, maybe better for the world ?
            void reset_meter();
            void increment_meter();
            /** 
             * 
             hud hud_; // for now hud is just a list of elements, will change when it becomes more intricate
             */
            
            void setup_control_maps();

            events::event_handler<events::selected_dog> select_dog_handler_;       
            
            hud::hud hud_;
            inventory inventory_;
            int edit_meter_ = 0;

            int bones_ = player_config::max_bones;
            int level_ = player_config::max_level;
            int selected_dog_;
            
            std::vector<int> mouse_controls_; 
            std::map<int, std::function<void(float)>> key_hold_controls_;
            std::map<int, std::function<void()>> key_press_controls_; 
            
            std::unique_ptr<state> state_;
            Vector2 mouse_position_;
            
    };
    class controls {
        public:
            ~controls() {
                event_interface::unsubscribe<events::enter_edit_mode>(enter_edit_mode_handler_);
                event_interface::unsubscribe<events::exit_edit_mode>(exit_edit_mode_handler_);
            }
            controls(player* player, size_t index = control_states::regular)
            :player_(player), current_scheme_(index),
            enter_edit_mode_handler_([this](const events::enter_edit_mode& event) -> void {on_enter_edit_mode(event);}),
            exit_edit_mode_handler_([this](const events::exit_edit_mode& event) -> void {on_exit_edit_mode(event);})
            {

                build_controls();
                event_interface::subscribe<events::enter_edit_mode>(enter_edit_mode_handler_);
                event_interface::subscribe<events::exit_edit_mode>(exit_edit_mode_handler_);
            }
            controls(const controls& other) = default;
            controls(controls&& other) = default;

            controls& operator=(const controls& other) = delete;
            controls& operator=(controls&& other) = delete;

            void check(float delta);
            void on_enter_edit_mode(const events::enter_edit_mode& event);
            void on_exit_edit_mode(const events::exit_edit_mode& event);
        private:
            // operate as two parralel arrays
            void build_controls();
            void build_default_controls_state();
            void build_editing_controls_state();

            size_t current_scheme_;
            
            events::event_handler<events::enter_edit_mode> enter_edit_mode_handler_;
            events::event_handler<events::exit_edit_mode> exit_edit_mode_handler_;
            std::vector<std::map<int, std::function<void()>>> mouse_controls_; 
            std::vector<std::map<int, std::function<void(float)>>> key_hold_controls_;
            std::vector<std::map<int, std::function<void()>>> key_press_controls_; 

            player* player_;
            // and listen to events         
    };
} // namespace player

#endif