#include <cstddef>
#include <cstdint>
#include <new>
#include "Engine/Source/Services/TextBlock.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

// STATIC: %0x00CD2F10
static TSHashTable<FONTHASHOBJ, HASHKEY_STR> s_fontHash;

// INLINED
TEXTBLOCK::TEXTBLOCK() {
    this->m_string = NULL;
}

// INLINED
FONTHASHOBJ::FONTHASHOBJ() {
    this->m_font = NULL;
}

// OFFSET: %0x005590E0
static uint32_t ConvertStringFlags(uint32_t flags) {
    uint32_t result = (flags & 0x100) != 0;

    if (flags & 0x200) {
        result |= 0x4;
    }

    if (flags & 0x400) {
        result |= 0x8;
    }

    if (flags & 0x800) {
        result |= 0x10;
    }

    if (flags & 0x40) {
        result |= 0x2;
    }

    if (flags & 0x1000) {
        result |= 0x40;
    }

    if (flags & 0x2000) {
        result |= 0x100;
    }

    if (flags & 0x4000) {
        result |= 0x200;
    }

    if (flags & 0x8000) {
        result |= 0x400;
    }

    if (flags & 0x80) {
        result |= 0x800;
    }

    if (flags & 0x10000) {
        result |= 0x1000;
    }

    if (flags & 0x20000) {
        result |= 0x2000;
    }

    return result;
}

// OFFSET: %0x00559170
int32_t TextBlockSetGradient(HTEXTBLOCK string, int32_t start, int32_t length) {
    return GxuFontStringSetGradient(reinterpret_cast<TEXTBLOCK*>(string)->m_string, start, length);
}

// OFFSET: %0x00559190
TEXTBLOCK::~TEXTBLOCK() {
    GxuFontDestroyString(this->m_string);
}

// OFFSET: %0x005591D0
const char* TextBlockGetFontName(HTEXTFONT font) {
    if (!font) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    return GxuFontGetFontName(reinterpret_cast<FONTHASHOBJ*>(font)->m_font);
}

// OFFSET: %0x00559200
uint32_t TextBlockGetFontFlags(HTEXTFONT font) {
    if (!font) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    uint32_t fontFlags = GxuFontGetFontFlags(reinterpret_cast<FONTHASHOBJ*>(font)->m_font);
    uint32_t flags = (fontFlags & 0x1) != 0;

    if (fontFlags & 0x8) {
        flags |= 0x4;
    }

    if (fontFlags & 0x2) {
        flags |= 0x2;
    }

    return flags;
}

// OFFSET: %0x00559240
CGxFont* TextBlockGetFontPtr(HTEXTFONT font) {
    if (!font) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    return reinterpret_cast<FONTHASHOBJ*>(font)->m_font;
}

// OFFSET: %0x00559260
CGxString* TextBlockGetStringPtr(HTEXTBLOCK string) {
    if (!string) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    return reinterpret_cast<TEXTBLOCK*>(string)->m_string;
}

// OFFSET: %0x00559280
void TextBlockAddShadow(HTEXTBLOCK string, CImVector color, const C2Vector& offset) {
    if (!string) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    C2Vector ndcOffset = offset;
    DDCToNDC(offset.x, offset.y, &ndcOffset.x, &ndcOffset.y);
    GxuFontAddShadow(reinterpret_cast<TEXTBLOCK*>(string)->m_string, color, ndcOffset);
}

// OFFSET: %0x005592E0
void TextBlockRemoveShadow(HTEXTBLOCK string) {
    if (!string) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    GxuFontRemoveShadow(reinterpret_cast<TEXTBLOCK*>(string)->m_string);
}

