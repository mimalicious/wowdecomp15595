#ifndef ENGINE_SOURCE_GX_CGXDEVICED3D_CGXDEVICED3D_H
#define ENGINE_SOURCE_GX_CGXDEVICED3D_CGXDEVICED3D_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include <d3d9.h>
#include "ThirdParty/nvapi/nvapi.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Storm/H/stpl.h"

class CGxDeviceD3d : public CGxDevice {
    public:
    enum EDeviceState {
        Ds_SrcBlend = 0,
        Ds_DstBlend = 1,
        Ds_TssMagFilter0 = 2,
        Ds_TssMagFilter1 = 3,
        Ds_TssMagFilter2 = 4,
        Ds_TssMagFilter3 = 5,
        Ds_TssMagFilter4 = 6,
        Ds_TssMagFilter5 = 7,
        Ds_TssMagFilter6 = 8,
        Ds_TssMagFilter7 = 9,
        Ds_TssMagFilter8 = 10,
        Ds_TssMagFilter9 = 11,
        Ds_TssMagFilter10 = 12,
        Ds_TssMagFilter11 = 13,
        Ds_TssMagFilter12 = 14,
        Ds_TssMagFilter13 = 15,
        Ds_TssMagFilter14 = 16,
        Ds_TssMagFilter15 = 17,
        Ds_TssMinFilter0 = 18,
        Ds_TssMinFilter1 = 19,
        Ds_TssMinFilter2 = 20,
        Ds_TssMinFilter3 = 21,
        Ds_TssMinFilter4 = 22,
        Ds_TssMinFilter5 = 23,
        Ds_TssMinFilter6 = 24,
        Ds_TssMinFilter7 = 25,
        Ds_TssMinFilter8 = 26,
        Ds_TssMinFilter9 = 27,
        Ds_TssMinFilter10 = 28,
        Ds_TssMinFilter11 = 29,
        Ds_TssMinFilter12 = 30,
        Ds_TssMinFilter13 = 31,
        Ds_TssMinFilter14 = 32,
        Ds_TssMinFilter15 = 33,
        Ds_TssMipFilter0 = 34,
        Ds_TssMipFilter1 = 35,
        Ds_TssMipFilter2 = 36,
        Ds_TssMipFilter3 = 37,
        Ds_TssMipFilter4 = 38,
        Ds_TssMipFilter5 = 39,
        Ds_TssMipFilter6 = 40,
        Ds_TssMipFilter7 = 41,
        Ds_TssMipFilter8 = 42,
        Ds_TssMipFilter9 = 43,
        Ds_TssMipFilter10 = 44,
        Ds_TssMipFilter11 = 45,
        Ds_TssMipFilter12 = 46,
        Ds_TssMipFilter13 = 47,
        Ds_TssMipFilter14 = 48,
        Ds_TssMipFilter15 = 49,
        Ds_TssWrapU0 = 50,
        Ds_TssWrapU1 = 51,
        Ds_TssWrapU2 = 52,
        Ds_TssWrapU3 = 53,
        Ds_TssWrapU4 = 54,
        Ds_TssWrapU5 = 55,
        Ds_TssWrapU6 = 56,
        Ds_TssWrapU7 = 57,
        Ds_TssWrapU8 = 58,
        Ds_TssWrapU9 = 59,
        Ds_TssWrapU10 = 60,
        Ds_TssWrapU11 = 61,
        Ds_TssWrapU12 = 62,
        Ds_TssWrapU13 = 63,
        Ds_TssWrapU14 = 64,
        Ds_TssWrapU15 = 65,
        Ds_TssWrapV0 = 66,
        Ds_TssWrapV1 = 67,
        Ds_TssWrapV2 = 68,
        Ds_TssWrapV3 = 69,
        Ds_TssWrapV4 = 70,
        Ds_TssWrapV5 = 71,
        Ds_TssWrapV6 = 72,
        Ds_TssWrapV7 = 73,
        Ds_TssWrapV8 = 74,
        Ds_TssWrapV9 = 75,
        Ds_TssWrapV10 = 76,
        Ds_TssWrapV11 = 77,
        Ds_TssWrapV12 = 78,
        Ds_TssWrapV13 = 79,
        Ds_TssWrapV14 = 80,
        Ds_TssWrapV15 = 81,
        Ds_TssWrapW0 = 82,
        Ds_TssWrapW1 = 83,
        Ds_TssWrapW2 = 84,
        Ds_TssWrapW3 = 85,
        Ds_TssWrapW4 = 86,
        Ds_TssWrapW5 = 87,
        Ds_TssWrapW6 = 88,
        Ds_TssWrapW7 = 89,
        Ds_TssWrapW8 = 90,
        Ds_TssWrapW9 = 91,
        Ds_TssWrapW10 = 92,
        Ds_TssWrapW11 = 93,
        Ds_TssWrapW12 = 94,
        Ds_TssWrapW13 = 95,
        Ds_TssWrapW14 = 96,
        Ds_TssWrapW15 = 97,
        Ds_VtxWrapU0 = 98,
        Ds_VtxWrapU1 = 99,
        Ds_VtxWrapU2 = 100,
        Ds_VtxWrapU3 = 101,
        Ds_VtxWrapV0 = 102,
        Ds_VtxWrapV1 = 103,
        Ds_VtxWrapV2 = 104,
        Ds_VtxWrapV3 = 105,
        Ds_VtxWrapW0 = 106,
        Ds_VtxWrapW1 = 107,
        Ds_VtxWrapW2 = 108,
        Ds_VtxWrapW3 = 109,
        Ds_TssTexTransformFlags0 = 110,
        Ds_TssTexTransformFlags1 = 111,
        Ds_TssTexTransformFlags2 = 112,
        Ds_TssTexTransformFlags3 = 113,
        Ds_TssTexTransformFlags4 = 114,
        Ds_TssTexTransformFlags5 = 115,
        Ds_TssTexTransformFlags6 = 116,
        Ds_TssTexTransformFlags7 = 117,
        Ds_TssMaxAnisotropy0 = 118,
        Ds_TssMaxAnisotropy1 = 119,
        Ds_TssMaxAnisotropy2 = 120,
        Ds_TssMaxAnisotropy3 = 121,
        Ds_TssMaxAnisotropy4 = 122,
        Ds_TssMaxAnisotropy5 = 123,
        Ds_TssMaxAnisotropy6 = 124,
        Ds_TssMaxAnisotropy7 = 125,
        Ds_TssMaxAnisotropy8 = 126,
        Ds_TssMaxAnisotropy9 = 127,
        Ds_TssMaxAnisotropy10 = 128,
        Ds_TssMaxAnisotropy11 = 129,
        Ds_TssMaxAnisotropy12 = 130,
        Ds_TssMaxAnisotropy13 = 131,
        Ds_TssMaxAnisotropy14 = 132,
        Ds_TssMaxAnisotropy15 = 133,
        Ds_TssTexCoordIndex0 = 134,
        Ds_TssTexCoordIndex1 = 135,
        Ds_TssTexCoordIndex2 = 136,
        Ds_TssTexCoordIndex3 = 137,
        Ds_TssTexCoordIndex4 = 138,
        Ds_TssTexCoordIndex5 = 139,
        Ds_TssTexCoordIndex6 = 140,
        Ds_TssTexCoordIndex7 = 141,
        Ds_TssColorOp0 = 142,
        Ds_TssColorOp1 = 143,
        Ds_TssColorOp2 = 144,
        Ds_TssColorOp3 = 145,
        Ds_TssColorOp4 = 146,
        Ds_TssColorOp5 = 147,
        Ds_TssColorOp6 = 148,
        Ds_TssColorOp7 = 149,
        Ds_TssAlphaOp0 = 150,
        Ds_TssAlphaOp1 = 151,
        Ds_TssAlphaOp2 = 152,
        Ds_TssAlphaOp3 = 153,
        Ds_TssAlphaOp4 = 154,
        Ds_TssAlphaOp5 = 155,
        Ds_TssAlphaOp6 = 156,
        Ds_TssAlphaOp7 = 157,
        Ds_TssColorArg10 = 158,
        Ds_TssColorArg11 = 159,
        Ds_TssColorArg12 = 160,
        Ds_TssColorArg13 = 161,
        Ds_TssColorArg14 = 162,
        Ds_TssColorArg15 = 163,
        Ds_TssColorArg16 = 164,
        Ds_TssColorArg17 = 165,
        Ds_TssColorArg20 = 166,
        Ds_TssColorArg21 = 167,
        Ds_TssColorArg22 = 168,
        Ds_TssColorArg23 = 169,
        Ds_TssColorArg24 = 170,
        Ds_TssColorArg25 = 171,
        Ds_TssColorArg26 = 172,
        Ds_TssColorArg27 = 173,
        Ds_TssAlphaArg10 = 174,
        Ds_TssAlphaArg11 = 175,
        Ds_TssAlphaArg12 = 176,
        Ds_TssAlphaArg13 = 177,
        Ds_TssAlphaArg14 = 178,
        Ds_TssAlphaArg15 = 179,
        Ds_TssAlphaArg16 = 180,
        Ds_TssAlphaArg17 = 181,
        Ds_TssAlphaArg20 = 182,
        Ds_TssAlphaArg21 = 183,
        Ds_TssAlphaArg22 = 184,
        Ds_TssAlphaArg23 = 185,
        Ds_TssAlphaArg24 = 186,
        Ds_TssAlphaArg25 = 187,
        Ds_TssAlphaArg26 = 188,
        Ds_TssAlphaArg27 = 189,
        Ds_AmbientMaterialSource = 190,
        Ds_DiffuseMaterialSource = 191,
        Ds_SpecularMaterialSource = 192,
        Ds_EmissiveMaterialSource = 193,
        Ds_Ambient = 194,
        Ds_AlphaBlendEnable = 195,
        Ds_AlphaTestEnable = 196,
        Ds_AlphaRef = 197,
        Ds_FogEnable = 198,
        Ds_ZWriteEnable = 199,
        Ds_ColorWriteEnable = 200,
        Ds_Lighting = 201,
        Ds_SpecularEnable = 202,
        Ds_CullMode = 203,
        Ds_ClipPlaneEnable = 204,
        Ds_ZFunc = 205,
        Ds_PointScaleA = 206,
        Ds_PointScaleB = 207,
        Ds_PointScaleC = 208,
        Ds_PointScaleEnable = 209,
        DeviceStates_Last = 210,
    };

