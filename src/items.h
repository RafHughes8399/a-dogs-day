#ifndef ITEMS_H
#define ITEMS_H

#include <string>
#include <vector>
#include <memory>
#include "events.h"
#include "sprite.h"
namespace items{
    class item {
        public:
        protected:
            size_t id_;
            sprite::sprite icon_; // the store / inventory icon
            sprite::sprite sprite_; // the actual sprite with all four directions,  
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
                    virtual bool can_buy() = 0;
                    virtual bool is_locked() = 0;
                private:
            };
            class locked : public access_state{
                public:
                    bool can_buy() override { return false;}
                    bool is_locked() override { return true;}
                    private:
                };
                class unlocked : public access_state{
                    public:
                    bool can_buy() override { return true;}
                    bool is_locked() override { return false;}
                    private:

            };
            bool can_buy();
            bool is_locked();

            void on_level_up_event(const events::level_up& event);

        private:
            // listen for level_up events
            int price_;
            int level_requirement_;
            std::unique_ptr<access_state> locked_state_;
    };


    class item_builder{

    };
    extern item_builder i_builder_;
}
#endif 