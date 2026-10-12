#ifndef ENGINE_SOURCE_IMAGES_MIPPEDIMG_INL
#define ENGINE_SOURCE_IMAGES_MIPPEDIMG_INL

// INLINED
inline void FullShrink(C4Pixel* dest, uint32_t destWidth, uint32_t destHeight, C4Pixel* source, uint32_t sourceWidth, uint32_t sourceHeight) {
    uint32_t xRatio = sourceWidth / destWidth;
    uint32_t yRatio = sourceHeight / destHeight;

    for (uint32_t y = 0; y < destHeight; y++) {
        for (uint32_t x = 0; x < destWidth; x++) {
            int64_t weightedB = 0;
            int64_t weightedG = 0;
            int64_t weightedR = 0;
            int64_t alpha = 0;
            int64_t b = 0;
            int64_t g = 0;
            int64_t r = 0;
            int64_t count = 0;
            C4Pixel* row = source;

            for (uint32_t j = 0; j < yRatio; j++) {
                C4Pixel* pixel = row;

                for (uint32_t i = 0; i < xRatio; i++) {
                    alpha += pixel->m_a;
                    weightedR += pixel->m_a * pixel->m_r;
                    weightedG += pixel->m_a * pixel->m_g;
                    weightedB += pixel->m_a * pixel->m_b;
                    count++;
                    r += pixel->m_r;
                    g += pixel->m_g;
                    b += pixel->m_b;
                    pixel++;
                }

                row += sourceWidth;
            }

            C4Pixel result;

            if (alpha) {
                result.m_r = static_cast<uint8_t>(weightedR / alpha);
                result.m_g = static_cast<uint8_t>(weightedG / alpha);
                result.m_b = static_cast<uint8_t>(weightedB / alpha);
            } else {
                result.m_r = static_cast<uint8_t>(r / count);
                result.m_g = static_cast<uint8_t>(g / count);
                result.m_b = static_cast<uint8_t>(b / count);
            }

            result.m_a = static_cast<uint8_t>(alpha / (xRatio * yRatio));
            *dest++ = result;
            source += xRatio;
        }

        source += sourceWidth * (yRatio - 1);
    }
}

#endif
