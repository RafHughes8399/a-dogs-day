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
            void draw(UnaryPred p){
                for(auto & entity : entities_){
                    if(p(entity)){
                        std::cout << "fits " << std::endl;
                        entity->render(); 
                    }
                }
            }
        private:
            std::vector<entities::entity*> entities_;

    };
}

#endif