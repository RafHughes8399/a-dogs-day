#ifndef WORLD_H
#define WORLD_H
#include "graph.h"
    class world {
        public:
            ~world() = default;
            world()
            : footpath_(Rectangle{level_config::footpath_x, level_config::footpath_y,
                level_config::footpath_width, level_config::footpath_height}, false),
            cafe_(Rectangle{level_config::cafe_x, level_config::cafe_y,
                level_config::cafe_width, level_config::cafe_height}, false){};
            world(const world& other) = delete;
            world(world&& other) = delete;

            world& operator=(const world& other) = delete;
            world& operator=(world&& other) = delete;
            
        private:
            graph::level_graph footpath_;
            graph::level_graph cafe_;
    };
#endif