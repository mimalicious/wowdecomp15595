#ifndef ENGINE_SOURCE_GX_CGXDEVICED3D11_CGXDEVICED3D11_H
#define ENGINE_SOURCE_GX_CGXDEVICED3D11_CGXDEVICED3D11_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/EmergencyMem.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Storm/H/stpl.h"

class CGxDeviceD3d11 : public CGxDevice {
    public:
    struct TexViews {
        /* 0000 */ uint32_t m_type;
        /* 0004 */ ID3D11ShaderResourceView* m_srv;
    };

    struct TexTargetViews : public TexViews {
        /* 0008 */ ID3D11View* m_targetView;
    };

    struct ScratchTex {
        /* 0000 */ uint32_t m_frame;
        /* 0004 */ uint32_t m_size;
        /* 0008 */ EGxTexFormat m_format;
        /* 000C */ DXGI_FORMAT m_dxgiFormat;
        /* 0010 */ ID3D11Texture2D* m_texture;
        /* 0014 */ TSLink<ScratchTex> m_link;
    };

    struct InputLayoutKey {
        /* 0000 */ uint32_t m_format;
        /* 0004 */ uint32_t m_mask;

        bool operator==(const InputLayoutKey& key);
    };

    class InputLayoutRec : public TSHashObject<InputLayoutRec, InputLayoutKey> {
        public:
        /* 001C */ ID3D11InputLayout* m_layout;
    };

    class RasterizerStateRec : public TSHashObject<RasterizerStateRec, HASHKEY_NONE> {
        public:
        /* 0018 */ ID3D11RasterizerState* m_state;
    };

    class BlendStateRec : public TSHashObject<BlendStateRec, HASHKEY_NONE> {
        public:
        /* 0018 */ ID3D11BlendState* m_state;
    };

    class DepthStateRec : public TSHashObject<DepthStateRec, HASHKEY_NONE> {
        public:
        /* 0018 */ ID3D11DepthStencilState* m_state;
    };

    class SamplerStateRec : public TSHashObject<SamplerStateRec, HASHKEY_NONE> {
        public:
        /* 0018 */ ID3D11SamplerState* m_state;
    };

    enum {
        StateDirty_Blend = 0x1,
        StateDirty_BlendApply = 0x2,
        StateDirty_Depth = 0x4,
        StateDirty_DepthApply = 0x8,
        StateDirty_Rasterizer = 0x10,
        StateDirty_RasterizerApply = 0x20,
        StateDirty_InputLayout = 0x40,
        StateDirty_InputLayoutApply = 0x80,
    };

    /* 27B0 */ int32_t m_ownhwnd;
    /* 27B4 */ HWND m_hwnd;
    /* 27B8 */ ATOM m_hwndClass;
    /* 27BA */ uint16_t unk_27BA;
    /* 27BC */ uint32_t m_featureLevel;
    /* 27C0 */ ID3D11Device* m_d3dDevice;
    /* 27C4 */ IDXGIDevice1* m_dxgiDevice;
    /* 27C8 */ IDXGIFactory1* m_dxgiFactory;
    /* 27CC */ IDXGIAdapter1* m_dxgiAdapter;
    /* 27D0 */ IDXGISwapChain* m_swapChain;
    /* 27D4 */ ID3D11DeviceContext* m_d3dContext;
    /* 27D8 */ TexTargetViews* m_backBufferViews;
    /* 27DC */ ID3D11Texture2D* m_depthTexture;
    /* 27E0 */ ID3D11DepthStencilView* m_depthView;
    /* 27E4 */ ID3D11Texture2D* m_resolveTexture;
    /* 27E8 */ ID3D11Buffer* m_constantBuffers [GxShTargets_Last];
    /* 2800 */ uint32_t m_numRenderTargets;
    /* 2804 */ ID3D11RenderTargetView* m_renderTargets [8];
    /* 2824 */ ID3D11DepthStencilView* m_curDepthView;
    /* 2828 */ EGxTexFormat m_colorTargetFormat;
    /* 282C */ ID3D11InputLayout* m_curInputLayout;
    /* 2830 */ ID3D11Buffer* m_curIndexBuffer;
    /* 2834 */ ID3D11Buffer* m_curVertexBuffers [2];
    /* 283C */ uint32_t m_vertexStrides [2];
    /* 2844 */ uint32_t m_instanceCount;
    /* 2848 */ ID3D11RasterizerState* m_curRasterizerState;
    /* 284C */ ID3D11BlendState* m_curBlendState;
    /* 2850 */ ID3D11DepthStencilState* m_curDepthState;
    /* 2854 */ EmergencyMem m_staticEmergencyMem [2];
    /* 287C */ HICON m_cursor;
    /* 2880 */ TSExplicitList<ScratchTex, offsetof(ScratchTex, m_link)> m_scratchTextures [78];
    /* 2C28 */ uint32_t m_scratchCount [GxTexFormats_Last];
    /* 2C88 */ CGxShader* m_clearVertexShader;
    /* 2C8C */ CGxShader* m_clearPixelShader;
    /* 2C90 */ CGxShader* m_passThroughVertexShader;
    /* 2C94 */ CGxShader* m_stretchRectPixelShader;
    /* 2C98 */ CGxShader* m_resolveVertexShader;
    /* 2C9C */ CGxShader* m_resolvePixelShader;
    /* 2CA0 */ uint32_t m_stateDirty;
    /* 2CA4 */ TSHashTable<InputLayoutRec, InputLayoutKey> m_inputLayouts;
    /* 2CD0 */ TSHashTable<RasterizerStateRec, HASHKEY_NONE> m_rasterizerStates;
    /* 2CFC */ TSHashTable<BlendStateRec, HASHKEY_NONE> m_blendStates;
    /* 2D28 */ TSHashTable<DepthStateRec, HASHKEY_NONE> m_depthStates;
    /* 2D54 */ TSHashTable<SamplerStateRec, HASHKEY_NONE> m_samplerStates;

