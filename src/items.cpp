#include "items.h"
// ------------------------- builder ------------------------------------- // 
items::item_builder items::i_builder_;


std::unique_ptr<items::item> items::item_builder::build_item(size_t id, sprite::sprite icon, sprite::spriteset sprite, std::string name){
    return std::make_unique<item>(id, icon, sprite, name);
}
std::unique_ptr<items::item> items::item_builder::build_shop_item(size_t id, sprite::sprite icon, sprite::spriteset sprite, std::string name, int price, int level_req){
    return std::make_unique<shop_item>(id, icon, sprite, name, price, level_req);
}
std::unique_ptr<items::shop_item> items::item_builder::build_torn_couch(){}
std::unique_ptr<items::shop_item> items::item_builder::build_intact_couch(){}
std::unique_ptr<items::shop_item> items::item_builder::build_leaning_tower_dog_bowls(){}
// food and plants
std::unique_ptr<items::shop_item> items::item_builder::build_cheese_shelf(){}
std::unique_ptr<items::shop_item> items::item_builder::build_cured_meat_shelf(){}
std::unique_ptr<items::shop_item> items::item_builder::build_shishamo_tank(){}

std::unique_ptr<items::shop_item> items::item_builder::build_food_cart(){}
                
std::unique_ptr<items::shop_item> items::item_builder::build_lemongrass_plant_a(){}
std::unique_ptr<items::shop_item> items::item_builder::build_lemongrass_plant_b(){}
std::unique_ptr<items::shop_item> items::item_builder::build_frangipani_tree(){}
std::unique_ptr<items::shop_item> items::item_builder::build_succulent_tree(){}
std::unique_ptr<items::shop_item> items::item_builder::build_propogation_a(){}
std::unique_ptr<items::shop_item> items::item_builder::build_propogation_b(){}
std::unique_ptr<items::shop_item> items::item_builder::build_propogation_c(){}
std::unique_ptr<items::shop_item> items::item_builder::build_herb_garden(){}
                
                // statues and people
std::unique_ptr<items::shop_item> items::item_builder::build_pavlov(){}
std::unique_ptr<items::shop_item> items::item_builder::build_gargoyle_a(){}
std::unique_ptr<items::shop_item> items::item_builder::build_gargoyle_b(){}
std::unique_ptr<items::shop_item> items::item_builder::build_gargoyle_c(){}
std::unique_ptr<items::shop_item> items::item_builder::build_cantina_band(){}
                
                
                // other items
std::unique_ptr<items::shop_item> items::item_builder::build_koi_pond(){}
std::unique_ptr<items::shop_item> items::item_builder::dog_poker_table(){}
std::unique_ptr<items::shop_item> items::item_builder::dog_digsite(){}
// dog cosmetics
std::unique_ptr<items::shop_item> items::item_builder::build_jester_hat(){}
std::unique_ptr<items::shop_item> items::item_builder::build_crown(){}
std::unique_ptr<items::shop_item> items::item_builder::build_beholder_eyes(){}
std::unique_ptr<items::shop_item> items::item_builder::build_tarrasque_spines(){}
                
std::unique_ptr<items::shop_item> items::item_builder::build_eyepatch(){}
std::unique_ptr<items::shop_item> items::item_builder::build_pirate_hat_and_hook(){}
std::unique_ptr<items::shop_item> items::item_builder::build_valkyire_armour(){}
std::unique_ptr<items::shop_item> items::item_builder::build_cerberus_heads(){}
std::unique_ptr<items::shop_item> items::item_builder::build_medusa_snakes(){}
std::unique_ptr<items::shop_item> items::item_builder::build_hermes_wings(){}
std::unique_ptr<items::shop_item> items::item_builder::build_bloody_knife(){}
std::unique_ptr<items::shop_item> items::item_builder::build_jason_mask(){}
std::unique_ptr<items::shop_item> items::item_builder::build_vampire_fangs(){}
std::unique_ptr<items::shop_item> items::item_builder::build_hannibal_mask(){}
std::unique_ptr<items::shop_item> items::item_builder::build_hawkgirl_mask(){}
std::unique_ptr<items::shop_item> items::item_builder::build_blue_collar(){}
std::unique_ptr<items::shop_item> items::item_builder::build_red_collar(){}
std::unique_ptr<items::shop_item> items::item_builder::build_red_pyjamas(){}
std::unique_ptr<items::shop_item> items::item_builder::build_paddlepop_pyjamas(){}
std::unique_ptr<items::shop_item> items::item_builder::build_rugby_helmet(){}
std::unique_ptr<items::shop_item> items::item_builder::build_boxing_wraps(){}
std::unique_ptr<items::shop_item> items::item_builder::build_bee_companion(){}
std::unique_ptr<items::shop_item> items::item_builder::build_toy_ghost_companion(){}
std::unique_ptr<items::shop_item> items::item_builder::build_pots(){}
std::unique_ptr<items::shop_item> items::item_builder::build_pans(){}

                // cooking stations 
std::unique_ptr<items::shop_item> items::item_builder::build_oven(){}
std::unique_ptr<items::shop_item> items::item_builder::build_stove(){}
std::unique_ptr<items::shop_item> items::item_builder::build_freezer(){}
std::unique_ptr<items::shop_item> items::item_builder::snack_cupboard(){}
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