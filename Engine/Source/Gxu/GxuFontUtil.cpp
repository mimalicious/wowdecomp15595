#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cwctype>
#include "Engine/Source/Gxu/GxuFontUtil.h"
#include "Engine/Source/Base/ConvertUTF.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Gxu/GxuFontFont.h"
#include "Engine/Source/Gxu/GxuFontMiscClasses.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRect.h"

// OFFSET: %0x006277D0
static int32_t CanWrapBetween(uint32_t previous, uint32_t next) {
    if (!previous) {
        return 0;
    }

    if (previous == '-' || previous == ';' || previous == '/' || next == '|' || next == 0xFFFFFFFF) {
        return 1;
    }

    if (iswspace(previous)) {
        return 0;
    }

    if (iswspace(next)) {
        return 1;
    }

    switch (previous) {
        case 0x0024:
        case 0x0028:
        case 0x005B:
        case 0x005C:
        case 0x007B:
        case 0x2018:
        case 0x201C:
        case 0x2035:
        case 0x3008:
        case 0x300A:
        case 0x300C:
        case 0x300E:
        case 0x3010:
        case 0x3014:
        case 0x301D:
        case 0xFE59:
        case 0xFE5B:
        case 0xFE5D:
        case 0xFF04:
        case 0xFF08:
        case 0xFF3B:
        case 0xFF5B:
        case 0xFFE1:
        case 0xFFE5:
        case 0xFFE6:
            return 0;
    }

    switch (next) {
        case 0x0021:
        case 0x0025:
        case 0x0029:
        case 0x002C:
        case 0x002E:
        case 0x003A:
        case 0x003B:
        case 0x003F:
        case 0x005D:
        case 0x007D:
        case 0x00B0:
        case 0x00B7:
        case 0x2013:
        case 0x2014:
        case 0x2019:
        case 0x201D:
        case 0x2022:
        case 0x2026:
        case 0x2027:
        case 0x2032:
        case 0x2033:
        case 0x2103:
        case 0x3001:
        case 0x3002:
        case 0x3009:
        case 0x300B:
        case 0x300D:
        case 0x300F:
        case 0x3011:
        case 0x3015:
        case 0x301E:
        case 0x30FC:
        case 0xFE30:
        case 0xFE50:
        case 0xFE51:
        case 0xFE52:
        case 0xFE54:
        case 0xFE55:
        case 0xFE56:
        case 0xFE57:
        case 0xFE5A:
        case 0xFE5C:
        case 0xFE5E:
        case 0xFF01:
        case 0xFF05:
        case 0xFF09:
        case 0xFF0C:
        case 0xFF0E:
        case 0xFF1A:
        case 0xFF1B:
        case 0xFF1F:
        case 0xFF3D:
        case 0xFF5D:
        case 0xFF70:
        case 0xFF9E:
        case 0xFF9F:
        case 0xFFE0:
            return 0;
    }

    if (previous == 0x3002 || previous == 0xFF0C) {
        return 1;
    }

    if ((next >= 0x1100 && next <= 0x11FF) || (next >= 0x3000 && next <= 0xD7AF) || (next >= 0xF900 && next <= 0xFAFF) || (next >= 0xFF00 && next <= 0xFF9F)) {
        return 1;
    }

    return next - 0xFFA0 <= 0x3C;
}

// OFFSET: %0x00627E80
static float GetCharacterWidth(const char* text, uint32_t flags, uint32_t code, CGxFont* face, float fontHeight) {
    if (!code) {
        return 0.0f;
    }

    float width = 0.0f;

    while (*text) {
        int32_t advance;
        uint32_t wide;
        QUOTEDCODE quotedCode = GxuDetermineQuotedCode(text, advance, NULL, flags, wide, 1);
        text += advance;

        if (quotedCode == CODE_INVALIDCODE) {
            width = flags & 0x10 ? face->ComputeStepFixedWidth(code, wide) : face->ComputeStep(code, wide);

            if (0.0f != width) {
                return width;
            }

            break;
        }

        if (quotedCode == CODE_NEWLINE) {
            break;
        }
    }

    CHARCODEDESC* desc = face->NewCodeDesc(code);

    if (desc) {
        width = face->GetGlyphBearing(desc, (flags >> 7) & 1, fontHeight) + desc->m_bitmapData.m_glyphCellWidth;
    }

    return width;
}

