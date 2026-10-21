#ifndef COLOUR_CONFIG_H
#define COLOUR_CONFIG_H

#include "raylib.h"
#include <vector>

namespace colour_config{
    inline const std::vector<Color> tiled_table_base_colours = {
        GetColor(0x000000ffu), GetColor(0xf5f5f5ffu), GetColor(0xeeeef4ffu)};
    inline const std::vector<Color> tiled_table_orange_colours = {
        GetColor(0xfcf7dcffu), GetColor(0x7a3d02ffu), GetColor(0x733a02ffu)};
    inline const std::vector<Color> tiled_table_red_colours = {
        GetColor(0xfcf7dcffu), GetColor(0x660701ffu), GetColor(0x630802ffu)};
    inline const std::vector<Color> tiled_table_green_colours = {
        GetColor(0xfcf7dcffu), GetColor(0x1d3303ffu), GetColor(0x1a2f03ffu)};
    inline const std::vector<Color> tiled_table_blue_colours = {
        GetColor(0xfcf7dcffu), GetColor(0x042f61ffu), GetColor(0x032b59ffu)};
    inline const std::vector<Color> khiri_base_colours = {
        GetColor(0x88673effu), GetColor(0xafa38affu), GetColor(0x6c491fffu), GetColor(0x7d5d35ffu), GetColor(0x8a5a09ffu), GetColor(0x825c2dffu),
        GetColor(0x80560fffu), GetColor(0xcfc4abffu), GetColor(0x84570cffu), GetColor(0x825f33ffu), GetColor(0x825b19ffu), GetColor(0x8a5f2affu),
        GetColor(0x915e07ffu), GetColor(0xbaae94ffu), GetColor(0xc2b393ffu), GetColor(0x72522cffu), GetColor(0x5f3f19ffu), GetColor(0x866121ffu),
        GetColor(0x1c4aa9ffu), GetColor(0x8a6019ffu), GetColor(0x8f6012ffu), GetColor(0x303030ffu), GetColor(0x16429cffu), GetColor(0x063697ffu),
        GetColor(0x754e1fffu), GetColor(0x5b3a14ffu), GetColor(0xa0780affu), GetColor(0x704d23ffu), GetColor(0x9c7b44ffu), GetColor(0x8f6b2dffu)};
    inline const std::vector<Color> mack_base_colours = {
        GetColor(0xe7eef6ffu), GetColor(0xe0e5e9ffu), GetColor(0xd8e0e9ffu), GetColor(0xf0f3f6ffu), GetColor(0x88755effu), GetColor(0x917c64ffu),
        GetColor(0x6a5741ffu), GetColor(0x4f4545ffu), GetColor(0x72604bffu), GetColor(0x806d57ffu), GetColor(0x655a5affu), GetColor(0xd7cdcdffu),
        GetColor(0x4b4040ffu), GetColor(0x776a6affu), GetColor(0xd7dee5ffu), GetColor(0x807373ffu), GetColor(0x353535ffu), GetColor(0x6e6363ffu),
        GetColor(0x956936ffu), GetColor(0x5c5252ffu), GetColor(0xd3ceceffu), GetColor(0xb48b8bffu), GetColor(0xbdb8b8ffu), GetColor(0x2c2b2bffu),
        GetColor(0x84623effu), GetColor(0x84623cffu), GetColor(0x7b5b38ffu)};
    inline const std::vector<Color> gianluca_base_colours = {
        GetColor(0x2b2424ffu), GetColor(0x221b1bffu), GetColor(0x998282ffu), GetColor(0xe7eef6ffu), GetColor(0xe4eefaffu)};
    inline const std::vector<Color> lionel_base_colours = {
        GetColor(0x481b0affu), GetColor(0x53210fffu), GetColor(0x4f1905ffu), GetColor(0x5a2b1affu), GetColor(0x642a15ffu), GetColor(0x60240effu),
        GetColor(0x121413ffu), GetColor(0x4f2312ffu)};
}
#endif
