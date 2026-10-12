#ifndef ENGINE_SOURCE_GX_GX_H
#define ENGINE_SOURCE_GX_GX_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2iVector.h"

class C2Vector;
class C3Vector;
class C44Matrix;
class CGxBatch;
class CGxDevice;
class CGxFormat;
class CGxFrameBuffer;
struct CGxBuf;
class CGxQuery;
class CGxShader;
class CGxTex;
class CGxTexParms;
class CImVector;
class CiRect;
class CRect;

class GxuDrawListState {
    public:
    virtual ~GxuDrawListState();
    virtual void Begin() = 0;
    virtual void End() = 0;
};

class GxuDrawListItem {
    public:
    virtual ~GxuDrawListItem();
    virtual float GetDepth(const C3Vector& viewPos);
    virtual GxuDrawListState* GetState();
    virtual uintptr_t GetVtxStream();
    virtual void Discard();
    virtual void Draw(const C3Vector& viewPos) = 0;
};

class GxDrawListEntry {
    public:
    /* 0000 */ GxuDrawListItem* m_item;
    /* 0004 */ float m_depth;
    /* 0008 */ uintptr_t m_vtxStream;

    GxDrawListEntry();
};

extern const char* g_gxApiNames[GxApis_Last];
extern const char* g_gxShaderTargetNames[GxShTargets_Last];
extern const char* g_gxShaderModelNames[8];

CGxDevice* CreateCGxDeviceD3d();
CGxDevice* CreateCGxDeviceD3d11();
CGxDevice* CreateCGxDeviceOpenGl();
int32_t CreateGxDevice(EGxApi api, GxWindowProc windowProc, const CGxFormat& format);
int32_t GxAdapterDesktopMode(uint32_t monitor, CGxMonitorMode& mode);
int32_t GxAdapterID(uint16_t& vendorID, uint16_t& deviceID, uint32_t& driverVersionHi, uint32_t& driverVersionLo);
int32_t GxAdapterInfer(uint16_t& deviceID);
int32_t GxAdapterD3dAvailable();
int32_t GxAdapterHasDriver();
int32_t GxAdapterMonitorModes(uint32_t monitor, TSGrowableArray<CGxMonitorMode>& modes);
void GxReadPixels(CiRect& rect, TSGrowableArray<CImVector>& pixels);
int32_t GxVertexHasAttrib(EGxVertexBufferFormat format, EGxVertexAttrib attrib);
void GxXformSet(EGxXform xf, const C44Matrix& matrix);
char* GxBufLock(CGxBuf* buf);
void GxBufUnlock(CGxBuf* buf, uint32_t size);
void GxBufDestroy(CGxBuf* buf);
int32_t GxBufIsReady(CGxBuf* buf);
void GxPoolDestroy(CGxPool* pool);
void GxRenderTargetSet(EGxBuffer buffer, CGxTex* texture, uint32_t plane);
void GxFrameBufferSet(CGxFrameBuffer* frameBuffer);
int32_t GxFrameBufferFinalize(CGxFrameBuffer* frameBuffer);
int32_t GxFrameBufferAttach(CGxFrameBuffer* frameBuffer, EGxBuffer buffer, CGxTex* texture, uint32_t plane);
CGxFrameBuffer* GxFrameBufferCreate(CGxTex* depthTex, CGxTex* colorTex);
void GxBufData(CGxBuf* buf, const void* data, uint32_t size, uint32_t offset);
CGxBuf* GxBufStream(EGxPoolTarget target, uint32_t itemSize, uint32_t itemCount);
void GxPrimIndexPtr(uint32_t indexCount, const uint16_t* indices);
void GxPrimIndexPtr(CGxBuf* buf);
void GxPrimLockVertexPtrs(uint32_t vertexCount, const C3Vector* pos, uint32_t posStride, const C3Vector* normal, uint32_t normalStride, const CImVector* color, uint32_t colorStride, const uint8_t* bone, uint32_t boneStride, const C2Vector* tex0, uint32_t tex0Stride, const C2Vector* tex1, uint32_t tex1Stride);
void GxPrimVertexPtr(CGxBuf* buf, EGxVertexBufferFormat format);
void GxPrimVertexPtr(uint32_t vertexCount, const C3Vector* pos, uint32_t posStride, const C3Vector* normal, uint32_t normalStride, const CImVector* color, uint32_t colorStride, const C2Vector* tex0, uint32_t tex0Stride, const C2Vector* tex1, uint32_t tex1Stride);
void GxPrimLockIndexPtr(EGxPrim primType, uint32_t indexCount, const uint16_t* indices);
void GxPrimUnlockVertexPtrs();
void GxTexParameters(const CGxTex* texId, CGxTexParms& parms);
void GxTexSetCannotUpdate(CGxTex* texId);
void GxTexSetDataFormat(CGxTex* texId, EGxTexFormat dataFormat);
void GxTexSetUserData(CGxTex* texId, GxTexUserFunc userFunc, void* userArg);
void GxTexUpdate(CGxTex* texId, int32_t minX, int32_t minY, int32_t maxX, int32_t maxY, int32_t immediate);
void GxTexUpdate(CGxTex* texId, CiRect& updateRect, int32_t immediate);
int32_t GxTexCreate(const CGxTexParms& parms, CGxTex*& texId);
int32_t GxTexCreate(EGxTexTarget target, uint32_t width, uint32_t height, uint32_t depth, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags flags, void* userArg, GxTexUserFunc userFunc, const char* name, CGxTex*& texId);
int32_t GxTexStretch(CGxTex* source, CGxTex* dest, const CiRect* destRect, const CiRect* sourceRect, uint32_t level, uint32_t face);
int32_t GxTexCreate(uint32_t width, uint32_t height, EGxTexFormat format, CGxTexFlags flags, void* userArg, GxTexUserFunc userFunc, CGxTex*& texId);
void GxXformPush(EGxXform xf, const C44Matrix& matrix);
void GxCapsWindowSizeInScreenCoords(CRect& rect);
void GxFormatColor(CImVector& color);
CImVector GxSwizzleColor(const CImVector& color);
void GxDrawNonIndexed(const CGxBatch& batch);

