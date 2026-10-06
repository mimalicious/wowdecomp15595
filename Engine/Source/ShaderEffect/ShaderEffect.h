#ifndef ENGINE_SOURCE_SHADEREFFECT_SHADEREFFECT_H
#define ENGINE_SOURCE_SHADEREFFECT_SHADEREFFECT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/ShaderEffect/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/C4Vector.h"

class C2Vector;
class C44Matrix;
class CGxShader;
class CGxShaderPermute;
class CImVector;
class CM2Light;
class CM2Lighting;
class CTexture;

struct LocalLights {
    /* 0000 */ C4Vector m_color[4];
    /* 0040 */ C4Vector m_position[4];
    /* 0080 */ float m_constantAttenuation[4];
    /* 0090 */ float m_linearAttenuation[4];
    /* 00A0 */ float m_quadraticAttenuation[4];
};

class CShaderEffect : public TSHashObject<CShaderEffect, HASHKEY_STRI> {
    public:
    static int32_t s_clipEnabled;
    static C4Plane s_clipPlane;
    static CShaderEffect* s_curEffect;
    static float s_displFactor;
    static CTexture* s_displTex;
    static int32_t s_enableShaders;
    static C4Vector s_fogColorAlphaRef;
    static float s_fogMul;
    static C4Vector s_fogParams;
    static int32_t s_forceAlphaTest;
    static int32_t s_lightEnabled;
    static uint32_t s_localLightCount;
    static LocalLights s_localLights;
    static uint32_t s_shadowValue;
    static C3Vector s_sunAmbient;
    static C3Vector s_sunDiffuse;
    static C3Vector s_sunDir;
    static C3Vector s_sunSpec;
    static float s_terrainDisplFactor;
    static float s_tesselationDist;
    static float s_tesselationFactor;
    static int32_t s_useAlphaRef;
    static uint32_t s_usePcfFiltering;

    static void ComputeLocalLights(LocalLights* localLights, uint32_t lightCount, CM2Light** lights, const C3Vector* origin);
    static int32_t DisplacementEnabled();
    static void FlushFogParams();
    static int32_t GetClipEnabled();
    static void GetClipPlane(C4Plane& plane);
    static void InitShaderSystem(int32_t enableShaders, int32_t usePcfFiltering);
    static void ResetShaders(uint32_t hullPermute, uint32_t domainPermute);
    static uint32_t SelectShadowShader(uint32_t shadow);
    static void SetAlphaRef(float alphaRef);
    static void SetAlphaRefDefault();
    static void SetClipEnabled(int32_t enabled);
    static void SetClipPlane(const C4Plane& plane);
    static void SetDefaultShaders(uint32_t shadow, uint32_t boneInfluences);
    static void SetDiffuse(const C4Vector& color);
    static void SetEmissive(const C4Vector& color);
    static void SetFogEnabled(int32_t enabled);
    static void SetFogParams(float start, float end, float density, const CImVector& color, float extra);
    static void SetGlobalLighting(CM2Light* light);
    static void SetLightingEnabled(int32_t enabled);
    static void SetLocalLighting(CM2Lighting* lighting, int32_t enabled, const C3Vector* origin);
    static void SetShaders(uint32_t vertexPermute, int32_t hullPermute, int32_t domainPermute, uint32_t pixelPermute);
    static void SetTexMtx(const C2Vector& scale, const C2Vector& offset, uint32_t stage);
    static void SetTexMtx(const C44Matrix& matrix, uint32_t stage);
    static void SetTexMtx_Identity(uint32_t stage);
    static void SetTexMtx_SphereMap(uint32_t stage);
    static void SetTexMtx_TexGen(EGxTexGen texGen, const C44Matrix& matrix, uint32_t stage);
    static void SetUsePcfFiltering(uint32_t usePcfFiltering);
    static int32_t TerrainDisplEnabled();
    static int32_t TesselationEnabled();
    static void UpdateDisplacement();
    static void UpdateProjMatrix();
    static void UpdateTerrainDispl();
    static void UpdateTesselation();
    static void UpdateWorldViewMatrix();

    /* 0018 */ int32_t m_refCount;
    /* 001C */ uint32_t m_fixedFuncPassCount;
    /* 0020 */ EGxTexOp m_colorOps[2];
    /* 0028 */ EGxTexOp m_alphaOps[2];
    /* 0030 */ CGxShader* m_vertexShader;
    /* 0034 */ CGxShader* m_hullShader;
    /* 0038 */ CGxShader* m_domainShader;
    /* 003C */ CGxShader* m_pixelShader;

    CShaderEffect();
    ~CShaderEffect();
    void AddRef();
    CGxShaderPermute* GetPixelPermute(uint32_t permute);
    CGxShaderPermute* GetVertexPermute(uint32_t permute);
    void InitEffect(const char* vertexShader, const char* hullShader, const char* domainShader, const char* pixelShader);
    void InitFixedFuncPass(const EGxTexOp* colorOps, const EGxTexOp* alphaOps, uint32_t count);
    void SetCurrent();
};

#endif
