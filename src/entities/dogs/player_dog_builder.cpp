#include "entities.h"
#include "texture.h"
#include "debug_log_interface.h"
#include "raglib.h"
#include <vector>
// ------------------------------- builder ------------------------------- //
std::unique_ptr<entities::entity> entities::entity_builder::build_khiri(Vector2 position, int id){
    auto khiri_left_texture = textures::textures_.get_texture(textures::khiri_left, entity_config::khiri_left_path);
    auto khiri_right_texture = textures::textures_.get_texture(textures::khiri_right, entity_config::khiri_right_path);

    auto khiri_left_outline_texture = textures::textures_.get_texture(textures::khiri_left_out, entity_config::khiri_left_outline_path);
    auto khiri_right_outline_texture = textures::textures_.get_texture(textures::khiri_right_out, entity_config::khiri_right_outline_path);

    auto khiri_left_sprite = sprite::sprite(khiri_left_texture,
        entity_config::khiri_across_attributes[entity_config::attributes::frame_width],
        entity_config::khiri_across_attributes[entity_config::attributes::frame_height],
        entity_config::khiri_across_attributes[entity_config::attributes::frames],
        entity_config::khiri_across_attributes[entity_config::attributes::animations]);

    auto khiri_right_sprite = sprite::sprite(khiri_right_texture,
        entity_config::khiri_across_attributes[entity_config::attributes::frame_width],
        entity_config::khiri_across_attributes[entity_config::attributes::frame_height],
        entity_config::khiri_across_attributes[entity_config::attributes::frames],
        entity_config::khiri_across_attributes[entity_config::attributes::animations]);

    auto khiri_left_outline_sprite = sprite::sprite(khiri_left_outline_texture,
        entity_config::khiri_across_attributes[entity_config::attributes::frame_width],
        entity_config::khiri_across_attributes[entity_config::attributes::frame_height],
        entity_config::khiri_across_attributes[entity_config::attributes::frames],
        entity_config::khiri_across_attributes[entity_config::attributes::animations]);

    auto khiri_right_outline_sprite = sprite::sprite(khiri_right_outline_texture,
        entity_config::khiri_across_attributes[entity_config::attributes::frame_width],
        entity_config::khiri_across_attributes[entity_config::attributes::frame_height],
        entity_config::khiri_across_attributes[entity_config::attributes::frames],
        entity_config::khiri_across_attributes[entity_config::attributes::animations]);


    auto across_hitbox = hitbox_builders::build_dog_across_hitbox(position);

    // ! up down left right
    std::vector<sprite::sprite> khiri_sprites;
    khiri_sprites.push_back(std::move(khiri_left_sprite));
    khiri_sprites.push_back(std::move(khiri_right_sprite));

    std::vector<sprite::sprite> khiri_outlines;
    khiri_outlines.push_back(std::move(khiri_left_outline_sprite));
    khiri_outlines.push_back(std::move(khiri_right_outline_sprite));

    std::vector<hitbox::hitbox> khiri_hitboxes;
    khiri_hitboxes.push_back(across_hitbox);
    khiri_hitboxes.push_back(across_hitbox);

    auto body = body::body(khiri_hitboxes, khiri_sprites);
    // Head sprite art pending.
    // auto khiri_head_left_texture = textures::textures_.get_texture(textures::khiri_head_left, entity_config::khiri_head_left_path);
    // auto khiri_head_right_texture = textures::textures_.get_texture(textures::khiri_head_right, entity_config::khiri_head_right_path);
    // auto khiri_head_left_sprite = sprite::sprite(khiri_head_left_texture,
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::frame_width],
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::frame_height],
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::frames],
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::animations],
    //     entity_config::khiri_head_left_offset);
    // auto khiri_head_right_sprite = sprite::sprite(khiri_head_right_texture,
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::frame_width],
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::frame_height],
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::frames],
    //     entity_config::khiri_head_across_attributes[entity_config::attributes::animations],
    //     entity_config::khiri_head_right_offset);
    // std::vector<sprite::sprite> khiri_head_sprites;
    // khiri_head_sprites.push_back(std::move(khiri_head_left_sprite));
    // khiri_head_sprites.push_back(std::move(khiri_head_right_sprite));
    // std::vector<hitbox::hitbox> khiri_head_hitboxes;
    // khiri_head_hitboxes.push_back(across_hitbox);
    // khiri_head_hitboxes.push_back(across_hitbox);
    // auto head = body::body(khiri_head_hitboxes, khiri_head_sprites);
    auto head = body::body(); // TODO fill !
    return std::make_unique<entities::player_dog>(
        std::move(body),
        std::move(head),
        std::move(khiri_outlines),
        position,
        id,
        next_debug_id(entity_config::player_dog_debug_id_prefix));
}
std::unique_ptr<entities::entity> entities::entity_builder::build_mack(Vector2 position, int id){
    auto mack_left_texture = textures::textures_.get_texture(textures::mack_left, entity_config::mack_left_path);
    auto mack_right_texture = textures::textures_.get_texture(textures::mack_right, entity_config::mack_right_path);

   auto mack_left_outline_texture = textures::textures_.get_texture(textures::mack_left_out, entity_config::mack_left_outline_path);
   auto mack_right_outline_texture = textures::textures_.get_texture(textures::mack_right_out, entity_config::mack_right_outline_path);


    auto mack_left_sprite = sprite::sprite(mack_left_texture,
        entity_config::mack_across_attributes[entity_config::attributes::frame_width],
        entity_config::mack_across_attributes[entity_config::attributes::frame_height],
        entity_config::mack_across_attributes[entity_config::attributes::frames],
        entity_config::mack_across_attributes[entity_config::attributes::animations]);

    auto mack_right_sprite = sprite::sprite(mack_right_texture,
        entity_config::mack_across_attributes[entity_config::attributes::frame_width],
        entity_config::mack_across_attributes[entity_config::attributes::frame_height],
        entity_config::mack_across_attributes[entity_config::attributes::frames],
        entity_config::mack_across_attributes[entity_config::attributes::animations]);

    auto mack_left_outline_sprite = sprite::sprite(mack_left_outline_texture,
        entity_config::mack_across_attributes[entity_config::attributes::frame_width],
        entity_config::mack_across_attributes[entity_config::attributes::frame_height],
        entity_config::mack_across_attributes[entity_config::attributes::frames],
        entity_config::mack_across_attributes[entity_config::attributes::animations]);

    auto mack_right_outline_sprite = sprite::sprite(mack_right_outline_texture,
        entity_config::mack_across_attributes[entity_config::attributes::frame_width],
        entity_config::mack_across_attributes[entity_config::attributes::frame_height],
        static_cast<int>(entity_config::mack_across_attributes[entity_config::attributes::frames]),
        static_cast<int>(entity_config::mack_across_attributes[entity_config::attributes::animations]));

    auto across_hitbox = hitbox_builders::build_dog_across_hitbox(position);

    // ! up down left right
    std::vector<sprite::sprite> mack_sprites;
    mack_sprites.push_back(std::move(mack_left_sprite));
    mack_sprites.push_back(std::move(mack_right_sprite));

    std::vector<sprite::sprite> mack_outlines;
    mack_outlines.push_back(std::move(mack_left_outline_sprite));
    mack_outlines.push_back(std::move(mack_right_outline_sprite));

    std::vector<hitbox::hitbox> mack_hitboxes;
    mack_hitboxes.push_back(across_hitbox);
    mack_hitboxes.push_back(across_hitbox);

    auto body = body::body(mack_hitboxes, mack_sprites);
    // Head sprite art pending.
    // auto mack_head_left_texture = textures::textures_.get_texture(textures::mack_head_left, entity_config::mack_head_left_path);
    // auto mack_head_right_texture = textures::textures_.get_texture(textures::mack_head_right, entity_config::mack_head_right_path);
    // auto mack_head_left_sprite = sprite::sprite(mack_head_left_texture,
    //     entity_config::mack_head_across_attributes[entity_config::attributes::frame_width],
    //     entity_config::mack_head_across_attributes[entity_config::attributes::frame_height],
    //     entity_config::mack_head_across_attributes[entity_config::attributes::frames],
    //     entity_config::mack_head_across_attributes[entity_config::attributes::animations],
    //     entity_config::mack_head_left_offset);
    // auto mack_head_right_sprite = sprite::sprite(mack_head_right_texture,
    //     entity_config::mack_head_across_attributes[entity_config::attributes::frame_width],
    //     entity_config::mack_head_across_attributes[entity_config::attributes::frame_height],
    //     entity_config::mack_head_across_attributes[entity_config::attributes::frames],
    //     entity_config::mack_head_across_attributes[entity_config::attributes::animations],
    //     entity_config::mack_head_right_offset);
    // std::vector<sprite::sprite> mack_head_sprites;
    // mack_head_sprites.push_back(std::move(mack_head_left_sprite));
    // mack_head_sprites.push_back(std::move(mack_head_right_sprite));
    // std::vector<hitbox::hitbox> mack_head_hitboxes;
    // mack_head_hitboxes.push_back(across_hitbox);
    // mack_head_hitboxes.push_back(across_hitbox);
    // auto head = body::body(mack_head_hitboxes, mack_head_sprites);
    auto head = body::body(); // TODO fill ! and build the head sprites and hitboxes

    // and build the head, pending
    return std::make_unique<entities::player_dog>(
        std::move(body),
        std::move(head),
        std::move(mack_outlines),
        position,
        id,
        next_debug_id(entity_config::player_dog_debug_id_prefix));
}