    virtual void ITexMarkAsUpdated(CGxTex* texId);
    virtual void IRsSendToHw(EGxRenderState which);
    virtual void ICursorCreate(const CGxFormat& format);
    virtual void ICursorDestroy();
    virtual ~CGxDeviceD3d11();
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
    virtual void DeviceCopyTex(CGxTex* source, uint32_t sourcePlane, CGxTex* dest, uint32_t destPlane, uint32_t level);
    virtual void DeviceOverride(EGxOverride override, uint32_t value);
    virtual void CapsWindowSize(CRect& rect);
    virtual void CapsWindowSizeInScreenCoords(CRect& rect);
    virtual void ScenePresent(void* window);
    virtual void SceneClear(uint32_t mask, CImVector color);
    virtual void SceneClearToViewport(uint32_t mask, CImVector color);
    virtual int32_t XformIsValidViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
    virtual void XformSetProjection(const C44Matrix& matrix);
    virtual void XformSetView(const C44Matrix& matrix);
    virtual void PrimVertexPtr(CGxBuf* buf, EGxVertexBufferFormat format);
    virtual void PrimVertexPtrs(CGxBuf** bufs, const EGxVertexBufferFormat* formats, EGxVertexBufferFormat format);
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
    virtual int32_t TexResolve(CGxTex* source, CGxTex* dest);
    virtual void QueryCreate(CGxQuery*& query, EGxQueryType type);
    virtual void QueryDestroy(CGxQuery*& query);
    virtual int32_t QueryBegin(CGxQuery* query);
    virtual int32_t QueryEnd(CGxQuery* query);
    virtual int32_t QueryGetParam(CGxQuery* query, EGxQueryParam param, uint32_t* data);
    virtual int32_t QueryGetData(CGxQuery* query, uint32_t* data);
    virtual void IShaderCreate(CGxShaderPermute* permute);
    virtual void IShaderDestroy(CGxShaderPermute* permute);
    virtual void CursorSetVisible(int32_t visible);
    virtual void CursorUnlock(uint32_t x, uint32_t y);
    virtual void StereoSetConvergence(float convergence);
    virtual float StereoGetConvergence();
    virtual void StereoSetSeparation(float separation);
    virtual float StereoGetSeparation();
    virtual bool StereoEnabled();
    virtual void DebugGroupBegin(const char* name, char* buffer);
    virtual void DebugGroupEnd();
    virtual void DebugEvent(const char* name, char* buffer);
    virtual void ICheckResult(HRESULT result);

