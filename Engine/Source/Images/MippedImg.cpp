#include <cstdint>
#include "Engine/Source/Images/MippedImg.h"
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: %0x00645890
uint32_t GetBitDepth(uint32_t format) {
    switch (format) {
        case PIXEL_DXT1:
            return 4;

        case PIXEL_DXT3:
        case PIXEL_A8:
        case PIXEL_DXT5:
            return 8;

        case PIXEL_ARGB8888:
            return 32;

        case PIXEL_ARGB1555:
        case PIXEL_ARGB4444:
        case PIXEL_RGB565:
            return 16;

        default:
            return 0;
    }
}

// OFFSET: %0x006458F0
uint32_t CalcLevelSize(uint32_t level, uint32_t width, uint32_t height, uint32_t format) {
    uint32_t levelWidth = width >> level ? width >> level : 1;
    uint32_t levelHeight = height >> level ? height >> level : 1;

    if (format == PIXEL_DXT1 || format == PIXEL_DXT3 || format == PIXEL_DXT5) {
        if (levelWidth == 6 * levelHeight) {
            levelHeight = levelHeight > 4 ? levelHeight : 4;
            levelWidth = 6 * levelHeight;
        } else {
            levelWidth = levelWidth > 4 ? levelWidth : 4;
            levelHeight = levelHeight > 4 ? levelHeight : 4;
        }
    }

    if (format == PIXEL_ARGB2565) {
        uint32_t pixels = levelHeight * levelWidth;
        uint32_t alphaBytes = pixels >> 2 ? pixels >> 2 : 1;
        return alphaBytes + 2 * pixels;
    }

    return (levelHeight * levelWidth * GetBitDepth(format)) >> 3;
}

// PROVEN: 3f5f40b7 IDENTICAL
// OFFSET: 0x006393A0
uint32_t CalcLevelOffset(uint32_t level, uint32_t width, uint32_t height, uint32_t format) {
    uint32_t offset = 0;

    for (uint32_t i = 0; i < level; i++) {
        offset += CalcLevelSize(i, width, height, format);
    }

    return offset;
}

// OFFSET: %0x006459D0
uint32_t CalcLevelCount(uint32_t width, uint32_t height) {
    uint32_t levels = 1;

    if (width == 6 * height) {
        width /= 6;
    }

    while (width > 1 || height > 1) {
        levels++;
        width = width >> 1 ? width >> 1 : 1;
        height = height >> 1 ? height >> 1 : 1;
    }

    return levels;
}

// PROVEN: ee9100fb IDENTICAL
// OFFSET: 0x00639440
MipBits* MippedImgAllocA(uint32_t format, uint32_t width, uint32_t height, const char* fileName, int32_t lineNumber) {
    uint32_t levelCount = CalcLevelCount(width, height);
    MipBits* images = static_cast<MipBits*>(SMemAlloc(CalcLevelOffset(levelCount, width, height, format) + levelCount * sizeof(MipBits) + 16, fileName, lineNumber, 0));

    uintptr_t offset = ((reinterpret_cast<uintptr_t>(&images[levelCount]) + 15) & ~static_cast<uintptr_t>(15)) - reinterpret_cast<uintptr_t>(images);

    for (uint32_t i = 0; i < levelCount; i++) {
        images[i].m_mip[0] = reinterpret_cast<C4Pixel*>(reinterpret_cast<uint8_t*>(images) + offset);
        offset += CalcLevelSize(i, width, height, format);
    }

    return images;
}

// PROVEN: 57394fb9 IDENTICAL
// OFFSET: 0x006394C0
uint32_t MippedImgCalcSize(uint32_t format, uint32_t width, uint32_t height) {
    uint32_t levelCount = CalcLevelCount(width, height);
    return CalcLevelOffset(levelCount, width, height, format) + levelCount * sizeof(MipBits);
}

// PROVEN: fcdb5fa1 IDENTICAL
// OFFSET: 0x006394F0
void MippedImgSet(uint32_t format, uint32_t width, uint32_t height, MipBits* images) {
    uint32_t levelCount = CalcLevelCount(width, height);
    uint32_t offset = 0;

    for (uint32_t i = 0; i < levelCount; i++) {
        images[i].m_mip[0] = reinterpret_cast<C4Pixel*>(reinterpret_cast<uint8_t*>(&images[levelCount]) + offset);
        offset += CalcLevelSize(i, width, height, format);
    }
}

// OFFSET: %0x00645B30
void FullShrink(C4Pixel* dest, uint32_t destWidth, uint32_t destHeight, C4Pixel* source, uint32_t sourceWidth, uint32_t sourceHeight) {
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
