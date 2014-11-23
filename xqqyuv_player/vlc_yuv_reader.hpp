#ifndef _XQQYUV_PLAYER_VLC_YUV_READER_HPP
#define _XQQYUV_PLAYER_VLC_YUV_READER_HPP

#include <vector>
#include <string>
#include <fstream>
#include <cstdint>

namespace xqqyuv_player {

#ifndef FCC
#define FCC(ch4) ((((uint32_t)(ch4) & 0xFF) << 24) |     \
                  (((uint32_t)(ch4) & 0xFF00) << 8) |    \
                  (((uint32_t)(ch4) & 0xFF0000) >> 8) |  \
                  (((uint32_t)(ch4) & 0xFF000000) >> 24))
#endif

    struct VlcYUVHeader {
        uint32_t fourCC = 0;
    
        uint32_t visibleWidth = 0;
        uint32_t visibleHeight = 0;
    
        uint32_t frameRate = 0;
        uint32_t frameRateBase = 0;
    
        char type = 0;
    
        uint32_t sarNum = 0;
        uint32_t sarDen = 0;
    };

    class VlcYUVReader {
    public:
        VlcYUVReader(std::ifstream& openedInputFile);
        VlcYUVHeader ParseHeader();
        int GetFrame(uint8_t* buffer, int bufferSize);
        void VlcYUVReader::SeekFrame(int index);
    private:
        std::vector<std::string> SplitString(const char* config, char delimiter);
    private:
        std::ifstream* inputFile = nullptr;
        VlcYUVHeader header;
        bool headerParsed = false;
        int headerSize = 0;
        int frameSize = 0;
    };

} // namespace xqqyuv_player

#endif // _XQQYUV_PLAYER_VLC_YUV_READER_HPP
