#ifndef HUD_SYSTEMS_H
#define HUD_SYSTEMS_H

#include "component.h"
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
        : hud_rendering_(hud_rendering_system::get_instance()){}

        hud_rendering_system& hud_rendering_;
    };
}
#endif
