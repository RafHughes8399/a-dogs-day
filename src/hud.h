#ifndef HUD_H
#define HUD_H

#include <memory>
namespace hud{
    class hud_element{

    };
    class hud{

    };

    class hud_builder{
        std::unique_ptr<hud> builder_player_hud();
    };
    extern hud_builder h_builder_;

}
#endif 