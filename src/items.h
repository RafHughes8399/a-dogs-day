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
            item(size_t id, sprite::sprite icon, sprite::sprite sprite, std::string name)
            : id_(id), icon_(icon), sprite_(sprite), name_(name){}
            item(const item& other) = default;
            item(item&& other) = default;

            item& operator=(const item& other) = default;
            item& operator=(item&& other) = default;


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

            ~shop_item() {
                event_interface::unsubscribe<events::level_up>(level_up_handler_);
            }
            shop_item(size_t id, sprite::sprite icon, sprite::sprite sprite, std::string name, int price, int level_req)
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


    class item_builder{

    };
    extern item_builder i_builder_;
}
#endif 