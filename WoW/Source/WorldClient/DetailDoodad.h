#ifndef WOW_SOURCE_WORLDCLIENT_DETAILDOODAD_H
#define WOW_SOURCE_WORLDCLIENT_DETAILDOODAD_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

class C33Matrix;
class C4Plane;
struct CGxBuf;
class CGxShader;
class CGxTex;
class CM2Model;
class CMapChunk;
class CTexture;
class CWorldView;
class DDMaterial;
class GroundEffectDoodadRec;

class CDetailDoodadData {
    public:
    static void ModelLoadedCallback(CM2Model* model, void* arg);

    /* 0000 */ const GroundEffectDoodadRec* m_record;
    /* 0004 */ CM2Model* m_model;
    /* 0008 */ CTexture* m_texture;
    /* 000C */ float m_windScale;

    CDetailDoodadData();
    void CheckGeometry();
    int32_t Load();
};

class CDetailDoodadInstAdd {
    public:
    /* 0000 */ uint16_t unk_0000;
    /* 0002 */ uint16_t m_triIndex;
    /* 0004 */ uint32_t m_doodadId;
    /* 0008 */ C3Vector m_position;
    /* 0014 */ float m_rotation;
    /* 0018 */ float m_scale;
    /* 001C */ C3Vector m_normal;
    /* 0028 */ CImVector m_color;
    /* 002C */ CImVector m_light;
    /* 0030 */ float m_windPhase;
};

class CDetailDoodadInstBatch {
    public:
    /* 0000 */ uint32_t m_handle;
    /* 0004 */ uint32_t m_id;
    /* 0008 */ CTexture* m_texture;
    /* 000C */ uint32_t m_count;
    /* 0010 */ uint32_t m_vertexCount;
    /* 0014 */ uint32_t m_indexCount;
    /* 0018 */ CGxBuf* m_vertexBuf;
    /* 001C */ CGxBuf* m_indexBuf;
    /* 0020 */ CGxBuf* m_instanceBuf;
    /* 0024 */ TSGrowableArray<CDetailDoodadInstAdd> m_adds;
    /* 0034 */ CMapChunk* m_chunk;
    /* 0038 */ DDMaterial* m_material;
    /* 003C */ TSLink<CDetailDoodadInstBatch> m_materialLink;
    /* 0044 */ TSLink<CDetailDoodadInstBatch> m_instLink;

    CDetailDoodadInstBatch();
    ~CDetailDoodadInstBatch();
    void FillIndices();
    void FillInstanceBuf();
    void FillVertices();
    void Free();
};

class DDMaterial : public TSHashObject<DDMaterial, HASHKEY_NONE> {
    public:
    /* 0018 */ uint32_t m_id;
    /* 001C */ CTexture* m_texture;
    /* 0020 */ CGxBuf* m_vertexBuf;
    /* 0024 */ CGxBuf* m_indexBuf;
    /* 0028 */ uint32_t m_nVertices;
    /* 002C */ uint32_t m_nIndices;
    /* 0030 */ float m_fadeDist;
    /* 0034 */ float m_fadeScale;
    /* 0038 */ TSExplicitList<CDetailDoodadInstBatch, offsetof(CDetailDoodadInstBatch, m_materialLink)> m_batches;
};

class DDMaterialSort {
    public:
    bool operator()(const DDMaterial* a, const DDMaterial* b) const;
};

class CDetailDoodadInst {
    public:
    /* 0000 */ uint32_t m_handle;
    /* 0004 */ TSExplicitList<CDetailDoodadInstBatch, offsetof(CDetailDoodadInstBatch, m_instLink)> m_batches;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ CMapChunk* m_chunk;
    /* 0018 */ TSLink<CDetailDoodadInst> m_link;

    CDetailDoodadInst();
    ~CDetailDoodadInst();
    void AddDoodad(uint32_t id, const C3Vector& position, float rotation, float scale, const C4Plane& plane, uint16_t triIndex, const CImVector& color, const CImVector& light, float windPhase);
    void FlagVisible(float dist);
    void Free();
};

