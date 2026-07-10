/**
 * entity_builder: factory for every entity kind. The build_* definitions are
 * distributed across the per-class *_builder.cpp translation units.
 */
#ifndef ENTITY_BUILDER_H
#define ENTITY_BUILDER_H

#include "entity.h"
#include "food.h"
#include <optional>

namespace entities{
    class entity_builder{
        public:
            std::unique_ptr<entity> build_cursor(Vector2 position, int id);
            // NPC dog sprite art/config pending.
            std::unique_ptr<entity> build_customer_dog(int id, int dog_type, Vector2 position, std::optional<Vector2> destination = std::nullopt);
            std::unique_ptr<entity> build_food_counter(Vector2 position, int id);
            std::unique_ptr<entity> build_gargoyle(Vector2 position, int id);
            std::unique_ptr<entity> build_khiri(Vector2 position, int id);
            std::unique_ptr<entity> build_mack(Vector2 position, int id);
            std::unique_ptr<entity> build_paw_mark(Vector2 position, int id);
            std::unique_ptr<entity> build_table(Vector2 position, int id);
            std::unique_ptr<entity> build_test_decoration(Vector2 position, int id);
            // basic/first food builder; producers stocking a counter pass
            // counter.get_position() + entity_config::food_draw_offset as the position.
            std::unique_ptr<food> build_test_food(Vector2 position, int id);
            std::unique_ptr<entity> build_waiter_dog(int id, int dog_type, Vector2 position, std::optional<Vector2> destination = std::nullopt);
            ~entity_builder() = default;
            entity_builder() : debug_id_counts_() {}
            entity_builder(const entity_builder& other) = default;
            entity_builder(entity_builder&& other) = default;

            entity_builder& operator=(const entity_builder& other) = default;
            entity_builder& operator=(entity_builder&& other) = default;

        private:
            std::string next_debug_id(const std::string& prefix);
            std::map<std::string, size_t> debug_id_counts_;

    };
    extern entity_builder e_builder;
}
#endif
