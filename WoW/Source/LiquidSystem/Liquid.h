#ifndef WOW_SOURCE_LIQUIDSYSTEM_LIQUID_H
#define WOW_SOURCE_LIQUIDSYSTEM_LIQUID_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/LiquidSystem/Types.h"

class CGxBatch;
class CGxFrameBuffer;
class CGxShader;
class CGxTex;
class CM2Lighting;
class CTexture;
struct CGxBuf;

class WaterSurfaceGen {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ int32_t m_resolution;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ float* m_alignedBuffers[5];
    /* 0020 */ uint8_t* unk_0020;
    /* 0024 */ uint8_t* unk_0024;
    /* 0028 */ uint8_t* unk_0028;
    /* 002C */ uint8_t unk_002C[56];
    /* 0064 */ TSGrowableArray<uint16_t> m_indices;
    /* 0074 */ CTexture* m_texture;

    ~WaterSurfaceGen();
    void Release();
};

namespace Liquid {
    class CMaterialSettings;

    enum ERenderPass {
        RenderPass_Opaque = 0,
        RenderPass_Transparent = 1,
    };

    enum EPertSurface {
        PertSurface_Water = 0,
        PertSurface_Ocean = 1,
    };

    struct DisplMapPerturbation {
        /* 0000 */ C3Vector m_pos;
        /* 000C */ float m_radius;
        /* 0010 */ float m_amplitude;
        /* 0014 */ uint32_t m_period;
        /* 0018 */ uint32_t unk_0018;
    };

    struct DisplMapPerturbationMesh {
        /* 0000 */ C44Matrix m_transform;
        /* 0040 */ uint32_t m_vertexAlloc;
        /* 0044 */ uint32_t m_vertexCount;
        /* 0048 */ C3Vector* m_vertices;
        /* 004C */ uint32_t m_vertexChunk;
        /* 0050 */ uint32_t m_indexAlloc;
        /* 0054 */ uint32_t m_indexCount;
        /* 0058 */ uint16_t* m_indices;
        /* 005C */ uint32_t m_indexChunk;
        /* 0060 */ float m_amplitude;
        /* 0064 */ uint32_t m_period;
        /* 0068 */ uint32_t unk_0068;

        DisplMapPerturbationMesh();
    };

    typedef void (*CONFIG_CHANGED_CALLBACK)();
    typedef void (*TEXTURE_LOAD_CALLBACK)(const char* name);

    class IEnvironment {
        public:
        virtual ~IEnvironment();
        virtual void Release() = 0;
        virtual void SetInteriorFog(int32_t interiorFog) = 0;
        virtual void SelectLights(CM2Lighting& lighting) = 0;
    };

    class IGeomFactory {
        public:
        virtual ~IGeomFactory();
        virtual void Release() = 0;
        virtual void GetBuffers(EGxVertexBufferFormat format, CGxBuf*& vertexBuf, CGxBuf*& indexBuf, CGxBatch& batch, int32_t a5) = 0;
        virtual void Purge() = 0;
        virtual float GetHeight() = 0;
        virtual bool SetTransform(const C44Matrix& transform) = 0;
        virtual bool GetWorldSpace() = 0;
    };

    class CInstance;

    class IMaterial {
        public:
        typedef void (*RENDER_FUNC)(uint32_t pass, const C3Vector& camPos, CInstance* instance);

        /* 0004 */ RENDER_FUNC m_render;

        virtual ~IMaterial();
        virtual void Release();
        virtual void Reset();
        virtual void OnAddToScene();
        virtual int32_t SetupRender(uint32_t time, ERenderPass pass, const C3Vector& camPos, IEnvironment* environment, CMaterialSettings* settings, uint32_t reflection);
        virtual int32_t SetupDefaultTransform(const C3Vector& camPos);
        virtual int32_t SetupLighting(const C3Vector& camPos, CInstance* instance);
        virtual int32_t IsAllLoaded(CMaterialSettings* settings) = 0;
        virtual void LinkToAsyncHead(CMaterialSettings* settings) = 0;
    };

