#include "colour_replacer.h"
#include "config.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <utility>

namespace {
    std::uint32_t pack(const Color& colour){
        return (static_cast<std::uint32_t>(colour.r) << 24) | (static_cast<std::uint32_t>(colour.g) << 16)
            | (static_cast<std::uint32_t>(colour.b) << 8) | static_cast<std::uint32_t>(colour.a);
    }

    std::vector<std::string> dog_part_paths(const entity_config::dog_part (&parts)[entity_config::dog_sprite_slots_size]){
        std::vector<std::string> paths;
        for(const auto& part : parts){
            paths.emplace_back(part.left_path);
            paths.emplace_back(part.right_path);
        }
        return paths;
    }
}

bool colour_replacement::colour_order::operator()(const Color& lhs, const Color& rhs) const{
    return pack(lhs) < pack(rhs);
}

colour_replacement::colour_replacer::colour_replacer(std::string path, colour_map replacements, Color primary)
: path_(std::move(path)), replacements_(std::move(replacements)), primary_(primary),
  image_(LoadImage(path_.c_str())), copy_(ImageCopy(image_)){}

colour_replacement::colour_replacer::~colour_replacer(){
    UnloadImage(copy_);
    UnloadImage(image_);
}

bool colour_replacement::colour_replacer::is_loaded() const{
    return IsImageValid(image_) and IsImageValid(copy_);
}

void colour_replacement::colour_replacer::replace(){
    if(not is_loaded()){ return; }
    for(const auto& [base, destination] : replacements_){
        ImageColorReplace(&copy_, base, destination);
    }
}

bool colour_replacement::colour_replacer::export_copy() const{
    if(not is_loaded()){ return false; }
    return ExportImage(copy_, get_output_path().c_str());
}

std::string colour_replacement::colour_replacer::get_suffix() const{
    return colour_to_hex(primary_);
}

std::string colour_replacement::colour_replacer::get_output_path() const{
    return suffixed_path(path_, get_suffix());
}

const Image& colour_replacement::colour_replacer::get_image() const{
    return image_;
}

const Image& colour_replacement::colour_replacer::get_copy() const{
    return copy_;
}

colour_replacement::colour_map colour_replacement::build_colour_map(const std::vector<Color>& bases,
    const std::vector<Color>& destinations){
    colour_map replacements;
    auto count = std::min(bases.size(), destinations.size());
    for(size_t i = 0; i < count; ++i){
        replacements.insert({bases[i], destinations[i]});
    }
    return replacements;
}

std::string colour_replacement::colour_to_hex(Color colour){
    char buffer[7];
    std::snprintf(buffer, sizeof(buffer), "%02x%02x%02x", colour.r, colour.g, colour.b);
    return std::string(buffer);
}

bool colour_replacement::hex_to_colour(const std::string& hex, Color& colour){
    auto digits = hex;
    if(not digits.empty() and digits.front() == '#'){
        digits.erase(digits.begin());
    }
    if(digits.size() != 6 and digits.size() != 8){ return false; }
    if(not std::all_of(digits.begin(), digits.end(), [](char c){ return std::isxdigit(static_cast<unsigned char>(c)) != 0; })){
        return false;
    }
    if(digits.size() == 6){
        digits += "ff";
    }
    colour = GetColor(static_cast<unsigned int>(std::stoul(digits, nullptr, 16)));
    return true;
}

std::string colour_replacement::suffixed_path(const std::string& path, const std::string& suffix){
    auto dot = path.find_last_of('.');
    auto slash = path.find_last_of('/');
    if(dot == std::string::npos or (slash != std::string::npos and dot < slash)){
        return path + "_" + suffix;
    }
    return path.substr(0, dot) + "_" + suffix + path.substr(dot);
}

std::vector<std::string> colour_replacement::replace_images(const std::vector<std::string>& paths,
    const std::vector<Color>& bases, const std::vector<Color>& destinations){
    std::vector<std::string> written;
    if(destinations.empty()){ return written; }
    for(const auto& path : paths){
        colour_replacer replacer(path, build_colour_map(bases, destinations), destinations.front());
        replacer.replace();
        if(replacer.export_copy()){
            written.push_back(replacer.get_output_path());
        }
    }
    return written;
}

std::vector<std::string> colour_replacement::tiled_table_replacement(const std::vector<Color>& destinations){
    return replace_images({entity_config::tiled_table_station_path}, entity_config::tiled_table_base_colours, destinations);
}

std::vector<std::string> colour_replacement::khiri_dog_replacement(const std::vector<Color>& destinations){
    return replace_images(dog_part_paths(entity_config::khiri_parts), entity_config::khiri_base_colours, destinations);
}

std::vector<std::string> colour_replacement::mack_dog_replacement(const std::vector<Color>& destinations){
    return replace_images(dog_part_paths(entity_config::mack_parts), entity_config::mack_base_colours, destinations);
}

std::vector<std::string> colour_replacement::gianluca_dog_replacement(const std::vector<Color>& destinations){
    return replace_images(dog_part_paths(entity_config::gianluca_parts), entity_config::gianluca_base_colours, destinations);
}

std::vector<std::string> colour_replacement::lionel_dog_replacement(const std::vector<Color>& destinations){
    return replace_images(dog_part_paths(entity_config::lionel_parts), entity_config::lionel_base_colours, destinations);
}
