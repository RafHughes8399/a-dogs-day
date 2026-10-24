#ifndef COLOUR_REPLACER_H
#define COLOUR_REPLACER_H

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "raylib.h"

namespace colour_replacement{
    class colour_order{
        public:
            bool operator()(const Color& lhs, const Color& rhs) const;
    };
    using colour_map = std::map<Color, Color, colour_order>;

    class colour_replacer{
        public:
            colour_replacer(std::string path, colour_map replacements, Color primary);
            ~colour_replacer();
            colour_replacer(const colour_replacer& other) = delete;
            colour_replacer(colour_replacer&& other) = delete;

            colour_replacer& operator=(const colour_replacer& other) = delete;
            colour_replacer& operator=(colour_replacer&& other) = delete;

            bool is_loaded() const;
            void replace();
            bool export_copy() const;
            std::string get_suffix() const;
            std::string get_output_path() const;
            const Image& get_image() const;
            const Image& get_copy() const;
        private:
            std::string path_;
            colour_map replacements_;
            Color primary_;
            Image image_;
            Image copy_;
    };

    colour_map build_colour_map(const std::vector<Color>& bases, const std::vector<Color>& destinations);
    std::string colour_to_hex(Color colour);
    bool hex_to_colour(const std::string& hex, Color& colour);
    std::string suffixed_path(const std::string& path, const std::string& suffix);

    std::vector<std::string> replace_images(const std::vector<std::string>& paths,
        const std::vector<Color>& bases, const std::vector<Color>& destinations);

    std::vector<std::string> tiled_table_replacement(const std::vector<Color>& destinations);
    std::vector<std::string> khiri_dog_replacement(const std::vector<Color>& destinations);
    std::vector<std::string> mack_dog_replacement(const std::vector<Color>& destinations);
    std::vector<std::string> gianluca_dog_replacement(const std::vector<Color>& destinations);
    std::vector<std::string> lionel_dog_replacement(const std::vector<Color>& destinations);
}
#endif