    class CMaterialWater : public IMaterial {
        public:
        static uint32_t s_refCount;
        static CGxShader* s_vertexShader;
        static CGxShader* s_pixelShader;
        static CGxShader* s_pixelShaderBelow;
        static CGxShader* s_pixelShaderAboveFog;
        static C4Vector s_vsConsts[53];
        static C4Vector s_psConsts[32];

        static void DestroyResources();
        static void InitResources();
        static void RenderProcedural(uint32_t pass, const C3Vector& camPos, CInstance* instance);
        static void RenderShader(uint32_t pass, const C3Vector& camPos, CInstance* instance);
        static void SetFogConstants();
        static void SetPixelShaderAbove(uint32_t reflection);
        static void SetPixelShaderAboveFog();
        static void SetPixelShaderBelow(uint32_t reflection);

        CMaterialWater();
        virtual ~CMaterialWater();
        virtual void Reset();
        virtual void OnAddToScene();
        virtual int32_t SetupRender(uint32_t time, ERenderPass pass, const C3Vector& camPos, IEnvironment* environment, CMaterialSettings* settings, uint32_t reflection);
        virtual int32_t SetupDefaultTransform(const C3Vector& camPos);
        virtual int32_t SetupLighting(const C3Vector& camPos, CInstance* instance);
        virtual int32_t IsAllLoaded(CMaterialSettings* settings);
        virtual void LinkToAsyncHead(CMaterialSettings* settings);
    };

    class CMaterialMagma : public IMaterial {
        public:
        static uint32_t s_refCount;
        static CGxShader* s_vertexShader;
        static CGxShader* s_pixelShader;
        static CGxShader* s_procVertexShader;
        static CGxShader* s_procPixelShader;
        static C4Vector s_vsConsts[51];
        static C4Vector s_psConsts[18];

        static void DestroyResources();
        static void InitResources();
        static void RenderFixedFunction(uint32_t pass, const C3Vector& camPos, CInstance* instance);
        static void RenderProcedural(uint32_t pass, const C3Vector& camPos, CInstance* instance);
        static void RenderShader(uint32_t pass, const C3Vector& camPos, CInstance* instance);

        CMaterialMagma();
        virtual ~CMaterialMagma();
        virtual void Reset();
        virtual int32_t SetupRender(uint32_t time, ERenderPass pass, const C3Vector& camPos, IEnvironment* environment, CMaterialSettings* settings, uint32_t reflection);
        virtual int32_t SetupDefaultTransform(const C3Vector& camPos);
        virtual int32_t SetupLighting(const C3Vector& camPos, CInstance* instance);
        virtual int32_t IsAllLoaded(CMaterialSettings* settings);
        virtual void LinkToAsyncHead(CMaterialSettings* settings);
    };

    class CMaterialMercury : public IMaterial {
        public:
        static uint32_t s_refCount;
        static CGxShader* s_vertexShader;
        static CGxShader* s_pixelShader;
        static C4Vector s_vsConsts[53];
        static C4Vector s_psConsts[32];

        static void InitResources();
        static void RenderFixedFunction(uint32_t pass, const C3Vector& camPos, CInstance* instance);
        static void RenderProcedural(uint32_t pass, const C3Vector& camPos, CInstance* instance);
        static void RenderShader(uint32_t pass, const C3Vector& camPos, CInstance* instance);
        static void SetFogConstants();

        CMaterialMercury();
        virtual ~CMaterialMercury();
        virtual void Reset();
        virtual void OnAddToScene();
        virtual int32_t SetupRender(uint32_t time, ERenderPass pass, const C3Vector& camPos, IEnvironment* environment, CMaterialSettings* settings, uint32_t reflection);
        virtual int32_t SetupDefaultTransform(const C3Vector& camPos);
        virtual int32_t SetupLighting(const C3Vector& camPos, CInstance* instance);
        virtual int32_t IsAllLoaded(CMaterialSettings* settings);
        virtual void LinkToAsyncHead(CMaterialSettings* settings);
    };

    class CMaterialDebug : public IMaterial {
        public:
        static uint32_t s_refCount;
        static CGxShader* s_vertexShader;
        static CGxShader* s_pixelShader;
        static C4Vector s_vsConsts[50];
        static C4Vector s_psConsts[17];

