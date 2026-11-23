#ifndef ITEMS_H
#define ITEMS_H

#include <array>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include "events.h"
#include "events_interface.h"
#include "sprite.h"
namespace items{
    class item_ref{
        public:
            ~item_ref() = default;
            item_ref(size_t category, size_t id)
            :category_(category), id_(id){}
            item_ref(const item_ref& other) = default;
            item_ref(item_ref&& other) = default;
            item_ref& operator=(const item_ref& other) = default;
            item_ref& operator=(item_ref&& other) = default;

            size_t get_category() const;
            size_t get_id() const;
            size_t pack() const;
            static item_ref unpack(size_t packed);
            bool operator==(const item_ref& other) const = default;
        private:
            static const size_t id_bits = 16;
            size_t category_;
            size_t id_;
    };
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
    class item_definition{
        public:
            ~item_definition() = default;
            item_definition(std::string name, int price, Vector2 size, size_t category)
            :name_(std::move(name)), price_(price), size_(size), category_(category){}

            item_definition(const item_definition& other) = default;
            item_definition(item_definition&& other) = default;

            const std::string& get_name() const;
            int get_price() const;
            Vector2 get_size() const;
            size_t get_category() const;
        private:
            std::string name_;
            int price_;
            Vector2 size_;
            size_t category_;
    };
    class item_encyclopedia{
        public:
            static item_encyclopedia& get_instance(){
                static item_encyclopedia instance;
                return instance;
            }
            ~item_encyclopedia() = default;
            item_encyclopedia(const item_encyclopedia& other) = delete;
            item_encyclopedia(item_encyclopedia&& other) = delete;

            item_encyclopedia& operator=(const item_encyclopedia& other) = delete;
            item_encyclopedia& operator=(item_encyclopedia&& other) = delete;

            const item_definition& definition_of(const item_ref& ref) const;
            const item_definition& definition_of(size_t category, size_t id) const;
            bool has_definition(const item_ref& ref) const;
        private:
            item_encyclopedia()
            :catalogues_(build_catalogues()){}

            static std::array<std::vector<item_definition>, entity_config::item_categories_size> build_catalogues();
            static std::vector<item_definition> build_decoration_definitions();
            static std::vector<item_definition> build_station_definitions();
            const std::array<std::vector<item_definition>, entity_config::item_categories_size> catalogues_;
    };
    class inventory_layout{
        public:
            ~inventory_layout() = default;
            inventory_layout(Vector2 size, float scale)
            :size_(size), scale_(scale){}

            inventory_layout(const inventory_layout& other) = default;
            inventory_layout(inventory_layout&& other) = default;

            inventory_layout& operator=(const inventory_layout& other) = default;
            inventory_layout& operator=(inventory_layout&& other) = default;

            Vector2 get_size() const;
            float get_scale() const;
            Vector2 get_footprint() const;
        private:
            Vector2 size_;
            float scale_;
    };
    inventory_layout inventory_layout_of(const item_ref& ref);
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