// OFFSET: %0x00627F50
void* FreeTypeAlloc(FT_Memory memory, long size) {
    return SMemAlloc(size, __FILE__, __LINE__, 0);
}

// PROVEN: 57edaa45 IDENTICAL/NONE
// OFFSET: 0x00617700
void FreeTypeFree(FT_Memory memory, void* block) {
    if (block) {
        SMemFree(block, __FILE__, __LINE__, 0);
    }
}

// OFFSET: %0x00627F90
void* FreeTypeRealloc(FT_Memory memory, long curSize, long newSize, void* block) {
    return SMemReAlloc(block, newSize, __FILE__, __LINE__, 0);
}

// PROVEN: 311b1b94 DIFFERENT/IDENTICAL
// OFFSET: 0x00617740/0x1402A46E0
float PixelToScreenHeight(int32_t height) {
    return static_cast<float>(height) / static_cast<float>(g_heightPixels);
}

// PROVEN: 4a8c5821 DIFFERENT/IDENTICAL
// OFFSET: 0x00617770/0x1402A4700
float GetMaxScreenHeight() {
    return 32.0f / static_cast<float>(g_heightPixels);
}

// OFFSET: %0x00628010
void InternalWipeUntilNameTokenStop(char* text) {
    if (!text || !*text) {
        return;
    }

    do {
        int32_t advance;
        uint32_t wide;

        if (GxuDetermineQuotedCode(text, advance, NULL, 0, wide, 1) == CODE_TOKENSTOP) {
            break;
        }

        if (advance) {
            memset(text, '0', advance);
            text += advance;
        }
    } while (*text);
}

// PROVEN: 94d175ba IDENTICAL/DIFFERENT
// OFFSET: 0x00617800/0x1402A47A0
GXUEMBEDDEDTEXTUREINFO::GXUEMBEDDEDTEXTUREINFO() {
    this->m_lineHeight = 0.0f;
    this->m_left = 0.0f;
    this->m_baseLineHeight = 0.0f;
    this->m_right = 0.0f;
    this->m_path = NULL;
    this->m_pathLength = 0;
    this->m_offsetX = 0.0f;
    this->m_offsetY = 0.0f;
    this->m_texCoords.minY = 0.0f;
    this->m_texCoords.minX = 0.0f;
    this->m_texCoords.maxY = 0.0f;
    this->m_texCoords.maxX = 0.0f;
    *reinterpret_cast<uint32_t*>(&this->m_color) = 0;
    this->m_hasTexCoords = 0;
    this->m_hasColor = 0;
}

// OFFSET: %0x006280C0
float ScreenToPixelHeight(int32_t unscaled, float height) {
    if (unscaled) {
        return height;
    }

    float pixels = g_heightPixels * height;
    return CMath::fint_n(pixels);
}

// OFFSET: %0x00628100
float ScreenToPixelWidth(int32_t unscaled, float width) {
    if (unscaled) {
        return width;
    }

    float pixels = g_widthPixels * width;
    return CMath::fint_n(pixels);
}

// OFFSET: %0x00628140
bool SetProjection() {
    float minX = g_theGxDevicePtr->m_viewport.m_x.m_l;
    float maxX = g_theGxDevicePtr->m_viewport.m_x.m_h;
    float minY = g_theGxDevicePtr->m_viewport.m_y.m_l;
    float maxY = g_theGxDevicePtr->m_viewport.m_y.m_h;
    float minZ = g_theGxDevicePtr->m_viewport.m_z.m_l;
    float maxZ = g_theGxDevicePtr->m_viewport.m_z.m_h;

    C44Matrix projection;

    float widthPixels = g_widthPixels;
    minX = floor(widthPixels * minX);
    maxX = floor(widthPixels * maxX);

    if (!(minX < maxX)) {
        return false;
    }

    float heightPixels = g_heightPixels;
    minY = floor(heightPixels * minY);
    maxY = floor(heightPixels * maxY);

    if (!(minY < maxY)) {
        return false;
    }

    if (heightPixels > maxY) {
        minY = floor(minY * 0.5);
        minY = minY + minY;
        maxY = ceil(maxY * 0.5);
        maxY = maxY + maxY;

        float screenHeight = g_heightPixels;
        float screenWidth = g_widthPixels;
        GxXformSetViewport(minX / screenWidth, maxX / screenWidth, minY / screenHeight, maxY / screenHeight, minZ, maxZ);
    }

    GxuXformCreateOrthoDepth(minX, maxX, minY, maxY, -5000.0f, 5000.0f, projection);
    GxuAdjustProjForHalfPixel(projection);
    GxXformSetProjection(projection);

    return true;
}

