#ifndef ITEMS_H
#define ITEMS_H

#include <string>
#include <vector>
#include <memory>
#include "events.h"
#include "events_interface.h"
#include "sprite.h"
namespace items{
    class food_item{
        public:
            ~food_item() = default;
            food_item(size_t item_id, std::uint8_t count = 1)
            :item_id_(item_id), count_(count){}

            food_item(const food_item& other) = default;
            food_item(food_item&& other) = default;

            food_item& operator=(const food_item& other) = default;
            food_item& operator=(food_item&& other) = default;
            food_item& operator++(){
                ++count_;
                return *this;
            }
            food_item operator++(int){
                auto temp = *this;
                ++count_;
                return temp;
            }
            food_item& operator--(){
                --count_;
                return *this;
            }
            food_item operator--(int){
                auto temp = *this;
                --count_;
                return temp;
            }
            friend bool operator==(const food_item& a, const food_item& b){
                return a.item_id_ == b.item_id_ and a.count_ == b.count_;
            }
            size_t get_id();
            std::uint8_t get_count() const;
            bool is_full();
            void adjust_count(size_t delta);
        private:
            size_t item_id_;
            std::uint8_t count_;
    };
    // todo DEFINE A SUBCLASS ITEM WHERE COUNT IS JUST ONE, I SUPPOSE THOUGH IT IS A COMPLEETLY DICFFERNT
    // ITEM IMPLEMENTATION SO WE SHOULD HONOUR THAT

    // * new item with the id, the name and the price ? yeah
    class inventory_item{
        public:
            ~inventory_item() = default;
            // TODO update inventory item construction - name_/sell_price_ default until a catalog/lookup exists
            inventory_item(size_t item_id, std::uint8_t count = 1)
            :item_id_(item_id), name_(""), sell_price_(0), count_(count){}

            inventory_item(const inventory_item& other) = default;
            inventory_item(inventory_item&& other) = default;

            size_t get_id() const;
            std::uint8_t get_count() const;
            void adjust_count(size_t delta);
        private:
            const size_t item_id_;
            const std::string name_;
            const int sell_price_;
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