void GxDraw(const CGxBatch& batch);
void GxDevDestroy();
void GxDrawLockedElements();
void GxDrawLockedElements(EGxPrim primType, uint32_t indexCount, const uint16_t* indices);

BlitFormat GxGetBlitFormat(EGxTexFormat format);
uint32_t GxGetMaxFPS();
uint32_t GxGetMaxFPSBk();
int32_t GxHasContext();
void GxLog(const char* format, ...);
void GxLogClose();
void GxShutDown();
void GxLogCrashInfo(char* buffer, uint32_t size);
void GxLogOpen();
int32_t GxIsWindowVisible();
int32_t GxMasterEnable(EGxMasterEnables state);
void GxMasterEnableSet(EGxMasterEnables state, int32_t enable);
int32_t GxQueryBegin(CGxQuery* query);
void GxQueryCreate(CGxQuery*& query, EGxQueryType type);
void GxQueryDestroy(CGxQuery*& query);
int32_t GxQueryEnd(CGxQuery* query);
int32_t GxQueryGetData(CGxQuery* query, uint32_t* data);
int32_t GxQueryGetParam(CGxQuery* query, EGxQueryParam param, uint32_t* data);
void GxRsSet(EGxRenderState which, void* value);
void GxRsSet(EGxRenderState which, CGxShader* shader, uint32_t permute);
int32_t GxShaderValid(CGxShader* shader, uint32_t permute);
void GxRsSet(EGxRenderState which, int32_t value);
void GxRsSet(EGxRenderState which, float value);
void GxRsSet(EGxRenderState which, CImVector value);
void GxUpdateAlphaRef(EGxRenderState which);
void GxSceneClear(uint32_t mask, CImVector color);
void GxScenePresent(void* window);
void GxuAddToDrawList(EGxuDrawListCategory category, GxuDrawListItem* item);
void GxuClearDrawList(EGxuDrawListCategory category);
void GxuFlushDrawList(EGxuDrawListCategory category, const C3Vector& viewPos);
void GxuSortDrawList(EGxuDrawListCategory category, EGxuDrawListSorting sorting, const C3Vector& viewPos);
void GxTexSetFlags(CGxTex* texId, CGxTexFlags flags);
void GxTexSetWrap(CGxTex* texId, EGxTexWrapMode wrapU, EGxTexWrapMode wrapV);
void GxSetMaxFPS(uint32_t maxFPS);
void GxSetMaxFPSBk(uint32_t maxFPSBk);
CGxShader* GxShaderCreate(EGxShTarget target, const char* name);
void GxShaderConstantsSet(EGxShTarget target, uint32_t index, const float* constants, uint32_t count);
int32_t GxTexCopy(CGxTex* source, CGxTex* dest, const C2iVector& destPos, const C2iVector& sourcePos, const C2iVector& size, uint32_t level, uint32_t plane);
void GxTexDestroy(CGxTex* texId);
int32_t GxTexReusable(CGxTexParms& parms);
int32_t GxTexReusable(CGxTex* texId);
void GxXform(EGxXform xf, C44Matrix& matrix);
void GxXformPop(EGxXform xf);
void GxXformProjection(C44Matrix& matrix);
void GxXformPush(EGxXform xf);
void GxXformScale(EGxXform xf, const C3Vector& scale);
void GxXformSetProjection(const C44Matrix& matrix);
void GxXformSetView(const C44Matrix& matrix);
void GxXformSetViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
void GxXformTranslate(EGxXform xf, const C3Vector& move);
void GxXformView(C44Matrix& matrix);
void GxXformViewport(float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ);
void GxXformViewProj(C44Matrix& matrix);
void GxXformProjNative(C44Matrix& matrix);
void GxXformViewProjNative(C44Matrix& matrix);
void GxXformViewProjNativeTranspose(C44Matrix& matrix);
int32_t GxApiSupported(EGxApi api);
int32_t GxApiSelectable(EGxApi api);
void GxCVarInitialize();
void GxSetDepthResolveHandleCaching(int32_t enable);
int32_t GxGetDepthResolveHandleCaching();
EGxApi GxGetRequestedApi();
void GxSetRequestedApi(EGxApi api);
uint32_t GxAdapterMonitorCount();
int32_t GxAdapterMonitorName(uint32_t monitor, char* name, uint32_t size);
int32_t GxAdapterFormats(TSGrowableArray<CGxFormat>& formats);
void GxStartUp();
bool GxDevExists();
void* GxDevWindow();
void GxPrimUnlockIndexPtr();
void GxSceneClearToViewport(uint32_t mask, CImVector color);
void GxTexFlags(const CGxTex* texId, CGxTexFlags& flags);
void GxTexSetFilterMode(EGxTexFilter filter, uint32_t maxAnisotropy);
int32_t GxTexResolve(CGxTex* dest, CGxTex* source);
int32_t GxXformIsValidViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
EGxApi GxDefaultApi();
uint32_t GxCalcTexelStrideInBytes(EGxTexFormat format, uint32_t width);
uint32_t GxVertexAttribOffset(EGxVertexBufferFormat format, EGxVertexAttrib attrib);
uint32_t GxVertexSize(EGxVertexBufferFormat format);

#include "Engine/Source/Gx/Gx.inl"

#endif