// OFFSET: %0x00628410
bool ParseEmbeddedTexture(const char* text, GXUEMBEDDEDTEXTUREINFO* info, float fontPixelHeight, float scale, float fontHeight) {
    CRect window;
    window.minY = 0.0f;
    window.minX = 0.0f;
    window.maxY = 0.0f;
    window.maxX = 0.0f;
    g_theGxDevicePtr->CapsWindowSize(window);

    float windowHeight = window.maxY - window.minY;

    double height = fontHeight;
    float referenceHeight = 14.0 / (CoordinateGetAspectCompensation() * 1024.0);
    double heightRatio = height / NDCToDDCWidth(referenceHeight);
    double windowPixels = windowHeight;
    float pixelScale = windowPixels / (CoordinateGetAspectCompensation() * 1024.0) * heightRatio;

    if (!text || !*text || !info) {
        return false;
    }

    info->m_offsetX = 0.0f;
    info->m_path = NULL;
    info->m_offsetY = 0.0f;
    info->m_pathLength = 0;
    info->m_hasTexCoords = 0;
    info->m_hasColor = 0;

    const char* start = text + g_quotedCodeLengths[CODE_TEXTURESTART];
    const char* end = SStrChr(start, '|');

    if (!end || start >= end || end[1] != 't') {
        return false;
    }

    const char* colon = SStrChr(start, ':');

    if (!colon || colon >= end) {
        return false;
    }

    info->m_pathLength = colon - start;
    info->m_path = start;

    char buffer[32];

    const char* field = colon + 1;
    const char* next = SStrChr(field, ':');

    if (!next || next > end) {
        next = end;
    }

    if (!next || next > end || field >= end) {
        return false;
    }

    int32_t length = next - field + 1;
    bool heightFromFont = false;
    SStrCopy(buffer, field, length > 32 ? 32 : length);

    float textureHeight = SStrToFloat(buffer) * scale * pixelScale;
    info->m_height = textureHeight;

    if (!(0.0f < textureHeight)) {
        heightFromFont = true;
        info->m_height = fontPixelHeight;
    }

    field = next + 1;
    next = SStrChr(field, ':');

    if (!next || next > end) {
        next = end;
    }

    if (!next || next > end || field >= end) {
        info->m_width = info->m_height;
        return true;
    }

    length = next - field + 1;
    SStrCopy(buffer, field, length > 32 ? 32 : length);

    float textureWidth = SStrToFloat(buffer) * scale;
    info->m_width = textureWidth;

    if (0.0f < textureWidth) {
        info->m_width = heightFromFont ? textureWidth * info->m_height : textureWidth * pixelScale;
    } else {
        info->m_width = fontPixelHeight;
    }

    field = next + 1;
    next = SStrChr(field, ':');

    if (!next || next > end) {
        next = end;
    }

    if (!next || next > end || field >= end) {
        return true;
    }

    length = next - field + 1;
    SStrCopy(buffer, field, length > 32 ? 32 : length);
    info->m_offsetX = SStrToFloat(buffer) * scale;

    field = next + 1;
    next = SStrChr(field, ':');

    if (!next || next > end) {
        next = end;
    }

    if (!next || next > end || field >= end) {
        return true;
    }

    length = next - field + 1;
    SStrCopy(buffer, field, length > 32 ? 32 : length);
    info->m_offsetY = SStrToFloat(buffer) * scale;

    field = next + 1;
    next = SStrChr(field, ':');

    if (!next || next > end) {
        next = end;
    }

    if (!next || next > end || field >= end) {
        return true;
    }

    length = next - field + 1;
    SStrCopy(buffer, field, length > 32 ? 32 : length);
    int32_t imageWidth = SStrToInt(buffer);

    field = next + 1;
    next = SStrChr(field, ':');

    if (!next || next > end) {
        next = end;
    }

    if (!next || next > end || field >= end) {
        return true;
    }

    length = next - field + 1;
    SStrCopy(buffer, field, length > 32 ? 32 : length);
    int32_t imageHeight = SStrToInt(buffer);

    field = next + 1;
    next = SStrChr(field, ':');

    if (!next || next > end) {
        next = end;
    }

    if (imageHeight <= 0 || imageWidth <= 0 || !next || next > end || field >= end) {
        return true;
    }

    length = next - field + 1;
    SStrCopy(buffer, field, length > 32 ? 32 : length);
    float imageWidthF = imageWidth;
    int32_t left = SStrToInt(buffer);
    info->m_texCoords.minX = left / imageWidthF;

    field = next + 1;
    next = SStrChr(field, ':');

    if (!next || next > end) {
        next = end;
    }

    if (!next || next > end || field >= end) {
        return true;
    }

    length = next - field + 1;
    SStrCopy(buffer, field, length > 32 ? 32 : length);
    int32_t right = SStrToInt(buffer);
    info->m_texCoords.maxX = right / imageWidthF;

    field = next + 1;
    next = SStrChr(field, ':');

    if (!next || next > end) {
        next = end;
    }

    if (!next || next > end || field >= end) {
        return true;
    }

    length = next - field + 1;
    SStrCopy(buffer, field, length > 32 ? 32 : length);
    float imageHeightF = imageHeight;
    int32_t top = SStrToInt(buffer);
    info->m_texCoords.minY = top / imageHeightF;

    field = next + 1;
    next = SStrChr(field, ':');

    if (!next || next > end) {
        next = end;
    }

    if (!next || next > end || field >= end) {
        return true;
    }

    length = next - field + 1;
    SStrCopy(buffer, field, length > 32 ? 32 : length);
    int32_t bottom = SStrToInt(buffer);
    info->m_hasTexCoords = 1;
    info->m_texCoords.maxY = bottom / imageHeightF;

    field = next + 1;
    next = SStrChr(field, ':');

    if (!next || next > end) {
        next = end;
    }

    if (!next || next > end || field >= end) {
        return true;
    }

    length = next - field + 1;
    SStrCopy(buffer, field, length > 32 ? 32 : length);
    info->m_color.a = 0xFF;
    info->m_color.r = SStrToInt(buffer);

    field = next + 1;
    next = SStrChr(field, ':');

    if (!next || next > end) {
        next = end;
    }

    if (!next || next > end || field >= end) {
        return true;
    }

    length = next - field + 1;
    SStrCopy(buffer, field, length > 32 ? 32 : length);
    info->m_color.g = SStrToInt(buffer);

    field = next + 1;
    next = SStrChr(field, ':');

    if (!next || next > end) {
        next = end;
    }

    if (!next || next > end || field >= end) {
        return true;
    }

    length = next - field + 1;
    SStrCopy(buffer, field, length > 32 ? 32 : length);
    info->m_color.b = SStrToInt(buffer);
    info->m_hasColor = 1;

    SStrChr(next + 1, ':');

    return true;
}

