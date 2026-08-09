#ifndef PATH_H
#define PATH_H

#include <vector>
#include <optional>
#include "raylib.h"
#include "raymath.h"
namespace path{
    enum assignment{
        replace = 0,
        append = 1
    };
    class path{
        public:
            ~path() = default;
            path(Vector2 source, Vector2 destination, std::vector<Vector2> positions, std::optional<size_t> destination_entity = std::nullopt)
            : source_(source), destination_(destination), positions_(positions), destination_entity_(destination_entity){}

            path(const path& other) =default;
            path(path&& other) = default;

            path& operator=(const path& other) = default;
            path& operator=(path&& other) = default;

            friend bool operator==(const path& a, const path& b){
                return Vector2Equals(a.source_, b.source_) and Vector2Equals(a.destination_, b.destination_) and 
                    a.positions_ == b.positions_ and a.destination_entity_ == b.destination_entity_;
            }
            Vector2 get_source();
            Vector2 get_destination();
            Vector2 get_next_position();
            bool is_path_complete();
            void advance();
        private:
            Vector2 source_;
            Vector2 destination_;
            std::vector<Vector2> positions_;
            std::optional<size_t> destination_entity_;
    };
    path build_path(Vector2 source, Vector2 destination, std::vector<Vector2> positions, std::optional<size_t> destination_entity = std::nullopt);
    inline path empty_path = path(Vector2Zero(), Vector2Zero(), {});
}
#endif