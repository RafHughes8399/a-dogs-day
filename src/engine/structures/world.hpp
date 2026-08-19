#ifndef WORLD_H
#define WORLD_H
#include "graph.h"
    class world {
        public:
            ~world() = default;
            world()
            : footpath_(graph::level_graph::level_graph()),
            cafe_(graph::level_graph::level_graph()){};
            world(const world& other) = default;
            world(world&& other) = default;

            world& operator 
        private:
            graph::level_graph footpath_;
            graph::level_graph cafe_;
    };
#endif