/**
 * level is a more appropriate name I feel. A level is essentially a container that manages all of the entities
 * it organises the quadtree that stores the entities and handles other information about the level, perhaps any goals, 
 * the structure of rooms, the map, etc
 * 
 * perhaps a requirement to define two types of levels, the main hub and then exploration levels, but that can 
 * come later
 * 
 * author: raffa, October 25
 */
#ifndef LEVEL_H
#define LEVEL_H
#include "quadtree.h"
namespace level{
    class level{
        public:
            ~level() = default;
            level()
                : level_entities_(tree::quadree(raglib::bounding_box_2{Vector2Zero(), Vector2 {config::world_x, config::world_y}})){};
            level(const level& other) = default;
            level(level&& other) = default;
            
            level& operator=(const level& other) = default;
            level& operator=(level&& other) = default;

            void update(float delta);
            void render();
        private:
            // quadtree entities
            // level dimensions
            // etc.
            tree::quadree level_entities_;
    };
}

#endif