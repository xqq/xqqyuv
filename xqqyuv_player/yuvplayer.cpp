#include <cstdint>
#include <sstream>
#include <fstream>
#include <queue>
#include <string>
#include <chrono>
#include <thread>
#include <memory>
#include <stdexcept>
#include <functional>
#include "../xqqyuv/xqqyuv.h"
#include "vlc_yuv_reader.hpp"
#include "clock.hpp"
#include "yuvplayer.hpp"

#ifdef _MSC_VER     // MSVC
#include <SDL.h>
#else               // GCC
#include <SDL/SDL.h>
#endif

using namespace xqqyuv;

namespace xqqyuv_player {

    void YUVPlayer::Prepare(const char* inputFileName, int frameRate) {
        if (!inputFileName) {
            throw std::invalid_argument("YUVPlayer::Prepare(): Null-Pointer in params");
        }

        inputFile.open(inputFileName, std::ios::in | std::ios::binary);
        if (inputFile.fail()) {
            log("YUVPlayer::Prepare(): Open input file failed.\n");
            exit(-1);
        }

        this->yuvReader = new VlcYUVReader(inputFile);
        this->yuvHeader = yuvReader->ParseHeader();
        this->frameRate = frameRate;

        status = YUVPlayerStatus::PREPARED;
    }

    void YUVPlayer::AttachSDLSurface(SDL_Surface* screen) {
        if (!screen) {
            throw std::invalid_argument("YUVPlayer::AttachSDLSurface(): Null-Pointer of SDL_Surface");
        }

        this->screen = screen;
    }

    void YUVPlayer::SetCallback(std::function<void(const char*)> logHandler,
                                std::function<void(int, int, int)> showMSHandler) {
        this->printLog = logHandler;
        this->showMS = showMSHandler;
    }

    void YUVPlayer::log(const char* message) {
        if (printLog != nullptr) {
            printLog(message);
        } else {
            std::ofstream err(stdout);
            err << message;
        }
    }

    void YUVPlayer::outMS(int timeCalculate, int timeDisplay, int fps) {
        if (showMS != nullptr) {
            showMS(timeCalculate, timeDisplay, fps);
        }
    }

    void YUVPlayer::threadProc() {
        int width = yuvHeader.visibleWidth;
        int height = yuvHeader.visibleHeight;

        int framePixels = width * height;
        int frameSize = framePixels * 3 / 2;

        uint8_t* I420Buffer = new uint8_t[frameSize];
        uint8_t* src_y = I420Buffer;
        uint8_t* src_u = I420Buffer + framePixels;
        uint8_t* src_v = src_u + framePixels / 4;

        switch (yuvHeader.fourCC) {
        case FCC('I420'):
            break;
        case FCC('YV12'):
            //src_v = I420Buffer + framePixels;
            //src_u = src_v + framePixels / 4;
            break;
        default:
            throw std::exception("YUVPlayer: Unsupported fourCC format.");
            break;
        }
        
        int delay = 1000 / frameRate - 1;
        PlayerClock clock, totalClock;
        int timeCalculate = 0, timeDisplay = 0;
        int fps = 0, fpsCounter = 0;
        int fpsTimeRecoder = PlayerClock::GetCurrentMillis();

        do {
            totalClock.Begin();
            handleMessage(); 

            if (status == YUVPlayerStatus::PLAYING) {
                SDL_LockSurface(screen);
                
                if (yuvReader->GetFrame(I420Buffer, frameSize) == -1) {
                    status = YUVPlayerStatus::STOPPED;
                    break;
                }

                clock.Begin();
                I420ToARGB_SSE2(src_y, width, src_u, width / 2, src_v, width / 2, 
                                static_cast<ARGB*>(screen->pixels), width, height, COLORSPACE_BT709);
                timeCalculate = clock.End().milliseconds;

                clock.Begin();
                SDL_UnlockSurface(screen);
                SDL_Flip(screen);
                timeDisplay = clock.End().milliseconds;

                fpsCounter++;
                if (PlayerClock::GetCurrentMillis() - fpsTimeRecoder >= 1000) {
                    fps = fpsCounter;
                    fpsCounter = 0;
                    fpsTimeRecoder = PlayerClock::GetCurrentMillis();
                }

                outMS(timeCalculate, timeDisplay, fps);
            } else if (status == YUVPlayerStatus::STOPPED) {
                break;
            }
            
            int delayTime = delay - totalClock.End().milliseconds;
            if (delayTime > 0) {
                std::chrono::milliseconds sleepTime(delayTime);
                std::this_thread::sleep_for(sleepTime);
            }
        } while (true);
        delete [] I420Buffer;
    }

    void YUVPlayer::handleMessage() {
        while (!messageQueue.empty()) {
            YUVPlayerMessage message = messageQueue.front();
            switch (message) {
            case START:
                status = YUVPlayerStatus::PLAYING;
                break;
            case PAUSE:
                status = YUVPlayerStatus::PAUSED;
                break;
            case RESUME:
                status = YUVPlayerStatus::PLAYING;
                break;
            case STOP:
                status = YUVPlayerStatus::STOPPED;
                break;
            default:
                throw std::invalid_argument("YUVPlayer: Unknown message\n");
            }
            messageQueue.pop();
        }
    }

    void YUVPlayer::postMessage(YUVPlayerMessage message) {
        messageQueue.push(message);
    }

    void YUVPlayer::Start() {
        if (status == YUVPlayerStatus::NOP) {
            throw std::exception("YUVPlayer not prepared, call YUVPlayer::Prepare() first.");
        }
        renderThread = new std::thread(&YUVPlayer::threadProc, this);
        postMessage(START);
    }

    void YUVPlayer::Pause() {
        postMessage(PAUSE);
    }

    void YUVPlayer::Resume() {
        postMessage(RESUME);
    }

    void YUVPlayer::Stop() {
        postMessage(STOP);
        renderThread->join();
    }

    int YUVPlayer::GetWidth() {
        return yuvHeader.visibleWidth;
    }

    int YUVPlayer::GetHeight() {
        return yuvHeader.visibleHeight;
    }

    int YUVPlayer::EventLoop() {
        SDL_Event event;
        while (SDL_WaitEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                Stop();
                SDL_Quit();
                break;
            default:
                break;
            }
        }
        return 0;
    }

    YUVPlayer::~YUVPlayer() {
        if (status != NOP && inputFile.is_open()) {
            inputFile.close();
            delete yuvReader;
        }
    }

} // namespace xqqyuv_player