// OFFSET: %0x00628CA0
void InternalGetTextExtent(CGxFont* face, const char* text, uint32_t numBytes, float fontHeight, float* extent, uint32_t flags, float shadowWidth, float scale) {
    if (!face || !text || !extent) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    if (0.0f == fontHeight || (flags & 4)) {
        fontHeight = GxuFontGetOneToOneHeight(face);
    }

    float width = 0.0f;

    if (shadowWidth > 0.0f) {
        float shadowPixels = shadowWidth * g_widthPixels;
        width = ceil(static_cast<double>(CMath::fint_n(shadowPixels)));
    }

    if (face->m_fontFlags & 8) {
        width = width + 4.0;
    } else if (face->m_fontFlags & 1) {
        width = width + 2.0;
    }

    float characterWidth = 0.0f;
    float maxWidth = 0.0f;
    uint32_t lastCode = 0;
    char* tokenStart = NULL;

    while (*text && numBytes) {
        int32_t advance;
        uint32_t wide;
        QUOTEDCODE code = GxuDetermineQuotedCode(text, advance, NULL, flags, wide, 1);
        numBytes -= advance;
        text += advance;

        switch (code) {
            case CODE_COLORON:
            case CODE_COLORRESTORE:
            case CODE_HYPERLINKSTART:
            case CODE_HYPERLINKSTOP:
            case CODE_TEXTURESTOP:
            case CODE_NOWRAPSTART:
            case CODE_NOWRAPSTOP: {
                break;
            }

            case CODE_TOKENSTART: {
                tokenStart = const_cast<char*>(text);
                break;
            }

            case CODE_TOKENSTOP: {
                if (tokenStart) {
                    for (; tokenStart != text - advance; tokenStart++) {
                        *tokenStart = '0';
                    }

                    tokenStart = NULL;
                }

                break;
            }

            case CODE_TEXTURESTART: {
                GXUEMBEDDEDTEXTUREINFO info;
                float fontPixelHeight = ScreenToPixelHeight(flags & 0x80, fontHeight);

                if (ParseEmbeddedTexture(text - advance, &info, fontPixelHeight, scale, fontHeight)) {
                    width = info.m_width + width;
                }

                break;
            }

            case CODE_NEWLINE: {
                float lineWidth = characterWidth + width;
                maxWidth = maxWidth >= lineWidth ? maxWidth : lineWidth;
                width = 0.0f;
                characterWidth = 0.0f;
                break;
            }

            default: {
                if (wide == '\n') {
                    float lineWidth = characterWidth + width;
                    maxWidth = maxWidth >= lineWidth ? maxWidth : lineWidth;
                    width = 0.0f;
                    characterWidth = 0.0f;
                    break;
                }

                if (!face->NewCodeDesc(wide)) {
                    break;
                }

                float step = 0.0f;

                if (lastCode) {
                    step = flags & 0x10 ? face->ComputeStepFixedWidth(lastCode, wide) : face->ComputeStep(lastCode, wide);
                }

                characterWidth = GetCharacterWidth(text, flags, wide, face, fontHeight);
                lastCode = wide;
                width = step + width;
                break;
            }
        }
    }

    InternalWipeUntilNameTokenStop(tokenStart);

    float lineWidth = characterWidth + width;
    double totalWidth = maxWidth >= lineWidth ? maxWidth : lineWidth;

    float fontPixelHeight = ScreenToPixelHeight(flags & 0x80, fontHeight);
    double pixelHeight = fontPixelHeight;
    float ratio = pixelHeight / face->GetPixelSize();
    float total = totalWidth;
    float result = ratio * total;

    if (flags & 0x80) {
        *extent = result;
    } else {
        *extent = result / g_widthPixels;
    }
}

