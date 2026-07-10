/**
 * food entity.
 * food is a generic world entity. Different foods are produced by different
 * builders (see build_test_food); subclasses are introduced only as-needed.
 */
#ifndef FOOD_H
#define FOOD_H

#include "entity.h"

namespace entities{
    class food : public entity {
        public:
            food(body::body body, Vector2 position, int id, std::string debug_id)
            : entity(body, position, id, std::move(debug_id)){}
            food(const food& other) = default;
            food(food&& other) = default;

            food& operator=(const food& other) = delete;
            food& operator=(food&& other) = delete;
    };
}
#endif
