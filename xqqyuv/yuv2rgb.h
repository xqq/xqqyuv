#ifndef _XQQYUV_YUV2RGB_H
#define _XQQYUV_YUV2RGB_H

#include "xqqyuv_def.h"

#ifdef __cplusplus
namespace xqqyuv {
extern "C" {
#endif

    int I420ToARGB_SSE2(const uint8_t* src_y, int src_linesize_y,
                        const uint8_t* src_u, int src_linesize_u,
                        const uint8_t* src_v, int src_linesize_v,
                        ARGB* dst_argb, int width, int height,
                        ColorSpace colorSpace);

#ifdef __cplusplus
} // extern "C"
} // namespace xqqyuv
#endif

#endif // _XQQYUV_YUV2RGB_H
