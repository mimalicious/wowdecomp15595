#ifndef ENGINE_SOURCE_GX_CGXDEVICEOPENGL_CGXDEVICEOPENGL_H
#define ENGINE_SOURCE_GX_CGXDEVICEOPENGL_CGXDEVICEOPENGL_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxDeviceOpenGL/GlExt.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

class CGxShaderPermute;

class CGxDeviceOpenGl : public CGxDevice {
    public:
    enum EDeviceState {
        Ds_DepthMask = 0,
        Ds_ColorMask = 1,
        Ds_ActiveTexture = 2,
        Ds_TexTarget0 = 3,
        Ds_TexGenS0 = 19,
        Ds_TexGenT0 = 27,
        Ds_TexGenR0 = 35,
        Ds_TexGenQ0 = 43,
        Ds_ColorOp0 = 51,
        Ds_AlphaOp0 = 59,
        Ds_VertexArray0 = 67,
        Ds_TexCoordArray0 = 73,
        Ds_Unk81 = 81,
        Ds_PolygonOffsetFill = 82,
        Ds_PolygonOffset = 83,
        Ds_Blend = 84,
        Ds_AlphaTest = 85,
        Ds_RegisterCombiners = 86,
        Ds_PerStageConstants = 87,
        Ds_TextureShader = 88,
        Ds_FragmentProgram = 89,
        Ds_VertexProgram = 90,
        Ds_VertexProgramNV = 91,
        Ds_MatrixMode = 92,
        Ds_BlendFunc = 93,
        Ds_ArrayBuffer = 94,
        Ds_ElementArrayBuffer = 95,
        Ds_CullFace = 96,
        Ds_ClipPlanes = 97,
        DeviceStates_Last = 98,
    };

    /* 27B0 */ uint32_t m_deviceStates [98];
    /* 2938 */ uint32_t unk_2938;
    /* 293C */ uint32_t m_materialDirty;
    /* 2940 */ GLenum m_sphereMapMode;
    /* 2944 */ int32_t m_renderTargetMode;
    /* 2948 */ const GLubyte* m_glVendor;
    /* 294C */ const GLubyte* m_glRenderer;
    /* 2950 */ const GLubyte* m_glVersion;
    /* 2954 */ HWND m_hwnd;
    /* 2958 */ int32_t m_ownhwnd;
    /* 295C */ ATOM m_hwndClass;
    /* 295E */ uint16_t unk_295E;
    /* 2960 */ HDC m_hdc;
    /* 2964 */ HGLRC m_hglrc;
    /* 2968 */ HICON m_cursor;
    /* 296C */ HPBUFFERARB m_pbuffer;
    /* 2970 */ HDC m_pbufferDC;
    /* 2974 */ HGLRC m_pbufferRC;
    /* 2978 */ uintptr_t m_indexPtr;
    /* 297C */ int32_t m_viewDirty;
    /* 2980 */ uint32_t unk_2980;
    /* 2984 */ int32_t m_useVboBroker [2][3];
    /* 299C */ int32_t m_useVbo [2][3];
    /* 29B4 */ GLuint m_boundTexture;
    /* 29B8 */ uint32_t m_attribDirty;
    /* 29BC */ EGxVertexBufferFormat m_curVertexFormat;
    /* 29C0 */ CGxBuf* m_curVertexBuf;
    /* 29C4 */ GLuint m_fbo;
    /* 29C8 */ GLuint m_boundFbo;
    /* 29CC */ int32_t m_ambientOnlyMode;

