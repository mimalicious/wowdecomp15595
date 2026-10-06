#ifndef ENGINE_SOURCE_GX_TYPES_H
#define ENGINE_SOURCE_GX_TYPES_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Storm/H/stpl.h"

struct CGxBuf;
struct CGxPool;
class CGxQuery;

enum EGxApi {
    GxApi_OpenGl = 0,
    GxApi_D3d9 = 1,
    GxApi_D3d11 = 2,
    GxApi_GLL = 3,
    GxApis_Last = 4,
};

enum EGxBlend {
    GxBlend_Opaque = 0,
    GxBlend_AlphaKey = 1,
    GxBlend_Alpha = 2,
    GxBlend_Add = 3,
    GxBlend_Mod = 4,
    GxBlend_Mod2x = 5,
    GxBlend_ModAdd = 6,
    GxBlend_InvSrcAlphaAdd = 7,
    GxBlend_InvSrcAlphaOpaque = 8,
    GxBlend_SrcAlphaOpaque = 9,
    GxBlend_NoAlphaAdd = 10,
    GxBlend_ConstantAlpha = 11,
    GxBlends_Last = 12,
};

enum EGxTexFormat {
    GxTex_Unknown = 0,
    GxTex_Abgr8888 = 1,
    GxTex_Argb8888 = 2,
    GxTex_Argb4444 = 3,
    GxTex_Argb1555 = 4,
    GxTex_Rgb565 = 5,
    GxTex_Uv88 = 6,
    GxTex_Dxt1 = 7,
    GxTex_Dxt3 = 8,
    GxTex_Dxt5 = 9,
    GxTex_R16 = 10,
    GxTex_Gr1616F = 11,
    GxTex_Abgr16161616F = 12,
    GxTex_R24X8 = 13,
    GxTex_R32F = 14,
    GxTex_Abgr32323232F = 15,
    GxTex_Null = 16,
    GxTex_D24X8 = 17,
    GxTex_D16 = 18,
    GxTex_D32 = 19,
    GxTex_Intz = 20,
    GxTex_Rawz = 21,
    GxTex_Resz = 22,
    GxTex_AtiD24 = 23,
    GxTexFormats_Last = 24,
};

enum EGxVertexBufferFormat {
    GxVBF_P = 0,
    GxVBF_PN = 1,
    GxVBF_PNC = 2,
    GxVBF_PNT = 3,
    GxVBF_PNCT = 4,
    GxVBF_PNCT_TS = 5,
    GxVBF_PNT2 = 6,
    GxVBF_PNCT2 = 7,
    GxVBF_PC = 8,
    GxVBF_PCT = 9,
    GxVBF_PCT2 = 10,
    GxVBF_PCT3 = 11,
    GxVBF_PCT4 = 12,
    GxVBF_PT = 13,
    GxVBF_PT2 = 14,
    GxVBF_PBNT2 = 15,
    GxVBF_PNC2 = 16,
    GxVBF_PNC2T = 17,
    GxVBF_PNC2T_TS = 18,
    GxVBF_PNC2T2 = 19,
    GxVBF_Unk20 = 20,
    GxVBF_PT2_iDD = 21,
    GxVertexBufferFormats_Last = 22,
};

enum EGxBuffer {
    GxBuffers_Color = 0,
    GxBuffers_Color1 = 1,
    GxBuffers_Color2 = 2,
    GxBuffers_Color3 = 3,
    GxBuffers_Depth = 4,
    GxBuffers_Last = 5,
};

enum EGxOverride {
    GxOverride_PixelShader = 0,
    GxOverride_Unk1 = 1,
    GxOverride_Unk2 = 2,
    GxOverride_Unk3 = 3,
    GxOverride_Unk4 = 4,
    GxOverride_Unk5 = 5,
    GxOverride_Unk6 = 6,
    GxOverride_Unk7 = 7,
    GxOverride_Unk8 = 8,
    GxOverrides_Last = 9,
};

enum EGxQueryParam {
    GxQueryParam_Unk0 = 0,
    GxQueryParam_Unk1 = 1,
    GxQueryParams_Last = 2,
};

enum EGxTexCommand {
    GxTex_Lock = 0,
    GxTex_Latch = 1,
    GxTex_Unlock = 2,
    GxTex_3 = 3,
    GxTexCommands_Last = 4,
};

