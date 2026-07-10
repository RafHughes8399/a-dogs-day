#include "entities.h"
#include <string>
// --------------------------- builder --------------------------- //
std::string entities::entity_builder::next_debug_id(const std::string& prefix){
    auto next_id = debug_id_counts_[prefix];
    debug_id_counts_[prefix] += 1;
    return prefix + std::to_string(next_id);
}

// defining the builder
entities::entity_builder entities::e_builder;
