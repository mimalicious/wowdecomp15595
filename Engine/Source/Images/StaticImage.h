#ifndef ENGINE_SOURCE_IMAGES_STATICIMAGE_H
#define ENGINE_SOURCE_IMAGES_STATICIMAGE_H

#include <cstdint>
#include "Engine/Source/BLPFile/Types.h"
#include "Engine/Source/Services/Types.h"

enum COLOR_FILE_FORMAT {
    COLOR_JPEG = 0,
    COLOR_PAL = 1,
    COLOR_DXT = 2,
    COLOR_ARGB8888 = 3,
    COLOR_ARGB8888_2 = 4,
};

enum ALPHA_FORMAT {
    ALPHA_0 = 0,
    ALPHA_1 = 1,
    ALPHA_4 = 4,
    ALPHA_8 = 8,
};

class StaticImage {
    public:
    /* 0000 */ uint8_t* m_mips[16];
    /* 0040 */ uint32_t m_mipSizes[16];
    /* 0080 */ uint32_t m_width;
    /* 0084 */ uint32_t m_height;
    /* 0088 */ uint32_t m_mipCount;
    /* 008C */ COLOR_FILE_FORMAT m_colorFormat;
    /* 0090 */ PIXEL_FORMAT m_pixelFormat;
    /* 0094 */ uint32_t m_alphaFormat;
    /* 0098 */ uint32_t m_isAtlasable;
    /* 009C */ const BlpPalPixel* m_palette;

    uint32_t Height(uint32_t mipLevel) const;
    uint32_t Width(uint32_t mipLevel) const;
    int32_t Convert(PIXEL_FORMAT format, MipBits* images, uint32_t mipLevel);
    int32_t ConvertMip(PIXEL_FORMAT format, uint8_t* out, uint32_t mipLevel);
    void Set(MipBits* images, uint32_t width, uint32_t height, uint32_t mipCount, COLOR_FILE_FORMAT colorFormat, PIXEL_FORMAT pixelFormat, ALPHA_FORMAT alphaFormat);
    int32_t SetToBLPData(uint8_t* data, uint32_t size);

    protected:
    void DecompPalARGB1555DitherFloydSteinberg(uint8_t* out, const void* in, uint32_t width, uint32_t height);
    void DecompPalARGB2565DitherFloydSteinberg(uint8_t* out, const void* in, uint32_t width, uint32_t height);
    void DecompPalARGB4444DitherFloydSteinberg(uint8_t* out, const void* in, uint32_t width, uint32_t height);
    void DecompPalARGB565DitherFloydSteinberg(uint8_t* out, const void* in, uint32_t width, uint32_t height);
    void DecompPalARGB8888(uint8_t* out, const void* in, uint32_t count);
    int32_t DecompPalDither(PIXEL_FORMAT format, uint32_t mipLevel, uint8_t* out, const void* in);
    void DecompPalFastPath(uint8_t* out, const void* in, uint32_t count);
    int32_t GetFormatSize(PIXEL_FORMAT format, uint32_t mipLevel, uint32_t* size, uint32_t* stride);
};

#include "Engine/Source/Images/StaticImage.inl"

#endif