enum EGxWM {
    GxWM_Size = 0,
    GxWM_DisplayChange = 1,
    GxWM_Destroy = 2,
    GxWM_SetFocus = 3,
    GxWM_KillFocus = 4,
};

enum EGxTexGen {
    GxTexGen_Disabled = 0,
    GxTexGen_ObjectLinear = 1,
    GxTexGen_EyeLinear = 2,
    GxTexGen_Unk3 = 3,
    GxTexGen_ReflectionMap = 4,
    GxTexGen_Normal = 5,
    GxTexGen_SphereMap = 6,
    GxTexGens_Last = 7,
};

enum EGxTexFilter {
    GxTex_Nearest = 0,
    GxTex_Linear = 1,
    GxTex_NearestMipNearest = 2,
    GxTex_LinearMipNearest = 3,
    GxTex_LinearMipLinear = 4,
    GxTex_Anisotropic = 5,
    GxTexFilters_Last = 6,
};

enum EGxTexWrapMode {
    GxTex_Clamp = 0,
    GxTex_Wrap = 1,
};

enum EGxXform {
    GxXform_Tex0 = 0,
    GxXform_Tex1 = 1,
    GxXform_Tex2 = 2,
    GxXform_Tex3 = 3,
    GxXform_Tex4 = 4,
    GxXform_Tex5 = 5,
    GxXform_Tex6 = 6,
    GxXform_Tex7 = 7,
    GxXform_World = 8,
    GxXform_Projection = 9,
    GxXform_View = 10,
    GxXforms_Last = 11,
};

enum BlitFormat {
    BlitFormat_Unknown = 0,
    BlitFormat_Abgr8888 = 1,
    BlitFormat_Argb8888 = 2,
    BlitFormat_Argb4444 = 3,
    BlitFormat_Argb1555 = 4,
    BlitFormat_Rgb565 = 5,
    BlitFormat_Uv88 = 6,
    BlitFormat_Dxt1 = 7,
    BlitFormat_Dxt3 = 8,
    BlitFormat_Dxt5 = 9,
    BlitFormat_Gr1616F = 10,
    BlitFormat_Abgr16161616F = 11,
    BlitFormat_R32F = 12,
    BlitFormat_Abgr32323232F = 13,
    BlitFormat_D24X8 = 14,
    BlitFormat_D32 = 15,
    BlitFormats_Last = 16,
};

class CGxBatch {
    public:
    /* 0000 */ EGxPrim m_primType;
    /* 0004 */ uint32_t m_start;
    /* 0008 */ uint32_t m_count;
    /* 000C */ uint16_t m_minIndex;
    /* 000E */ uint16_t m_maxIndex;
};

enum EGxColorFormat {
    GxCF_argb = 0,
    GxCF_rgba = 1,
    GxColorFormats_Last = 2,
};

class CGxCaps {
    public:
    /* 0000 */ int32_t m_numTmus;
    /* 0004 */ int32_t m_numVertexTmus;
    /* 0008 */ int32_t m_numShaderTmus;
    /* 000C */ int32_t m_pixelCenterOnEdge;
    /* 0010 */ int32_t m_texelCenterOnEdge;
    /* 0014 */ int32_t m_numStreams;
    /* 0018 */ EGxColorFormat m_colorFormat;
    /* 001C */ int32_t unk_001C;
    /* 0020 */ uint32_t m_maxIndex;
    /* 0024 */ int32_t m_texTarget [5];
    /* 0038 */ uint32_t m_texNonPow2;
    /* 003C */ uint32_t m_texMaxSize [5];
    /* 0050 */ int32_t m_vertexTexFmt [24];
    /* 00B0 */ int32_t m_texFmt [24];
    /* 0110 */ int32_t m_rttFormat [24];
    /* 0170 */ int32_t m_pcfFormat [24];
    /* 01D0 */ uint32_t unk_01D0;
    /* 01D4 */ uint32_t m_numRenderTargets;
    /* 01D8 */ int32_t m_shaderModel;
    /* 01DC */ int32_t m_shaderProfiles [38];
    /* 0274 */ int32_t m_shaderTargets [6];
    /* 028C */ uint32_t m_shaderConstants [6];
    /* 02A4 */ int32_t m_texFilterTrilinear;
    /* 02A8 */ int32_t m_texFilterAnisotropic;
    /* 02AC */ uint32_t m_maxTexAnisotropy;
    /* 02B0 */ int32_t m_depthBias;
    /* 02B4 */ int32_t m_colorWrite;
    /* 02B8 */ int32_t m_maxClipPlanes;
    /* 02BC */ int32_t unk_02BC;
    /* 02C0 */ int32_t m_hwCursor;
    /* 02C4 */ int32_t m_occlusionQuery;
    /* 02C8 */ int32_t m_pointParameters;
    /* 02CC */ float m_pointScaleMax;
    /* 02D0 */ int32_t m_pointSprite;
    /* 02D4 */ uint32_t m_blendFactor;
    /* 02D8 */ uint32_t unk_02D8 [5];
    /* 02EC */ int32_t m_oglRtAlpha;
    /* 02F0 */ int32_t m_stereoAvailable;
    /* 02F4 */ int32_t unk_02F4;
    /* 02F8 */ int32_t unk_02F8;
    /* 02FC */ int32_t unk_02FC;
    /* 0300 */ int32_t unk_0300;
    /* 0304 */ int32_t unk_0304;
};