// OFFSET: %0x00628FD0
uint32_t InternalGetMaxCharsWithinWidth(CGxFont* face, const char* text, float fontHeight, float maxWidth, uint32_t numBytes, float* extent, uint32_t flags, float shadowWidth, float scale, uint32_t* bytesInString, float* widthArray) {
    if (!face || !text || !extent) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    uint32_t numChars = 0;
    uint32_t unscaled = flags & 0x80;

    if (!unscaled && (0.0f == fontHeight || (flags & 4))) {
        fontHeight = GxuFontGetOneToOneHeight(face);
    }

    float width = 0.0f;

    if (shadowWidth > 0.0f) {
        float shadowPixels = shadowWidth * g_widthPixels;
        width = ceil(static_cast<double>(CMath::fint_n(shadowPixels)));
    }

    if (face->m_fontFlags & 8) {
        width = width + 4.0;
    } else if (face->m_fontFlags & 1) {
        width = width + 2.0;
    }

    float characterWidth = 0.0f;
    uint32_t lastCode = 0;

    float targetPixels = static_cast<float>(face->GetPixelSize()) * (g_widthPixels * maxWidth);
    float fontPixelHeight;

    if (unscaled) {
        fontPixelHeight = fontHeight;
    } else {
        float pixels = g_heightPixels * fontHeight;
        fontPixelHeight = CMath::fint_n(pixels);
    }

    float divisor = fontPixelHeight > 1.0f ? fontPixelHeight : 1.0f;
    float maxPixelsF = targetPixels / divisor;
    uint32_t maxPixels = CMath::fuint_pi(maxPixelsF);

    double pixelHeight = fontPixelHeight;
    float ratio = pixelHeight / face->GetPixelSize();

    float* widthOut = widthArray;
    const char* start = text;
    char* tokenStart = NULL;

    while (*text && numBytes) {
        int32_t advance;
        uint32_t wide;
        QUOTEDCODE code = GxuDetermineQuotedCode(text, advance, NULL, flags, wide, 1);
        numBytes -= advance;
        text += advance;

        if (code == CODE_NEWLINE) {
            break;
        }

        if (code == CODE_COLORON || code == CODE_COLORRESTORE || code == CODE_HYPERLINKSTART || code == CODE_HYPERLINKSTOP || code == CODE_TEXTURESTOP || code == CODE_NOWRAPSTART || code == CODE_NOWRAPSTOP) {
            continue;
        }

        if (code == CODE_TOKENSTART) {
            tokenStart = const_cast<char*>(text);
            continue;
        }

        if (code == CODE_TOKENSTOP) {
            if (tokenStart) {
                for (; tokenStart != text - advance; tokenStart++) {
                    *tokenStart = '0';
                }

                tokenStart = NULL;
            }

            continue;
        }

        if (code == CODE_TEXTURESTART) {
            GXUEMBEDDEDTEXTUREINFO info;

            if (!ParseEmbeddedTexture(text - advance, &info, fontPixelHeight, scale, fontHeight)) {
                continue;
            }

            float newWidth = info.m_width + width;

            if (maxPixels >= newWidth) {
                lastCode = 0;
                width = newWidth;
                numChars++;
                continue;
            }

            text -= advance;
        }

        if (!face->NewCodeDesc(wide)) {
            numChars++;
            continue;
        }

        float step = 0.0f;

        if (lastCode) {
            step = flags & 0x10 ? face->ComputeStepFixedWidth(lastCode, wide) : face->ComputeStep(lastCode, wide);
        }

        float nextCharacterWidth = GetCharacterWidth(text, flags, wide, face, fontHeight);
        float newWidth = step + width;

        if (maxPixels < nextCharacterWidth + newWidth) {
            text -= advance;
            break;
        }

        numChars++;
        width = newWidth;
        lastCode = wide;
        characterWidth = nextCharacterWidth;

        if (widthArray) {
            float scaled = ratio * width;

            if (unscaled) {
                *widthOut = scaled;
            } else {
                *widthOut = scaled / g_widthPixels;
            }

            widthOut++;
        }
    }

    InternalWipeUntilNameTokenStop(tokenStart);

    float total = characterWidth + width;
    float result = total * ratio;

    if (unscaled) {
        *extent = result;
    } else {
        *extent = result / g_widthPixels;
    }

    if (bytesInString) {
        *bytesInString = text - start;
    }

    return numChars;
}