// OFFSET: %0x00559310
HTEXTBLOCK TextBlockCreate(HTEXTFONT font, const char* text, const CImVector& color, const C3Vector& position, float fontHeight, float blockWidth, float blockHeight, uint32_t flags, float charSpacing, float lineSpacing, float scale, float stringScale, float rotation) {
    if (!font || !text) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    void* mem = SMemAlloc(sizeof(TEXTBLOCK), "HTEXTBLOCK", SLOG_OBJECT, 0);
    TEXTBLOCK* textBlock = mem ? new (mem) TEXTBLOCK : NULL;

    C3Vector ndcPosition(0.0f, 0.0f, position.z);
    DDCToNDC(position.x, position.y, &ndcPosition.x, &ndcPosition.y);

    EGxFontHJusts horzJust = GxHJ_Center;

    if (flags & 0x4) {
        horzJust = GxHJ_Right;
    } else if (!(flags & 0x2) && (flags & 0x1)) {
        horzJust = GxHJ_Left;
    }

    EGxFontVJusts vertJust = GxVJ_Middle;

    if (flags & 0x8) {
        vertJust = GxVJ_Top;
    } else if (flags & 0x20) {
        vertJust = GxVJ_Bottom;
    }

    GxuFontCreateString(reinterpret_cast<FONTHASHOBJ*>(font)->m_font, text, DDCToNDCHeight(fontHeight), ndcPosition, DDCToNDCWidth(blockWidth), DDCToNDCHeight(blockHeight), DDCToNDCHeight(lineSpacing), textBlock->m_string, vertJust, horzJust, ConvertStringFlags(flags), color, DDCToNDCWidth(charSpacing), scale, stringScale, rotation);

    return reinterpret_cast<HTEXTBLOCK>(HandleCreate(textBlock, "HTEXTBLOCK"));
}

// OFFSET: %0x00559470
void TextBlockSetStringPos(HTEXTBLOCK string, const C3Vector& position) {
    if (!string) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    C3Vector ndcPosition(0.0f, 0.0f, position.z);
    DDCToNDC(position.x, position.y, &ndcPosition.x, &ndcPosition.y);
    GxuFontSetStringPosition(reinterpret_cast<TEXTBLOCK*>(string)->m_string, ndcPosition);
}

// OFFSET: %0x005594E0
C3Vector TextBlockGetStringPos(HTEXTBLOCK string) {
    C3Vector ndcPosition = GxuFontGetStringPosition(reinterpret_cast<TEXTBLOCK*>(string)->m_string);
    C3Vector position(0.0f, 0.0f, ndcPosition.z);
    NDCToDDC(ndcPosition.x, ndcPosition.y, &position.x, &position.y);

    return position;
}

// OFFSET: %0x00559540
void TextBlockUpdateColor(HTEXTBLOCK string, const CImVector& color) {
    if (!string) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    GxuFontSetStringColor(reinterpret_cast<TEXTBLOCK*>(string)->m_string, color);
}

// OFFSET: %0x00559570
void TextBlockGetTextExtent(HTEXTFONT font, const char* text, uint32_t numBytes, float fontHeight, float* extent, float shadowWidth, float scale, float unused, uint32_t flags) {
    if (!font || !text || !extent) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    *extent = 0.0f;

    CGxFont* face = reinterpret_cast<FONTHASHOBJ*>(font)->m_font;

    if (!face) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    GxuFontGetTextExtent(face, text, numBytes, DDCToNDCHeight(fontHeight), extent, DDCToNDCWidth(shadowWidth), scale, DDCToNDCWidth(unused), ConvertStringFlags(flags));
    NDCToDDC(*extent, 0.0f, extent, NULL);
}

// OFFSET: %0x00559620
float TextBlockGetWrappedTextHeight(HTEXTFONT font, const char* text, float fontHeight, float blockWidth, const C2Vector& shadowOffset, float scale, float spacing, uint32_t flags) {
    if (!font || !text || !reinterpret_cast<FONTHASHOBJ*>(font)->m_font) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0.0f;
    }

    C2Vector ndcShadow = shadowOffset;
    DDCToNDC(ndcShadow.x, ndcShadow.y, &ndcShadow.x, &ndcShadow.y);

    float height = GxuFontGetWrappedTextHeight(reinterpret_cast<FONTHASHOBJ*>(font)->m_font, text, DDCToNDCHeight(fontHeight), DDCToNDCWidth(blockWidth), ndcShadow, scale, DDCToNDCHeight(spacing), ConvertStringFlags(flags));

    return NDCToDDCHeight(height);
}

// OFFSET: %0x005596E0
uint32_t TextBlockGetMaxCharsWithinWidth(HTEXTFONT font, const char* text, float fontHeight, float maxWidth, uint32_t numBytes, float* extent, float shadowWidth, float scale, float unused, uint32_t flags) {
    if (!font || !text || !reinterpret_cast<FONTHASHOBJ*>(font)->m_font) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    uint32_t numChars = GxuFontGetMaxCharsWithinWidth(reinterpret_cast<FONTHASHOBJ*>(font)->m_font, text, DDCToNDCHeight(fontHeight), DDCToNDCWidth(maxWidth), numBytes, extent, DDCToNDCWidth(shadowWidth), scale, DDCToNDCWidth(unused), ConvertStringFlags(flags));
    NDCToDDC(*extent, 0.0f, extent, NULL);

    return numChars;
}

