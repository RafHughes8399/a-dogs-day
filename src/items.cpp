#include "items.h"
// ------------------------- builder ------------------------------------- // 
items::item_builder items::i_builder_;


std::unique_ptr<items::item> items::item_builder::build_item(size_t id, sprite::sprite icon, sprite::spriteset sprite, std::string name){
    return std::make_unique<item>(id, icon, sprite, name);
}
std::unique_ptr<items::item> items::item_builder::build_shop_item(size_t id, sprite::sprite icon, sprite::spriteset sprite, std::string name, int price, int level_req){
    return std::make_unique<shop_item>(id, icon, sprite, name, price, level_req);
}
std::unique_ptr<items::shop_item> items::item_builder::build_torn_couch(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_intact_couch(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_leaning_tower_dog_bowls(){
    return nullptr;
}
// food and plants
std::unique_ptr<items::shop_item> items::item_builder::build_cheese_shelf(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_cured_meat_shelf(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_shishamo_tank(){
    return nullptr;
}

std::unique_ptr<items::shop_item> items::item_builder::build_food_cart(){
    return nullptr;
}
                
std::unique_ptr<items::shop_item> items::item_builder::build_lemongrass_plant_a(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_lemongrass_plant_b(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_frangipani_tree(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_succulent_tree(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_propogation_a(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_propogation_b(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_propogation_c(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_herb_garden(){
    return nullptr;
}
                
                // statues and people
std::unique_ptr<items::shop_item> items::item_builder::build_pavlov(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_gargoyle_a(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_gargoyle_b(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_gargoyle_c(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_cantina_band(){
    return nullptr;
}
                
                
                // other items
std::unique_ptr<items::shop_item> items::item_builder::build_koi_pond(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::dog_poker_table(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::dog_digsite(){
    return nullptr;
}
// dog cosmetics
std::unique_ptr<items::shop_item> items::item_builder::build_jester_hat(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_crown(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_beholder_eyes(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_tarrasque_spines(){
    return nullptr;
}
                
std::unique_ptr<items::shop_item> items::item_builder::build_eyepatch(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_pirate_hat_and_hook(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_valkyire_armour(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_cerberus_heads(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_medusa_snakes(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_hermes_wings(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_bloody_knife(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_jason_mask(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_vampire_fangs(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_hannibal_mask(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_hawkgirl_mask(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_blue_collar(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_red_collar(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_red_pyjamas(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_paddlepop_pyjamas(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_rugby_helmet(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_boxing_wraps(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_bee_companion(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_toy_ghost_companion(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_pots(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_pans(){
    return nullptr;
}

                // cooking stations 
std::unique_ptr<items::shop_item> items::item_builder::build_oven(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_stove(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::build_freezer(){
    return nullptr;
}
std::unique_ptr<items::shop_item> items::item_builder::snack_cupboard(){
    return nullptr;
}
// ------------------------- manager ------------------------------------- // 


// -------------------------------------- item ---------------------------------- //
// -------------------------------------- inventory item ---------------------------------- //
// -------------------------------------- shop item ---------------------------------- //
bool items::shop_item::can_buy(){
    return locked_state_->can_buy(); // and if the price is greater or equal
}
bool items::shop_item::is_locked(){
    return locked_state_->is_locked();
}

void items::shop_item::on_level_up_event(const events::level_up& event){
    int new_level = event.get_new_level();
    if(new_level >= level_requirement_){
        locked_state_ = std::make_unique<unlocked>();
    }
}