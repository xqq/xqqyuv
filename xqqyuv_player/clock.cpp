#include <ctime>
#include "clock.hpp"

namespace xqqyuv_player {

    PlayerClock::PlayerClock() { }

    PlayerClock PlayerClock::Begin() {
        milliseconds = clock() / (CLOCKS_PER_SEC / 1000);
        return *this;
    }

    PlayerClock PlayerClock::End() {
        milliseconds = clock() / (CLOCKS_PER_SEC / 1000) - milliseconds;
        return *this;
    }

    PlayerClock::~PlayerClock() { }

    clock_t PlayerClock::GetCurrentMillis() {
        return clock() / (CLOCKS_PER_SEC / 1000);
    }

}
