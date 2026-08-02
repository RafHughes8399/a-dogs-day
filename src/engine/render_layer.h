#ifndef RENDER_LAYER_H
#define RENDER_LAYER_H

#include <algorithm>
#include <vector>
#include "component.h"
#include "entities.h"
namespace render_layer{
    class layer{
        public:
            ~layer() = default;
            layer() {}
            layer(const layer& other) = default;
            layer(layer&& other) = default;

            layer& operator=(const layer& other) = default;
            layer& operator=(layer&& other) = default;

            template<typename P> // P for predicate
            void render(P p);
            
            void render(){
                render(true);
            }

            void add_entity(entities::entity* entity);
            void remove_entity(entities::entity* entity);
            void remove_entities(std::vector<int> entity_ids);

            template<typename UnaryPred>
            void draw(UnaryPred p, Vector2 frame_position, int frame){
                for(auto & entity : entities_){
                    if(p(entity)){
                        auto draw_position = Vector2Subtract(entity->get_position(), frame_position);
                        entity->render(draw_position, frame); 
                    }
                }
            }
        private:
            std::vector<entities::entity*> entities_;

    };

    // TODO: RENAME AFTER REFACTOR IS COMPLETE - replaces layer once level is gone.
    // same shape as layer, holding ids - a stale entry is skipped via a null
    // component lookup rather than dereferenced
    class ecs_layer{
        public:
            ~ecs_layer() = default;
            ecs_layer() = default;
            ecs_layer(const ecs_layer& other) = default;
            ecs_layer(ecs_layer&& other) = default;

            ecs_layer& operator=(const ecs_layer& other) = default;
            ecs_layer& operator=(ecs_layer&& other) = default;

            void add_entity(size_t entity_id);
            void remove_entity(size_t entity_id);
            void remove_entities(const std::vector<size_t>& entity_ids);

            template<typename UnaryPred>
            void draw(UnaryPred p, Vector2 frame_position, int frame){
                for(auto entity_id : entities_){
                    auto* renderable = component_managers::renderable_manager_.get_component(entity_id);
                    auto* position = component_managers::positional_manager_.get_component(entity_id);
                    if(renderable == nullptr || position == nullptr){ continue; }
                    if(!p(entity_id)){ continue; }

                    auto draw_position = Vector2Subtract(position->get_position(), frame_position);
                    // body, outlines and cosmetics all draw at the same position
                    for(auto & sprite_component : renderable->get_sprites()){
                        sprite_component.get_sprite().render(draw_position, frame);
                    }
                }
            }
#ifdef DOG_DAYS_TESTING
            size_t size() const{
                return entities_.size();
            }
            bool contains(size_t entity_id) const{
                return std::find(entities_.begin(), entities_.end(), entity_id) != entities_.end();
            }
#endif
        private:
            std::vector<size_t> entities_;
    };
}

#endif