    static const DXGI_FORMAT s_GxFormatToDxgiFormat[CGxFormat::Formats_Last];
    static const DXGI_FORMAT s_GxFormatToDxgiTypeless[CGxFormat::Formats_Last];
    static const D3D11_QUERY s_queryTypes[1];
    static const uint32_t s_maxIndex[6];
    static const uint32_t s_texMaxSize2d[6];
    static const uint32_t s_texMaxSizeCube[6];
    static const uint32_t s_texMaxSizeVolume[6];
    static const uint32_t s_maxAnisotropy[6];
    static const uint32_t s_numRenderTargets[6];
    static const DXGI_FORMAT s_gxAttribToDxgiFormat[8];
    static const uint32_t s_gxAttribToSemanticIndex[GxVAs_Last];
    static const char* s_gxAttribToSemanticName[GxVAs_Last];
    static const EGxTexFormat s_GxTexFormatToUse[GxTexFormats_Last];
    static const DXGI_FORMAT s_GxTexFormatToViewFormat[GxTexFormats_Last];
    static const DXGI_FORMAT s_GxTexFormatToDxgiFormat[GxTexFormats_Last];
    static const uint32_t s_texFormatResolvable[GxTexFormats_Last];
    static const DXGI_FORMAT s_GxTexFormatToSrvFormat[GxTexFormats_Last];
    static const D3D11_BLEND s_srcBlend[13];
    static const D3D11_BLEND s_srcBlendAlpha[13];
    static const D3D11_BLEND s_dstBlend[13];
    static const D3D11_BLEND s_dstBlendAlpha[13];
    static const D3D11_COMPARISON_FUNC s_cmpFunc[4];
    static const D3D11_FILTER s_filterModes[GxTexFilters_Last];
    static const D3D11_FILTER s_compareFilterModes[GxTexFilters_Last];
    static const D3D11_TEXTURE_ADDRESS_MODE s_wrapModes[3];
    static const D3D11_PRIMITIVE_TOPOLOGY s_primitiveTopology[5];
    static const D3D11_PRIMITIVE_TOPOLOGY s_patchTopology[5];

    static int32_t ILoadD3dLib();
    static void IUnloadD3dLib();
    static IDXGIOutput* IGetOutputForMonitor(IDXGIAdapter* adapter, uint32_t monitor);
    static void ITexCalcOffset(EGxTexFormat format, uint32_t pitch, uint32_t height, const RECT& rect, uint32_t& offset, uint32_t& size);
    static void ITexRectToBox(EGxTexFormat format, const RECT& rect, D3D11_BOX& box);
    static void ITexReleaseD3dTexture(CGxTex* texId);

    CGxDeviceD3d11();
    void ISetCaps(const CGxFormat& format);
    uint32_t IFeatureLevelIndex(D3D_FEATURE_LEVEL featureLevel);
    int32_t IQueryDxgi();
    int32_t ICreateTargets(const C2iVector& size);
    void IReleaseTargets();
    void ISetWindowFocus(int32_t focus);
    int32_t CreatePoolAPI(CGxPool* pool);
    char* IBufLock(CGxBuf* buf);
    void IBufUnlock(CGxBuf* buf, uint32_t size);
    void IQueryCreateApi(CGxQuery* query);
    int32_t ICreateD3dDevice(const CGxFormat& format);
    void IReleaseD3dPools(int32_t freePools);
    void IReleaseD3dQueries();
    void IReleaseD3dShaders(int32_t freeShaders);
    void IDestroyD3d();
    int32_t ITexCreate(CGxTex* texId);
    void ITexScratchClear();
    void ITexScratchRelease(ScratchTex* scratch);
    void ITexScratchAlloc(EGxTexFormat format, uint32_t count);
    ScratchTex* ITexScratchGet(uint32_t frame, uint32_t size, EGxTexFormat format, int32_t& reused);
    void ITexScratchDestroy();
    void ITexUploadRect(uint32_t size, CGxTex* texId, const void* bits, uint32_t pitch, uint32_t height, const RECT& rect, uint32_t plane, uint32_t subresource);
    void ITexUpload(CGxTex* texId);
    void ITexForceRecreation(int32_t all);
    void IStateReset();
    void ISetIndexBuffer();
    void ISetVertexBuffer();
    void ISetVertexBuffers();
    void IStateSyncScissorRect();
    void IUpdateInputLayout();
    void ISetInputLayout();
    void ISetInputLayoutInstanced();
    void IStateSyncRasterizer();
    void IStateSyncBlend();
    void IStateSyncDepth();
    ID3D11SamplerState* IGetSamplerState(CGxTexFlags flags);
    void ISetVertexTexture(uint32_t slot, CGxTex* texId);
    void ISetDomainTexture(uint32_t slot, CGxTex* texId);
    void ISetTexture(uint32_t slot, CGxTex* texId);
    void IStateSync();
    void IXformSetViewport();
    void IBindVertexShader(CGxShaderPermute* permute);
    void IBindHullShader(CGxShaderPermute* permute);
    void IBindDomainShader(CGxShaderPermute* permute);
    void IBindGeometryShader(CGxShaderPermute* permute);
    void IBindPixelShader(CGxShaderPermute* permute);
    void IShaderConstantsFlush();
};

CGxDevice* CreateCGxDeviceD3d11();

#endif
