#include <catch2/catch_test_macros.hpp>

#include <vector>

#include "ecs_test_game.h"
#include "config.h"
#include "sprite.h"

namespace {

    const float base_width = 2 * hud_config::nine_sprite_corner_width + hud_config::nine_sprite_centre_width;
    const float base_height = 2 * hud_config::nine_sprite_corner_height + hud_config::nine_sprite_centre_height;

    const std::vector<float> factors = {1.5f, 2.0f, 2.25f};

    Rectangle scaled_frame(float width_factor, float height_factor){
        return Rectangle{0.0f, 0.0f, base_width * width_factor, base_height * height_factor};
    }

    float centre_width(const sprite::nine_sprite& nine){
        return hud_config::nine_sprite_centre_width * nine.get_scale().x;
    }

    float centre_height(const sprite::nine_sprite& nine){
        return hud_config::nine_sprite_centre_height * nine.get_scale().y;
    }

    bool same_colour(Color left, Color right){
        return left.r == right.r and left.g == right.g and left.b == right.b and left.a == right.a;
    }
}

SCENARIO("a nine sprite at its base dimensions is unscaled", "[hud]"){
    GIVEN("a nine sprite whose frame is two corners plus one centre"){
        testing::ecs_test_game game;
        auto nine = sprite::nine_sprite(scaled_frame(1.0f, 1.0f), WHITE);

        THEN("the frame is kept as given"){
            REQUIRE(nine.get_frame().width == base_width);
            REQUIRE(nine.get_frame().height == base_height);
        }
        THEN("the centre is drawn at its own size"){
            REQUIRE(nine.get_scale().x == 1.0f);
            REQUIRE(nine.get_scale().y == 1.0f);
            REQUIRE(centre_width(nine) == hud_config::nine_sprite_centre_width);
            REQUIRE(centre_height(nine) == hud_config::nine_sprite_centre_height);
        }
    }
}

SCENARIO("a nine sprite stretches its centre to fill a larger frame", "[hud]"){
    GIVEN("frames scaled uniformly from the base dimensions"){
        testing::ecs_test_game game;

        THEN("the frame is the base dimensions times the factor"){
            for(auto factor : factors){
                auto nine = sprite::nine_sprite(scaled_frame(factor, factor), WHITE);
                CAPTURE(factor);
                REQUIRE(nine.get_frame().width == base_width * factor);
                REQUIRE(nine.get_frame().height == base_height * factor);
            }
        }
        THEN("the corners keep their size and the centre takes up the rest"){
            for(auto factor : factors){
                auto nine = sprite::nine_sprite(scaled_frame(factor, factor), WHITE);
                CAPTURE(factor);
                REQUIRE(2 * hud_config::nine_sprite_corner_width + centre_width(nine) == base_width * factor);
                REQUIRE(2 * hud_config::nine_sprite_corner_height + centre_height(nine) == base_height * factor);
            }
        }
        THEN("the centre grows by more than the factor, since the corners do not grow"){
            for(auto factor : factors){
                auto nine = sprite::nine_sprite(scaled_frame(factor, factor), WHITE);
                CAPTURE(factor);
                REQUIRE(nine.get_scale().x > factor);
                REQUIRE(nine.get_scale().y > factor);
            }
        }
        THEN("a square frame scales both axes alike"){
            for(auto factor : factors){
                auto nine = sprite::nine_sprite(scaled_frame(factor, factor), WHITE);
                CAPTURE(factor);
                REQUIRE(nine.get_scale().x == nine.get_scale().y);
            }
        }
    }

    GIVEN("a frame scaled differently on each axis"){
        testing::ecs_test_game game;
        auto nine = sprite::nine_sprite(scaled_frame(1.5f, 2.25f), WHITE);

        THEN("each axis is scaled from its own dimension"){
            REQUIRE(2 * hud_config::nine_sprite_corner_width + centre_width(nine) == base_width * 1.5f);
            REQUIRE(2 * hud_config::nine_sprite_corner_height + centre_height(nine) == base_height * 2.25f);
            REQUIRE(nine.get_scale().x < nine.get_scale().y);
        }
    }
}

SCENARIO("a nine sprite keeps the tint it was built with", "[hud]"){
    GIVEN("nine sprites built with different tints"){
        testing::ecs_test_game game;
        const Color translucent = Color{12, 34, 56, 78};

        auto white = sprite::nine_sprite(scaled_frame(1.0f, 1.0f), WHITE);
        auto red = sprite::nine_sprite(scaled_frame(1.0f, 1.0f), RED);
        auto faded = sprite::nine_sprite(scaled_frame(1.0f, 1.0f), translucent);

        THEN("each holds its own tint, alpha included"){
            REQUIRE(same_colour(white.get_tint(), WHITE));
            REQUIRE(same_colour(red.get_tint(), RED));
            REQUIRE(same_colour(faded.get_tint(), translucent));
        }
        THEN("one tint does not leak into another"){
            REQUIRE_FALSE(same_colour(white.get_tint(), red.get_tint()));
            REQUIRE_FALSE(same_colour(red.get_tint(), faded.get_tint()));
        }
    }

    GIVEN("a tinted nine sprite in a scaled frame"){
        testing::ecs_test_game game;

        THEN("scaling leaves the tint alone"){
            for(auto factor : factors){
                auto nine = sprite::nine_sprite(scaled_frame(factor, factor), RED);
                CAPTURE(factor);
                REQUIRE(same_colour(nine.get_tint(), RED));
            }
        }
    }
}
