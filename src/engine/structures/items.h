#ifndef ITEMS_H
#define ITEMS_H

#include <string>
#include <vector>
#include <memory>
#include "events.h"
#include "events_interface.h"
#include "sprite.h"
namespace items{
    class item{
        public:
            ~item() = default;
            item(size_t item_id, std::uint8_t count = 1)
            :item_id_(item_id), count_(count){}

            item(const item& other) = default;
            item(item&& other) = default;

            item& operator=(const item& other) = default;
            item& operator=(item&& other) = default;
            item& operator++(){
                ++count_;
                return *this;
            }
            item operator++(int){
                auto temp = *this;
                ++count_;
                return temp;
            }
            item& operator--(){
                --count_;
                return *this;
            }
            item operator--(int){
                auto temp = *this;
                --count_;
                return temp;
            }
            friend bool operator==(const item& a, const item& b){
                return a.item_id_ == b.item_id_ and a.count_ == b.count_;
            }
            size_t get_id();
            std::uint8_t get_count();
        private:
            size_t item_id_;
            std::uint8_t count_;
    };
/**

class item_builder{
    public:
    std::unique_ptr<item> build_item(size_t id, sprite::sprite icon, sprite::spriteset sprite, std::string name);
    std::unique_ptr<item> build_shop_item(size_t id, sprite::sprite icon, sprite::spriteset sprite, std::string name, int price, int level_req);
    
    
    // decorations 
    // furniture
    std::unique_ptr<shop_item> build_torn_couch();
    std::unique_ptr<shop_item> build_intact_couch();
    std::unique_ptr<shop_item> build_leaning_tower_dog_bowls();
    
    
    // food and plants
    std::unique_ptr<shop_item> build_cheese_shelf();
    std::unique_ptr<shop_item> build_cured_meat_shelf();
    std::unique_ptr<shop_item> build_shishamo_tank();
    
    std::unique_ptr<shop_item> build_food_cart();
    
    std::unique_ptr<shop_item> build_lemongrass_plant_a();
    std::unique_ptr<shop_item> build_lemongrass_plant_b();
    std::unique_ptr<shop_item> build_frangipani_tree();
    std::unique_ptr<shop_item> build_succulent_tree();
    std::unique_ptr<shop_item> build_propogation_a();
    std::unique_ptr<shop_item> build_propogation_b();
    std::unique_ptr<shop_item> build_propogation_c();
    std::unique_ptr<shop_item> build_herb_garden();
    
    // statues and people
    std::unique_ptr<shop_item> build_pavlov();
    std::unique_ptr<shop_item> build_gargoyle_a();
    std::unique_ptr<shop_item> build_gargoyle_b();
    std::unique_ptr<shop_item> build_gargoyle_c();
    std::unique_ptr<shop_item> build_cantina_band();
    
    
    // other items
    std::unique_ptr<shop_item> build_koi_pond();
    std::unique_ptr<shop_item> dog_poker_table();
    std::unique_ptr<shop_item> dog_digsite();
    
    
    // dog cosmetics
    std::unique_ptr<shop_item> build_jester_hat();
    std::unique_ptr<shop_item> build_crown();
    std::unique_ptr<shop_item> build_beholder_eyes();
    std::unique_ptr<shop_item> build_tarrasque_spines();
    
    std::unique_ptr<shop_item> build_eyepatch();
    std::unique_ptr<shop_item> build_pirate_hat_and_hook();

    
                std::unique_ptr<shop_item> build_valkyire_armour();
                std::unique_ptr<shop_item> build_cerberus_heads();
                std::unique_ptr<shop_item> build_medusa_snakes();
                std::unique_ptr<shop_item> build_hermes_wings();
                
                
                std::unique_ptr<shop_item> build_bloody_knife();
                std::unique_ptr<shop_item> build_jason_mask();
                std::unique_ptr<shop_item> build_vampire_fangs();
                std::unique_ptr<shop_item> build_hannibal_mask();
                
                std::unique_ptr<shop_item> build_krypto_cape();
                std::unique_ptr<shop_item> build_hawkgirl_mask();
                
                std::unique_ptr<shop_item> build_blue_collar();
                std::unique_ptr<shop_item> build_red_collar();
                std::unique_ptr<shop_item> build_red_pyjamas();
                std::unique_ptr<shop_item> build_paddlepop_pyjamas();
                
                std::unique_ptr<shop_item> build_rugby_helmet();
                std::unique_ptr<shop_item> build_boxing_wraps();
                
                
                
                std::unique_ptr<shop_item> build_bee_companion();
                std::unique_ptr<shop_item> build_toy_ghost_companion();
                
                
                // utility items 
                // kitchen equipment
                std::unique_ptr<shop_item> build_pots();
                std::unique_ptr<shop_item> build_pans();
                std::unique_ptr<shop_item> build_moka();
                // cooking stations 
                std::unique_ptr<shop_item> build_oven();
                std::unique_ptr<shop_item> build_stove();
                std::unique_ptr<shop_item> build_freezer();
                std::unique_ptr<shop_item> snack_cupboard();
                
                // tableware
                
                // serving islands
                
            };
            
            // an interface to manage, convert, and move items 
            class item_manager{
                public:
                void buy_item(size_t id);
                std::unique_ptr<inventory_item> shop_to_inventory_item();
                private:
            };
            extern item_builder i_builder_;
            */
}
#endif 
