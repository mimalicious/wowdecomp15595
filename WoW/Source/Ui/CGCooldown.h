#ifndef WOW_SOURCE_UI_CGCOOLDOWN_H
#define WOW_SOURCE_UI_CGCOOLDOWN_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

class CRect;
class CRenderBatch;
class CStatus;
class CTexture;
class linkObject;
struct lua_State;

class CGCooldown : public CSimpleFrame {
    public:
    static CDataAllocator s_cooldownHeap;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;
    static const char* s_starTextureName;
    static const char* s_edgeTextureName;
    static const uint16_t s_starIndices[24];
    static const uint16_t s_flashIndices[6];
    static const uint16_t s_edgeIndices[6];
    static float s_flashScale[7];
    static float s_flashAlpha[7];
    static float s_flashRotation[7];

    static void operator delete(void* ptr);
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);

    /* 01F0 */ uint32_t m_startTime;
    /* 01F4 */ uint32_t m_duration;
    /* 01F8 */ uint32_t m_elapsed;
    /* 01FC */ uint32_t m_flashing;
    /* 0200 */ uint32_t m_reverse;
    /* 0204 */ CTexture* m_starTexture;
    /* 0208 */ C3Vector m_starVerts [10];
    /* 0280 */ C2Vector m_starTexCoords [10];
    /* 02D0 */ CImVector m_starColor;
    /* 02D4 */ C3Vector m_flashVerts [4];
    /* 0304 */ C2Vector m_flashTexCoords [4];
    /* 0324 */ CImVector m_flashColor;
    /* 0328 */ CTexture* m_edgeTexture;
    /* 032C */ uint32_t m_drawEdge;
    /* 0330 */ C3Vector m_edgeVerts [4];
    /* 0360 */ C2Vector m_edgeTexCoords [4];
    /* 0380 */ CImVector m_edgeColor;

    ~CGCooldown();
    int32_t GetScriptMetaTable() override;
    bool IsA(int32_t type) override;
    bool IsA(const char* typeName) override;
    const char* GetObjectTypeName() override;
    void OnLayerUpdate(float elapsedSec) override;
    void OnFrameRender(CRenderBatch* batch, uint32_t layer) override;
    void UpdateAlpha() override;
    void LoadXML(linkObject* node, CStatus* status) override;
    void OnFrameSizeChanged(const CRect& rect) override;

    CGCooldown(CSimpleFrame* parent);
    void AnimateOctants01(uint32_t octant, float t);
    void AnimateOctants23(uint32_t octant, float t);
    void AnimateOctants45(uint32_t octant, float t);
    void AnimateOctants67(uint32_t octant, float t);
    void AnimateVerts();
    void ResetVerts();
    void SetCooldown(uint32_t start, uint32_t duration);
    void UpdateColors();
};

#include "WoW/Source/Ui/CGCooldown.inl"

#endif