// OFFSET: %0x00629400
static void CalcWrapPointBillboarded(const char* text, uint32_t flags, CGxFont* face, float fontHeight, uint32_t* numBytes, float* width, const char** nextText, float* height, float scale) {
    if (0.0f == fontHeight) {
        *numBytes = 0;
        *width = 0.0f;
        *nextText = NULL;

        if (height) {
            *height = 0.0f;
        }

        return;
    }

    float characterWidth = 0.0f;
    float lineWidth = 0.0f;
    uint32_t lastCode = 0;
    const char* start = text;
    int32_t advance;
    uint32_t wide;

    if (height) {
        *height = 0.0f;
    }

    char* tokenStart = NULL;

    while (*text) {
        QUOTEDCODE code = GxuDetermineQuotedCode(text, advance, NULL, flags, wide, 1);

        if (code == CODE_NEWLINE) {
            break;
        }

        switch (code) {
            case CODE_COLORON:
            case CODE_COLORRESTORE:
            case CODE_HYPERLINKSTART:
            case CODE_HYPERLINKSTOP:
            case CODE_TEXTURESTOP:
            case CODE_NOWRAPSTART:
            case CODE_NOWRAPSTOP: {
                break;
            }

            case CODE_TOKENSTART: {
                tokenStart = const_cast<char*>(text + advance);
                break;
            }

            case CODE_TOKENSTOP: {
                if (tokenStart) {
                    if (tokenStart != text) {
                        memset(tokenStart, '0', text - tokenStart);
                    }

                    tokenStart = NULL;
                }

                break;
            }

            case CODE_TEXTURESTART: {
                GXUEMBEDDEDTEXTUREINFO info;
                float fontPixelHeight = ScreenToPixelHeight(flags & 0x80, fontHeight);

                if (ParseEmbeddedTexture(text, &info, fontPixelHeight, scale, fontHeight)) {
                    lineWidth = info.m_width + lineWidth;

                    if (height && *height < info.m_height) {
                        *height = info.m_height;
                    }
                }

                break;
            }

            default: {
                if (face->NewCodeDesc(wide)) {
                    float step = lastCode ? (flags & 0x10 ? face->ComputeStepFixedWidth(lastCode, wide) : face->ComputeStep(lastCode, wide)) : 0.0f;
                    lineWidth = step + lineWidth;
                    characterWidth = GetCharacterWidth(text + advance, flags, wide, face, fontHeight);
                    lastCode = wide;
                }

                break;
            }
        }

        text += advance;
    }

    InternalWipeUntilNameTokenStop(tokenStart);

    double pixelHeight = fontHeight;
    float ratio = pixelHeight / face->GetPixelSize();
    *numBytes = text - start;
    float total = lineWidth + characterWidth;
    *width = ratio * total;

    while (*text) {
        wide = sgetu8(reinterpret_cast<const uint8_t*>(text), &advance);

        if (!iswspace(wide)) {
            break;
        }

        text += advance;
    }

    *nextText = text;
}

