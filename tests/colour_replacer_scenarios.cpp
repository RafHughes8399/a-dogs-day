#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <string>
#include <vector>

#include "colour_replacer.h"
#include "raylib.h"

namespace {
    bool same_colour(Color lhs, Color rhs){
        return lhs.r == rhs.r and lhs.g == rhs.g and lhs.b == rhs.b and lhs.a == rhs.a;
    }

    std::string write_two_colour_image(const std::filesystem::path& directory, Color left, Color right){
        Image image = GenImageColor(4, 2, left);
        ImageDrawPixel(&image, 2, 0, right);
        ImageDrawPixel(&image, 3, 0, right);
        ImageDrawPixel(&image, 2, 1, right);
        ImageDrawPixel(&image, 3, 1, right);
        auto path = (directory / "source.png").string();
        ExportImage(image, path.c_str());
        UnloadImage(image);
        return path;
    }
}

SCENARIO("a colour map pairs the known bases with the given destinations in order"){
    const std::vector<Color> bases{RED, GREEN, BLUE};

    GIVEN("as many destinations as bases"){
        auto map = colour_replacement::build_colour_map(bases, {YELLOW, PURPLE, ORANGE});
        THEN("every base maps to the destination at its position"){
            REQUIRE(map.size() == 3);
            REQUIRE(same_colour(map.at(RED), YELLOW));
            REQUIRE(same_colour(map.at(GREEN), PURPLE));
            REQUIRE(same_colour(map.at(BLUE), ORANGE));
        }
    }
    GIVEN("fewer destinations than bases"){
        auto map = colour_replacement::build_colour_map(bases, {YELLOW});
        THEN("only the leading bases are replaced"){
            REQUIRE(map.size() == 1);
            REQUIRE(same_colour(map.at(RED), YELLOW));
            REQUIRE(map.find(GREEN) == map.end());
        }
    }
}

SCENARIO("colours read and write as hex"){
    Color colour{};
    THEN("six digits are opaque, eight carry alpha, and a leading # is allowed"){
        REQUIRE(colour_replacement::hex_to_colour("ff8800", colour));
        REQUIRE(same_colour(colour, Color{255, 136, 0, 255}));
        REQUIRE(colour_replacement::hex_to_colour("#10203040", colour));
        REQUIRE(same_colour(colour, Color{16, 32, 48, 64}));
        REQUIRE(colour_replacement::colour_to_hex(Color{255, 136, 0, 255}) == "ff8800");
    }
    THEN("anything else is rejected"){
        REQUIRE_FALSE(colour_replacement::hex_to_colour("ff88", colour));
        REQUIRE_FALSE(colour_replacement::hex_to_colour("gg8800", colour));
    }
}

SCENARIO("the output path is the source path with the primary colour as a suffix"){
    REQUIRE(colour_replacement::suffixed_path("../sprites/tiled-table.png", "ff8800") == "../sprites/tiled-table_ff8800.png");
    REQUIRE(colour_replacement::suffixed_path("../sprites.v2/table", "ff8800") == "../sprites.v2/table_ff8800");
}

SCENARIO("a replacer recolours a copy of the image and exports it beside the source"){
    SetTraceLogLevel(LOG_WARNING);
    auto directory = std::filesystem::temp_directory_path() / "dog_days_colour_replacer";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    auto source = write_two_colour_image(directory, RED, BLUE);

    GIVEN("a map from both source colours"){
        colour_replacement::colour_replacer replacer(source,
            colour_replacement::build_colour_map({RED, BLUE}, {YELLOW, GREEN}), YELLOW);
        REQUIRE(replacer.is_loaded());

        WHEN("it replaces"){
            replacer.replace();
            THEN("the copy holds the destinations and the original is untouched"){
                REQUIRE(same_colour(GetImageColor(replacer.get_copy(), 0, 0), YELLOW));
                REQUIRE(same_colour(GetImageColor(replacer.get_copy(), 3, 1), GREEN));
                REQUIRE(same_colour(GetImageColor(replacer.get_image(), 0, 0), RED));
                REQUIRE(same_colour(GetImageColor(replacer.get_image(), 3, 1), BLUE));
            }
            AND_WHEN("it exports"){
                REQUIRE(replacer.export_copy());
                THEN("the file sits beside the source, named after the primary destination"){
                    auto expected = (directory / ("source_" + colour_replacement::colour_to_hex(YELLOW) + ".png")).string();
                    REQUIRE(replacer.get_output_path() == expected);
                    REQUIRE(std::filesystem::exists(expected));
                    Image written = LoadImage(expected.c_str());
                    REQUIRE(same_colour(GetImageColor(written, 0, 0), YELLOW));
                    REQUIRE(same_colour(GetImageColor(written, 3, 1), GREEN));
                    UnloadImage(written);
                }
            }
        }
    }
    GIVEN("a path that does not exist"){
        colour_replacement::colour_replacer replacer((directory / "missing.png").string(),
            colour_replacement::build_colour_map({RED}, {YELLOW}), YELLOW);
        THEN("nothing loads and nothing is exported"){
            REQUIRE_FALSE(replacer.is_loaded());
            replacer.replace();
            REQUIRE_FALSE(replacer.export_copy());
        }
    }
    GIVEN("several images replaced at once"){
        auto written = colour_replacement::replace_images({source, (directory / "missing.png").string()},
            {RED, BLUE}, {YELLOW, GREEN});
        THEN("only the images that loaded are written"){
            REQUIRE(written.size() == 1);
            REQUIRE(std::filesystem::exists(written.front()));
        }
    }
    GIVEN("no destinations"){
        THEN("nothing is written"){
            REQUIRE(colour_replacement::replace_images({source}, {RED}, {}).empty());
        }
    }
    std::filesystem::remove_all(directory);
}