        static void Render(uint32_t pass, const C3Vector& camPos, CInstance* instance);

        CMaterialDebug();
        virtual ~CMaterialDebug();
        virtual void Reset();
        virtual int32_t SetupRender(uint32_t time, ERenderPass pass, const C3Vector& camPos, IEnvironment* environment, CMaterialSettings* settings, uint32_t reflection);
        virtual int32_t IsAllLoaded(CMaterialSettings* settings);
        virtual void LinkToAsyncHead(CMaterialSettings* settings);
    };

    class CMaterialSettings {
        public:
        /* 0000 */ uint32_t m_liquidType;
        /* 0004 */ char m_textureNames[6][128];
        /* 0304 */ CImVector m_colors[2];
        /* 030C */ int32_t m_ints[4];
        /* 031C */ float m_floats[18];
        /* 0364 */ uint8_t m_transparent;
        /* 0368 */ int32_t m_texturesReady[6];
        /* 0380 */ TSGrowableArray<CTexture*> m_textures[6];
        /* 03E0 */ TSGrowableArray<CTexture*> m_pendingTextures[6];

        CMaterialSettings();
        ~CMaterialSettings();
        void Destroy();
        float GetFloat(uint32_t index);
        int32_t GetInt(uint32_t index);
        CGxTex* GetTexture(uint32_t index, uint32_t period);
        void Initialize(uint32_t liquidType);
        void InitTexture(uint32_t index);
        int32_t IsAllLoaded(uint32_t index);
        void LinkToAsyncHead(uint32_t index);
    };

    class Object : public TSHashObject<Object, HASHKEY_NONE> {
        public:
        /* 0018 */ uint32_t m_objectId;
        /* 001C */ uint32_t m_refCount;
        /* 0020 */ uint8_t m_noReflection;
        /* 0024 */ uint32_t m_reflectionType;
        /* 0028 */ C4Plane m_reflectionPlane;

        void SetReflectionPlane(float height);
    };

    class CInstance {
        public:
        static void Initialize();
        static CInstance* Create();
        static void Destroy();

        /* 0000 */ IMaterial* m_material;
        /* 0004 */ CMaterialSettings* m_settings;
        /* 0008 */ IGeomFactory* m_geomFactory;
        /* 000C */ IEnvironment* m_environment;
        /* 0010 */ Object* m_object;
        /* 0014 */ C44Matrix m_transform;
        /* 0054 */ CAaSphere m_bounds;
        /* 0064 */ uint16_t unk_0064;
        /* 0066 */ uint16_t m_isMesh;
        /* 0068 */ int32_t m_refCount;

        CInstance();
        void AddRef();
        void Purge();
        void Release();
        void SetTransform(const C44Matrix& transform);
    };

    class CRenderer {
        public:
        typedef int32_t (*DrawListSorter)(const void* a, const void* b);

        static DrawListSorter s_drawListSorter[2];

        static int32_t CompareOpaque(const void* a, const void* b);
        static int32_t CompareTransparent(const void* a, const void* b);

        /* 0000 */ TSGrowableArray<CInstance*> m_instances[2];
        /* 0020 */ int32_t m_sorted[2];
        /* 0028 */ uint32_t m_isReflection;

        CRenderer(uint32_t isReflection);
        ~CRenderer();
        void Draw(CInstance* instance);
        void Flush(uint32_t time, const C3Vector& camPos, uint32_t pass);
    };

    class CSettingsBank {
        public:
        static uint32_t s_refCount;
        static TSGrowableArray<CMaterialSettings*> s_settings;

        static void Create();
        static void Destroy();
        static CMaterialSettings* GetSettings(uint32_t liquidType);
        static void Purge();
        static int32_t UpdateRecord(uint32_t liquidType, CMaterialSettings* settings);
    };

    class CMaterialBank {
        public:
        static uint32_t s_refCount;
        static TSGrowableArray<IMaterial*> s_materials;

        static void Create();
        static void Destroy();
        static IMaterial* GetMaterial(uint32_t liquidType);
        static void Purge();
        static void ResetMaterials();
    };

