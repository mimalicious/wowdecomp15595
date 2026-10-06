#ifndef ENGINE_SOURCE_GX_CGXDEVICE_CGXDEVICE_H
#define ENGINE_SOURCE_GX_CGXDEVICE_CGXDEVICE_H

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxDevice/EmergencyMem.h"
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/CGxFormat.h"
#include "Engine/Source/Gx/CGxGammaRamp.h"
#include "Engine/Source/Gx/CGxLight.h"
#include "Engine/Source/Gx/CGxMatrixStack.h"
#include "Engine/Source/Gx/CGxShaderPermute.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/Types.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CBoundingBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "Engine/Source/Tempest/CRect.h"

struct CGxAppRenderState;
struct CGxBuf;
struct CGxPool;
struct tagRECT;
class CGxQuery;
class CGxStateBom;
class CGxTex;
class CImVector;

class ShadowConstants {
    public:
    /* 0000 */ uint32_t dirtyMin;
    /* 0004 */ uint32_t dirtyMax;
    /* 0008 */ float constants[256 * 4];
};

class CGxStateBom {
    public:
    /* 0000 */ union {
        int32_t i[3];
        uint32_t u[3];
        float f[3];
        void* p;
    } m_data;
    /* 000C */ int32_t m_filler;
};

struct CGxPushedRenderState {
    /* 0000 */ EGxRenderState m_which;
    /* 0004 */ CGxStateBom m_value;
    /* 0014 */ uint32_t m_stackDepth;
};

struct CGxAppRenderState {
    /* 0000 */ CGxStateBom m_value;
    /* 0010 */ uint32_t m_stackDepth;
    /* 0014 */ int32_t m_dirty;
};

typedef void (__fastcall* GxDeviceCallback)();
typedef void (*GxDisplayChangeCallback)(uint32_t value);
typedef void (*GxShaderModelCallback)(void* arg);

class CGxDevice {
    public:
    struct TextureTarget {
        /* 0000 */ CGxTex* m_texture;
        /* 0004 */ uint32_t m_plane;
        /* 0008 */ void* m_apiSpecific;
    };

    struct ShaderModelCall {
        /* 0000 */ GxShaderModelCallback m_callback;
        /* 0004 */ void* m_arg;
    };

    /* 0004 */ TSGrowableArray<CGxPushedRenderState> m_pushedStates;
    /* 0014 */ TSGrowableArray<uint32_t> m_stackOffsets;
    /* 0024 */ CRect m_defWindowRect;
    /* 0034 */ CRect m_curWindowRect;
    /* 0044 */ EGxApi m_api;
    /* 0048 */ uint32_t m_cpuFeatures;
    /* 004C */ CGxFormat m_format;
    /* 0098 */ CGxCaps m_caps;
    /* 03A0 */ uint32_t m_baseMipLevel;
    /* 03A4 */ CGxGammaRamp m_gammaRamp;
    /* 09A4 */ CGxGammaRamp m_systemGammaRamp;
    /* 0FA4 */ GxWindowProc m_windowProc;
    /* 0FA8 */ int32_t m_context;
    /* 0FAC */ int32_t m_intF5C;
    /* 0FB0 */ int32_t m_windowVisible;
    /* 0FB4 */ int32_t m_windowFocus;
    /* 0FB8 */ int32_t m_frameCount;
    /* 0FBC */ int32_t m_viewportDirty;
    /* 0FC0 */ CBoundingBox m_viewport;
    /* 0FD8 */ C44Matrix m_projection;
    /* 1018 */ C44Matrix m_projNative;
    /* 1058 */ CGxMatrixStack m_xforms [11];
    /* 1C60 */ CGxMatrixStack m_texGen [8];
    /* 2520 */ uint32_t m_clipPlaneMask;
    /* 2524 */ C4Plane m_clipPlane [6];
    /* 2584 */ int32_t m_scissorRectDirty;
    /* 2588 */ CRect m_scissorRect;
    /* 2598 */ CGxApiLight m_lights [4];
    /* 26B8 */ uint32_t m_appMasterEnables;
    /* 26BC */ uint32_t m_hwMasterEnables;
    /* 26C0 */ CGxBuf* m_bufLocked [2];
    /* 26C8 */ CGxPool* m_vertexPool;
    /* 26CC */ CGxPool* m_indexPool;
    /* 26D0 */ CGxBuf* m_streamBufs [2];
    /* 26D8 */ EGxVertexBufferFormat m_primVertexFormat;
    /* 26DC */ EGxVertexBufferFormat m_primVertexFormats [2];
    /* 26E4 */ CGxBuf* m_primIndexBuf;
    /* 26E8 */ CGxBuf* m_primVertexBufs [2];
    /* 26F0 */ EmergencyMem m_emergencyMem [2];
    /* 2718 */ TSFixedArray<CGxAppRenderState> m_appRenderStates;
    /* 2724 */ TSFixedArray<CGxStateBom> m_hwRenderStates;
    /* 2730 */ TSGrowableArray<EGxRenderState> m_dirtyStates;
    /* 2740 */ CGxDevice::TextureTarget m_textureTarget [GxBuffers_Last];
    /* 277C */ TSExplicitList<CGxQuery, offsetof(CGxQuery, m_link)> m_queryList;
    /* 2788 */ int32_t m_scrShotClick;
    /* 278C */ uint32_t m_scrShotWidth;
    /* 2790 */ uint32_t m_scrShotHeight;
    /* 2794 */ TSGrowableArray<CImVector> m_scrShotPixels;
    /* 27A4 */ int32_t m_hwCursor;
    /* 27A8 */ CGxTex* m_cursorTexture;
    /* 27AC */ float m_cursorDepth;

