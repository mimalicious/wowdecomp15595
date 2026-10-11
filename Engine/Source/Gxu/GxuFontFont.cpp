#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include "Engine/Source/Gxu/GxuFontFont.h"
#include "Engine/Source/Base/ConvertUTF.h"
#include "Engine/Source/Gxu/GxuFontFace.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Gxu/GxuFontUtil.h"
#include "Engine/Source/Gxu/IGxuFontGlyph.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"

// INLINED
KERNINGHASHKEY& KERNINGHASHKEY::operator=(const KERNINGHASHKEY& key) {
    if (this != &key) {
        this->m_code = key.m_code;
    }

    return *this;
}

// INLINED
bool KERNINGHASHKEY::operator==(const KERNINGHASHKEY& key) {
    return this == &key || this->m_code == key.m_code;
}

// INLINED
const char* CGxFont::GetName() {
    return FontFaceGetFontName(this->m_fontFaceHandle);
}

// PROVEN: 5507014c IDENTICAL/IDENTICAL
// OFFSET: 0x0061B910/0x1402A95C0
uint32_t CGxFont::GetPixelSize() {
    return this->m_pixelSize;
}

// OFFSET: 0x0061B920
float CGxFont::FindNearestFixedSize(FT_Face face, float height) {
    if (face->num_fixed_sizes == 0) {
        return height;
    }

    int32_t smallest = 0x7FFFFFFF;

    for (int32_t i = 0; i < face->num_fixed_sizes; i++) {
        if (smallest >= face->available_sizes[i].height) {
            smallest = face->available_sizes[i].height;
        }
    }

    float fixedHeight = PixelToScreenHeight(smallest);

    return fixedHeight < height ? height : fixedHeight;
}

// OFFSET: 0x0061BA10/0x1402A9630
int32_t CGxFont::UpdateDimensions() {
    FT_Face face = FontFaceGetFace(this->m_fontFaceHandle);

    float height = this->FindNearestFixedSize(face, this->m_fontHeight);
    double minHeight = 2.0 / g_heightPixels;
    height = height > minHeight ? height : minHeight;

    uint32_t pixelSize = static_cast<uint32_t>(ScreenToPixelHeight(0, height));

    if (pixelSize > 32) {
        pixelSize = 32;
    } else if (pixelSize == 0) {
        DebugLog("Error, font %s being created with height %g at screen res of %dx%d which is %d pixels!", this->m_fontName, height, g_widthPixels, g_heightPixels, pixelSize);
    }

    this->m_pixelSize = pixelSize;

    int32_t units = face->ascender + abs(face->descender);

    if (units == 0) {
        return 0;
    }

    this->m_cellHeight = pixelSize;

    float baseline = static_cast<float>(pixelSize * face->ascender) / units;
    this->m_baseline = static_cast<uint32_t>(baseline + 0.5);

    if (this->m_fontFlags & 8) {
        this->m_cellHeight = pixelSize + 4;
    } else if (this->m_fontFlags & 1) {
        this->m_cellHeight = pixelSize + 2;
    }

    int32_t result = FT_Set_Pixel_Sizes(face, pixelSize, 0) == 0;

    this->m_pixelsPerUnit = static_cast<float>(face->size->metrics.x_ppem) / static_cast<float>(face->units_per_EM);

    return result;
}

// OFFSET: 0x0061BBA0
bool CGxFont::GetGlyphData(GLYPHBITMAPDATA* data, uint32_t code) {
    FT_Face face = FontFaceGetFace(this->m_fontFaceHandle);
    FT_Set_Pixel_Sizes(face, this->m_pixelSize, 0);

    uint32_t padding = 0;

    if (this->m_fontFlags & 8) {
        padding = 4;
    } else if (this->m_fontFlags & 1) {
        padding = 2;
    }

    return IGxuFontGlyphRenderGlyph(face, this->m_pixelSize, code, this->m_baseline, data, this->m_fontFlags & 2, padding);
}

// OFFSET: 0x0061B990/0x1402A95D0
float CGxFont::GetGlyphBearing(const CHARCODEDESC* desc, bool scaled, float height) {
    if (!scaled) {
        float bearing = desc->m_bitmapData.m_glyphBearing;
        return ceil(bearing);
    }

    float pixelHeight = ScreenToPixelHeight(1, height);
    float scale = pixelHeight / this->m_pixelSize;

    return scale * desc->m_bitmapData.m_glyphBearing;
}

