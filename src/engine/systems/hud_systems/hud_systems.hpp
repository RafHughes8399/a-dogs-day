#ifndef HUD_SYSTEMS_H
#define HUD_SYSTEMS_H

#include <array>
#include <functional>

#include "component.h"
#include "config.h"
namespace hud_systems{
    // rendering huyd elements
    class hud_rendering_system {
    public:
        static hud_rendering_system& get_instance(){
            static hud_rendering_system instance;
            return instance;
        }
        ~hud_rendering_system() = default;
        hud_rendering_system(const hud_rendering_system& other) = delete;
        hud_rendering_system(hud_rendering_system&& other) = delete;
        
        hud_rendering_system& operator=(const hud_rendering_system& other) = delete;
        hud_rendering_system& operator=(hud_rendering_system&& other) = delete;
        void render(int frame);
        private:
            hud_rendering_system() = default;
    };
    class click_system {
    public:
        static click_system& get_instance(){
            static click_system instance;
            return instance;
        }
        ~click_system() = default;
        click_system(const click_system& other) = delete;
        click_system(click_system&& other) = delete;

        click_system& operator=(const click_system& other) = delete;
        click_system& operator=(click_system&& other) = delete;

        bool click(Vector2 mouse_screen);
    private:
        click_system()
        : defined_clicks_(){
            defined_clicks_[hud_config::click_actions::select_recipe] = select_recipe;
            defined_clicks_[hud_config::click_actions::change_page] = change_page;
        }

        static void select_recipe(size_t button, int payload);
        static void change_page(size_t button, int payload);
        std::array<std::function<void(size_t, int)>, hud_config::click_actions::size> defined_clicks_;
    };
    // storing hud elements 
    // processing hud behaviours
    // and then the hud as a whole that has the instances of the 
    class hud {
    public:
        static hud& get_instance(){
            static hud instance;
            return instance;
        }
        ~hud() = default;
        hud(const hud& other) = delete;
        hud(hud&& other) = delete;
        
        hud& operator=(const hud& other) = delete;
        hud& operator=(hud&& other) = delete;
        
        void init();
        void update(float delta_time);
        void render(int frame);
        
    private:
        hud()
        : hud_rendering_(hud_rendering_system::get_instance()),
          clicks_(click_system::get_instance()){}

        hud_rendering_system& hud_rendering_;
        click_system& clicks_;
    };
}
#endif
