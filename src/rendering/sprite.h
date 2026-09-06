/**
 *  header file for handling spritesheets for entities, holds the animation details too. 
 *  ! basic overview of how it works !
 *  author: raffa, october 25 
 */
#ifndef SPRITE_H
#define SPRITE_H

#include "animation.h"
#include "config.h"
#include <raylib.h>
#include <vector>

namespace sprite{
    class sprite{
        public:
            ~sprite() = default;
            sprite(Texture2D texture, float frame_width, float frame_height, float frames, float animations, Vector2 draw_position_offset = Vector2Zero(), Color tint = WHITE)
                : sprite_animation_(animation::animation(frame_width, frame_height, static_cast<int>(frames), static_cast<int>(animations))),
                sprite_texture_(texture),
                draw_position_offset_(draw_position_offset),
                tint_(tint){}
            sprite(const sprite& other) = default;
            sprite(sprite&& other) = default;
            
            sprite& operator=(const sprite& other) = delete;
            sprite& operator=(sprite&& other) = delete;

            animation::animation& get_animation();
            const Texture2D& get_texture();
            Vector2 get_draw_position_offset() const;
            void render(Vector2 position, int frame);
        private:
            // has the texture 
            // and the animation
            animation::animation sprite_animation_;
            const Texture2D sprite_texture_; 
            Vector2 draw_position_offset_;
            Color tint_;
    };
    class spriteset{
        public:
            ~spriteset() = default;
            spriteset(std::vector<sprite>& sprites, size_t index = 0)
            : current_(index), sprites_(sprites){}
            spriteset(const spriteset& other) = default;
            spriteset(spriteset&& other) = default;

            spriteset& operator=(const spriteset& other) = default;
            spriteset& operator=(spriteset&& other) = default;

            sprite& operator[](size_t index){
                return sprites_[index];
            }
            
            size_t index();
            sprite& get_sprite();
            std::vector<sprite>& get_sprites();
            
            void set_index(size_t index);
            void render(Vector2 position, int frame);
        private:
            size_t current_;
            std::vector<sprite> sprites_;
    };
    // a sprite should have multiple textures, up to 4 (up down left right)
    // potential use for displaying things like shop items
    // i.e all the hats exist on one sprite sheet, then the shop displays them, idk more thought needded
    class spritesheet{
        public:
        private:
        // has a texture
        // not an animation though, something else, ? maybe a grid or something ?
    };
    class tilesheet{
        public:
        private:
            // has a texture
            // and a grid thing as well, denoting how many tiles 
            // and can select a tile by referencing its position in the grid 

    };

    // perhaps a sprite builder could be of use
    // sprite_builder::build_background();
}
namespace sprite_builders{
    sprite::sprite build_sprite(Texture2D texture, float frame_width, float frame_height, float frames, float animations,
        Vector2 draw_position_offset = Vector2Zero(), Color tint = WHITE);
    sprite::sprite build_cursor_sprite();
    sprite::sprite build_background_sprite();
    // every dog sprite is the same four attribute lookups off a cached texture
    sprite::sprite build_dog_sprite(int texture_key, const char* path,
        const float attributes[entity_config::attributes::size],
        Vector2 draw_position_offset = Vector2Zero());
    // one inner vector per dog_sprite_slots entry, each {left, right}
    std::vector<std::vector<sprite::sprite>> build_dog_part_layers(
        const entity_config::dog_part parts[entity_config::dog_sprite_slots_size],
        const int texture_keys[entity_config::dog_sprite_slots_size][entity_config::dog_part_directions_size],
        float expected_total_width);
    std::vector<sprite::sprite> build_gianluca_sprites();
    std::vector<sprite::sprite> build_lionel_sprites();
    // decorations, stations and food all draw off the test_decoration sheet and
    // differ only by their attribute block and tint
    sprite::sprite build_test_decoration_sprite(const float attributes[entity_config::attributes::size],
        Color tint = WHITE);
    
    // *-------------------- deocration sprites --------------------* //
    sprite::sprite build_decoration_sprite(size_t texture_id, const char* decoration_path, const float attributes[entity_config::attributes::size],
        Vector2 draw_position_offset = Vector2Zero());
    sprite::sprite build_poker_table();
    sprite::sprite build_dog_painting();
    sprite::sprite build_gargoyle();

    // *-------------------- station sprites --------------------* //
    sprite::sprite build_table_sprite();
    sprite::sprite build_dining_table_sprite();
    sprite::sprite build_food_counter_sprite();
    sprite::sprite build_dishwasher_sprite();
    sprite::sprite build_stove_sprite();
    sprite::sprite build_food_sprite();
    std::vector<sprite::sprite> build_food_sprites();
}
#endif
