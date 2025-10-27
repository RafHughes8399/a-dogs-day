#include "texture.h"
#include <iostream>
// define the global texture map, init it with empty textures, loaded upon first request of the texture
// should ensure that texture is loaded once and used many times 
textures::textures_map textures::textures_ = textures::textures_map();

// ------------------------- textures ------------------------- //
Texture2D textures::texture::get_texture(){
    return texture_;
}
// ------------------------- texture map ------------------------- //

bool textures::textures_map::check_texture(int texture_id){
    return loaded_textures_[texture_id] ? true : false;
}
Texture2D textures::textures_map::get_texture(int texture_id, const char * texture_path){
    if(! check_texture(texture_id)){
        load_texture(texture_id, LoadTexture(texture_path));
    }

    return loaded_textures_[texture_id]->get_texture();
}
void textures::textures_map::load_texture(int texture_id, Texture2D texture){
    loaded_textures_[texture_id] = std::make_unique<textures::texture>(texture);
    return;
}