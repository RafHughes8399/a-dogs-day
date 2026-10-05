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
#include <array>
#define NINE_SPRITE_ROWS 3
#define NINE_SPRITE_COLS NINE_SPRITE_ROWS
namespace sprite{
    class sprite{
        public:
            ~sprite() = default;
            sprite(Texture2D texture, float frame_width, float frame_height, float frames, float animations, Vector2 draw_position_offset = Vector2Zero())
                : animations_(animation_builders::build_uniform_animations(frame_width, frame_height,
                    static_cast<int>(animations), static_cast<int>(frames))),
                sprite_texture_(texture),
                draw_position_offset_(draw_position_offset){}
            sprite(Texture2D texture, std::vector<animation::animation> animations,
                Vector2 draw_position_offset = Vector2Zero())
                : animations_(std::move(animations)),
                sprite_texture_(texture),
                draw_position_offset_(draw_position_offset){}
            sprite(const sprite& other) = default;
            sprite(sprite&& other) = default;
            
            sprite& operator=(const sprite& other) = delete;
            sprite& operator=(sprite&& other) = delete;

            animation::animation& get_animation();
            animation::animation& get_animation(size_t index);
            size_t get_animation_index() const;
            size_t num_animations() const;
            void set_animation(size_t index);

            const Texture2D& get_texture();
            Vector2 get_draw_position_offset() const;
            void render(Vector2 position, int frame, Vector2 scale = Vector2One(), float rotation = 0.0f, Color tint = WHITE);
        private:
            // has the texture
            // and the animation
            std::vector<animation::animation> animations_;
            size_t animation_index_ = 0;
            const Texture2D sprite_texture_;
            Vector2 draw_position_offset_;
    };
    class nine_sprite {
    public:
        ~nine_sprite() = default;
        nine_sprite(Rectangle frame, Color tint);
        nine_sprite(const nine_sprite& other) = default;
        nine_sprite(nine_sprite&& other) = default;
    
        nine_sprite& operator=(const nine_sprite& other) = delete;
        nine_sprite& operator=(nine_sprite&& other) = delete;

        void render(Vector2 position, int frame);

#ifdef DOG_DAYS_TESTING
        Rectangle get_frame() const{
            return frame_;
        }
        Vector2 get_scale() const{
            return scale_;
        }
        Color get_tint() const{
            return tint_;
        }
#endif
    private:
        sprite corner_piece_;
        sprite centre_piece_;
        sprite vertical_piece_;
        sprite horizontal_piece_;
        Rectangle frame_;
        Color tint_;
        Vector2 scale_;
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
            Color tint_;
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
        Vector2 draw_position_offset = Vector2Zero());
    sprite::sprite build_cursor_sprite();
    sprite::sprite build_background_sprite();
    // every dog sprite is the same four attribute lookups off a cached texture
    sprite::sprite build_dog_sprite(int texture_key, const char* path,
        const float attributes[entity_config::attributes::size],
        Vector2 draw_position_offset = Vector2Zero());
    sprite::sprite build_dog_part_sprite(int texture_key, const char* path,
        const float attributes[entity_config::attributes::size],
        std::vector<animation::animation> animations,
        Vector2 draw_position_offset = Vector2Zero());
    // one inner vector per dog_sprite_slots entry, each {left, right}
    std::vector<std::vector<sprite::sprite>> build_dog_part_layers(
        const entity_config::dog_part parts[entity_config::dog_sprite_slots_size],
        const int texture_keys[entity_config::dog_sprite_slots_size][entity_config::dog_part_directions_size],
        float expected_total_width);
    std::vector<sprite::sprite> build_gianluca_sprites();
    std::vector<sprite::sprite> build_lionel_sprites();
    // decorations, stations and food all draw off the test_decoration sheet and
    // differ only by their attribute block
    sprite::sprite build_test_decoration_sprite(const float attributes[entity_config::attributes::size]);
    
    // *-------------------- deocration sprites --------------------* //
    sprite::sprite build_decoration_sprite(size_t texture_id, const char* decoration_path, const float attributes[entity_config::attributes::size],
        Vector2 draw_position_offset = Vector2Zero());
    sprite::sprite build_pavlov();
    sprite::sprite build_poker_table();
    sprite::sprite build_dog_painting();
    sprite::sprite build_gargoyle();

    // *-------------------- station sprites --------------------* //
    sprite::sprite build_table_sprite();
    sprite::sprite build_dining_table_sprite();
    sprite::sprite build_tiled_table_sprite();
    sprite::sprite build_food_counter_sprite();
    sprite::sprite build_dishwasher_sprite();
    sprite::sprite build_stove_sprite();
    sprite::sprite build_oven_sprite();
    sprite::sprite build_espresso_station_sprite();
    sprite::sprite build_mini_fridge_sprite();
    sprite::sprite build_bush_sprite();
    sprite::sprite build_chopping_board_sprite();
    sprite::sprite build_food_sprite();
    sprite::sprite build_lasagna_sprite();
    sprite::sprite build_espresso_sprite();
    sprite::sprite build_cutlets_sprite();
    std::vector<sprite::sprite> build_food_sprites();

    sprite::sprite build_nine_corner_piece();
    sprite::sprite build_nine_centre_piece();
    sprite::sprite build_nine_vertical_piece();
    sprite::sprite build_nine_horizontal_piece();
}
#endif
