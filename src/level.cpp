#include "level.h"
#include <iostream>
// --------------------- level ----------------------------------------- //
void level::level::update(float delta){
    (void) delta;
    return;
}
void level::level::render(){
    // draw the background 
    DrawTextureRec(background_.get_texture(), view_frame_, Vector2{0.0f, 0.0f}, WHITE);
    // draw the entities, based on the view frame
    
    auto bounds = raglib::bounding_box_2{Vector2{view_frame_.x, view_frame_.y}, 
    Vector2{view_frame_.x + view_frame_.width, view_frame_.y +view_frame_.height}};
    auto render_precdicate = [bounds](auto & entity) -> bool {
        return bounds.contains(entity->get_bounds());
    };
    level_entities_.render(render_precdicate);
    return;
}
// --------------------- level builder ----------------------------------------- //
level::level level::level_builder::build_main_level(){
    auto background = sprite::sprite(LoadTexture(config::background_path), 
        config::background_attributes[config::attributes::frame_width], 
        config::background_attributes[config::attributes::frame_height],
        config::background_attributes[config::attributes::frames],
        config::background_attributes[config::attributes::animations]);
                
    auto view_frame = Rectangle{0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
    auto dimensions = Vector2{config::world_x, config::world_y};

    auto l = level(background, view_frame, dimensions);
    std::cout << "built level "<< std::endl;
    return l;
}