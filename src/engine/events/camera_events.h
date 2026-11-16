#ifndef CAMERA_EVENTS_H
#define CAMERA_EVENTS_H

#include "event_core.h"

namespace events{
    class camera_moved : public event{
    public:
        camera_moved()
        :event(ids::camera_moved_id){}

        static int get_static_type(){
            return ids::camera_moved_id;
        }
    };
}
#endif