// OFFSET: %0x006296B0
static void CalcWrapPointNonBillboarded(const char* text, CGxFont* face, float fontHeight, float blockWidth, uint32_t* numBytes, float* width, const char** nextText, float shadowWidth, uint32_t flags, bool* wordBreak, float* height, float scale) {
    if (0.0f == fontHeight || (flags & 4)) {
        fontHeight = GxuFontGetOneToOneHeight(face);
    }

    float lineWidth = 0.0f;

    if (shadowWidth > 0.0f) {
        float shadowPixels = shadowWidth * g_widthPixels;
        lineWidth = ceil(static_cast<double>(CMath::fint_n(shadowPixels)));
    }

    if (face->m_fontFlags & 8) {
        lineWidth = lineWidth + 4.0;
    } else if (face->m_fontFlags & 1) {
        lineWidth = lineWidth + 2.0;
    }

    float pixels = g_heightPixels * fontHeight;
    float fontPixelHeight = CMath::fint_n(pixels);
    float ratio = face->GetPixelSize() / fontPixelHeight;
    float maxWidth = ratio * blockWidth;
    maxWidth = maxWidth * g_widthPixels;
    maxWidth = ceil(maxWidth);

    uint32_t breakBytes = 0;
    uint32_t lastCode = 0;
    float characterWidth = 0.0f;
    float breakWidth = 0.0f;
    const char* start = text;
    int32_t advance = 0;
    uint32_t wide;
    QUOTEDCODE code = CODE_NEWLINE;
    int32_t noWrapDepth = 0;

    if (height) {
        *height = 0.0f;
    }

    char* tokenStart = NULL;

    while (*text) {
        code = GxuDetermineQuotedCode(text, advance, NULL, flags, wide, 1);

        if (code == CODE_NEWLINE) {
            text += advance;
            breakBytes = text - start;
            breakWidth = characterWidth + lineWidth;
            break;
        }

        if (code == CODE_TEXTURESTART) {
            GXUEMBEDDEDTEXTUREINFO info;

            if (!ParseEmbeddedTexture(text, &info, fontPixelHeight, scale, fontHeight)) {
                text += advance;
                continue;
            }

            float newWidth = info.m_width + lineWidth;

            if (maxWidth >= newWidth) {
                lineWidth = newWidth;

                if (height && *height < info.m_height) {
                    *height = info.m_height;
                }

                text += advance;
                continue;
            }

            lastCode = wide;
            advance = 0;
        } else if (code == CODE_COLORON || code == CODE_COLORRESTORE || code == CODE_HYPERLINKSTART || code == CODE_HYPERLINKSTOP || code == CODE_TEXTURESTOP) {
            text += advance;
            continue;
        } else if (code == CODE_TOKENSTART) {
            text += advance;
            noWrapDepth++;
            tokenStart = const_cast<char*>(text);
            continue;
        } else if (code == CODE_TOKENSTOP) {
            if (tokenStart) {
                if (tokenStart != text) {
                    memset(tokenStart, '0', text - tokenStart);
                }

                tokenStart = NULL;
            }

            text += advance;

            if (noWrapDepth > 0) {
                noWrapDepth--;
            }

            continue;
        } else if (code == CODE_NOWRAPSTART) {
            noWrapDepth++;
            text += advance;
            continue;
        } else if (code == CODE_NOWRAPSTOP) {
            if (noWrapDepth > 0) {
                noWrapDepth--;
            }

            text += advance;
            continue;
        }

        if (face->NewCodeDesc(wide)) {
            float step = lastCode ? (flags & 0x10 ? face->ComputeStepFixedWidth(lastCode, wide) : face->ComputeStep(lastCode, wide)) : 0.0f;

            if (noWrapDepth <= 0 && CanWrapBetween(lastCode, wide)) {
                if (*wordBreak || breakBytes) {
                    breakBytes = text - start;
                }

                *wordBreak = true;
                breakWidth = characterWidth + lineWidth;
            }

            float nextCharacterWidth = GetCharacterWidth(text + advance, flags, wide, face, fontHeight);
            float newWidth = step + lineWidth;

            if (maxWidth < nextCharacterWidth + newWidth) {
                break;
            }

            lastCode = wide;
            lineWidth = newWidth;
            characterWidth = nextCharacterWidth;
        }

        text += advance;
    }

    InternalWipeUntilNameTokenStop(tokenStart);

    float widthPixels = g_widthPixels;
    float total = characterWidth + lineWidth;
    float totalWidth = total / widthPixels;
    breakWidth = breakWidth / widthPixels;

    float heightPixels = g_heightPixels * fontHeight;
    float roundedHeight = CMath::fint_n(heightPixels);
    float scaleRatio = roundedHeight / face->GetPixelSize();
    totalWidth = scaleRatio * totalWidth;

    if (code == CODE_NEWLINE || !*text) {
        *width = totalWidth;
        *nextText = text;
        *numBytes = text - start;
        return;
    }

    if (breakBytes && !(flags & 2)) {
        *numBytes = breakBytes;
        *width = scaleRatio * breakWidth;
        text = start + breakBytes;
    } else {
        *wordBreak = (flags >> 6) & 1;
        *numBytes = text - start;
        *width = totalWidth;
    }

    while (*text) {
        wide = sgetu8(reinterpret_cast<const uint8_t*>(text), &advance);

        if (!iswspace(wide)) {
            break;
        }

        text += advance;
    }

    *nextText = text;
}

// OFFSET: %0x00629BD0
void CalcWrapPoint(CGxFont* face, const char* text, float fontHeight, float blockWidth, uint32_t* numBytes, float* width, const char** nextText, float shadowWidth, uint32_t flags, bool* wordBreak, float* height, float scale) {
    if (fontHeight >= 0.0f && blockWidth > 0.0f && text && *text) {
        if (flags & 0x80) {
            CalcWrapPointBillboarded(text, flags, face, fontHeight, numBytes, width, nextText, height, scale);
        } else {
            CalcWrapPointNonBillboarded(text, face, fontHeight, blockWidth, numBytes, width, nextText, shadowWidth, flags, wordBreak, height, scale);
        }
    } else {
        *numBytes = 0;
        *width = 0.0f;
        *nextText = NULL;
    }
}
