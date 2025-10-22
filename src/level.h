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
#include "config.h"
#include "quadtree.h"
#include "textures.h"
namespace level{
    class level{
        public:
            ~level() = default;
            level()
                : background_(sprite::sprite(LoadTexture(config::background_path), config::background_attributes[config::attributes::frame_width], config::background_attributes[config::attributes::frame_height],
                config::background_attributes[config::attributes::frames], config::background_attributes[config::attributes::animations])), 
                
                level_entities_(tree::quadree(raglib::bounding_box_2{Vector2Zero(), Vector2{config::world_x, config::world_y}})),
                view_frame_(Rectangle{0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())}),
                dimensions_(Vector2{config::world_x, config::world_y}){};
            level(const level& other) = default;
            level(level&& other) = default;
            
            level& operator=(const level& other) = default;
            level& operator=(level&& other) = default;

            void update(float delta);
            void render();
        private:
            sprite::sprite background_;
            tree::quadree level_entities_;
            Rectangle view_frame_;
            Vector2 dimensions_;
    };
}

#endif