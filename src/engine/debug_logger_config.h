#ifndef DEBUG_LOGGER_CONFIG_H
#define DEBUG_LOGGER_CONFIG_H

#include "raylib.h"
#include <cstddef>

namespace debug_logger_config{
    inline const char* log_directory = "../debug_logs";
    inline const float logger_height_ratio = 0.6f;
    inline const float logger_y_position_scalar = 1.0f - logger_height_ratio;
    inline const int backdrop_opacity = 102;
    inline const Color backdrop = Color{28, 28, 28, backdrop_opacity};
    inline const Color text = Color{245, 240, 225, 255};
    // max_messages lines at line_height must fit inside the backdrop, which is
    // logger_height_ratio of the screen less padding_y top and bottom
    inline const int font_size = 20;
    inline const int line_height = 30;
    inline const int padding_x = 18;
    inline const int padding_y = 16;
    inline const size_t max_messages = 20;
}
#endif