    /* 27EC */ HWND m_hwnd;
    /* 27F0 */ ATOM m_hwndClass;
    /* 27F2 */ uint16_t unk_27B6;
    /* 27F4 */ int32_t m_ownhwnd;
    /* 27F8 */ HINSTANCE m_d3dLib;
    /* 27FC */ LPDIRECT3D9 m_d3d;
    /* 2800 */ LPDIRECT3DDEVICE9 m_d3dDevice;
    /* 2804 */ D3DCAPS9 m_d3dCaps;
    /* 2934 */ int32_t m_d3dIsHwDevice;
    /* 2938 */ int32_t m_d3dNVAPI;
    /* 293C */ uint32_t m_d3dStereoEnabled;
    /* 2940 */ uint32_t m_d3dStereoRestore;
    /* 2944 */ void* m_d3dStereoHandle;
    /* 2948 */ float m_d3dStereoConvergence;
    /* 294C */ float m_d3dStereoSeparation;
    /* 2950 */ int32_t m_d3dStereoDirty;
    /* 2954 */ LPDIRECT3DVERTEXDECLARATION9 m_d3dVertexDecl [22];
    /* 29AC */ D3DDISPLAYMODE m_desktopDisplayMode;
    /* 29BC */ int32_t m_inScene;
    /* 29C0 */ int32_t m_ambientOnlyMode;
    /* 29C4 */ D3DFORMAT m_devDepthFormat;
    /* 29C8 */ D3DFORMAT m_devAdapterFormat;
    /* 29CC */ LPDIRECT3DSURFACE9 unk_29CC;
    /* 29D0 */ LPDIRECT3DSURFACE9 m_defColorSurface;
    /* 29D4 */ LPDIRECT3DSURFACE9 m_defDepthSurface;
    /* 29D8 */ NVDX_ObjectHandle m_nvDepthSurfaceHandle;
    /* 29DC */ int32_t unk_29DC;
    /* 29E0 */ int32_t unk_29E0;
    /* 29E4 */ LPDIRECT3DSURFACE9 m_readbackSurface;
    /* 29E8 */ LPDIRECT3DQUERY9 m_eventQuery;
    /* 29EC */ int32_t m_hwCursorDirty;
    /* 29F0 */ LPDIRECT3DTEXTURE9 m_hwCursorTexture;
    /* 29F4 */ LPDIRECT3DSURFACE9 m_hwCursorBitmap;
    /* 29F8 */ uint32_t unk_29B4;
    /* 29FC */ LPDIRECT3DVERTEXDECLARATION9 m_d3dCurrentVertexDecl;
    /* 2A00 */ LPDIRECT3DINDEXBUFFER9 m_d3dCurrentIndexBuf;
    /* 2A04 */ LPDIRECT3DVERTEXBUFFER9 m_d3dVertexStreamBuf [2];
    /* 2A0C */ uint32_t m_d3dVertexStreamStride [2];
    /* 2A14 */ LPDIRECT3DTEXTURE9 m_scratchTextures [24];
    /* 2A74 */ TSExplicitList<CGxTex, offsetof(CGxTex, m_link2)> m_texCacheList;
    /* 2A80 */ TSExplicitList<CGxTex, offsetof(CGxTex, m_link)> m_texDestroyList;
    /* 2A8C */ uint32_t m_texCacheUsed;
    /* 2A90 */ uint32_t m_texCacheSize;
    /* 2A94 */ uint32_t m_deviceStates [210];
    /* 2DDC */ bool m_texEnabled [8];

