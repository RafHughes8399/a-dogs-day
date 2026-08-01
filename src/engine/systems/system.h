#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "component.h"
#include "events.h"
#include "events_interface.h"

// this should replace most of the logic required by the level and render layers
namespace systems{
    // storage system [quadtree managemet]
    // moovemnet sytem [ posiitons, pathfinding logic etc]
    // rendering system
    // menu system ? 
    // hud system ? 
    class movement_system{

    };
    class rendering_system{

    };
    // * entity storage
    class storage_system{

    };

    // hold a refernece to the glboal managers that they need to process things
    // and the events that they need ot process 
}

#endif