class CGxQuery {
    public:
    /* 0000 */ void* m_apiSpecific;
    /* 0004 */ EGxQueryType m_type;
    /* 0008 */ TSLink<CGxQuery> m_link;

    CGxQuery(EGxQueryType type);
};

// INLINED
inline CGxQuery::CGxQuery(EGxQueryType type) {
    this->m_apiSpecific = NULL;
    this->m_type = type;
}

enum EGxPoolTarget {
    GxPoolTarget_Vertex = 0,
    GxPoolTarget_Index = 1,
    GxPoolTargets_Last = 2,
};

enum EGxPoolUsage {
    GxPoolUsage_Static = 0,
    GxPoolUsage_Dynamic = 1,
    GxPoolUsage_Stream = 2,
    GxPoolUsages_Last = 3,
};

enum EGxRenderState {
    GxRs_PolygonOffset = 0,
    GxRs_MatDiffuse = 1,
    GxRs_MatEmissive = 2,
    GxRs_MatSpecular = 3,
    GxRs_MatSpecularExp = 4,
    GxRs_NormalizeNormals = 5,
    GxRs_BlendingMode = 6,
    GxRs_AlphaRef = 7,
    GxRs_FogStart = 8,
    GxRs_FogEnd = 9,
    GxRs_FogColor = 10,
    GxRs_Lighting = 11,
    GxRs_Fog = 12,
    GxRs_DepthTest = 13,
    GxRs_DepthFunc = 14,
    GxRs_DepthWrite = 15,
    GxRs_ColorWrite = 16,
    GxRs_Culling = 17,
    GxRs_ClipPlaneMask = 18,
    GxRs_Multisample = 19,
    GxRs_ScissorTest = 20,
    GxRs_Texture0 = 21,
    GxRs_Texture1 = 22,
    GxRs_Texture2 = 23,
    GxRs_Texture3 = 24,
    GxRs_Texture4 = 25,
    GxRs_Texture5 = 26,
    GxRs_Texture6 = 27,
    GxRs_Texture7 = 28,
    GxRs_Texture8 = 29,
    GxRs_Texture9 = 30,
    GxRs_Texture10 = 31,
    GxRs_Texture11 = 32,
    GxRs_Texture12 = 33,
    GxRs_Texture13 = 34,
    GxRs_Texture14 = 35,
    GxRs_Texture15 = 36,
    GxRs_VertexTexture0 = 37,
    GxRs_VertexTexture1 = 38,
    GxRs_VertexTexture2 = 39,
    GxRs_VertexTexture3 = 40,
    GxRs_Unk41 = 41,
    GxRs_ColorOp0 = 42,
    GxRs_ColorOp1 = 43,
    GxRs_ColorOp2 = 44,
    GxRs_ColorOp3 = 45,
    GxRs_ColorOp4 = 46,
    GxRs_ColorOp5 = 47,
    GxRs_ColorOp6 = 48,
    GxRs_ColorOp7 = 49,
    GxRs_AlphaOp0 = 50,
    GxRs_AlphaOp1 = 51,
    GxRs_AlphaOp2 = 52,
    GxRs_AlphaOp3 = 53,
    GxRs_AlphaOp4 = 54,
    GxRs_AlphaOp5 = 55,
    GxRs_AlphaOp6 = 56,
    GxRs_AlphaOp7 = 57,
    GxRs_TexGen0 = 58,
    GxRs_TexGen1 = 59,
    GxRs_TexGen2 = 60,
    GxRs_TexGen3 = 61,
    GxRs_TexGen4 = 62,
    GxRs_TexGen5 = 63,
    GxRs_TexGen6 = 64,
    GxRs_TexGen7 = 65,
    GxRs_TextureShader0 = 66,
    GxRs_TextureShader1 = 67,
    GxRs_TextureShader2 = 68,
    GxRs_TextureShader3 = 69,
    GxRs_TextureShader4 = 70,
    GxRs_TextureShader5 = 71,
    GxRs_TextureShader6 = 72,
    GxRs_TextureShader7 = 73,
    GxRs_TextureCoord0 = 74,
    GxRs_TextureCoord1 = 75,
    GxRs_TextureCoord2 = 76,
    GxRs_TextureCoord3 = 77,
    GxRs_TextureCoord4 = 78,
    GxRs_TextureCoord5 = 79,
    GxRs_TextureCoord6 = 80,
    GxRs_TextureCoord7 = 81,
    GxRs_VertexShader = 82,
    GxRs_HullShader = 83,
    GxRs_DomainShader = 84,
    GxRs_GeometryShader = 85,
    GxRs_PixelShader = 86,
    GxRs_PointScale = 87,
    GxRs_PointScaleAttenuation = 88,
    GxRs_PointScaleMin = 89,
    GxRs_PointScaleMax = 90,
    GxRs_PointSprite = 91,
    GxRs_LineWidth = 92,
    GxRs_BlendFactor = 93,
    GxRs_ColorMaterial = 94,
    GxRenderStates_Last = 95,
};