    virtual void ITexMarkAsUpdated(CGxTex* texId);
    virtual void IRsSendToHw(EGxRenderState which);
    virtual void ICursorCreate(const CGxFormat& format);
    virtual void ICursorDestroy();
    virtual ~CGxDeviceD3d();
    virtual int32_t DeviceCreate(GxWindowProc windowProc, const CGxFormat& format);
    virtual int32_t DeviceCreate(void* window, const CGxFormat& format);
    virtual void DeviceDestroy();
    virtual void DeviceEvictResources();
    virtual int32_t DeviceSetFormat(const CGxFormat& format);
    virtual void DeviceSetBaseMipLevel(uint32_t level);
    virtual void DeviceSetGamma(float gamma);
    virtual void DeviceSetGamma(const CGxGammaRamp& ramp);
    virtual void* DeviceWindow();
    virtual void DeviceReadPixels(CiRect& rect, TSGrowableArray<CImVector>& pixels);
    virtual void DeviceReadDepths(CiRect& rect, TSGrowableArray<float>& depths);
    virtual int32_t DeviceWM(EGxWM wm, uintptr_t param1, uintptr_t param2);
    virtual void DeviceSetRenderTarget(EGxBuffer buffer, CGxTex* texture, uint32_t plane);
    virtual void DeviceResolveDepthBuffer(CGxTex* texture);
    virtual void DeviceOverride(EGxOverride override, uint32_t value);
    virtual void CapsWindowSize(CRect& rect);
    virtual void CapsWindowSizeInScreenCoords(CRect& rect);
    virtual void LogCrashInfo(char* buffer, uint32_t size);
    virtual void ScenePresent(void* window);
    virtual void SceneClear(uint32_t mask, CImVector color);
    virtual void SceneClearToViewport(uint32_t mask, CImVector color);
    virtual int32_t XformIsValidViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
    virtual void XformSetProjection(const C44Matrix& matrix);
    virtual void XformSetView(const C44Matrix& matrix);
    virtual void DrawNonIndexed(const CGxBatch& batch);
    virtual void Draw(const CGxBatch& batch);
    virtual void DrawInstancedNonIndexed(const CGxBatch& batch);
    virtual void DrawInstanced(const CGxBatch& batch);
    virtual void SetInstanceCount(uint32_t count);
    virtual void ResetInstancing();
    virtual void PoolSizeSet(CGxPool* pool, uint32_t size);
    virtual void PoolDestroy(CGxPool* pool);
    virtual char* BufLock(CGxBuf* buf);
    virtual int32_t BufUnlock(CGxBuf* buf, uint32_t size);
    virtual void BufData(CGxBuf* buf, const void* data, uint32_t size, uint32_t offset);
    virtual void TexDestroy(CGxTex* texId);
    virtual int32_t TexCopy(CGxTex* source, CGxTex* dest, const C2iVector& destPos, const C2iVector& sourcePos, const C2iVector& size, uint32_t level, uint32_t plane);
    virtual int32_t TexStretch(CGxTex* source, CGxTex* dest, const CiRect* destRect, const CiRect* sourceRect, uint32_t level, uint32_t face);
    virtual void TexSetCacheSize(int32_t size);
    virtual void QueryCreate(CGxQuery*& query, EGxQueryType type);
    virtual void QueryDestroy(CGxQuery*& query);
    virtual int32_t QueryBegin(CGxQuery* query);
    virtual int32_t QueryEnd(CGxQuery* query);
    virtual int32_t QueryGetParam(CGxQuery* query, EGxQueryParam param, uint32_t* data);
    virtual int32_t QueryGetData(CGxQuery* query, uint32_t* data);
    virtual void IShaderCreate(CGxShaderPermute* permute);
    virtual void IShaderDestroy(CGxShaderPermute* permute);
    virtual void ICursorDraw();
    virtual void CursorSetVisible(int32_t visible);
    virtual void CursorUnlock(uint32_t x, uint32_t y);
    virtual int32_t ISetCursor();
    virtual void StereoSetConvergence(float convergence);
    virtual float StereoGetConvergence();
    virtual void StereoSetSeparation(float separation);
    virtual float StereoGetSeparation();
    virtual bool StereoEnabled();
    virtual void DebugGroupBegin(const char* name, char* buffer);
    virtual void DebugGroupEnd();
    virtual void DebugEvent(const char* name, char* buffer);
    virtual void DeviceCopyTex(CGxTex* source, uint32_t sourcePlane, CGxTex* dest, uint32_t destPlane);

