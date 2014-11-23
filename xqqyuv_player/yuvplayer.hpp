#ifndef _XQQYUV_PLAYER_YUVPLAYER_HPP
#define _XQQYUV_PLAYER_YUVPLAYER_HPP

#include <cstdint>
#include <fstream>
#include <thread>
#include <queue>
#include <functional>
#include "vlc_yuv_reader.hpp"

#ifdef _MSC_VER     // MSVC
#include <SDL.h>
#else               // GCC
#include <SDL/SDL.h>
#endif

namespace xqqyuv_player {

    enum YUVPlayerMessage {
        PREPARE = 0,
        START = 1,
        PAUSE = 2,
        RESUME = 3,
        SEEK = 4,
        STOP = 5,
        DISTORY = 6
    };

    enum YUVPlayerStatus {
        NOP = 0,
        PREPARED = 1,
        PLAYING = 2,
        PAUSED = 3,
        STOPPED = 4
    };

    class YUVPlayer {
    public:
        YUVPlayer() = default;
        void Prepare(const char* inputFileName, int frameRate);
        void AttachSDLSurface(SDL_Surface* screen);
        void SetCallback(std::function<void(const char*)> logHandler,
                         std::function<void(int, int, int)> showMSHandler);
        void Start();
        void Pause();
        void Resume();
        void Stop();
        int GetWidth();
        int GetHeight();
        int EventLoop();
        ~YUVPlayer();
    private:
        void log(const char* message);
        void outMS(int timeCalculate, int timeDisplay, int fps);
        void threadProc();
        void handleMessage();
        void postMessage(YUVPlayerMessage message);
    private:
        std::ifstream inputFile;

        VlcYUVHeader yuvHeader;
        VlcYUVReader* yuvReader = nullptr;
        int frameRate = 0;

        YUVPlayerStatus status = NOP;
        SDL_Surface* screen = nullptr;

        std::thread* renderThread = nullptr;
        std::queue<YUVPlayerMessage> messageQueue;
        std::function<void(const char*)> printLog = nullptr;
        std::function<void(int, int, int)> showMS = nullptr;
    };

} // namespace xqqyuv_player

#endif // _XQQYUV_PLAYER_YUVPLAYER_HPP