// OFFSET: %0x00559790
uint32_t TextBlockGetMaxCharsWithinWidthAndHeight(HTEXTFONT font, const char* text, float fontHeight, float blockWidth, float blockHeight, uint32_t numBytes, float shadowWidth, float scale, float spacing, uint32_t flags) {
    if (!font || !text || !reinterpret_cast<FONTHASHOBJ*>(font)->m_font) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return GxuFontGetMaxCharsWithinWidthAndHeight(reinterpret_cast<FONTHASHOBJ*>(font)->m_font, text, DDCToNDCHeight(fontHeight), DDCToNDCWidth(blockWidth), DDCToNDCHeight(blockHeight), numBytes, DDCToNDCWidth(shadowWidth), scale, DDCToNDCHeight(spacing), ConvertStringFlags(flags));
}

// OFFSET: %0x00559830
uint32_t TextBlockGetMaxCharsWithinWidthFromEnd(HTEXTFONT font, const char* text, float fontHeight, float maxWidth, uint32_t numBytes, float* extent, float shadowWidth, float scale, float unused, uint32_t flags) {
    if (!font || !text || !reinterpret_cast<FONTHASHOBJ*>(font)->m_font) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    uint32_t numChars = GxuFontGetMaxCharsWithinWidthFromEnd(reinterpret_cast<FONTHASHOBJ*>(font)->m_font, text, DDCToNDCHeight(fontHeight), DDCToNDCWidth(maxWidth), numBytes, extent, DDCToNDCWidth(shadowWidth), scale, DDCToNDCWidth(unused), ConvertStringFlags(flags));
    NDCToDDC(*extent, 0.0f, extent, NULL);

    return numChars;
}

// OFFSET: %0x005598E0
uint32_t TextBlockWrapText(HTEXTFONT font, const char* text, float fontHeight, float blockWidth, uint32_t* lineStarts, uint32_t maxLines, float shadowWidth, float scale, float unused, uint32_t flags) {
    if (!font || !text || !reinterpret_cast<FONTHASHOBJ*>(font)->m_font) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return GxuFontWrapText(reinterpret_cast<FONTHASHOBJ*>(font)->m_font, text, SStrLen(text), DDCToNDCHeight(fontHeight), DDCToNDCWidth(blockWidth), lineStarts, maxLines, DDCToNDCWidth(shadowWidth), scale, DDCToNDCWidth(unused), ConvertStringFlags(flags));
}

// OFFSET: %0x00559DE0
FONTHASHOBJ::~FONTHASHOBJ() {
    if (this->m_font) {
        GxuFontDestroyFont(this->m_font);
    }
}

// OFFSET: %0x0055A2A0
HTEXTFONT TextBlockGenerateFont(const char* fontName, uint32_t fontFlags, float fontHeight) {
    if (!fontName) {
        DebugValidate("fontName");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (!*fontName) {
        DebugValidate("*fontName");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    float height = DDCToNDCHeight(fontHeight);

    if (height >= 1.0f) {
        height = 1.0f;
    }

    char key[276];
    SStrPrintf(key, sizeof(key), "%s-%d-%f", fontName, fontFlags, height);

    FONTHASHOBJ* hashObj = s_fontHash.Ptr(key);

    if (hashObj) {
        return reinterpret_cast<HTEXTFONT>(HandleCreate(hashObj, "HTEXTFONT"));
    }

    hashObj = s_fontHash.New(SStrHashHT(key), key, 0, 0);

    uint32_t gxFlags = (fontFlags & 0x1) != 0;

    if (fontFlags & 0x4) {
        gxFlags |= 0x8;
    }

    if (fontFlags & 0x2) {
        gxFlags |= 0x2;
    }

    if (GxuFontCreateFont(fontName, height, hashObj->m_font, gxFlags)) {
        return reinterpret_cast<HTEXTFONT>(HandleCreate(hashObj, "HTEXTFONT"));
    }

    s_fontHash.Delete(hashObj);

    return NULL;
}
