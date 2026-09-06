#ifndef TEXTURE_H
#define TEXTURE_H


#include <map>
#include <memory>
#include "config.h"
#include "raylib.h"
namespace textures{
    //inline const Texture2D background_texture_ = LoadTexture(config::background_path);
    // could use the singleton pattern

    // or have some map, pairs an id to a texture
    // query the existance of the texutre

    // like the  entity builder would go get_texutre(texture::paw)

    // if already loaded then the texture is returned, otherwise it is laoded and returned
    enum texture_keys{
        cursor = 0,
        paw_mark,
        khiri_left,
        khiri_right,
        khiri_left_out,
        khiri_right_out,
        khiri_head_left,
        khiri_head_right,
        khiri_face_left,
        khiri_face_right,
        khiri_body_left,
        khiri_body_right,
        khiri_tail_left,
        khiri_tail_right,
        mack_left,
        mack_right,
        mack_left_out,
        mack_right_out,
        mack_head_left,
        mack_head_right,
        mack_face_left,
        mack_face_right,
        mack_body_left,
        mack_body_right,
        mack_tail_left,
        mack_tail_right,
        gianluca_left,
        gianluca_right,
        gianluca_head_left,
        gianluca_head_right,
        gianluca_face_left,
        gianluca_face_right,
        gianluca_body_left,
        gianluca_body_right,
        gianluca_tail_left,
        gianluca_tail_right,
        lionel_left,
        lionel_right,
        lionel_head_left,
        lionel_head_right,
        lionel_face_left,
        lionel_face_right,
        lionel_body_left,
        lionel_body_right,
        lionel_tail_left,
        lionel_tail_right,
        test_decoration,
        hud_edit_wheel,
        gargoyle_void,
        gargoyle_sick_of_it,
        poker_table,
        dog_painting,
        dining_table,
        food_counter,
        lasagna,
        coffee,
        background,
        size
        // and so on
    };
    // * parallel to entity_config::<dog>_parts - the keys live here rather than in
    // * the config table because texture.h includes config.h, not the reverse
    inline const int khiri_part_keys[entity_config::dog_sprite_slots_size][entity_config::dog_part_directions_size] = {
        {khiri_head_left, khiri_head_right},
        {khiri_face_left, khiri_face_right},
        {khiri_body_left, khiri_body_right},
        {khiri_tail_left, khiri_tail_right}};
    inline const int mack_part_keys[entity_config::dog_sprite_slots_size][entity_config::dog_part_directions_size] = {
        {mack_head_left, mack_head_right},
        {mack_face_left, mack_face_right},
        {mack_body_left, mack_body_right},
        {mack_tail_left, mack_tail_right}};
    inline const int gianluca_part_keys[entity_config::dog_sprite_slots_size][entity_config::dog_part_directions_size] = {
        {gianluca_head_left, gianluca_head_right},
        {gianluca_face_left, gianluca_face_right},
        {gianluca_body_left, gianluca_body_right},
        {gianluca_tail_left, gianluca_tail_right}};
    inline const int lionel_part_keys[entity_config::dog_sprite_slots_size][entity_config::dog_part_directions_size] = {
        {lionel_head_left, lionel_head_right},
        {lionel_face_left, lionel_face_right},
        {lionel_body_left, lionel_body_right},
        {lionel_tail_left, lionel_tail_right}};
    class texture{
        public:
            ~texture() = default;
            texture(Texture2D texture)
            : texture_(texture) {}
            texture(const texture& other) = default;
            texture(texture&& other) = default;

            texture& operator=(const texture& other) = default;
            texture& operator=(texture&& other) = default;

            Texture2D get_texture();

        private:
            Texture2D texture_;
    };

    class textures_map{
        public:
            ~textures_map() = default;
            // fill the map with the keys but empty textures, they are loaded when first used (for now (27/10) by the entity builder and level only )
            textures_map(){
                for(int i = 0; i < texture_keys::size; ++i){
                    loaded_textures_.insert({i, nullptr});
                }
            }
            textures_map(const textures_map& other) = default;
            textures_map(textures_map&& other) = default;

            textures_map& operator=(const textures_map& other) = default;
            textures_map& operator=(textures_map&& other) = default;

            bool check_texture(int texture_id);
            Texture2D get_texture(int texture_id, const char* texture_path);
            void load_texture(int texture_id, Texture2D);            


        private:
            std::map<int, std::unique_ptr<texture>> loaded_textures_;
    };
    extern textures_map textures_;
}
#endif