    virtual void ITexMarkAsUpdated(CGxTex* texId);
    virtual void IRsSendToHw(EGxRenderState which);
    virtual void ICursorCreate(const CGxFormat& format);
    virtual void ICursorDestroy();
    virtual ~CGxDeviceOpenGl();
    virtual int32_t DeviceCreate(GxWindowProc windowProc, const CGxFormat& format);
    virtual int32_t DeviceCreate(void* window, const CGxFormat& format);
    virtual void DeviceDestroy();
    virtual int32_t DeviceSetFormat(const CGxFormat& format);
    virtual void DeviceSetBaseMipLevel(uint32_t level);
    virtual void DeviceSetGamma(float gamma);
    virtual void DeviceSetGamma(const CGxGammaRamp& ramp);
    virtual void* DeviceWindow();
    virtual void DeviceReadPixels(CiRect& rect, TSGrowableArray<CImVector>& pixels);
    virtual void DeviceReadDepths(CiRect& rect, TSGrowableArray<float>& depths);
    virtual int32_t DeviceWM(EGxWM wm, uintptr_t param1, uintptr_t param2);
    virtual void DeviceSetRenderTarget(EGxBuffer buffer, CGxTex* texture, uint32_t plane);
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
    virtual void PoolSizeSet(CGxPool* pool, uint32_t size);
    virtual void PoolDestroy(CGxPool* pool);
    virtual char* BufLock(CGxBuf* buf);
    virtual int32_t BufUnlock(CGxBuf* buf, uint32_t size);
    virtual void BufData(CGxBuf* buf, const void* data, uint32_t size, uint32_t offset);
    virtual void TexDestroy(CGxTex* texId);
    virtual int32_t TexCopy(CGxTex* source, CGxTex* dest, const C2iVector& destPos, const C2iVector& sourcePos, const C2iVector& size, uint32_t level, uint32_t plane);
    virtual int32_t TexStretch(CGxTex* source, CGxTex* dest, const CiRect* destRect, const CiRect* sourceRect, uint32_t level, uint32_t face);
    virtual void QueryCreate(CGxQuery*& query, EGxQueryType type);
    virtual void QueryDestroy(CGxQuery*& query);
    virtual int32_t QueryBegin(CGxQuery* query);
    virtual int32_t QueryEnd(CGxQuery* query);
    virtual int32_t QueryGetParam(CGxQuery* query, EGxQueryParam param, uint32_t* data);
    virtual int32_t QueryGetData(CGxQuery* query, uint32_t* data);
    virtual void ShaderConstantsSet(EGxShTarget target, uint32_t index, const float* constants, uint32_t count);
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
    virtual void IHwCursorCreate();

    static void IXformGLModelView(const C44Matrix& view, C44Matrix& world);
    static int32_t SetFormatMode(CGxFormat& format);

    CGxDeviceOpenGl();
    void BindTexture(CGxTex* texId, uint32_t tmu);
    void CreatePoolAPI(CGxPool* pool);
    void DestroyPoolAPI(CGxPool* pool);
    void DeviceCreatePbuffer();
    void DeviceDestroyPbuffer();
    void DeviceDestroyPoolAPI();
    void DeviceQueryPbuffer();
    void DsInit();
    void DsSet(EDeviceState state, uint32_t value, int32_t force);
    int32_t IDevAttachGlContext(const CGxFormat& format);
    void IDevRemoveGlContext();
    void IDevSetFocus(int32_t focus, CGxFormat& format);
    void IDeviceSetRenderTargetCopy(EGxBuffer buffer, CGxTex* texture, uint32_t plane);
    void IDeviceSetRenderTargetFBO(EGxBuffer buffer, CGxTex* texture, uint32_t plane);
    void IBindPixelShader(CGxShaderPermute* permute);
    void IBindVertexShader(CGxShaderPermute* permute);
    void IQueryRelease();
    void IReleaseShaders(int32_t freeShaders);
    void IResourceTouch(const void* resource);
    void IPixelShaderPatch(char* code, uint32_t size);
    void IShaderConstantsClear();
    void IShaderConstantsReset();
    void IShaderConstantsSend(EGxShTarget target, uint32_t index, const float* constants, uint32_t count);
    void IShaderCreatePixel(CGxShaderPermute* permute);
    void IShaderCreateVertex(CGxShaderPermute* permute);
    void IShaderDestroyPixel(CGxShaderPermute* permute);
    void IShaderDestroyVertex(CGxShaderPermute* permute);
    void IShaderConstantsFlush();
    void ISetAlphaOp(uint32_t tmu, uint32_t op);
    void ISetCaps(const CGxFormat& format);
    void ISetColorOp(uint32_t tmu, uint32_t op);
    void ISetTexGen(uint32_t tmu, int32_t texGen);
    void ISetTexture(uint32_t tmu, CGxTex* texId);
    void IStateSetContextDefaults();
    void IStateSync();
    void IStateSyncClipPlanes();
    void IStateSyncEnables();
    void IStateSyncIndexPtr();
    void IStateSyncLights();
    void IStateSyncMaterial();
    void IStateSyncScissorRect();
    void IStateSyncVertexFormat();
    void IStateSyncVertexPtrs();
    void IStateSyncViewport();
    void IStateSyncXforms();
    void ITexDownload(CGxTex* texId, uint32_t width, uint32_t height, uint32_t face, uint32_t mip, uint32_t baseMip, uint32_t stride, const void* texels);
    void ITexForceRecreation();
    void ITexMarkAsUpdated(CGxTex* texId, uint32_t tmu);
    void ITexSetFlags(CGxTex* texId);
    void IVertexShaderPatch(char* code, uint32_t size);
    void IXformSetModelView(const C44Matrix& matrix);
    void IXformSetProjection(const C44Matrix& matrix);
};

CGxDevice* CreateCGxDeviceOpenGl();

#endif