    extern uint32_t s_displMapCount;
    extern uint8_t s_oceanDepthTexValid;
    extern uint8_t s_riverDepthTexValid;
    extern uint8_t s_wmoWaterTexValid;
    extern uint8_t s_displMapDirty;
    extern CGxFrameBuffer* s_displMapFBs[4];
    extern CTexture* s_displMaps[4];
    extern TSGrowableArray<DisplMapPerturbation*> s_eventPerturbations[2];
    extern TSGrowableArray<DisplMapPerturbationMesh*> s_eventPerturbationMeshes[2];
    extern CGxShader* s_clearDisplMapPS;
    extern CGxShader* s_clearDisplMapVS;
    extern CGxShader* s_fourTapBlurPS;
    extern CGxShader* s_fourTapBlurVS;
    extern CDataAllocator s_instanceAllocator;
    extern uint32_t s_kawasePingPong;
    extern CGxShader* s_perturbDisplMapMeshPS;
    extern CGxShader* s_perturbDisplMapMeshVS;
    extern CGxShader* s_perturbDisplMapPS;
    extern CGxShader* s_perturbDisplMapVS;
    extern CTexture* s_perturbTex;
    extern TSGrowableArray<DisplMapPerturbation*> s_perturbations[2];
    extern TSGrowableArray<DisplMapPerturbationMesh*> s_perturbationMeshes[2];
    extern CGxShader* s_propagateDisplMapPS;
    extern CGxShader* s_propagateDisplMapVS;
    extern CDataAllocator s_settingsAllocator;
    extern TEXTURE_LOAD_CALLBACK s_textureLoadCallback;

    void AddConfigChangedCallback(CONFIG_CHANGED_CALLBACK callback);
    void AddPerturbation(EPertSurface surface, const C3Vector& pos, float radius, float amplitude);
    void CacheProceduralTextures();
    void ClearDisplMap();
    void ClearEventPerturbations();
    void ClearPerturbatorCache();
    void CreateDynamicDisplacementMap();
    DisplMapPerturbation* CreatePerturbator(EPertSurface surface, int32_t force);
    DisplMapPerturbationMesh* CreatePerturbatorMesh(EPertSurface surface);
    void CreateProceduralTextures(int32_t materials);
    void DestroyPerturbator(EPertSurface surface, DisplMapPerturbation* perturbation);
    void DestroyPerturbatorMesh(EPertSurface surface, DisplMapPerturbationMesh* mesh);
    void DestroyProceduralTextures(int32_t materials);
    int32_t CameraSubmerged();
    CInstance* CreateInstance(uint32_t a1, uint32_t liquidType, IGeomFactory* factory);
    const SMLiquidChunkDesc* GetChunkDesc(const void* data, uint32_t x, uint32_t y);
    uint8_t GetChunkDepth(uint32_t format, const void* vertices, uint32_t count, uint32_t index);
    float GetChunkHeight(uint32_t format, const void* vertices, uint32_t count, uint32_t index);
    const uint16_t* GetChunkTexCoord0(uint32_t format, const void* vertices, uint32_t count, uint32_t index);
    const uint8_t* GetExistsTable(const void* data, const SMLiquidInstance* instance);
    const SMLiquidInstance* GetInstanceDesc(const void* data, const SMLiquidChunkDesc* desc, uint32_t index);
    const void* GetInstanceVertices(const void* data, const SMLiquidInstance* instance);
    int32_t GetVertexFormat(uint32_t liquidType, uint32_t format);
    bool HasData(const SMLiquidChunkDesc* desc, const SMLiquidInstance* instance);
    void Destroy();
    void DestroySurfaceGen();
    int32_t WaterDetailNeedsDepthCache(uint32_t detail);
    void GetAboveWaterFog(float& start, float& end, float& density, CImVector& color);
    CTexture* GetBackBuffer();
    CTexture* GetBlurredBackBuffer();
    CGxFrameBuffer* GetBlurredBackBufferFB();
    CTexture* GetBlurredReflectionBuffer();
    void GetDisplMapInfoRaw(uint32_t index, C3Vector& pos, C2Vector& size);
    C2iVector GetDisplMapTexSize(uint32_t index);
    CGxTex* GetDynamicDisplMap(uint32_t index);
    uint32_t GetDynamicDisplMapCnt();
    float GetDynamicDisplMapInterpolation(uint32_t index);
    CGxFrameBuffer* GetBlurredReflectionBufferFB();
    float GetClosestReflectionHeight();
    CTexture* GetFakeReflectionBuffer();
    CGxFrameBuffer* GetFakeReflectionFB();
    CTexture* GetRealReflectionBuffer();
    CGxFrameBuffer* GetRealReflectionFB();
    uint32_t GetReflectionDownscale();
    uint32_t GetReflectionMode();
    uint32_t GetRippleDetail();
    int32_t GetShadersEnabled();
    void GetUnderWaterFog(float& start, float& end, float& density, CImVector& color);
    uint32_t GetWaterDetail();
    extern WaterSurfaceGen* s_surfaceGen;

