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

            void draw();

        private:
            std::vector<entities::entity*> entities_;

    };
}

#endif