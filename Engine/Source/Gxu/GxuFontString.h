#ifndef ENGINE_SOURCE_GXU_GXUFONTSTRING_H
#define ENGINE_SOURCE_GXU_GXUFONTSTRING_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gxu/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CRect.h"

class CGxVertexPCT;
struct CGxFont;
struct CGxString;
struct TEXTLINETEXTURE;

struct GXUEMBEDDEDTEXTUREINFO {
    /* 0000 */ float m_lineHeight;
    /* 0004 */ float m_left;
    /* 0008 */ float m_baseLineHeight;
    /* 000C */ float m_right;
    /* 0010 */ const char* m_path;
    /* 0014 */ uint32_t m_pathLength;
    /* 0018 */ float m_height;
    /* 001C */ float m_width;
    /* 0020 */ float m_offsetX;
    /* 0024 */ float m_offsetY;
    /* 0028 */ CRect m_texCoords;
    /* 0038 */ CImVector m_color;
    /* 003C */ uint8_t m_hasTexCoords;
    /* 003D */ uint8_t m_hasColor;

    GXUEMBEDDEDTEXTUREINFO();
};

struct EMBEDDEDPARSEINFO {
    /* 0000 */ int32_t m_state;
    /* 0004 */ GXUFONTHYPERLINKINFO m_hyperlink;
    /* 002C */ GXUEMBEDDEDTEXTUREINFO m_texture;

    EMBEDDEDPARSEINFO();
};

struct CGxString : public TSLinkedNode<CGxString> {
    static CGxString* GetNewString(int32_t linkOnTop);

    /* 0008 */ TSLink<CGxString> m_fontStringLink;
    /* 0010 */ TSLink<CGxString> m_batchedStringLink;
    /* 0018 */ float m_stringScale;
    /* 001C */ float m_rotation;
    /* 0020 */ float m_requestedFontHeight;
    /* 0024 */ float m_currentFontHeight;
    /* 0028 */ C3Vector m_position;
    /* 0034 */ CImVector m_fontColor;
    /* 0038 */ CImVector m_shadowColor;
    /* 003C */ C2Vector m_shadowOffset;
    /* 0044 */ float m_blockWidth;
    /* 0048 */ float m_blockHeight;
    /* 004C */ CGxFont* m_currentFace;
    /* 0050 */ char* m_text;
    /* 0054 */ uint32_t m_textLen;
    /* 0058 */ EGxFontVJusts m_vertJust;
    /* 005C */ EGxFontHJusts m_horzJust;
    /* 0060 */ float m_spacing;
    /* 0064 */ uint32_t m_flags;
    /* 0068 */ uint32_t m_texturePagesUsed;
    /* 006C */ int32_t m_textureEvicted;
    /* 0070 */ int32_t m_lastGradientStart;
    /* 0074 */ int32_t m_lastGradientLength;
    /* 0078 */ C3Vector m_viewTranslation;
    /* 0084 */ float m_scale;
    /* 0088 */ float m_extraLineHeight;
    /* 008C */ TSGrowableArray<GXUFONTHYPERLINKINFO> m_hyperlinkInfo;
    /* 009C */ TSGrowableArray<GXUEMBEDDEDTEXTUREINFO> m_embeddedTextureInfo;
    /* 00AC */ TSGrowableArray<GRADIENTINFO> m_gradientInfo;
    /* 00BC */ int32_t m_numLines;
    /* 00C0 */ TEXTLINETEXTURE* m_textLines[8];
    /* 00E0 */ int32_t m_idleTicks;

    CGxString();
    ~CGxString();
    void AddEmbeddedTextureParseInfo(GXUEMBEDDEDTEXTUREINFO info);
    void AddHyperlinkParseInfo(GXUFONTHYPERLINKINFO info);
    void AddShadow(const C2Vector& offset, const CImVector& color);
    int32_t CalculateVertsNeeded(int32_t texturePage);
    int32_t CheckGeometry();
    void ClearInstanceData();
    void CreateGeometry();
    uint8_t GetGradientAlpha(const GRADIENTINFO& info);
    uint32_t GetEmbeddedTextureInfo(const GXUEMBEDDEDTEXTUREINFO*& info);
    uint32_t GetHyperLinkInfo(const GXUFONTHYPERLINKINFO*& info);
    void HandleScreenSizeChange();
    int32_t Initialize(float fontHeight, const C3Vector& position, float blockWidth, float blockHeight, CGxFont* face, const char* text, EGxFontVJusts vertJust, EGxFontHJusts horzJust, float spacing, uint32_t flags, const CImVector& color, float scale, float stringScale, float rotation);
    void InitializeTextLine(const char* text, uint32_t numBytes, CImVector& color, const C3Vector& position, uint32_t* texturePagesUsed, EMBEDDEDPARSEINFO& parseInfo);
    void InitializeViewTranslation();
    void Recycle();
    void RemoveShadow();
    void SetColor(const CImVector& color);
    void SetGradientAlpha(const GRADIENTINFO& info, uint8_t alpha);
    int32_t SetGradient(int32_t start, int32_t length);
    void SetStringPosition(const C3Vector& position);
    void TexturePageEvicted(uint32_t texturePage);
    void Tick();
    void WriteGeometry(CGxVertexPCT* buf, int32_t texturePage, int32_t vertIndex, int32_t numVerts) const;
};

struct VERT {
    /* 0000 */ C3Vector m_vc;
    /* 000C */ C2Vector m_tc;
};

#endif
