#ifndef HUD_EVENTS_H
#define HUD_EVENTS_H

#include "event_core.h"

namespace events{

    class hover_hud_entity : public event{
        public:
            hover_hud_entity(int prev_hovered, int current_hovered)
            :event(ids::hover_hud_entity_id), prev_hovered_(prev_hovered), current_hovered_(current_hovered){}

            static int get_static_type(){
                return ids::hover_hud_entity_id;
            }
            int get_previous_hovered() const{
                return prev_hovered_;
            }
            int get_current_hovered() const{
                return current_hovered_;
            }
        private:
            const int prev_hovered_;
            const int current_hovered_;
    };
}
#endif
