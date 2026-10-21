#ifndef TYPE_CONFIG_H
#define TYPE_CONFIG_H

#include "raylib.h"
#include <vector>

namespace type_config{
    // * an ordered run of world positions to walk through. Lives here rather
    // * than inside any one class so the graph, the query layer, the movement
    // * component and the dogs all name the same type.
    using path = std::vector<Vector2>;
}
#endif