class CDetailDoodad {
    public:
    struct SShaderConstants;

    static CGxTex* s_alphaRampTexture;
    static TSObjectAllocPool<CDetailDoodadInstBatch>* s_batchHeap;
    static int32_t s_bPoolsDirty;
    static int32_t s_bUseInstancing;
    static int32_t s_bUseShaders;
    static uint32_t s_currentFrame;
    static TSGrowableArray<CDetailDoodadData*> s_doodadList;
    static TSGrowableArray<DDMaterial*> s_frameMaterials;
    static uint32_t s_idxBufItems;
    static TSObjectAllocPool<CDetailDoodadInst>* s_instHeap;
    static TSHashTable<DDMaterial, HASHKEY_NONE> s_materials;
    static CGxShader* s_pixelShader;
    static SShaderConstants s_shaderConstants;
    static CGxShader* s_vertexShader;
    static uint32_t s_vtxBufItems;
    static uint32_t s_vtxSize;

    static void AllocGxBufBatch(uint32_t vertexCount, CGxBuf*& vertexBuf, uint32_t indexCount, CGxBuf*& indexBuf);
    static void AllocGxBufMat(DDMaterial* mat);
    static CDetailDoodadInst* AllocInst();
    static void CreateAlphaRampTexture(const void*& texels);
    static void Destroy();
    static void DestroyModels();
    static void DestroyPools();
    static void DirtyPools();
    static void FillIndicesInstanced(DDMaterial* mat);
    static void FillVerticesInstanced(DDMaterial* mat);
    static void FreeGxBuf(CGxBuf*& buf);
    static void FreeGxBufBatch(CGxBuf*& vertexBuf, CGxBuf*& indexBuf);
    static void FreeGxBufMat(DDMaterial* mat);
    static void FreeInst(CDetailDoodadInst* inst);
    static void GetMinMaxDensity(uint32_t& minDensity, uint32_t& maxDensity);
    static void GetMinMaxDist(float& minDist, float& maxDist);
    static void Initialize();
    static void InitializeRenderState(CWorldView* view);
    static void InitializeShaderConstants(CWorldView* view);
    static int32_t IsInstanced();
    static void LinkToAsyncHead(uint32_t id);
    static int32_t LoadModel(uint32_t id);
    static void Render(CWorldView* view);
    static void RenderInstanced(CWorldView* view);
    static void ResetVisibility();
    static void SetInstanced(int32_t instanced);
    static void SetPixelShader(uint32_t shadowMode);
    static void SetVertexShader(int32_t pointLights, uint32_t shadowMode);
    static void UpdateAlphaRampTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
    static void UpdatePools();
    static void UpdateWind();
};

struct CDetailDoodad::SShaderConstants {
    struct SPointLight {
        /* 0000 */ C4Vector m_position;
        /* 0010 */ C3Vector m_color;
        /* 001C */ float m_colorW;
        /* 0020 */ C3Vector m_attenuation;
        /* 002C */ float m_attenuationW;
    };

    /* 0000 */ C44Matrix m_projection;
    /* 0040 */ C4Vector m_fog;
    /* 0050 */ C4Vector m_fade;
    /* 0060 */ C4Vector m_sunDir;
    /* 0070 */ C3Vector m_sunAmbient;
    /* 007C */ float m_sunAmbientW;
    /* 0080 */ C3Vector m_sunDiffuse;
    /* 008C */ float m_sunDiffuseW;
    /* 0090 */ C3Vector m_sunSpecular;
    /* 009C */ float m_sunSpecularW;
    /* 00A0 */ SPointLight m_pointLights[3];
    /* 0130 */ C44Matrix m_world;
    /* 0170 */ C44Matrix m_worldView;
    /* 01B0 */ C44Matrix m_view;
    /* 01F0 */ C4Vector m_wind;
};

#include "WoW/Source/WorldClient/DetailDoodad.inl"

#endif
