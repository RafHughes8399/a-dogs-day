#include "colour_replacer.h"

#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

int main(int argc, char** argv){
    const std::map<std::string, std::function<std::vector<std::string>(const std::vector<Color>&)>> replacements{
        {"tiled_table", colour_replacement::tiled_table_replacement},
        {"khiri", colour_replacement::khiri_dog_replacement},
        {"mack", colour_replacement::mack_dog_replacement},
        {"gianluca", colour_replacement::gianluca_dog_replacement},
        {"lionel", colour_replacement::lionel_dog_replacement}};

    if(argc < 3 or replacements.find(argv[1]) == replacements.end()){
        std::cerr << "usage: colour-replacer <";
        for(auto it = replacements.begin(); it != replacements.end(); ++it){
            std::cerr << (it == replacements.begin() ? "" : "|") << it->first;
        }
        std::cerr << "> RRGGBB[AA] [RRGGBB[AA] ...]\n";
        return 1;
    }

    std::vector<Color> destinations;
    for(int i = 2; i < argc; ++i){
        Color colour{};
        if(not colour_replacement::hex_to_colour(argv[i], colour)){
            std::cerr << "not a colour: " << argv[i] << "\n";
            return 1;
        }
        destinations.push_back(colour);
    }

    SetTraceLogLevel(LOG_WARNING);
    auto written = replacements.at(argv[1])(destinations);
    for(const auto& path : written){
        std::cout << path << "\n";
    }
    return written.empty() ? 1 : 0;
}