    static const D3DFORMAT s_GxFormatToD3dFormat[CGxFormat::Formats_Last];
    static const D3DFORMAT s_GxTexFmtToD3dFmt[GxTexFormats_Last];
    static EGxTexFormat s_GxTexFormatToUse[GxTexFormats_Last];
    static const D3DCUBEMAP_FACES s_faceTypes[6];
    static const D3DPRIMITIVETYPE s_primitiveConversion[5];
    static const D3DBLEND s_srcBlend[13];
    static const D3DBLEND s_dstBlend[13];
    static const D3DCMPFUNC s_cmpFunc[4];
    static const D3DCULL s_cullMode[3];
    static D3DTEXTUREFILTERTYPE s_filterModes[GxTexFilters_Last][3];
    static const D3DTEXTUREADDRESS s_wrapModes[3];
    static const uint32_t s_gxAttribToD3dAttribSize[7];
    static D3DDECLTYPE s_gxAttribToD3dAttribType[7];
    static const D3DDECLUSAGE s_gxAttribToD3dAttribUsage[GxVAs_Last];
    static const uint32_t s_gxAttribToD3dAttribUsageIndex[GxVAs_Last];

    static int32_t ILoadD3dLib(HINSTANCE& d3dLib, LPDIRECT3D9& d3d);
    static void IUnloadD3dLib(HINSTANCE& d3dLib, LPDIRECT3D9& d3d);

