#include <cstdint>
#include <cstring>
#include "Engine/Source/Images/StaticImage.h"
#include "Engine/Source/Gx/Blit.h"
#include "Engine/Source/Images/MippedImg.h"
#include "Engine/Source/Tempest/C2iVector.h"

// STATIC: 0x00CC9180
static const uint8_t s_eightBitAlphaLookup[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

// STATIC: 0x00CC9190
static const uint8_t s_oneBitAlphaLookup[2] = { 0x00, 0xFF };

// STATIC: 0x00CC9194
static const uint16_t s_oneBitAlpha4444[2] = { 0x0000, 0xF000 };

// STATIC: 0x00CC9198
static const uint8_t s_twoBitAlphaMask[4] = { 0x03, 0x0C, 0x30, 0xC0 };

// STATIC: 0x00CC919C
static const uint32_t s_twoBitAlphaShift[4] = { 0, 2, 4, 6 };

// STATIC: 0x00CC91B0
static const uint32_t s_fourBitAlphaShift[2] = { 2, 6 };

// STATIC: 0x00CC91BC/0x140BB9F60
static const BlitFormat s_blitFormats[NUM_PIXEL_FORMATS] = {
    BlitFormat_Dxt1,
    BlitFormat_Dxt3,
    BlitFormat_Argb8888,
    BlitFormat_Argb1555,
    BlitFormat_Argb4444,
    BlitFormat_Rgb565,
    BlitFormat_Unknown,
    BlitFormat_Dxt5,
    BlitFormat_Unknown,
    BlitFormat_Unknown,
};

// INLINED
uint32_t StaticImage::Width(uint32_t mipLevel) const {
    uint32_t width = this->m_width >> mipLevel;
    return width <= 1 ? 1 : width;
}

// INLINED
uint32_t StaticImage::Height(uint32_t mipLevel) const {
    uint32_t height = this->m_height >> mipLevel;
    return height <= 1 ? 1 : height;
}

// OFFSET: 0x0063C2D0/0x1402D4E70
int32_t StaticImage::SetToBLPData(uint8_t* data, uint32_t size) {
    const BLPHeader* header = reinterpret_cast<const BLPHeader*>(data);
    uint32_t levels = CalcLevelCount(header->m_width, header->m_height);
    uint32_t count = 0;

    for (uint32_t i = 0; i < 16; i++) {
        uint32_t mipSize = header->m_mipSizes[i];

        if (mipSize && i < levels) {
            uint32_t offset = header->m_mipOffsets[i];

            if (offset + mipSize > size) {
                return 0;
            }

            count++;
            this->m_mips[i] = data + offset;
            this->m_mipSizes[i] = mipSize;
        } else {
            this->m_mips[i] = NULL;
            this->m_mipSizes[i] = 0;
        }
    }

    this->m_width = header->m_width;
    this->m_height = header->m_height;
    this->m_mipCount = count;
    this->m_colorFormat = static_cast<COLOR_FILE_FORMAT>(header->m_colorEncoding);
    this->m_pixelFormat = static_cast<PIXEL_FORMAT>(header->m_preferredFormat);

    if (this->m_pixelFormat == PIXEL_UNSPECIFIED) {
        this->m_pixelFormat = PIXEL_ARGB8888;
    }

    this->m_alphaFormat = header->m_alphaSize;
    this->m_isAtlasable = 0;

    if (header->m_hasMips & 0x10) {
        this->m_isAtlasable = 1;
    }

    if (header->m_colorEncoding == COLOR_PAL) {
        this->m_palette = header->m_extended.m_palette;
    }

    return 1;
}

// OFFSET: 0x0063C3E0/0x1402D4F90
void StaticImage::Set(MipBits* images, uint32_t width, uint32_t height, uint32_t mipCount, COLOR_FILE_FORMAT colorFormat, PIXEL_FORMAT pixelFormat, ALPHA_FORMAT alphaFormat) {
    for (uint32_t i = 0; i < 16; i++) {
        if (i < mipCount) {
            this->m_mips[i] = reinterpret_cast<uint8_t*>(images->m_mip[i]);
        } else {
            this->m_mips[i] = NULL;
        }

        this->m_mipSizes[i] = 0;
    }

    this->m_width = width;
    this->m_height = height;
    this->m_isAtlasable = 0;
    this->m_palette = NULL;
    this->m_mipCount = mipCount;
    this->m_colorFormat = colorFormat;
    this->m_pixelFormat = pixelFormat;
    this->m_alphaFormat = alphaFormat;
}

// OFFSET: 0x0063C460
void StaticImage::DecompPalFastPath(uint8_t* out, const void* in, uint32_t count) {
    BlpPalPixel* pixels = reinterpret_cast<BlpPalPixel*>(out);
    const uint8_t* indices = static_cast<const uint8_t*>(in);

    for (uint32_t i = count; i; i--) {
        *pixels = this->m_palette[*indices];
        pixels->m_pad = indices[count];
        pixels++;
        indices++;
    }
}

// OFFSET: 0x0063C4B0
void StaticImage::DecompPalARGB8888(uint8_t* out, const void* in, uint32_t count) {
    BlpPalPixel* pixels = reinterpret_cast<BlpPalPixel*>(out);
    const uint8_t* indices = static_cast<const uint8_t*>(in);

    for (uint32_t i = 0; i < count; i++) {
        pixels[i] = this->m_palette[*indices];
        pixels[i].m_pad = 0xFF;
        indices++;
    }

    const uint8_t* alpha = indices;

    switch (this->m_alphaFormat) {
        case 1: {
            uint32_t bytes = count >> 3;

            for (uint32_t i = 0; i < bytes; i++) {
                uint8_t bits = alpha[i];

                for (uint32_t bit = 0; bit < 8; bit++) {
                    pixels->m_pad = s_oneBitAlphaLookup[bits & 1];
                    bits >>= 1;
                    pixels++;
                }
            }

            if (count & 7) {
                uint8_t bits = alpha[bytes];

                for (uint32_t bit = count & 7; bit; bit--) {
                    pixels->m_pad = s_oneBitAlphaLookup[bits & 1];
                    bits >>= 1;
                    pixels++;
                }
            }

            break;
        }

        case 4: {
            for (uint32_t i = count >> 1; i; i--) {
                pixels[0].m_pad = s_eightBitAlphaLookup[*alpha & 0xF];
                pixels[1].m_pad = s_eightBitAlphaLookup[*alpha >> 4];
                pixels += 2;
                alpha++;
            }

            if (count & 1) {
                pixels->m_pad = s_eightBitAlphaLookup[*alpha & 0xF];
            }

            break;
        }

        case 8:
        case 0x48: {
            for (uint32_t i = 0; i < count; i++) {
                pixels[i].m_pad = *alpha;
                alpha++;
            }

            break;
        }
    }
}

// OFFSET: 0x0063C6D0
void StaticImage::DecompPalARGB4444DitherFloydSteinberg(uint8_t* out, const void* in, uint32_t width, uint32_t height) {
    // STATIC: 0x00E29FC0
    static int32_t s_errors[2][1026][3];

    uint16_t* pixels = reinterpret_cast<uint16_t*>(out);
    const uint8_t* indices = static_cast<const uint8_t*>(in);

    memset(s_errors, 0, (width + 2) * sizeof(s_errors[0][0]));

    for (uint32_t y = 0; y < height; y++) {
        int32_t (*next)[3] = &s_errors[(y - 1) & 1][1];
        int32_t (*current)[3] = &s_errors[y & 1][1];

        next[-1][1] = 0;
        next[-1][2] = 0;
        next[-1][0] = 0;
        next[0][2] = 0;
        next[0][1] = 0;
        next[0][0] = 0;

        for (uint32_t x = 0; x < width; x++) {
            const BlpPalPixel& color = this->m_palette[indices[x]];

            int32_t r = (current[x][0] >> 4) + (color.m_r << 16);
            int32_t g = (current[x][1] >> 4) + (color.m_g << 16);
            int32_t b = (current[x][2] >> 4) + (color.m_b << 16);

            int32_t quantB = (b + 0x80000) & 0xFFF00000;
            int32_t quantG = (g + 0x80000) & 0xFFF00000;
            int32_t quantR = (r + 0x80000) & 0xFFF00000;

            int32_t outR = quantR >> 20;
            int32_t outG = quantG >> 20;
            int32_t outB = quantB >> 20;

            outR = outR < 0 ? 0 : (outR > 15 ? 15 : outR);
            outG = outG < 0 ? 0 : (outG > 15 ? 15 : outG);
            outB = outB < 0 ? 0 : (outB > 15 ? 15 : outB);

            pixels[x] = static_cast<uint16_t>((((outR << 4) | outG) << 4) | outB);

            int32_t errorR = r - quantR;
            int32_t errorG = g - quantG;
            int32_t errorB = b - quantB;

            current[x + 1][0] += 7 * errorR;
            current[x + 1][1] += 7 * errorG;
            current[x + 1][2] += 7 * errorB;
            (next + x)[-1][0] += 5 * errorR;
            (next + x)[-1][1] += 5 * errorG;
            (next + x)[-1][2] += 5 * errorB;
            next[x][0] += 3 * errorR;
            next[x][1] += 3 * errorG;
            next[x][2] += 3 * errorB;
            next[x + 1][1] = errorG;
            next[x + 1][2] = errorB;
            next[x + 1][0] = errorR;
        }

        indices += width;
        pixels += width;
    }

    uint32_t count = width * height;
    uint16_t* pixel = reinterpret_cast<uint16_t*>(out);
    const uint8_t* alpha = indices;

    switch (this->m_alphaFormat) {
        case 1: {
            uint32_t mask = 1;

            for (; count; count--) {
                *pixel |= s_oneBitAlpha4444[*alpha & mask];
                mask += mask;
                pixel++;

                if (mask >= 0x100) {
                    mask = 1;
                    alpha++;
                }
            }

            break;
        }

        case 4: {
            for (uint32_t i = 0; i < count; i++) {
                if (i & 1) {
                    *pixel |= (*alpha & 0xF0) << 8;
                    alpha++;
                } else {
                    *pixel |= static_cast<uint16_t>(*alpha << 12);
                }

                pixel++;
            }

            break;
        }

        case 8:
        case 0x48: {
            for (; count; count--) {
                *pixel |= (*alpha & 0xF0) << 8;
                pixel++;
                alpha++;
            }

            break;
        }
    }
}

// OFFSET: 0x0063C9F0
void StaticImage::DecompPalARGB1555DitherFloydSteinberg(uint8_t* out, const void* in, uint32_t width, uint32_t height) {
    // STATIC: 0x00E2FFF0
    static int32_t s_errors[2][1026][3];

    uint16_t* pixels = reinterpret_cast<uint16_t*>(out);
    const uint8_t* indices = static_cast<const uint8_t*>(in);

    memset(s_errors, 0, (width + 2) * sizeof(s_errors[0][0]));

    for (uint32_t y = 0; y < height; y++) {
        int32_t (*next)[3] = &s_errors[(y - 1) & 1][1];
        int32_t (*current)[3] = &s_errors[y & 1][1];

        next[-1][1] = 0;
        next[-1][2] = 0;
        next[-1][0] = 0;
        next[0][2] = 0;
        next[0][1] = 0;
        next[0][0] = 0;

        for (uint32_t x = 0; x < width; x++) {
            const BlpPalPixel& color = this->m_palette[indices[x]];

            int32_t r = (current[x][0] >> 4) + (color.m_r << 16);
            int32_t g = (current[x][1] >> 4) + (color.m_g << 16);
            int32_t b = (current[x][2] >> 4) + (color.m_b << 16);

            int32_t quantB = (b + 0x40000) & 0xFFF80000;
            int32_t quantG = (g + 0x40000) & 0xFFF80000;
            int32_t quantR = (r + 0x40000) & 0xFFF80000;

            int32_t outR = quantR >> 19;
            int32_t outG = quantG >> 19;
            int32_t outB = quantB >> 19;

            outR = outR < 0 ? 0 : (outR > 31 ? 31 : outR);
            outG = outG < 0 ? 0 : (outG > 31 ? 31 : outG);
            outB = outB < 0 ? 0 : (outB > 31 ? 31 : outB);

            pixels[x] = static_cast<uint16_t>((((outR << 5) | outG) << 5) | outB);

            int32_t errorR = r - quantR;
            int32_t errorG = g - quantG;
            int32_t errorB = b - quantB;

            current[x + 1][0] += 7 * errorR;
            current[x + 1][1] += 7 * errorG;
            current[x + 1][2] += 7 * errorB;
            (next + x)[-1][0] += 5 * errorR;
            (next + x)[-1][1] += 5 * errorG;
            (next + x)[-1][2] += 5 * errorB;
            next[x][0] += 3 * errorR;
            next[x][1] += 3 * errorG;
            next[x][2] += 3 * errorB;
            next[x + 1][1] = errorG;
            next[x + 1][2] = errorB;
            next[x + 1][0] = errorR;
        }

        indices += width;
        pixels += width;
    }

    uint32_t count = width * height;
    uint16_t* pixel = reinterpret_cast<uint16_t*>(out);
    const uint8_t* alpha = indices;
    uint32_t shift = 0;

    switch (this->m_alphaFormat) {
        case 1: {
            for (; count; count--) {
                *pixel |= static_cast<uint16_t>((*alpha & (1 << shift)) << (15 - shift));
                shift++;
                pixel++;

                if (shift >= 8) {
                    shift = 0;
                    alpha++;
                }
            }

            break;
        }

        case 4: {
            for (; count; count--) {
                if (shift) {
                    shift = 0;
                    *pixel |= (*alpha & 0x80) << 8;
                    alpha++;
                } else {
                    *pixel |= static_cast<uint16_t>((*alpha & 0xFFF8) << 12);
                    shift = this->m_alphaFormat;
                }

                pixel++;
            }

            break;
        }

        case 8:
        case 0x48: {
            for (; count; count--) {
                *pixel |= (*alpha & 0x80) << 8;
                pixel++;
                alpha++;
            }

            break;
        }
    }
}

// OFFSET: 0x0063CD30
void StaticImage::DecompPalARGB565DitherFloydSteinberg(uint8_t* out, const void* in, uint32_t width, uint32_t height) {
    // STATIC: 0x00E36020
    static int32_t s_errors[2][1026][3];

    uint16_t* pixels = reinterpret_cast<uint16_t*>(out);
    const uint8_t* indices = static_cast<const uint8_t*>(in);

    memset(s_errors, 0, (width + 2) * sizeof(s_errors[0][0]));

    for (uint32_t y = 0; y < height; y++) {
        int32_t (*next)[3] = &s_errors[(y - 1) & 1][1];
        int32_t (*current)[3] = &s_errors[y & 1][1];

        next[-1][1] = 0;
        next[-1][2] = 0;
        next[-1][0] = 0;
        next[0][2] = 0;
        next[0][1] = 0;
        next[0][0] = 0;

        for (uint32_t x = 0; x < width; x++) {
            const BlpPalPixel& color = this->m_palette[indices[x]];

            int32_t r = (current[x][0] >> 4) + (color.m_r << 16);
            int32_t g = (current[x][1] >> 4) + (color.m_g << 16);
            int32_t b = (current[x][2] >> 4) + (color.m_b << 16);

            int32_t quantB = (b + 0x40000) & 0xFFF80000;
            int32_t quantG = (g + 0x20000) & 0xFFFC0000;
            int32_t quantR = (r + 0x40000) & 0xFFF80000;

            int32_t outR = quantR >> 19;
            int32_t outG = quantG >> 18;
            int32_t outB = quantB >> 19;

            outR = outR < 0 ? 0 : (outR > 31 ? 31 : outR);
            outG = outG < 0 ? 0 : (outG > 63 ? 63 : outG);
            outB = outB < 0 ? 0 : (outB > 31 ? 31 : outB);

            pixels[x] = static_cast<uint16_t>((((outR << 6) | outG) << 5) | outB);

            int32_t errorR = r - quantR;
            int32_t errorG = g - quantG;
            int32_t errorB = b - quantB;

            current[x + 1][0] += 7 * errorR;
            current[x + 1][1] += 7 * errorG;
            current[x + 1][2] += 7 * errorB;
            (next + x)[-1][0] += 5 * errorR;
            (next + x)[-1][1] += 5 * errorG;
            (next + x)[-1][2] += 5 * errorB;
            next[x][0] += 3 * errorR;
            next[x][1] += 3 * errorG;
            next[x][2] += 3 * errorB;
            next[x + 1][1] = errorG;
            next[x + 1][0] = errorR;
            next[x + 1][2] = errorB;
        }

        indices += width;
        pixels += width;
    }
}

// OFFSET: 0x0063CF30
void StaticImage::DecompPalARGB2565DitherFloydSteinberg(uint8_t* out, const void* in, uint32_t width, uint32_t height) {
    // STATIC: 0x00E3C050
    static int32_t s_errors[2][1026][3];

    uint16_t* pixels = reinterpret_cast<uint16_t*>(out);
    const uint8_t* indices = static_cast<const uint8_t*>(in);

    memset(s_errors, 0, (width + 2) * sizeof(s_errors[0][0]));

    for (uint32_t y = 0; y < height; y++) {
        int32_t (*next)[3] = &s_errors[(y - 1) & 1][1];
        int32_t (*current)[3] = &s_errors[y & 1][1];

        next[-1][1] = 0;
        next[-1][2] = 0;
        next[-1][0] = 0;
        next[0][2] = 0;
        next[0][1] = 0;
        next[0][0] = 0;

        for (uint32_t x = 0; x < width; x++) {
            const BlpPalPixel& color = this->m_palette[indices[x]];

            int32_t r = (current[x][0] >> 4) + (color.m_r << 16);
            int32_t g = (current[x][1] >> 4) + (color.m_g << 16);
            int32_t b = (current[x][2] >> 4) + (color.m_b << 16);

            int32_t quantB = (b + 0x40000) & 0xFFF80000;
            int32_t quantG = (g + 0x20000) & 0xFFFC0000;
            int32_t quantR = (r + 0x40000) & 0xFFF80000;

            int32_t outR = quantR >> 19;
            int32_t outG = quantG >> 18;
            int32_t outB = quantB >> 19;

            outR = outR < 0 ? 0 : (outR > 31 ? 31 : outR);
            outG = outG < 0 ? 0 : (outG > 63 ? 63 : outG);
            outB = outB < 0 ? 0 : (outB > 31 ? 31 : outB);

            pixels[x] = static_cast<uint16_t>((((outR << 6) | outG) << 5) | outB);

            int32_t errorR = r - quantR;
            int32_t errorG = g - quantG;
            int32_t errorB = b - quantB;

            current[x + 1][0] += 7 * errorR;
            current[x + 1][1] += 7 * errorG;
            current[x + 1][2] += 7 * errorB;
            (next + x)[-1][0] += 5 * errorR;
            (next + x)[-1][1] += 5 * errorG;
            (next + x)[-1][2] += 5 * errorB;
            next[x][0] += 3 * errorR;
            next[x][1] += 3 * errorG;
            next[x][2] += 3 * errorB;
            next[x + 1][1] = errorG;
            next[x + 1][2] = errorB;
            next[x + 1][0] = errorR;
        }

        indices += width;
        pixels += width;
    }

    uint32_t count = width * height;
    uint8_t* alphaOut = reinterpret_cast<uint8_t*>(pixels);
    const uint8_t* alpha = indices;

    switch (this->m_alphaFormat) {
        case 1: {
            for (uint32_t i = 0; i < count; i++) {
                uint8_t* dest = alphaOut + (i >> 2);
                uint8_t value = static_cast<uint8_t>(((alpha[i >> 3] >> (i & 7)) * 2) << s_twoBitAlphaShift[i & 3]);
                *dest = (*dest & ~s_twoBitAlphaMask[i & 3]) | value;
            }

            break;
        }

        case 4: {
            for (uint32_t i = 0; i < count; i++) {
                uint8_t* dest = alphaOut + (i >> 2);
                uint8_t value = static_cast<uint8_t>((alpha[i >> 1] >> s_fourBitAlphaShift[i & 1]) << s_twoBitAlphaShift[i & 3]);
                *dest = (*dest & ~s_twoBitAlphaMask[i & 3]) | value;
            }

            break;
        }

        case 8:
        case 0x48: {
            for (uint32_t i = 0; i < count; i++) {
                uint8_t* dest = alphaOut + (i >> 2);
                uint8_t value = static_cast<uint8_t>((alpha[i] >> 6) << s_twoBitAlphaShift[i & 3]);
                *dest = (*dest & ~s_twoBitAlphaMask[i & 3]) | value;
            }

            break;
        }
    }
}

// OFFSET: 0x0063D2A0
int32_t StaticImage::DecompPalDither(PIXEL_FORMAT format, uint32_t mipLevel, uint8_t* out, const void* in) {
    uint32_t width = this->Width(mipLevel);
    uint32_t height = this->Height(mipLevel);

    switch (format) {
        case PIXEL_ARGB8888:
            if ((this->m_alphaFormat & 0x3F) == 8) {
                this->DecompPalFastPath(out, in, width * height);
            } else {
                this->DecompPalARGB8888(out, in, width * height);
            }

            return 1;

        case PIXEL_ARGB1555:
            this->DecompPalARGB1555DitherFloydSteinberg(out, in, width, height);
            return 1;

        case PIXEL_ARGB4444:
            this->DecompPalARGB4444DitherFloydSteinberg(out, in, width, height);
            return 1;

        case PIXEL_RGB565:
            this->DecompPalARGB565DitherFloydSteinberg(out, in, width, height);
            return 1;

        case PIXEL_ARGB2565:
            this->DecompPalARGB2565DitherFloydSteinberg(out, in, width, height);
            return 1;

        default:
            return 0;
    }
}

// OFFSET: 0x0063D3B0
int32_t StaticImage::GetFormatSize(PIXEL_FORMAT format, uint32_t mipLevel, uint32_t* size, uint32_t* stride) {
    uint32_t width = this->Width(mipLevel);
    uint32_t height = this->Height(mipLevel);

    uint32_t pixels = width * height;
    uint32_t alphaBytes = pixels >> 2;

    if (!alphaBytes) {
        alphaBytes = 1;
    }

    switch (format) {
        case PIXEL_ARGB8888:
            *size = 4 * pixels;
            *stride = 4 * width;
            return 1;

        case PIXEL_ARGB1555:
        case PIXEL_ARGB4444:
        case PIXEL_RGB565:
            *size = 2 * pixels;
            *stride = 2 * width;
            return 1;

        case PIXEL_ARGB2565:
            *size = alphaBytes + 2 * pixels;
            *stride = 2 * width;
            return 1;

        default:
            *size = 0;
            *stride = 0;
            return 0;
    }
}

// OFFSET: 0x0063D490/0x1402D62E0
int32_t StaticImage::ConvertMip(PIXEL_FORMAT format, uint8_t* out, uint32_t mipLevel) {
    uint8_t* data = this->m_mips[mipLevel];
    uint32_t dataSize = this->m_mipSizes[mipLevel];

    switch (this->m_colorFormat) {
        case COLOR_PAL: {
            uint32_t size;
            uint32_t stride;

            if (!this->GetFormatSize(format, mipLevel, &size, &stride)) {
                return 0;
            }

            return this->DecompPalDither(format, mipLevel, out, data);
        }

        case COLOR_DXT:
            switch (format) {
                case PIXEL_DXT1:
                case PIXEL_DXT3:
                case PIXEL_DXT5:
                    memcpy(out, data, dataSize);
                    return 1;

                case PIXEL_ARGB8888:
                case PIXEL_ARGB1555:
                case PIXEL_ARGB4444:
                case PIXEL_RGB565: {
                    uint32_t width = this->Width(mipLevel);
                    uint32_t height = this->Height(mipLevel);

                    BlitFormat inFormat = s_blitFormats[this->m_pixelFormat];
                    BlitFormat outFormat = s_blitFormats[format];
                    C2iVector size = { static_cast<int32_t>(width), static_cast<int32_t>(height) };

                    return Blit(size, BlitAlpha_0, data, CalcRowStride(inFormat, width, height), inFormat, out, CalcRowStride(outFormat, width, height), outFormat);
                }

                default:
                    return 0;
            }

        case COLOR_ARGB8888:
        case COLOR_ARGB8888_2:
            memcpy(out, data, dataSize);
            return 1;

        default:
            return 0;
    }
}

// OFFSET: 0x0063D5F0/0x1402D64B0
int32_t StaticImage::Convert(PIXEL_FORMAT format, MipBits* images, uint32_t mipLevel) {
    uint32_t width = this->Width(mipLevel);
    uint32_t height = this->Height(mipLevel);

    MippedImgSet(format, width, height, images);

    for (C4Pixel** mip = images->m_mip; mipLevel < this->m_mipCount; mipLevel++, mip++) {
        if (!this->ConvertMip(format, reinterpret_cast<uint8_t*>(*mip), mipLevel)) {
            return 0;
        }
    }

    return 1;
}