    void GrabBackBuffer();
    const C44Matrix* GetTexScrollMtx(uint32_t time, const C2Vector& speed);
    void SetMatrixConstant(C4Vector* constants, uint32_t index, const C44Matrix& matrix);
    void SetScreenConstants(C4Vector* constants);
    void SetShaderLighting(C4Vector* vsConstants, C4Vector* psConstants, const CM2Lighting& lighting);
    void SetupTransform(C4Vector* vsConstants, C4Vector* psConstants, const C3Vector& camPos, const C44Matrix& transform);
    int32_t HasDynamicDisplMap();
    int32_t HasFakeReflection();
    int32_t HasRealReflection();
    void InitLiquidObjects();
    int32_t Initialize(int32_t a1, float worldCoordScale);
    int32_t IsAllLoaded(uint32_t liquidType);
    int32_t IsTransparent(uint32_t liquidType);
    void KawaseBlur(CTexture* source, CTexture* target, CGxFrameBuffer* frameBuffer, float a4);
    void LinkToAsyncHead(uint32_t liquidType);
    int32_t NeedBackBuffer();
    void __fastcall OnTexturesDirty();
    void PerturbDisplMap(TSGrowableArray<DisplMapPerturbation*>& perturbations, uint32_t index, uint32_t time);
    void PerturbDisplMapMesh(TSGrowableArray<DisplMapPerturbationMesh*>& meshes, uint32_t index, uint32_t time);
    void PropagateDisplMap(uint32_t index);
    int32_t NeedDynDisplMap();
    void PurgeMaterialBank();
    Object* RegisterLiquidObject(uint32_t a1, uint32_t liquidType, uint32_t materialId);
    void RemoveConfigChangedCallback(CONFIG_CHANGED_CALLBACK callback);
    void RendererDestroy();
    void RendererDraw(CInstance* instance);
    void RendererFlush(uint32_t time, const C3Vector& camPos, uint32_t pass);
    void RendererInit();
    void RendererValidate();
    void SetAboveWaterFog(float start, float end, float density, const CImVector& color);
    void SetCamPos(const C3Vector& pos);
    void SetCameraSubmerged(int32_t submerged);
    void SetDisplMapInfo(const C3Vector& pos, const C2Vector& size);
    void SetNeedBackBuffer(int32_t need);
    void SetNeedDynDisplMap(int32_t need);
    int32_t SetReflectionDownscale(uint32_t downscale);
    int32_t SetReflectionMode(uint32_t mode);
    int32_t SetRippleDetail(uint32_t detail);
    void SetUnderWaterFog(float start, float end, float density, const CImVector& color);
    int32_t SetWaterDetail(uint32_t detail);
    void SignalConfigChanged(int32_t materials);
    void UnCacheProceduralTextures();
    void UnregisterLiquidObject(Object* object);
    void Update();
    void UpdateDisplMap(uint32_t time);
    void UpdateProceduralTextures();
    void UpdateSurfaceGen(uint32_t time);
    int32_t UseCameraRelativeCoords();
    uint32_t ValidateReflectionDownscale(uint32_t downscale);
    uint32_t ValidateReflectionMode(uint32_t mode);
    uint32_t ValidateRippleDetail(uint32_t detail);
    uint32_t ValidateWaterDetail(uint32_t detail);
    float WorldCoordScale();
}

#endif
