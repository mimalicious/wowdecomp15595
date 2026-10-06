#ifndef ENGINE_SOURCE_FRAME_CSIMPLERENDER_H
#define ENGINE_SOURCE_FRAME_CSIMPLERENDER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleEmbeddedTexture.h"
#include "Engine/Source/Frame/CSimpleFontable.h"
#include "Engine/Source/Frame/CSimpleRegion.h"
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Storm/H/stpl.h"

class CGxShader;
class CRect;
class CRenderBatch;
class CSimpleFontStringAttributes;
class CSimpleFrame;
class CSimpleTexture;
class CStatus;
struct FONTHASHOBJ;
class TEXTBLOCK;
class linkObject;

class CSimpleRender {
    public:
    static CGxShader* s_vertexShader;
    static int32_t s_mergeEnable;

    static int32_t CalcBatchCounts(CRenderBatch* batch, uint32_t* vertCounts, uint32_t* idxCounts);
    static void Destroy();
    static void DrawBatch(CRenderBatch* batch);
    static void Init();
};

class CSimpleFontString : public CSimpleRegion, public CSimpleFontable {
    public:
    static CDataAllocator s_allocator;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static void FreeEmbededTextureStorage();
    static CSimpleEmbeddedTexture* GetEmbeddedTextureFromStorage();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);

    /* 00D0 */ float m_rotation;
    /* 00D4 */ float m_scale;
    /* 00D8 */ float m_animBaseRotation;
    /* 00DC */ float m_animBaseScale;
    /* 00E0 */ HTEXTFONT m_fontHandle;
    /* 00E4 */ float m_fontHeight;
    /* 00E8 */ uint16_t m_textMaxSize;
    /* 00EA */ uint16_t m_textCurSize;
    /* 00EC */ char* m_text;
    /* 00F0 */ float m_spacing;
    /* 00F4 */ HTEXTBLOCK m_string;
    /* 00F8 */ float m_textWidth;
    /* 00FC */ float m_textHeight;
    /* 0100 */ CImVector m_shadowColor;
    /* 0104 */ C2Vector m_shadowOffset;
    /* 010C */ C2Vector m_justificationOffset;
    /* 0114 */ int16_t m_alphaGradientStart;
    /* 0116 */ int16_t m_alphaGradientLength;
    /* 0118 */ uint32_t m_styleFlags;
    /* 011C */ uint32_t m_settableStyleFlags;
    /* 0120 */ uint32_t m_maxLines;
    /* 0124 */ TSExplicitList<CSimpleEmbeddedTexture, offsetof(CSimpleEmbeddedTexture, m_link)> m_textures;

    virtual ~CSimpleFontString();
    virtual int32_t GetScriptMetaTable();
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void PreOnAnimUpdate();
    virtual void AddAnimTranslation(CScriptRegion* region, const C2Vector& translation);
    virtual void AddAnimRotation(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, float radians);
    virtual void AddAnimScale(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, const C2Vector& scale);
    virtual void OnColorChanged(bool changed);
    virtual void OnScreenSizeChanged();
    virtual void Draw(CRenderBatch* batch);
    virtual void StoreAnimPosition();
    virtual void LoadXML(linkObject* node, CStatus* status);
    virtual bool SetLayoutScale(float scale, bool force);
    virtual bool SetLayoutDepth(float depth, bool force);
    virtual void SetWidth(float width);
    virtual void SetHeight(float height);
    virtual float GetWidth();
    virtual float GetHeight();
    virtual void OnFrameSizeChanged(const CRect& rect);
    virtual void AnimData_Init();
    virtual void AnimData_ResetPositionData();
    virtual void FontObjectUpdated(CSimpleFontStringAttributes& attributes);

    CSimpleFontString(CSimpleFrame* frame, uint32_t drawLayer, bool show);
    void AddShadow(const CImVector& color, const C2Vector& offset);
    void ClearString();
    void FreeEmbeddedTextures();
    const char* GetDisplayText(float width, float height);
    uint32_t GetFontFlags();
    float GetFontHeight(bool recalculate);
    const char* GetFontName();
    uint32_t GetNumCharsWithinWidth(const char* text, uint32_t numBytes, float maxWidth);
    uint32_t GetNumCharsWithinWidthFromEnd(const char* text, uint32_t numBytes, float maxWidth);
    float GetStringHeight();
    float GetStringWidth();
    float GetTextWidth(const char* text, uint32_t numBytes);
    float GetTextBlockHeight();
    float GetTextBlockWidth();
    float GetWrappedWidth();
    float GetWrappedWidth(float width);
    bool IsTruncated();
    void PostLoadXML(linkObject* node, CStatus* status);
    void QueueTextures(CRenderBatch* batch);
    void RefreshEmbeddedTextures();
    void RemoveShadow();
    bool SetAlphaGradient(int32_t start, int32_t length);
    int32_t SetFont(const char* fontName, float fontHeight, uint32_t fontFlags, bool force);
    void SetIndented(int32_t indented);
    void SetJustificationOffset(float x, float y);
    void SetJustifyH(uint8_t justify);
    void SetJustifyV(uint8_t justify);
    void SetNonSpaceWrap(int32_t wrap);
    void SetNonWordWrap(int32_t noWrap);
    void SetShadowColor(const CImVector& color);
    void SetShadowOffset(const C2Vector& offset);
    void SetSpacing(float spacing);
    void SetStyleFlags(uint32_t styleFlags);
    void SetText(const char* text, int32_t process);
    void SetTextHeight(float height);
    void SetTextLength(uint32_t length);
    void UpdateString();
    uint32_t WrapText(const char* text, float width, uint32_t* lineStarts, uint32_t maxLines);
};

int32_t GetDeclinedWord(const char* word, int32_t declension, char* buffer, uint32_t bufferSize);
const char* LanguageProcess(const char* text);

#include "Engine/Source/Frame/CSimpleRender.inl"

#endif
