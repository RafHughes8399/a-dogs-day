/** 
 * file for the player class containing 
 * ! add overview here ! 
 *  author: raffa, october 25
 */
#ifndef PLAYER_H
#define PLAYER_H
namespace player{
    class controls{

    };
    class cursor{

    };
    class viewport{

    };
    class hud{

    };
    class inventory{

    };
    // also include at some point hud and inventory
    class player{
        public:
        private:

            controls control_scheme_;
            cursor cursor_;
            hud hud_;
            inventory inventory_;
            viewport frame_;
    };

} // namespace player

#endif