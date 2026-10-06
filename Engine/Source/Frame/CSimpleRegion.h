#ifndef ENGINE_SOURCE_FRAME_CSIMPLEREGION_H
#define ENGINE_SOURCE_FRAME_CSIMPLEREGION_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CImVector.h"

class C2Vector;
class C3Vector;
class CRenderBatch;
class CSimpleAnimGroup;
class CSimpleFrame;
class CStatus;
class linkObject;

class CGradient {
    public:
    static CDataAllocator s_allocator;

    /* 0000 */ uint32_t m_alphaCount;
    /* 0004 */ uint8_t m_alphas[4];
    /* 0008 */ uint32_t m_colorCount;
    /* 000C */ CImVector m_colors[4];

    CGradient();
    void ApplyAlpha(uint8_t alpha);
    bool IsWhite();
    void Set(ORIENTATION orientation, const CImVector& minColor, const CImVector& maxColor);
};

class CSimpleRegion : public CScriptRegion {
    public:
    /* 00A4 */ CImVector m_color;
    /* 00A8 */ TSLink<CSimpleRegion> m_regionLink;
    /* 00B0 */ TSLink<CSimpleRegion> m_layerLink;
    /* 00B8 */ uint8_t m_vertexAlpha;
    /* 00B9 */ int8_t m_subLevel : 4;
    /* 00B9 */ uint8_t m_drawLayer : 4;
    /* 00BA */ uint8_t m_hasGradient : 1;
    /* 00BA */ uint8_t m_hasColor : 1;
    /* 00BA */ uint8_t m_ignoreParentAlpha : 1;

    virtual ~CSimpleRegion();
    virtual void SetParent(CSimpleFrame* parent);
    virtual void AnimActivated(CSimpleAnimGroup* group, int32_t alpha, int32_t position);
    virtual void AnimDeactivated(CSimpleAnimGroup* group, int32_t alpha, int32_t position);
    virtual void AddAnimAlpha(CScriptRegion* region, int16_t alpha);
    virtual void OnColorChanged(bool changed);
    virtual void SignalLoaded();
    virtual void MarkDirty();
    virtual void OnScreenSizeChanged();
    virtual void Draw(CRenderBatch* batch) = 0;
    virtual void StoreAnimPosition();
    virtual int32_t CanBeAnchorFor(CLayoutFrame* frame);
    virtual void DumpInfo(int32_t depth, void (*print)(char*, void*), void* param);
    virtual void HideThis();
    virtual void ShowThis(bool loadPending);

    CSimpleRegion(CSimpleFrame* frame, uint32_t drawLayer, int8_t subLevel, bool show);
    void EnableGradient(bool enable);
    void GetBaseColor(CImVector& color);
    CGradient*& Gradient();
    uint8_t GetVertexAlpha();
    void GetVertexColor(CImVector& color);
    void Hide();
    bool IsColorWhite();
    void NotifyDelayedLoad();
    void OnRegionChanged();
    void PreLoadXML(linkObject* node, CStatus* status);
    void SetFrame(CSimpleFrame* frame, uint32_t drawLayer, int8_t subLevel, bool show);
    void SetIgnoreParentAlpha(bool ignore);
    void SetVertexAlpha(uint8_t alpha);
    void SetVertexColor(const CImVector& color);
    void SetVertexColorRGBA(float r, float g, float b, float a);
    void SetVertexGradient(ORIENTATION orientation, const CImVector& minColor, const CImVector& maxColor);
    void Show();
};

void ApplyRotationAnim(C3Vector* positions, FRAMEPOINT origin, const C2Vector& offset, float radians);
void ApplyScaleAnim(C3Vector* positions, FRAMEPOINT origin, const C2Vector& offset, const C2Vector& scale);
void ApplyTranslationAnim(C3Vector* positions, const C2Vector& translation);
C3Vector& ApplyTranslationAnim(C3Vector& position, const C2Vector& translation);

#include "Engine/Source/Frame/CSimpleRegion.inl"

#endif
