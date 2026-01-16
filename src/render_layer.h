#ifndef RENDER_LAYER_H
#define RENDER_LAYER_H

#include <vector>
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
            void draw(UnaryPred p, Vector2 frame_position){
                for(auto & entity : entities_){
                    if(p(entity)){

                        auto position = entity->get_position();
                        auto draw_position = Vector2Subtract(entity->get_position(), frame_position);

                        entity->render(draw_position); 
                    }
                }
            }
        private:
            std::vector<entities::entity*> entities_;

    };
}

#endif