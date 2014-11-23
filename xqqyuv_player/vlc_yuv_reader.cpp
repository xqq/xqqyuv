#include <vector>
#include <string>
#include <fstream>
#include <cstdint>
#include "vlc_yuv_reader.hpp"

namespace xqqyuv_player {

    VlcYUVReader::VlcYUVReader(std::ifstream& openedInputFile) {
        inputFile = &openedInputFile;
        inputFile->seekg(0, std::ios::beg);
    }

    // Author: vczh
    std::vector<std::string> VlcYUVReader::SplitString(const char* input, char delimiter) {
        std::vector<std::string> fragments;
        while (auto next = strchr(input, delimiter)) {
            fragments.push_back(std::string(input, next));
            input = next + 1;
        }
        fragments.push_back(std::string(input));
        return fragments;
    }

    VlcYUVHeader VlcYUVReader::ParseHeader() {
        VlcYUVHeader header;
        char headerBuffer[64] = { 0 };
        char fourccBuffer[8] = { 0 };

        // for example, "YV12 W1280 H720 F0:0 Ip A1:1"
        inputFile->getline(headerBuffer, 64, '\n');
        headerSize = strlen(headerBuffer) + 1;

        // parse header string
        auto headerArray = SplitString(headerBuffer, ' ');
        
        header.fourCC = *reinterpret_cast<const uint32_t*>(headerArray[0].c_str());
        header.visibleWidth = std::stoul(headerArray[1].substr(1, headerArray[1].length() - 1));
        header.visibleHeight = std::stoul(headerArray[2].substr(1, headerArray[2].length() - 1));

        auto frameArray = SplitString(headerArray[3].substr(1, headerArray[3].length() - 1).c_str(), ':');
        header.frameRate = std::stoul(frameArray[0]);
        header.frameRateBase = std::stoul(frameArray[1]);
        header.type = headerArray[4][1];

        std::vector<std::string> sarArray = SplitString(headerArray[5].substr(1, headerArray[5].length() - 1).c_str(), ':');
        header.sarNum = std::stoul(sarArray[0]);
        header.sarDen = std::stoul(sarArray[1]);

        switch (header.fourCC) {
        case FCC('I420'):
        case FCC('YV12'):
            frameSize = header.visibleWidth * header.visibleHeight * 3 / 2;
            break;
        default:
            throw std::exception("VlcYUVReader: unknown fourCC format");
            break;
        }

        headerParsed = true;
        return header;
    }

    int VlcYUVReader::GetFrame(uint8_t* buffer, int bufferSize) {
        if (!headerParsed)
            ParseHeader();

        if (bufferSize < frameSize)
            throw std::length_error("VlcYUVReader: buffer size not enough");

        if (inputFile->eof())
            return EOF;

        char strFRAME[16] = { 0 };
        inputFile->getline(strFRAME, 6, '\n');
        
        inputFile->read(reinterpret_cast<char*>(buffer), frameSize);
        return frameSize;
    }

    void VlcYUVReader::SeekFrame(int index) {
        inputFile->seekg(headerSize + index * frameSize, std::ios::beg);
    }

} // namespace xqqyuv_player
