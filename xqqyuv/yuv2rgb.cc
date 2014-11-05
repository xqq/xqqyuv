#include "yuv2rgb.h"
#include "xqqyuv_def.h"

#ifdef __cplusplus
namespace xqqyuv {
extern "C" {
#endif
    
    static const s128_t c16 = { 16, 16, 16, 16, 16, 16, 16, 16 };
    static const s128_t c128 = { 128, 128, 128, 128, 128, 128, 128, 128 };
    static const s128_t c75 = { 75, 75, 75, 75, 75, 75, 75, 75 };

    static const s128_t BT601_cVToR = { 102, 102, 102, 102, 102, 102, 102, 102 };
    static const s128_t BT601_cUToG = { 25, 25, 25, 25, 25, 25, 25, 25 };
    static const s128_t BT601_cVToG = { 52, 52, 52, 52, 52, 52, 52, 52 };
    static const s128_t BT601_cUToB = { 129, 129, 129, 129, 129, 129, 129, 129 };

    static const s128_t BT709_cVToR = { 115, 115, 115, 115, 115, 115, 115, 115 };
    static const s128_t BT709_cUToG = { 14, 14, 14, 14, 14, 14, 14, 14 };
    static const s128_t BT709_cVToG = { 34, 34, 34, 34, 34, 34, 34, 34 };
    static const s128_t BT709_cUToB = { 135, 135, 135, 135, 135, 135, 135, 135 };

    // Only support MSVC x86 now.
    // TODO: gcc inline-asm support
    int I420ToARGB_SSE2(const uint8_t* src_y, int src_linesize_y,
                        const uint8_t* src_u, int src_linesize_u,
                        const uint8_t* src_v, int src_linesize_v,
                        ARGB* dst_argb, int width, int height,
                        ColorSpace colorSpace) {
        s128_t cVToR;
        s128_t cUToG;
        s128_t cVToG;
        s128_t cUToB;

        s128_t tempR;

        if (!src_y || !src_u || !src_v || !dst_argb ||
            width <= 0 || height <= 0) {
            return -1;
        }

        if (colorSpace == COLORSPACE_BT601) {
            __asm {
                movdqa xmm0, BT601_cVToR
                movdqa xmm1, BT601_cUToG
                movdqa xmm2, BT601_cVToG
                movdqa xmm3, BT601_cUToB
                movdqa cVToR, xmm0
                movdqa cUToG, xmm1
                movdqa cVToG, xmm2
                movdqa cUToB, xmm3
            }
        } else if (colorSpace == COLORSPACE_BT709) {
            __asm {
                movdqa xmm0, BT709_cVToR
                movdqa xmm1, BT709_cUToG
                movdqa xmm2, BT709_cVToG
                movdqa xmm3, BT709_cUToB
                movdqa cVToR, xmm0
                movdqa cUToG, xmm1
                movdqa cVToG, xmm2
                movdqa cUToB, xmm3
            }
        } else {
            return -1;
        }

        for (int i = 0; i < height; i++) {
            __asm {
                mov esi, [src_y]
                mov edx, [src_u]
                mov edi, [src_v]
                mov eax, [dst_argb]

                mov ecx, src_linesize_y
            line_loop :
                pxor xmm7, xmm7			// xmm7 = 0
                // xmm0-xmm1 => 75 * C		xmm2-xmm3 => E		xmm4-xmm5 => D
                movdqa xmm0, [esi]				// Y
                movdqa xmm1, xmm0
                punpcklbw xmm0, xmm7
                punpckhbw xmm1, xmm7
                psubw xmm0, c16
                psubw xmm1, c16
                pmullw xmm0, c75
                pmullw xmm1, c75

                movq xmm2, qword ptr [edi]		// V
                punpcklbw xmm2, xmm2            // upsample: nearest neighbor
                movdqa xmm3, xmm2
                punpcklbw xmm2, xmm7
                punpckhbw xmm3, xmm7
                psubw xmm2, c128
                psubw xmm3, c128

                movdqa xmm4, xmm2
                movdqa xmm5, xmm3
                pmullw xmm4, cVToR
                pmullw xmm5, cVToR
                paddw xmm4, xmm0
                paddw xmm5, xmm1

                psraw xmm4, 6
                psraw xmm5, 6
                packuswb xmm4, xmm5
                movdqa tempR, xmm4
                // tempR: dst_R
                //----------------------
                movq xmm4, qword ptr [edx]		// U
                punpcklbw xmm4, xmm4            // upsample: nearest neighbor
                movdqa xmm5, xmm4
                punpcklbw xmm4, xmm7
                punpckhbw xmm5, xmm7
                psubw xmm4, c128
                psubw xmm5, c128

                movdqa xmm6, xmm4
                movdqa xmm7, xmm5
                pmullw xmm6, cUToG
                pmullw xmm7, cUToG

                pmullw xmm2, cVToG
                pmullw xmm3, cVToG

                paddw xmm2, xmm6
                paddw xmm3, xmm7

                movdqa xmm6, xmm0
                movdqa xmm7, xmm1

                psubw xmm6, xmm2
                psubw xmm7, xmm3

                psraw xmm6, 6
                psraw xmm7, 6
                packuswb xmm6, xmm7
                // xmm6: dst_G
                //----------------------
                pmullw xmm4, cUToB
                pmullw xmm5, cUToB

                paddw xmm4, xmm0
                paddw xmm5, xmm1

                psraw xmm4, 6
                psraw xmm5, 6
                packuswb xmm4, xmm5
                // xmm4: dst_B
                //----------------------
                //-----ARGB Repack------
                //----------------------
                pxor xmm0, xmm0			// 0x00, for unpack
                pcmpeqb xmm1, xmm1		// 0xFF, for alpha
                // xmm2-xmm3: R/R2		xmm4-xmm5: B/B2		xmm6-xmm7: G/G2
                movdqa xmm2, tempR		// R

                movdqa xmm3, xmm2		// R2
                punpcklbw xmm2, xmm0
                punpckhbw xmm3, xmm0
                packuswb xmm2, xmm2
                packuswb xmm3, xmm3

                movdqa xmm5, xmm4		// B2
                punpcklbw xmm4, xmm0
                punpckhbw xmm5, xmm0
                packuswb xmm4, xmm4
                packuswb xmm5, xmm5

                movdqa xmm7, xmm6		// G2
                punpcklbw xmm6, xmm0
                punpckhbw xmm7, xmm0
                packuswb xmm6, xmm6
                packuswb xmm7, xmm7		// xmm2-xmm3: R/R2		xmm4-xmm5: B/B2		xmm6-xmm7: G/G2

                punpcklbw xmm2, xmm6	// GR; R op= G
                punpcklbw xmm3, xmm7

                punpcklbw xmm4, xmm1	// AB; B op= A
                punpcklbw xmm5, xmm1

                movdqa xmm1, xmm2
                punpcklwd xmm2, xmm4
                punpckhwd xmm1, xmm4

                movdqa xmm6, xmm3
                punpcklwd xmm3, xmm5
                punpckhwd xmm6, xmm5

                movdqa [eax], xmm2
                movdqa [eax + 16], xmm1
                movdqa [eax + 32], xmm3
                movdqa [eax + 48], xmm6

                //----------------------
                add esi, 16
                add edx, 8
                add edi, 8
                add eax, 64

                sub ecx, 16
                jg line_loop
                //-------------------------
            }

            dst_argb += width;
            src_y += src_linesize_y;
            if (i & 1) {
                src_u += src_linesize_u;
                src_v += src_linesize_v;
            }
        } 

        return 0;
    }

#ifdef __cplusplus
} // extern "C"
} // namespace xqqyuv
#endif
