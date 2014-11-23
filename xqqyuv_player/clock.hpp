#ifndef _XQQYUV_PLAYER_CLOCK_HPP
#define _XQQYUV_PLAYER_CLOCK_HPP

#include <ctime>

namespace xqqyuv_player {

    class PlayerClock {
    public:
        inline PlayerClock() = default;

        inline PlayerClock& Begin() {
            milliseconds = clock() / (CLOCKS_PER_SEC / 1000);
            return *this;
        }

        inline PlayerClock& End() {
            milliseconds = clock() / (CLOCKS_PER_SEC / 1000) - milliseconds;
            return *this;
        }

        inline ~PlayerClock() { }

        inline static clock_t GetCurrentMillis() {
            return clock() / (CLOCKS_PER_SEC / 1000);
        }
    public:
        clock_t milliseconds = 0;
    };

}

#endif // _XQQYUV_PLAYER_CLOCK_HPP
