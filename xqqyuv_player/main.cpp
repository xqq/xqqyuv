#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "assembly.hpp"
#include "yuvplayer.hpp"

#ifdef _MSC_VER     // MSVC
#include <SDL.h>
#else               // GCC/other
#include <SDL/SDL.h>
#endif

using namespace xqqyuv_player;

static int videoWidth = 0, videoHeight = 0;

int initSDL(SDL_Surface** screen, int width, int height) {
    _putenv("SDL_VIDEO_WINDOW_POS=50,50");
    
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        std::cout << "Error while initializing SDL." << std::endl;
        return -1;
    }
    
    SDL_Surface* _screen = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE);
    if (screen == nullptr) {
        std::cout << "Error while creating window." << std::endl;
        return -1;
    }

    *screen = _screen;

    SDL_WM_SetCaption("xqqyuv_player", nullptr);

    return 0;
}

void printLog(const char* message) {
    std::cout << message;
}

void showMS(int timeCalculate, int timeDisplay, int fps) {
    char titleBuffer[128] = { '\0' };
    sprintf_s(titleBuffer, "xqqyuv_player - %dx%d, calc = %dms, disp = %dms, fps = %d",
              videoWidth, videoHeight, timeCalculate, timeDisplay, fps);
    SDL_WM_SetCaption(titleBuffer, nullptr);
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "xqqyuv_player: no input file" << std::endl
                  << "usage: xqqyuv_player FileName FrameRate" << std::endl;
        return -1;
    } else if (argc == 2) {
        std::cout << "missing param: frame_rate" << std::endl;
        return -1;
    } else if (argc == 3) {
        // okay
    } else {
        std::cout << "xqqyuv_player: to many params" << std::endl;
        return -1;
    }

    std::cout << "xqq's YUV player, based on xqqyuv library." << std::endl << std::endl;
    std::cout << "Input: " << argv[1] << std::endl;
    std::cout << "FrameRate: " << argv[2] << std::endl;

    YUVPlayer player;
    player.Prepare(argv[1], std::stoi(argv[2]));

    videoWidth = player.GetWidth();
    videoHeight = player.GetHeight();

    SDL_Surface* screen = nullptr;
    if (initSDL(&screen, videoWidth, videoHeight) == -1) {
        return -1;
    }

    player.AttachSDLSurface(screen);
    player.SetCallback(printLog, showMS);

    player.Start();
    
    return player.EventLoop();
}