    CGxDeviceD3d();
    void DsSet(EDeviceState state, uint32_t value);
    int32_t ICheckDepthStencilFormat(D3DFORMAT format);
    void ICreateReadbackSurface();
    void IBindPixelShader(CGxShaderPermute* permute);
    void IBindVertexShader(CGxShaderPermute* permute);
    char* IBufLock(CGxBuf* buf);
    void IBufUnlock(CGxBuf* buf);
    int32_t CreatePoolAPI(CGxPool* pool);
    int32_t ICreateD3d(uint32_t monitor);
    int32_t ICreateD3dDevice(const CGxFormat& format);
    LPDIRECT3DINDEXBUFFER9 ICreateD3dIB(EGxPoolUsage usage, uint32_t size);
    LPDIRECT3DVERTEXBUFFER9 ICreateD3dVB(EGxPoolUsage usage, uint32_t size);
    void IDestroyD3d();
    void IReleaseD3dPool(CGxPool* pool);
    void IReleaseD3dPools(int32_t freePools);
    void IReleaseD3dQueries();
    void IReleaseD3dResources(int32_t freeTextures);
    void IReleaseD3dShaders(int32_t freeShaders);
    void IReleaseD3dVertexDecl();
    void ISceneBegin();
    void IRetrieveNvDepthHandle();
    void ISceneEnd();
    void ISetAlphaOp(uint32_t tmu, uint32_t op);
    void ISetCaps(const CGxFormat& format);
    void ISetColorOp(uint32_t tmu, uint32_t op);
    void ISetPresentParms(uint32_t adapter, D3DDEVTYPE deviceType, D3DPRESENT_PARAMETERS& d3dpp, const CGxFormat& format);
    uint32_t IGetAdapterIndex(uint32_t monitor);
    LPDIRECT3DSURFACE9 ITexGetSurface(CGxTex* texId, uint32_t level, uint32_t face);
    void ISetMaterial(CImVector diffuse, CImVector emissive, CImVector specular, float specularExp);
    void ISetTexCoord(uint32_t tmu, uint32_t coord);
    void ISetTexCoordIndex(uint32_t tmu, int32_t texGen, uint32_t coord);
    void ISetTexGen(uint32_t tmu, int32_t texGen);
    void ISetTexture(uint32_t tmu, CGxTex* texId);
    LPDIRECT3DVERTEXDECLARATION9 IGetVertexDecl();
    void ISetIndexBuffer();
    void ISetVertexDecl();
    void ISetVertexDeclInstanced();
    void ISetVertexStream();
    void ISetVertexStreams();
    void ISetVertexTexture(uint32_t tmu, CGxTex* texId);
    void ISetWindowFocus(int32_t focus);
    void IShaderConstantsFlush();
    void IStateSetD3DDefaults();
    void IStateSync();
    void IStateSyncClipPlanes();
    void IStateSyncEnables();
    void IStateSyncLights();
    void IStateSyncMaterial();
    void IStateSyncScissorRect();
    void IStateSyncXforms();
    void ITexAcquire(CGxTex* texId);
    int32_t ITexAllocD3dTexture(CGxTex* texId);
    void ITexCreate(CGxTex* texId);
    int32_t ITexEvictFor(CGxTex* texId, uint32_t size);
    LPDIRECT3DTEXTURE9 ITexGetScratch(EGxTexFormat format);
    void ITexLinkBound();
    void ITexReleaseD3dTexture(CGxTex* texId);
    void ITexTakeD3dTexture(CGxTex* dest, CGxTex* source);
    void ITexTouch(CGxTex* texId);
    void ITexUnlinkBound();
    void ITexUploadAll(CGxTex* texId);
    void ITexUploadRect(CGxTex* texId, uint32_t face, uint32_t level, const RECT* rect, const void* bits, uint32_t pitch);
    void ITexUploadBox(CGxTex* texId, uint32_t level, const D3DBOX* box, const void* bits, uint32_t pitch);
    void ITexForceRecreation(int32_t all);
    void ITexUpload(CGxTex* texId);
    void IXformSetTex(uint32_t tmu);
    void IXformSetViewport();
    void IXformSetWorld();
};

CGxDevice* CreateCGxDeviceD3d();

#endif