    virtual void ITexMarkAsUpdated(CGxTex* texId);
    virtual void IRsSendToHw(EGxRenderState which) = 0;
    virtual void ICursorCreate(const CGxFormat& format);
    virtual void ICursorDestroy();
    virtual ~CGxDevice();
    virtual int32_t DeviceCreate(GxWindowProc windowProc, const CGxFormat& format);
    virtual int32_t DeviceCreate(void* window, const CGxFormat& format);
    virtual void DeviceDestroy();
    virtual void DeviceEvictResources();
    virtual int32_t DeviceSetFormat(const CGxFormat& format);
    virtual void DeviceSetBaseMipLevel(uint32_t level);
    virtual void DeviceSetGamma(float gamma);
    virtual void DeviceSetGamma(const CGxGammaRamp& ramp);
    virtual void* DeviceWindow() = 0;
    virtual void DeviceTakeScreenShot();
    virtual void DeviceReadScreenShot(uint32_t& width, uint32_t& height, const CImVector*& pixels);
    virtual void DeviceReadPixels(CiRect& rect, TSGrowableArray<CImVector>& pixels) = 0;
    virtual void DeviceReadDepths(CiRect& rect, TSGrowableArray<float>& depths) = 0;
    virtual int32_t DeviceWM(EGxWM wm, uintptr_t param1, uintptr_t param2) = 0;
    virtual void DeviceSetRenderTarget(EGxBuffer buffer, CGxTex* texture, uint32_t plane);
    virtual void DeviceResolveDepthBuffer(CGxTex* texture);
    virtual void DeviceCopyTex(CGxTex* source, uint32_t sourcePlane, CGxTex* dest, uint32_t destPlane, uint32_t level);
    virtual void DeviceOverride(EGxOverride override, uint32_t value);
    virtual void CapsWindowSize(CRect& rect) = 0;
    virtual void CapsWindowSizeInScreenCoords(CRect& rect) = 0;
    virtual void LogCrashInfo(char* buffer, uint32_t size);
    virtual void ScenePresent(void* window);
    virtual void SceneClear(uint32_t mask, CImVector color);
    virtual void SceneClearToViewport(uint32_t mask, CImVector color);
    virtual int32_t XformIsValidViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) = 0;
    virtual void XformSetProjection(const C44Matrix& matrix);
    virtual void XformSetView(const C44Matrix& matrix);
    virtual void PrimVertexPtr(CGxBuf* buf, EGxVertexBufferFormat format);
    virtual void PrimVertexPtrs(CGxBuf** bufs, const EGxVertexBufferFormat* formats, EGxVertexBufferFormat format);
    virtual void DrawNonIndexed(const CGxBatch& batch) = 0;
    virtual void Draw(const CGxBatch& batch) = 0;
    virtual void DrawInstancedNonIndexed(const CGxBatch& batch) = 0;
    virtual void DrawInstanced(const CGxBatch& batch) = 0;
    virtual void SetInstanceCount(uint32_t count);
    virtual void ResetInstancing();
    virtual void MasterEnableSet(EGxMasterEnables state, int32_t enable);
    virtual void PoolSizeSet(CGxPool* pool, uint32_t size) = 0;
    virtual void PoolDestroy(CGxPool* pool);
    virtual char* BufLock(CGxBuf* buf);
    virtual int32_t BufUnlock(CGxBuf* buf, uint32_t size);
    virtual void BufData(CGxBuf* buf, const void* data, uint32_t size, uint32_t offset);
    virtual int32_t TexCreate(EGxTexTarget target, uint32_t width, uint32_t height, uint32_t depth, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags flags, void* userArg, GxTexUserFunc userFunc, const char* name, CGxTex*& texId);
    virtual void TexDestroy(CGxTex* texId);
    virtual int32_t TexCopy(CGxTex* source, CGxTex* dest, const C2iVector& destPos, const C2iVector& sourcePos, const C2iVector& size, uint32_t level, uint32_t plane) = 0;
    virtual int32_t TexStretch(CGxTex* source, CGxTex* dest, const CiRect* destRect, const CiRect* sourceRect, uint32_t level, uint32_t face) = 0;
    virtual int32_t TexResolve(CGxTex* dest, CGxTex* source);
    virtual void TexSetCacheSize(int32_t size);
    virtual void QueryCreate(CGxQuery*& query, EGxQueryType type);
    virtual void QueryDestroy(CGxQuery*& query);
    virtual int32_t QueryBegin(CGxQuery* query) = 0;
    virtual int32_t QueryEnd(CGxQuery* query) = 0;
    virtual int32_t QueryGetParam(CGxQuery* query, EGxQueryParam param, uint32_t* data) = 0;
    virtual int32_t QueryGetData(CGxQuery* query, uint32_t* data) = 0;
    virtual void ShaderConstantsSet(EGxShTarget target, uint32_t index, const float* constants, uint32_t count);
    virtual void IShaderCreate(CGxShaderPermute* permute) = 0;
    virtual void IShaderDestroy(CGxShaderPermute* permute) = 0;
    virtual void ICursorDraw();
    virtual void CursorSetVisible(int32_t visible);
    virtual uint32_t* CursorLock();
    virtual void CursorUnlock(uint32_t x, uint32_t y);
    virtual int32_t ISetCursor();
    virtual void StereoSetConvergence(float convergence) = 0;
    virtual float StereoGetConvergence() = 0;
    virtual void StereoSetSeparation(float separation) = 0;
    virtual float StereoGetSeparation() = 0;
    virtual bool StereoEnabled() = 0;
    virtual void DebugGroupBegin(const char* name, char* buffer) = 0;
    virtual void DebugGroupEnd() = 0;
    virtual void DebugEvent(const char* name, char* buffer) = 0;

    static HSLOG m_log;
    static C2iVector s_clientAdjust;
    static const char* s_shaderTargetNames[GxShTargets_Last];
    static const char* s_shaderModelNames[8];
    static const EGxShTarget s_shaderProfileTarget[GxShProfiles_Last];
    static const char* s_shaderProfileNames[GxShProfiles_Last];
    static const char* s_shaderPaths[GxShTargets_Last];
    static const char* s_texFormatNames[GxTexFormats_Last];
    static C3Vector s_pointScaleAttenuation;
    static CGxShader* s_uiVertexShader;
    static CGxShader* s_uiPixelShader;
    static ShadowConstants s_shadowConstants[GxShTargets_Last];
    static TSHashTable<CGxShader, HASHKEY_STRI> s_shaderList[GxShTargets_Last];
    static TSGrowableArray<GxDeviceCallback> s_deviceRestoredCallbacks;
    static TSGrowableArray<GxDeviceCallback> s_textureRecreationCallbacks;
    static TSGrowableArray<GxDisplayChangeCallback> s_displayChangeCallbacks;
    static TSGrowableArray<CGxDevice::ShaderModelCall> s_shaderModelChangeCallbacks;
    static TSList<CGxPool, TSGetLink<CGxPool> > s_poolList;
    static TSList<CGxBuf, TSGetLink<CGxBuf> > s_bufList;
    static TSExplicitList<CGxTex, offsetof(CGxTex, m_link)> s_textureList;
    static int32_t s_cursorVisible;
    static uint32_t s_cursorHotspotX;
    static uint32_t s_cursorHotspotY;
    static uint32_t s_cursor[1024];
    static uint32_t s_alphaRef[12];
    static const uint32_t s_texFormatBitDepth[GxTexFormats_Last];
    static const uint32_t s_texFormatBlockSize[GxTexFormats_Last];
    static const uint32_t s_texFormatBytesPerBlock[GxTexFormats_Last];
    static const uint32_t s_texFormatIsDepth[GxTexFormats_Last];
    static const float s_polygonOffsetValues[3];
    static uint32_t m_logBytes;
    static int32_t s_monitorsInit;
    static TSGrowableArray<void*> s_monitors;

    static int32_t AdapterDesktopMode(uint32_t monitor, CGxMonitorMode& mode);
    static int32_t AdapterFormats(TSGrowableArray<CGxFormat>& formats);
    static int32_t AdapterID(uint16_t& vendorID, uint16_t& deviceID, uint32_t& driverVersionHi, uint32_t& driverVersionLo);
    static int32_t AdapterInfer(uint16_t& deviceID);
    static void* AdapterGetMonitorHandle(uint32_t monitor);
    static void AdapterInitMonitors();
    static void AdapterResetMonitors();
    static uint32_t AdapterMonitorCount();
    static int32_t AdapterMonitorModes(uint32_t monitor, TSGrowableArray<CGxMonitorMode>& modes);
    static int32_t AdapterMonitorName(uint32_t monitor, char* name, uint32_t size);
    static int32_t AdjustWindowRectToWorkArea(uint32_t monitor, float aspectRatio, C2iVector* adjust, tagRECT* rect);
    static int32_t D3d11AdapterInfer(uint16_t& deviceID);
    static int32_t D3d11AdapterMonitorModes(uint32_t monitor, TSGrowableArray<CGxMonitorMode>& modes);
    static int32_t D3d11EnumMonitorModes(uint32_t monitor, TSGrowableArray<CGxMonitorMode>& modes);
    static int32_t WinAdapterDesktopMode(uint32_t monitor, CGxMonitorMode& mode);
    static int32_t WinAdapterMonitorModes(uint32_t monitor, TSGrowableArray<CGxMonitorMode>& modes);
    static int32_t D3d11AdapterFormats(TSGrowableArray<CGxFormat>& formats);
    static int32_t D3dAdapterFormats(TSGrowableArray<CGxFormat>& formats);
    static int32_t DeviceAdapterID(uint16_t& vendorID, uint16_t& deviceID, uint32_t& driverVersionHi, uint32_t& driverVersionLo);
    static int32_t DeviceAdapterInfer(uint16_t& deviceID);
    static int32_t FormatExists(TSGrowableArray<CGxFormat>& formats, const CGxFormat& format);
    static int32_t OpenGlAdapterFormats(TSGrowableArray<CGxFormat>& formats);
    static void ClampRectBottom(CiRect& windowRect);
    static void ClampRectBottomByAspect(CiRect& windowRect);
    static void ClampRectLeftByAspect(CiRect& windowRect);
    static void ClampRectRightByAspect(CiRect& windowRect);
    static void ClampRectTopByAspect(CiRect& windowRect);
    static void ClampRectLeft(CiRect& windowRect);
    static void ClampRectRight(CiRect& windowRect);
    static void ClampRectTop(CiRect& windowRect);
    static void Log(const char* format, ...);
    static void LogV(const char* format, va_list args);
    static void LogClose();
    static void LogOpen();
    static uint32_t IVertexAttribToShAttrib(EGxVertexAttrib attrib);
    static void ICursorUpdate(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
    CGxBuf* BufCreate(CGxPool* pool, uint32_t itemSize, uint32_t itemCount, uint32_t index);
    CGxPool* PoolCreate(EGxPoolTarget target, EGxPoolUsage usage, int32_t size, EGxPoolHintBits hint, const char* name);
    void TexSetUserData(CGxTex* texId, GxTexUserFunc userFunc, void* userArg);
    void NotifyOnDeviceRestored();
    void NotifyOnTextureRecreation();
    void NotifyOnDisplayChange(uint32_t value);
    void AddDeviceRestoredCallback(GxDeviceCallback callback);
    void RemoveDeviceRestoredCallback(GxDeviceCallback callback);
    void AddTextureRecreationCallback(GxDeviceCallback callback);
    void RemoveTextureRecreationCallback(GxDeviceCallback callback);
    void AddDisplayChangeCallback(GxDisplayChangeCallback callback);
    void RemoveDisplayChangeCallback(GxDisplayChangeCallback callback);
    void AddShaderModelChangeCallback(GxShaderModelCallback callback, void* arg);
    void RemoveShaderModelChangeCallback(GxShaderModelCallback callback, void* arg);

    CGxDevice();
    void BufDestroy(CGxBuf*& buf);
    void DeviceCreatePools();
    void DeviceCreateStreamBufs();
    CGxBuf* BufStream(EGxPoolTarget target, uint32_t itemSize, uint32_t itemCount);
    void BufSizeSet(CGxBuf* buf, uint32_t itemSize, uint32_t itemCount);
    CGxCaps& Caps();
    int32_t CapsHasContext(int32_t context);
    int32_t CapsIsWindowVisible(int32_t visible);
    int32_t CapsWindowHasFocus(int32_t focus);
    void ChooseShaderModel(int32_t shaderModel);
    void ClampRectToWindow(CiRect& rect);
    void Log(const CGxCaps& caps) const;
    void Log(const CGxFormat& format) const;
    void IRsDirty(EGxRenderState which);
    void IRsForceUpdate();
    void IRsForceUpdate(EGxRenderState which);
    void IRsInit();
    void IRsSync(int32_t force);
    void RsPop();
    void RsInit();
    void RsPush();
    void RsSet(EGxRenderState which, void* value);
    void RsSet(EGxRenderState which, int32_t value);
    void RsSet(EGxRenderState which, float value);
    void RsSet(EGxRenderState which, const C3Vector& value);
    void TexSetFlags(CGxTex* texId, CGxTexFlags flags);
    void TexFlags(const CGxTex* texId, CGxTexFlags& flags);
    void TexSetFilterMode(EGxTexFilter filter, uint32_t maxAnisotropy);
    void TexSetWrap(CGxTex* texId, EGxTexWrapMode wrapU, EGxTexWrapMode wrapV);
    void ClipPlaneSet(uint32_t index, const C4Plane& plane);
    void CursorSetDepth(float depth);
    EGxApi DeviceApi();
    uint32_t DeviceBaseMipLevel();
    void DeviceClearScreenShot();
    const CRect& DeviceCurWindow();
    const CRect& DeviceDefWindow();
    const CGxFormat& DeviceFormat();
    void DeviceGamma(CGxGammaRamp& ramp);
    void DeviceGetRenderTarget(EGxBuffer buffer, CGxTex*& texture);
    void DeviceScreenShot();
    void DeviceSetCurWindow(const CRect& rect);
    void DeviceSetDefWindow(const CRect& rect);
    void GetSurveyModeInfo(uint32_t& desktopWidth, uint32_t& desktopHeight, uint32_t& desktopBpp, uint32_t& width, uint32_t& height, uint32_t& bpp);
    int32_t IDevIsWindowed();
    uint16_t IWindowClassCreate();
    void IWindowClassDestroy(uint16_t* windowClass);
    int32_t IWindowCreate(void* window, CGxFormat& format, int32_t show);
    void IWindowCursorCreate(void* window, void* cursor);
    void IWindowCursorDestroy(void* cursor);
    void IWindowDestroy(void* window);
    void InitLights();
    bool IShaderLoad(CGxShader* shader, EGxShTarget target, const char* name);
    uint32_t ITexBitsSize(CGxTex* texId, uint32_t width, uint32_t height, uint32_t depth);
    void ITexWHDStartEnd(CGxTex* texId, uint32_t& width, uint32_t& height, uint32_t& depth, uint32_t& baseMip, uint32_t& mipCount);
    void Light(uint32_t index, CGxLight& light);
    void LightEnable(uint32_t index, int32_t enable);
    int32_t GetLightEnabled(uint32_t index);
    void LightSet(uint32_t index, const CGxLight& light, const C3Vector& pos);
    int32_t MasterEnable(EGxMasterEnables state);
    int32_t NeedsUpdate(uint32_t app, uint32_t hw, uint32_t appDisables, uint32_t hwDisables, uint32_t flagPos, int32_t& enable);
    void NotifyOnShaderModelChange();
    uint32_t PrimCalcCount(EGxPrim primType, uint32_t indexCount);
    void PrimIndexPtr(CGxBuf* buf);
    EGxVertexBufferFormat PrimVertexFormat();
    void SetScissor(const CRect& rect);
    CGxShader* ShaderCreate(EGxShTarget target, const char* name);
    void ShaderConstantsClear();
    float* ShaderConstantsLock(EGxShTarget target);
    void ShaderConstantsUnlock(EGxShTarget target, uint32_t index, uint32_t count);
    void ShaderRelease(CGxShader* shader);
    bool ShaderReload(CGxShader* shader, const char* name);
    void ShaderReloadAll();
    int32_t TexCanUpdate(CGxTex* texId);
    int32_t TexIsReadyNow(CGxTex* texId);
    void TexMarkForUpdate(CGxTex* texId, const CiRect& updateRect, int32_t immediate);
    void TexParameters(const CGxTex* texId, CGxTexParms& parms);
    void TexSetCannotUpdate(CGxTex* texId);
    void TexSetDataFormat(CGxTex* texId, EGxTexFormat dataFormat);
    void WaitForFPSCap();
    void XformPush(EGxXform xf);
    void XformPushAndSet(EGxXform xf, const C44Matrix& matrix);
    void XformSetViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
};

extern CGxDevice* g_theGxDevicePtr;
extern const char* s_formatToInt[CGxFormat::Formats_Last];

#endif
