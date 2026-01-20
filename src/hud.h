#ifndef HUD_H
#define HUD_H

#include "items.h"
#include <memory>
#include <vector>
namespace hud{

    class hud_element{
        public:
        private:
    };
    class button : public hud_element{

    };

    class hud{
        public:

        private:
            std::vector<std::unique_ptr<hud_element>> elements_;
    };

    class hud_builder{
        hud build_player_hud();
        hud build_pause_menu_hud();
        std::unique_ptr<hud_element> build_item_hud_element(items::item&  item);
        // .....
    };
    extern hud_builder h_builder_;

}
#endif 