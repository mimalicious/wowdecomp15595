#ifndef WOW_SOURCE_WORLDCLIENT_MAPOBJRENDER_H
#define WOW_SOURCE_WORLDCLIENT_MAPOBJRENDER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CMapObj;
class CMapObjDefGroup;
class CMapObjGroup;
class CShaderEffect;
class CTexture;
class CWorldView;
namespace FFX { class Effect; }
struct SMOBatch;

class HASHKEY_MATERIAL {
    public:
    /* 0000 */ CTexture* m_textures[3];
    /* 000C */ uint32_t m_materialFlags;
    /* 0010 */ uint32_t m_ambColor;
    /* 0014 */ uint32_t m_color;
    /* 0018 */ uint32_t m_pass : 3;
    /* 0018 */ uint32_t m_shader : 4;
    /* 0018 */ uint32_t m_unified : 1;
    /* 0018 */ uint32_t m_blendMode : 4;
    /* 0018 */ uint32_t m_cull : 2;
    /* 0018 */ uint32_t m_lighting : 2;
    /* 0018 */ uint32_t m_interior : 1;
    /* 0018 */ uint32_t m_fog : 3;
    /* 0018 */ uint32_t m_secondVB : 1;
    /* 0018 */ uint32_t m_shadow : 2;
    /* 0018 */ uint32_t m_pad : 9;

    void SetState(uint32_t state);
    HASHKEY_MATERIAL& operator=(const HASHKEY_MATERIAL& key);
    bool operator==(const HASHKEY_MATERIAL& key) const;
};

struct RElement {
    /* 0000 */ CMapObjDefGroup* m_group;
    /* 0004 */ SMOBatch* m_batch;
};

class CSceneMaterial : public TSHashObject<CSceneMaterial, HASHKEY_MATERIAL> {
    public:
    /* 0030 */ uint32_t m_permuteHash;
    /* 0034 */ uint32_t m_refCount;
    /* 0038 */ TSGrowableArray<RElement> m_elements[3];
    /* 0068 */ CShaderEffect* m_effect;
    /* 006C */ uint32_t m_vertexPermute;
    /* 0070 */ uint32_t m_pixelPermute;

    void UpdateSceneMaterial(int32_t shadow);
};

class CMapObjRender {
    public:
    static CShaderEffect* s_effects[15];
    static CShaderEffect* s_unifiedEffects[15];
    static FFX::Effect* s_submarineEffect;
    static FFX::Effect* s_waterWindowEffect;
    static TSHashTable<CSceneMaterial, HASHKEY_MATERIAL> s_materialHash;

    static int32_t ApplyMaterial(CWorldView* view, CSceneMaterial* material);
    static void Destroy();
    static void Initialize();
    static CSceneMaterial* RegisterMaterial(const HASHKEY_MATERIAL& key);
    static void RenderLiquidBatch(FFX::Effect* effect, SMOBatch* batch, CMapObjDefGroup* defGroup, CMapObjGroup* group, uint32_t batchIndex);
    static void RenderMaterialElements(CWorldView* view, CSceneMaterial* material);
    static void RenderMaterialList(CWorldView* view, TSGrowableArray<CSceneMaterial*>& list);
    static void SceneSetup(CWorldView* view);
    static void UnregisterMaterial(CSceneMaterial* material);
    static void WaterResetCallback();

    /* 0000 */ TSGrowableArray<CSceneMaterial*> m_materials[5];

    void BuildFrameMaterials(CWorldView* view);
    void RenderPass(CWorldView* view, uint32_t pass);
};

#endif