// PROVEN: 94414b78 IDENTICAL/NONE
// OFFSET: 0x0061BCA0
void CGxFont::RegisterEvictNotice(uint32_t texturePage) {
    for (CGxString* string = this->m_strings.Head(), * nextString; !this->m_strings.IsEnd(string); string = nextString) {
        nextString = this->m_strings.RawNext(string);
        string->TexturePageEvicted(texturePage);
    }
}

// OFFSET: 0x0061BCF0/0x1402A9890
int32_t CGxFont::CheckStringGlyphs(const char* string) {
    while (string && *string) {
        int32_t advance;
        uint32_t code = sgetu8(reinterpret_cast<const uint8_t*>(string), &advance);
        string += advance;

        if (code != '\r' && code != '\n' && !this->m_activeCharacters.Ptr(code, HASHKEY_NONE())) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x0061BD50/0x1402A9930
float CGxFont::GetGlyphAdvance(uint32_t code) {
    CHARCODEDESC* desc = this->m_activeCharacters.Ptr(code, HASHKEY_NONE());

    if (!desc) {
        return 0.0f;
    }

    float advance = desc->m_bitmapData.m_glyphAdvance;

    if (this->m_fontFlags & 8) {
        advance += 1.0;
    }

    return advance;
}

// OFFSET: 0x0061CE50
void CGxFont::ClearGlyphs() {
    for (uint32_t i = 0; i < 8; i++) {
        this->m_textureCache[i].Destroy();
    }

    this->m_activeCharacters.Clear();
    this->m_activeCharacterCache.Clear();
    this->m_kernInfo.Clear();
}

// PROVEN: c5b09f48 IDENTICAL/DIFFERENT
// OFFSET: 0x0061D050/0x1402AB420
void CGxFont::HandleScreenSizeChange() {
    this->ClearGlyphs();
    this->UpdateDimensions();

    for (CGxString* string = this->m_strings.Head(), * nextString; !this->m_strings.IsEnd(string); string = nextString) {
        nextString = this->m_strings.RawNext(string);
        string->HandleScreenSizeChange();
    }
}

// OFFSET: 0x0061D090
CGxFont::CGxFont() : m_cellHeight(0), m_pixelsPerUnit(0.0f) {
}

// OFFSET: 0x0061D210
CGxFont::~CGxFont() {
    if (this->m_fontFaceHandle) {
        FontFaceCloseHandle(this->m_fontFaceHandle);
    }

    this->m_fontFaceHandle = NULL;
    this->ClearGlyphs();
}

// PROVEN: 476ad0d8 IDENTICAL/UNPROVEN
// OFFSET: 0x0061D3A0/0x1402AB900
int32_t CGxFont::Initialize(const char* name, uint32_t fontFlags, float fontHeight) {
    SStrPrintf(this->m_fontName, sizeof(this->m_fontName), "%s", name);
    this->m_fontHeight = fontHeight;

    if (this->m_fontFaceHandle) {
        FontFaceCloseHandle(this->m_fontFaceHandle);
    }

    this->m_fontFaceHandle = NULL;
    this->ClearGlyphs();
    this->m_fontFlags = fontFlags;

    if (this->m_fontFaceHandle) {
        FontFaceCloseHandle(this->m_fontFaceHandle);
    }

    this->m_fontFaceHandle = FontFaceGetHandle(name, g_FTLibrary);

    if (!this->m_fontFaceHandle) {
        return 0;
    }

    return this->UpdateDimensions();
}

// OFFSET: 0x0061D430/0x1402AB9C0
float CGxFont::ComputeStep(uint32_t code, uint32_t nextCode) {
    KERNINGHASHKEY key;
    key.m_code = (code << 16) | (nextCode & 0xFFFF);

    KERNNODE* node = this->m_kernInfo.Ptr(code, key);

    if (node && (node->m_flags & 2)) {
        return node->m_proportionalSpacing;
    }

    FT_Face face = FontFaceGetFace(this->m_fontFaceHandle);
    FT_UInt leftIndex = FT_Get_Char_Index(face, code);
    FT_UInt rightIndex = FT_Get_Char_Index(face, nextCode);

    FT_Vector kerning;
    kerning.x = 0;

    if (FT_HAS_KERNING(face)) {
        FT_Get_Kerning(face, leftIndex, rightIndex, ft_kerning_unscaled, &kerning);

        if (kerning.x >= 0) {
            kerning.x = 0;
        }
    }

    float step = this->GetGlyphAdvance(code);
    step = this->m_pixelsPerUnit * kerning.x + step;

    if (!node) {
        node = this->m_kernInfo.New(code, key, 0, 0);
    }

    node->m_flags |= 2;
    node->m_proportionalSpacing = ceil(step);

    return node->m_proportionalSpacing;
}

// OFFSET: 0x0061D570/0x1402ABBC0
float CGxFont::ComputeStepFixedWidth(uint32_t code, uint32_t nextCode) {
    KERNINGHASHKEY key;
    key.m_code = (code << 16) | (nextCode & 0xFFFF);

    KERNNODE* node = this->m_kernInfo.Ptr(code, key);

    if (node && (node->m_flags & 1)) {
        return node->m_fixedWidthSpacing;
    }

    if (!this->m_activeCharacters.Ptr(code, HASHKEY_NONE())) {
        if (!node) {
            node = this->m_kernInfo.New(code, key, 0, 0);
        }

        node->m_flags |= 1;
        node->m_fixedWidthSpacing = 0.0f;

        return 0.0f;
    }

    uint32_t advance = static_cast<uint32_t>(this->GetGlyphAdvance(code));

    if (advance >= this->m_cellHeight) {
        return static_cast<float>(static_cast<double>(this->m_cellHeight) * 0.75);
    }

    uint32_t padding = this->m_cellHeight - advance;

    if (padding & 1) {
        padding--;
    }

    uint32_t step = this->m_cellHeight - padding / 2;

    uint32_t nextAdvance = static_cast<uint32_t>(this->GetGlyphAdvance(nextCode));

    if (nextAdvance >= this->m_cellHeight) {
        return static_cast<float>(static_cast<double>(step) * 0.75);
    }

    uint32_t nextPadding = this->m_cellHeight - nextAdvance;

    if (nextPadding & 1) {
        nextPadding--;
    }

    return static_cast<float>(static_cast<double>(step + nextPadding / 2) * 0.75);
}

// OFFSET: 0x0061D750/0x1402ABDA0
CHARCODEDESC* CGxFont::NewCodeDesc(uint32_t code) {
    CHARCODEDESC* desc = this->m_activeCharacters.Ptr(code, HASHKEY_NONE());

    if (desc) {
        this->m_activeCharacterCache.LinkToHead(desc);
        return desc;
    }

    GLYPHBITMAPDATA data;

    if (!this->GetGlyphData(&data, code)) {
        return NULL;
    }

    TEXTURECACHE* cache = NULL;
    uint32_t textureNumber;

    for (textureNumber = 0; textureNumber < 8; textureNumber++) {
        TEXTURECACHE* current = &this->m_textureCache[textureNumber];

        if (!current->m_texture || !TextureGetGxTex(current->m_texture)) {
            cache = current;
            cache->CreateTexture(this->m_fontFlags & 4);
            cache->Initialize(this, this->m_cellHeight);
            desc = cache->AllocateNewGlyph(&data);
            break;
        }

        desc = current->AllocateNewGlyph(&data);

        if (desc) {
            cache = current;
            break;
        }
    }

    if (desc) {
        desc->m_textureNumber = textureNumber;
    } else {
        CHARCODEDESC* oldest = this->m_activeCharacterCache.Tail();

        if (oldest) {
            textureNumber = oldest->m_textureNumber;
            uint32_t rowNumber = oldest->m_rowNumber;
            cache = &this->m_textureCache[textureNumber];
            TEXTURECACHEROW* row = &cache->m_textureRows.m_data[rowNumber];

            row->EvictGlyph(oldest);
            this->RegisterEvictNotice(textureNumber);

            desc = row->CreateNewDesc(&data, rowNumber, this->m_cellHeight);

            if (desc) {
                desc->m_rowNumber = rowNumber;
                desc->m_textureNumber = textureNumber;
            }
        }
    }

    if (desc) {
        this->m_activeCharacters.Insert(desc, code, HASHKEY_NONE());
        this->m_activeCharacterCache.LinkToHead(desc);

        if (cache) {
            cache->m_anyDirtyGlyphs = 1;
        }
    }

    return desc;
}
