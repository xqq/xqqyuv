#ifndef _XQQYUV_PLAYER_CLOCK_HPP
#define _XQQYUV_PLAYER_CLOCK_HPP

#include <ctime>

namespace xqqyuv_player {

    class PlayerClock {
    public:
        clock_t milliseconds = 0;
    public:
        PlayerClock();
        PlayerClock Begin();
        PlayerClock End();
        ~PlayerClock();
        static clock_t GetCurrentMillis();
    };

}

#endif // _XQQYUV_PLAYER_CLOCK_HPP
