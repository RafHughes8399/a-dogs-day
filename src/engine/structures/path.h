#ifndef PATH_H
#define PATH_H

#include <vector>
#include <optional>
#include "raylib.h"
#include "raymath.h"
namespace path{
    class path{
        public:
            ~path() = default;
            path(Vector2 source, Vector2 destination, std::vector<Vector2> nodes, std::optional<size_t> destination_entity = std::nullopt)
            : source_(source), destination_(destination), nodes_(nodes), destination_entity_(destination_entity){}

            path(const path& other) =default;
            path(path&& other) = default;

            path& operator=(const path& other) = default;
            path& operator=(path&& other) = default;
            
        private:
            Vector2 source_;
            Vector2 destination_;
            std::vector<Vector2> nodes_;
            std::optional<size_t> destination_entity_;
    };
}
#endif