enum EGxShTarget {
    GxSh_Vertex = 0,
    GxSh_Hull = 1,
    GxSh_Domain = 2,
    GxSh_Geometry = 3,
    GxSh_Pixel = 4,
    GxSh_Compute = 5,
    GxShTargets_Last = 6,
};

enum EGxShaderProfile {
    GxShProfile_None = 0,
    GxShProfile_VsFf = 1,
    GxShProfile_PsFf = 2,
    GxShProfile_Vs11 = 3,
    GxShProfile_Vs20 = 4,
    GxShProfile_Vs30 = 5,
    GxShProfile_Vs40 = 6,
    GxShProfile_Vs50 = 7,
    GxShProfile_Ps11 = 8,
    GxShProfile_Ps14 = 9,
    GxShProfile_Ps20 = 10,
    GxShProfile_Ps30 = 11,
    GxShProfile_Ps40 = 12,
    GxShProfile_Ps50 = 13,
    GxShProfile_Gs40 = 14,
    GxShProfile_Gs50 = 15,
    GxShProfile_Hs50 = 16,
    GxShProfile_Ds50 = 17,
    GxShProfile_Cs40 = 18,
    GxShProfile_Cs50 = 19,
    GxShProfile_Arbvp1 = 20,
    GxShProfile_Arbvp1Cg12 = 21,
    GxShProfile_Arbfp1 = 22,
    GxShProfile_Glslv = 23,
    GxShProfile_Glslf = 24,
    GxShProfile_Opencl = 25,
    GxShProfile_Vp20 = 26,
    GxShProfile_Vp30 = 27,
    GxShProfile_Vp40 = 28,
    GxShProfile_Gp4vp = 29,
    GxShProfile_Fp10 = 30,
    GxShProfile_Fp20 = 31,
    GxShProfile_Fp25 = 32,
    GxShProfile_Fp30 = 33,
    GxShProfile_Fp40 = 34,
    GxShProfile_Gp4fp = 35,
    GxShProfile_Gp4gp = 36,
    GxShProfile_Cuda = 37,
    GxShProfiles_Last = 38,
};

typedef intptr_t (*GxWindowProc)(void* window, uint32_t message, uintptr_t wparam, intptr_t lparam);

enum EGxuDrawListCategory {
    GxuCat_0 = 0,
    GxuCat_1 = 1,
    GxuCat_2 = 2,
    GxuCats_Last = 3,
};

enum EGxuDrawListSorting {
    GxuSort_B2F = 0,
    GxuSort_VtxStream = 1,
    GxuSorts_Last = 2,
};

#endif
