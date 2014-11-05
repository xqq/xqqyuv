#ifndef _XQQYUV_XQQYUV_DEF_H
#define _XQQYUV_XQQYUV_DEF_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
namespace xqqyuv {
extern "C" {
#endif

    enum ColorSpace {
        COLORSPACE_BT601 = 601,
        COLORSPACE_BT709 = 709
    };

    typedef struct {
        uint8_t V;
        uint8_t U;
        uint8_t Y;
        uint8_t A;
    } YUV;

    typedef struct {
        uint8_t R;
        uint8_t G;
        uint8_t B;
        uint8_t A;
    } ARGB;

    _CRT_ALIGN(16)
    typedef struct {
        uint16_t a[8];
    } s128_t;

#ifdef __cplusplus
} // extern "C"
} // namespace xqqyuv
#endif

#endif // _XQQYUV_XQQYUV_DEF_H
