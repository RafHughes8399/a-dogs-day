#include "level.h"
#include <iostream>
// --------------------- level ----------------------------------------- //
void level::level::update(float delta){
    level_entities_.update(delta);
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

void level::level::add_entity(std::unique_ptr<entities::entity> entity){
    level_entities_.insert(entity);
}

int level::level::entity_id(){
    return level_entities_.get_next_id();
}
int level::level::num_entities(){
    return level_entities_.size();
}

void level::level::on_left_mouse_event(const events::left_mouse_down& event){
    // cast the event
    // do what you need to do
    auto delta = event.get_mouse_delta();
    auto frame_delta = Vector2Scale(delta, -1);



}
void level::level::on_right_mouse_event(const events::right_mouse_click& event){
    // cast the event
    // do what you need to do
    auto click_position = event.get_mouse_position();
    // create an entity at that position
   // std::unique_ptr<entities::entity> = std::make_unique<>();
   auto paw = entities::e_builder.build_paw_mark(click_position, level_entities_.get_next_id());
   // start playing the paw
   add_entity(std::move(paw));

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

    // append the cursor
    auto cursor = entities::e_builder.build_cursor(GetMousePosition(), l.entity_id());
    l.add_entity(std::move(cursor));
    return l;
}