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
        paw_mark = 1,
        khiri_left = 2,
        khiri_right = 3,
        khiri_left_out = 4,
        khiri_right_out = 5,
        mack_left = 6,
        mack_right = 7,
        mack_up = 8,
        mack_down = 9,
        mack_left_out = 10,
        mack_right_out = 11,
        test_decoration = 12,
        hud_edit_wheel = 13,
        gargoyle_void = 14,
        gargoyle_sick_of_it = 15,
        // Head sprite art pending.
        // khiri_head_left = 16,
        // khiri_head_right = 17,
        // mack_head_left = 18,
        // mack_head_right = 19,
        // NPC dog sprite art pending.
        // npc_dog_left = 20,
        // npc_dog_right = 21,
        // npc_dog_head_left = 22,
        // npc_dog_head_right = 23,
        background = 16,
        size = 17
        // and so on
    };
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
