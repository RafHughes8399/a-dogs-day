#include "level.h"

void level::level::update(float delta){
    (void) delta;
    return;
}
void level::level::render(){
    // draw the background 
    DrawTextureRec(background_.get_texture(), view_frame_, Vector2{0.0f, 0.0f}, WHITE);
    // draw the entities, based on the view frame
    
    auto bounds = raglib::bounding_box_2{Vector2{view_frame_.x, view_frame_.y}, Vector2{view_frame_.x + view_frame_.width, view_frame_.y +view_frame_.height}};
    auto render_precdicate = [bounds](auto & entity) -> bool {
        return bounds.contains(entity->get_bounds());
    };
    level_entities_.render(render_precdicate);
    return;
}