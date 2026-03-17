#ifndef ITEMS_H
#define ITEMS_H

#include <string>
#include <vector>
#include <memory>
#include "events.h"
#include "events_interface.h"
#include "sprite.h"
namespace items{
    class item {
        public:
            virtual ~item() = default;
            item(size_t id, sprite::sprite icon, sprite::spriteset sprite, std::string name)
            : id_(id), icon_(icon), sprite_(sprite), name_(name){}
            item(const item& other) = default;
            item(item&& other) = default;

            item& operator=(const item& other) = delete;
            item& operator=(item&& other) = delete;


            protected:
            size_t id_;
            sprite::sprite icon_; // the store / inventory icon
            sprite::spriteset sprite_; // the actual sprite with all four directions,  
            std::string name_;
        };
    class inventory_item : public item{
        public:
        private:
            int quantity_;                             
    };
    class shop_item : public item {
        public:
            // changes the appearnce of the item, and the functionality of 
            // the assocaited "buy button"
            class access_state{
                public:
                    virtual ~access_state() = default;
                    access_state() = default;

                    access_state(access_state&& other) = default;
                    virtual bool can_buy() = 0;
                    virtual bool is_locked() = 0;
                private:
            };
            class locked : public access_state{
                public:
                    locked() : access_state(){};
                    bool can_buy() override { return false;}
                    bool is_locked() override { return true;}
                    private:
                };
                class unlocked : public access_state{
                    public:
                    unlocked() : access_state(){};
                    bool can_buy() override { return true;}
                    bool is_locked() override { return false;}
                    private:

            };

            ~shop_item() {
                event_interface::unsubscribe<events::level_up>(level_up_handler_);
            }
            shop_item(size_t id, sprite::sprite icon, sprite::spriteset sprite, std::string name, int price, int level_req)
            : item(id, icon, sprite, name), price_(price), level_requirement_(level_req), locked_state_(std::make_unique<locked>()),
            level_up_handler_([this](const events::level_up& event)->void {on_level_up_event(event);}){
                event_interface::subscribe<events::level_up>(level_up_handler_);

            }

            // copy and move


            bool can_buy();
            bool is_locked();

            void on_level_up_event(const events::level_up& event);

        private:
            // listen for level_up events
            events::event_handler<events::level_up> level_up_handler_;
            int price_;
            int level_requirement_;
            std::unique_ptr<access_state> locked_state_;
    };

    /**
     * how can i pair the icon with the spriteset
     * 
     * to turn an item into an entity
     * 
     * 
     * maybe somewhere there exists an id to spriteset map
     * when you assign the item an id, you pull the spriteset from that map ?
     * or maybe, a sprite icon (new sprite class) has a field for the spriteset
     * then you assign when the icon is built
     * 
     * use pavlov as an example
     * 
     * 
     * 
     */
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
}
#endif 