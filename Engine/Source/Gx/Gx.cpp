#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Gx/Buffer.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00AFB210
static const uint32_t s_texelSize[GxTexFormats_Last] = { 0, 4, 4, 2, 2, 2, 2, 2, 4, 4, 2, 4, 8, 4, 4, 16, 4, 4, 2, 4, 4, 4, 4, 4 };

// STATIC: %0x00C0DBB0
static BlitFormat s_blitFormats[GxTexFormats_Last] = {
    BlitFormat_Unknown, BlitFormat_Abgr8888, BlitFormat_Argb8888, BlitFormat_Argb4444, BlitFormat_Argb1555, BlitFormat_Rgb565, BlitFormat_Uv88, BlitFormat_Dxt1,
    BlitFormat_Dxt3, BlitFormat_Dxt5, BlitFormat_Unknown, BlitFormat_Gr1616F, BlitFormat_Abgr16161616F, BlitFormat_Unknown, BlitFormat_R32F, BlitFormat_Abgr32323232F,
    BlitFormat_Unknown, BlitFormat_D24X8, BlitFormat_Unknown, BlitFormat_D32, BlitFormat_Unknown, BlitFormat_Unknown, BlitFormat_Unknown, BlitFormat_Unknown
};

// STATIC: %0x00C0D86C
const char* g_gxApiNames[GxApis_Last] = { "OpenGL", "D3D9", "D3D11", "GLL" };

// STATIC: 0x00D0853C
const char* g_gxShaderTargetNames[GxShTargets_Last] = { "Vertex", "Hull", "Domain", "Geometry", "Pixel", "Compute" };

// STATIC: 0x00D08554
const char* g_gxShaderModelNames[8] = { "ff", "nvrc", "ff_ps", "1_1", "2_0", "3_0", "4_0", "5_0" };

// STATIC: %0x00C0DBA0
static EGxApi s_requestedApi = GxApis_Last;

// STATIC: 0x00EC6680/0x140DB87C8
static uint32_t s_maxFPSBk;

// STATIC: 0x00EC667C/0x140DB87C4
static int32_t s_depthResolveHandleCaching;

// STATIC: 0x00EC668C/0x140DB87D4
static uint32_t s_apiSupported;

// STATIC: 0x00EC6688/0x140DB87D0
static uint32_t s_apiSelectable;

// STATIC: 0x00EBF470
static EGxPrim s_lockPrimType;

// STATIC: 0x00EC6684/0x140DB87CC
static uint32_t s_maxFPS;

// STATIC: %0x00D8BCD4
static uint32_t s_lockIndexCount;

// STATIC: %0x00D8BCD8
static uint32_t s_lockVertexCount;

// STATIC: %0x00CE310C
static bool s_drawListBusy;

// STATIC: %0x00CE3110
static TSGrowableArray<GxDrawListEntry> s_drawLists[GxuCats_Last];

// PROVEN: e358aa38 IDENTICAL/IDENTICAL
// OFFSET: 0x006128D0/0x14029DB60
static void DrawListPreprocessB2F(GxDrawListEntry* entry, const C3Vector& viewPos) {
    entry->m_depth = entry->m_item->GetDepth(viewPos);
}

// OFFSET: 0x006128F0/0x14029DB80
static int DrawListSorterB2F(const void* a, const void* b) {
    float depthA = static_cast<const GxDrawListEntry*>(a)->m_depth;
    float depthB = static_cast<const GxDrawListEntry*>(b)->m_depth;

    if (depthB < depthA) {
        return -1;
    }

    return depthB > depthA;
}

// PROVEN: fa45fcf0 IDENTICAL/DIFFERENT
// OFFSET: 0x00612930/0x14029DBB0
static void DrawListPreprocessVtxStream(GxDrawListEntry* entry, const C3Vector& viewPos) {
    entry->m_vtxStream = entry->m_item->GetVtxStream();
}

// PROVEN: 7349a485 IDENTICAL/DIFFERENT
// OFFSET: 0x00612950/0x14029DBD0
static int DrawListSorterVtxStream(const void* a, const void* b) {
    uint32_t streamA = static_cast<const GxDrawListEntry*>(a)->m_vtxStream;
    uint32_t streamB = static_cast<const GxDrawListEntry*>(b)->m_vtxStream;

    if (streamB > streamA) {
        return -1;
    }

    return streamB < streamA;
}

// STATIC: %0x00BBD4B4
static void (*s_drawListPreprocess[GxuSorts_Last])(GxDrawListEntry*, const C3Vector&) = { DrawListPreprocessB2F, DrawListPreprocessVtxStream };

// STATIC: %0x00BBD4BC
static int (*s_drawListSorters[GxuSorts_Last])(const void*, const void*) = { DrawListSorterB2F, DrawListSorterVtxStream };

// PROVEN: d10f29a8 IDENTICAL/DIFFERENT
// OFFSET: 0x009FCE20/0x14090FD50
EGxApi GxDefaultApi() {
    return GxApi_D3d9;
}

// PROVEN: d0349537 IDENTICAL/IDENTICAL
// OFFSET: 0x0083FF20/0x14056BD30
int32_t GxApiSupported(EGxApi api) {
    return (1 << api) & s_apiSupported;
}

// PROVEN: 579d984d IDENTICAL/IDENTICAL
// OFFSET: 0x0083FF40/0x14056BD40
int32_t GxApiSelectable(EGxApi api) {
    return (1 << api) & s_apiSelectable;
}

// PROVEN: 499fbf97 IDENTICAL/UNPROVEN
// OFFSET: 0x00840010/0x14056BE00
uint32_t GxVertexSize(EGxVertexBufferFormat format) {
    return Buffer::s_vertexBufDesc[format].size;
}

// OFFSET: %0x007A6080
int32_t GxAdapterID(uint16_t& vendorID, uint16_t& deviceID, uint32_t& driverVersionHi, uint32_t& driverVersionLo) {
    return CGxDevice::AdapterID(vendorID, deviceID, driverVersionHi, driverVersionLo);
}

// OFFSET: %0x007A6090
int32_t GxAdapterInfer(uint16_t& deviceID) {
    return CGxDevice::AdapterInfer(deviceID);
}

// PROVEN: ca432153 IDENTICAL/NONE
// OFFSET: 0x0083FFE0/0x14056BDD0
int32_t GxAdapterMonitorModes(uint32_t monitor, TSGrowableArray<CGxMonitorMode>& modes) {
    return CGxDevice::AdapterMonitorModes(monitor, modes);
}

// PROVEN: 552dc9b1 IDENTICAL/UNPROVEN
// OFFSET: 0x0083FFF0/0x14056BDE0
int32_t GxAdapterDesktopMode(uint32_t monitor, CGxMonitorMode& mode) {
    return CGxDevice::AdapterDesktopMode(monitor, mode);
}

// PROVEN: 9ff17c0c IDENTICAL/IDENTICAL
// OFFSET: 0x0083FF60/0x14056BD50
EGxApi GxGetRequestedApi() {
    return s_requestedApi;
}

// OFFSET: %0x007A6070
void GxSetRequestedApi(EGxApi api) {
    s_requestedApi = api;
}

// PROVEN: ee09dcee IDENTICAL/UNPROVEN
// OFFSET: 0x0083FFC0/0x14056BDB0
uint32_t GxAdapterMonitorCount() {
    return CGxDevice::AdapterMonitorCount();
}

// PROVEN: 3e28b123 IDENTICAL/UNPROVEN
// OFFSET: 0x0083FFD0/0x14056BDC0
int32_t GxAdapterMonitorName(uint32_t monitor, char* name, uint32_t size) {
    return CGxDevice::AdapterMonitorName(monitor, name, size);
}

// PROVEN: fb768403 IDENTICAL/NONE
// OFFSET: 0x00840000
int32_t GxAdapterFormats(TSGrowableArray<CGxFormat>& formats) {
    return CGxDevice::AdapterFormats(formats);
}

// PROVEN: 873aa4cf IDENTICAL/NONE
// OFFSET: 0x00840050
void GxStartUp() {
    CGxDevice::s_cursorVisible = 1;
    CGxDevice::s_cursorHotspotX = 0;
    CGxDevice::s_cursorHotspotY = 0;
    memset(CGxDevice::s_cursor, 0, sizeof(CGxDevice::s_cursor));
    CGxDevice::s_cursor[0] = 0x1000000;
    s_apiSupported = 0;
    s_apiSelectable = 0;

    for (uint32_t api = 0; api < GxApis_Last; api++) {
        int32_t supported;
        int32_t selectable;
        CGxDevice::ApiQuery(static_cast<EGxApi>(api), supported, selectable);

        if (supported) {
            s_apiSupported |= 1 << api;
        }

        if (selectable) {
            s_apiSelectable |= 1 << api;
        }
    }

    GxCVarInitialize();
}

// PROVEN: 8aa14420 IDENTICAL/IDENTICAL
// OFFSET: 0x00840190/0x14056BFC0
bool GxDevExists() {
    return g_theGxDevicePtr != NULL;
}

// PROVEN: 7a80fc4f IDENTICAL/IDENTICAL
// OFFSET: 0x008401A0/0x14056BFD0
void* GxDevWindow() {
    return g_theGxDevicePtr->DeviceWindow();
}

// PROVEN: 1e2e22aa IDENTICAL/NONE
// OFFSET: 0x008401C0
void GxPrimUnlockIndexPtr() {
    s_lockIndexCount = 0;
}

// PROVEN: 1034c42e IDENTICAL/NONE
// OFFSET: 0x00840210/0x14056C050
void GxSceneClearToViewport(uint32_t mask, CImVector color) {
    g_theGxDevicePtr->SceneClearToViewport(mask, color);
}

// PROVEN: 4e9baa2f IDENTICAL/DIFFERENT
// OFFSET: 0x00840340/0x14056C180
void GxTexFlags(const CGxTex* texId, CGxTexFlags& flags) {
    g_theGxDevicePtr->TexFlags(texId, flags);
}

// PROVEN: 10a32010 IDENTICAL/DIFFERENT
// OFFSET: 0x00840360/0x14056C1A0
void GxTexSetFilterMode(EGxTexFilter filter, uint32_t maxAnisotropy) {
    g_theGxDevicePtr->TexSetFilterMode(filter, maxAnisotropy);
}

// PROVEN: 63aa73e2 IDENTICAL/NONE
// OFFSET: 0x008403F0/0x14056C250
int32_t GxTexResolve(CGxTex* dest, CGxTex* source) {
    return g_theGxDevicePtr->TexResolve(dest, source);
}

// OFFSET: 0x00840580
int32_t GxXformIsValidViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    float cleanMinX = minX > 0.0f ? minX : 0.0f;
    float cleanMaxX = maxX < 1.0f ? maxX : 1.0f;
    float cleanMinY = minY > 0.0f ? minY : 0.0f;
    float cleanMaxY = maxY < 1.0f ? maxY : 1.0f;

    if (cleanMinX >= cleanMaxX || cleanMinY >= cleanMaxY || minZ > maxZ || minZ < 0.0f || maxZ > 1.0f) {
        return 0;
    }

    return g_theGxDevicePtr->XformIsValidViewport(cleanMinX, cleanMaxX, cleanMinY, cleanMaxY, minZ, maxZ);
}

// OFFSET: %0x007A6110
uint32_t GxVertexAttribOffset(EGxVertexBufferFormat format, EGxVertexAttrib attrib) {
    return Buffer::s_vertexBufDesc[format].attribPtrs[attrib]->m_offset;
}

// PROVEN: 279d59d0 IDENTICAL/UNPROVEN
// OFFSET: 0x00840030/0x14056BE20
int32_t GxVertexHasAttrib(EGxVertexBufferFormat format, EGxVertexAttrib attrib) {
    return Buffer::s_vertexBufDesc[format].attribPtrs[attrib] != NULL;
}

// OFFSET: 0x008400E0/0x14056BEE0
int32_t CreateGxDevice(EGxApi api, GxWindowProc windowProc, const CGxFormat& format) {
    CGxDevice* device;

    switch (api) {
        case GxApi_OpenGl:
            device = CreateCGxDeviceOpenGl();
            g_theGxDevicePtr = device;
            break;

        case GxApi_D3d9:
            device = CreateCGxDeviceD3d();
            g_theGxDevicePtr = device;
            break;

        case GxApi_D3d11:
            device = CreateCGxDeviceD3d11();
            g_theGxDevicePtr = device;
            break;

        default:
            device = g_theGxDevicePtr;
            break;
    }

    if (device->DeviceCreate(windowProc, format)) {
        return 1;
    }

    if (g_theGxDevicePtr) {
        delete g_theGxDevicePtr;
    }

    g_theGxDevicePtr = NULL;
    return 0;
}

// OFFSET: #0x006815F0
uint32_t GxCalcTexelStrideInBytes(EGxTexFormat format, uint32_t width) {
    if (format < GxTex_Dxt1 || format > GxTex_Dxt5) {
        return s_texelSize[format] * width;
    }

    return s_texelSize[format] * ((width + 3) & ~3u);
}

// PROVEN: 6089bcec IDENTICAL/NONE
// OFFSET: 0x00409F40
void GxXformSetProjection(const C44Matrix& matrix) {
    g_theGxDevicePtr->XformSetProjection(matrix);
}

// PROVEN: ac265db2 IDENTICAL/NONE
// OFFSET: 0x00409F60
void GxXformSetView(const C44Matrix& matrix) {
    g_theGxDevicePtr->XformSetView(matrix);
}

// PROVEN: 942fef9b IDENTICAL/NONE
// OFFSET: 0x00409F80
void GxXformViewport(float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ) {
    minX = g_theGxDevicePtr->m_viewport.m_x.m_l;
    maxX = g_theGxDevicePtr->m_viewport.m_x.m_h;
    minY = g_theGxDevicePtr->m_viewport.m_y.m_l;
    maxY = g_theGxDevicePtr->m_viewport.m_y.m_h;
    minZ = g_theGxDevicePtr->m_viewport.m_z.m_l;
    maxZ = g_theGxDevicePtr->m_viewport.m_z.m_h;
}

// PROVEN: 36c76efb IDENTICAL/NONE
// OFFSET: 0x0040A060
void GxXformView(C44Matrix& matrix) {
    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[GxXform_View];
    matrix = stack.m_mtx[stack.m_level];
}

// PROVEN: 4a5f0e73 IDENTICAL/NONE
// OFFSET: 0x00430AB0
void GxXform(EGxXform xf, C44Matrix& matrix) {
    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[xf];
    matrix = stack.m_mtx[stack.m_level];
}

// OFFSET: 0x00409FF0
void GxXformProjNative(C44Matrix& matrix) {
    CGxDevice* device = g_theGxDevicePtr;
    matrix = device->m_projNative;

    if (device->DeviceApi() == GxApi_OpenGl) {
        matrix.c0 = matrix.c0 * -1.0f;
        matrix.c1 = matrix.c1 * -1.0f;
        matrix.c2 = matrix.c2 * -1.0f;
        matrix.c3 = matrix.c3 * -1.0f;
    }
}

// OFFSET: 0x00841070/0x14056D1D0
void GxXformViewProjNative(C44Matrix& matrix) {
    CGxDevice* device = g_theGxDevicePtr;
    C44Matrix view;
    C44Matrix proj;

    view = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];
    proj = device->m_projNative;

    if (device->DeviceApi() == GxApi_OpenGl) {
        proj.c0 = proj.c0 * -1.0f;
        proj.c1 = proj.c1 * -1.0f;
        proj.c2 = proj.c2 * -1.0f;
        proj.c3 = proj.c3 * -1.0f;
    }

    matrix = view * proj;
}

// OFFSET: 0x0040A090
void GxXformViewProjNativeTranspose(C44Matrix& matrix) {
    CGxDevice* device = g_theGxDevicePtr;
    C44Matrix view;
    C44Matrix proj;

    view = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];
    proj = device->m_projNative;

    if (device->DeviceApi() == GxApi_OpenGl) {
        proj.c0 = proj.c0 * -1.0f;
        proj.c1 = proj.c1 * -1.0f;
        proj.c2 = proj.c2 * -1.0f;
        proj.c3 = -1.0f * proj.c3;
    }

    matrix = (view * proj).Transpose();
}

// PROVEN: f8ee56a9 IDENTICAL/NONE
// OFFSET: 0x0040A210
void GxShaderConstantsSet(EGxShTarget target, uint32_t index, const float* constants, uint32_t count) {
    if (constants) {
        g_theGxDevicePtr->ShaderConstantsSet(target, index, constants, count);
    }
}

// PROVEN: 26b19640 IDENTICAL/NONE
// OFFSET: 0x004DFB30
char* GxBufLock(CGxBuf* buf) {
    return g_theGxDevicePtr->BufLock(buf);
}

// PROVEN: c9925058 IDENTICAL/NONE
// OFFSET: 0x004DFB50
void GxBufUnlock(CGxBuf* buf, uint32_t size) {
    g_theGxDevicePtr->BufUnlock(buf, size);
    buf->unk_001C = 1;
}

// PROVEN: 9401f9b0 IDENTICAL/NONE
// OFFSET: 0x004DFBC0
void GxDrawNonIndexed(const CGxBatch& batch) {
    g_theGxDevicePtr->DrawNonIndexed(batch);
}

// OFFSET: %0x004E6690
void GxDraw(const CGxBatch& batch) {
    g_theGxDevicePtr->Draw(batch);
}

// PROVEN: bf0ce24b IDENTICAL/NONE
// OFFSET: 0x00481CB0
void GxCapsWindowSizeInScreenCoords(CRect& rect) {
    g_theGxDevicePtr->CapsWindowSizeInScreenCoords(rect);
}

// PROVEN: d9f7e9b6 IDENTICAL/NONE
// OFFSET: 0x00481D00
void GxXformProjection(C44Matrix& matrix) {
    matrix = g_theGxDevicePtr->m_projection;
}

// OFFSET: %0x004E6670
void GxXformPush(EGxXform xf) {
    g_theGxDevicePtr->XformPush(xf);
}

// PROVEN: a6e192f0 DIFFERENT/IDENTICAL
// OFFSET: 0x00481CD0/0x1400A0A80
void GxXformPop(EGxXform xf) {
    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[xf];
    stack.m_dirty = 1;

    if (stack.m_level) {
        stack.m_level--;
    }
}

// PROVEN: a75ed2d4 IDENTICAL/NONE
// OFFSET: 0x0057D0C0
void GxXformTranslate(EGxXform xf, const C3Vector& move) {
    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[xf];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level].Translate(move);
}

// INLINED
void GxXformScale(EGxXform xf, const C3Vector& scale) {
    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[xf];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level].Scale(scale);
}

// OFFSET: 0x00409F20
int32_t GxHasContext() {
    return g_theGxDevicePtr ? g_theGxDevicePtr->CapsHasContext(-1) : 0;
}

// OFFSET: 0x00409F00
int32_t GxIsWindowVisible() {
    return g_theGxDevicePtr ? g_theGxDevicePtr->CapsIsWindowVisible(-1) : 0;
}

// INLINED
int32_t GxMasterEnable(EGxMasterEnables state) {
    return g_theGxDevicePtr->MasterEnable(state);
}

// PROVEN: 5063f738 IDENTICAL/NONE
// OFFSET: 0x00401120
void GxMasterEnableSet(EGxMasterEnables state, int32_t enable) {
    g_theGxDevicePtr->MasterEnableSet(state, enable);
}

// PROVEN: a27e24db IDENTICAL/IDENTICAL
// OFFSET: 0x008401F0/0x14056C020
void GxSceneClear(uint32_t mask, CImVector color) {
    g_theGxDevicePtr->SceneClear(mask, color);
}

// PROVEN: d0acf498 IDENTICAL/IDENTICAL
// OFFSET: 0x008402E0/0x14056C120
void GxTexDestroy(CGxTex* texId) {
    g_theGxDevicePtr->TexDestroy(texId);
}

// PROVEN: 73d0112e IDENTICAL/IDENTICAL
// OFFSET: 0x00840380/0x14056C1C0
int32_t GxTexCopy(CGxTex* source, CGxTex* dest, const C2iVector& destPos, const C2iVector& sourcePos, const C2iVector& size, uint32_t level, uint32_t plane) {
    return g_theGxDevicePtr->TexCopy(source, dest, destPos, sourcePos, size, level, plane);
}

// PROVEN: 667a2867 IDENTICAL/IDENTICAL
// OFFSET: 0x00840410/0x14056C270
int32_t GxTexReusable(CGxTex* texId) {
    return texId && texId->m_char5D && !texId->m_flags.m_renderTarget && texId->m_target == GxTex_2d && texId->m_format <= GxTex_Dxt5;
}

// PROVEN: 9a55cb5d IDENTICAL/IDENTICAL
// OFFSET: 0x00840440/0x14056C2A0
int32_t GxTexReusable(CGxTexParms& parms) {
    return !parms.m_flags.m_renderTarget && parms.m_target == GxTex_2d && parms.m_format <= GxTex_Dxt5;
}

// PROVEN: 50fdde8c IDENTICAL/IDENTICAL
// OFFSET: 0x008404B0/0x14056C2F0
void GxQueryCreate(CGxQuery*& query, EGxQueryType type) {
    g_theGxDevicePtr->QueryCreate(query, type);
}

// PROVEN: 07b2b081 IDENTICAL/NONE
// OFFSET: 0x008404D0/0x14056C310
void GxQueryDestroy(CGxQuery*& query) {
    g_theGxDevicePtr->QueryDestroy(query);
}

// PROVEN: 6875628b IDENTICAL/NONE
// OFFSET: 0x0084AEE0
void GxSetMaxFPS(uint32_t maxFPS) {
    s_maxFPS = maxFPS;
}

// PROVEN: 2bd4c84f IDENTICAL/IDENTICAL
// OFFSET: 0x0084AEF0/0x140579590
uint32_t GxGetMaxFPS() {
    return s_maxFPS;
}

// PROVEN: 849b2a79 IDENTICAL/NONE
// OFFSET: 0x0084AF00
void GxSetMaxFPSBk(uint32_t maxFPSBk) {
    s_maxFPSBk = maxFPSBk;
}

// PROVEN: 25b261fc IDENTICAL/IDENTICAL
// OFFSET: 0x0084AF10/0x1405795B0
uint32_t GxGetMaxFPSBk() {
    return s_maxFPSBk;
}

// PROVEN: 194b7604 IDENTICAL/NONE
// OFFSET: 0x0084AF20
void GxSetDepthResolveHandleCaching(int32_t enable) {
    s_depthResolveHandleCaching = enable;
}

// PROVEN: 80b22173 IDENTICAL/IDENTICAL
// OFFSET: 0x0084AF30/0x1405795D0
int32_t GxGetDepthResolveHandleCaching() {
    return s_depthResolveHandleCaching;
}

// PROVEN: cd178014 IDENTICAL/IDENTICAL
// OFFSET: 0x00840660/0x14056C480
BlitFormat GxGetBlitFormat(EGxTexFormat format) {
    return s_blitFormats[format];
}

// OFFSET: 0x008406B0
void CGxDevice::XformSetViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    if (minX == this->m_viewport.m_x.m_l && maxX == this->m_viewport.m_x.m_h && minY == this->m_viewport.m_y.m_l && maxY == this->m_viewport.m_y.m_h && minZ == this->m_viewport.m_z.m_l && maxZ == this->m_viewport.m_z.m_h) {
        return;
    }

    this->m_viewportDirty = 1;
    this->m_viewport.m_x.m_l = minX;
    this->m_viewport.m_x.m_h = maxX;
    this->m_viewport.m_y.m_l = minY;
    this->m_viewport.m_y.m_h = maxY;
    this->m_viewport.m_z.m_l = minZ;
    this->m_viewport.m_z.m_h = maxZ;
}

// PROVEN: 81cb2df0 IDENTICAL/IDENTICAL
// OFFSET: 0x00840880/0x14056C5E0
void GxBufData(CGxBuf* buf, const void* data, uint32_t size, uint32_t offset) {
    if (!size) {
        size = buf->m_itemSize * buf->m_itemCount;
    }

    g_theGxDevicePtr->BufData(buf, data, size, offset);
    buf->unk_001C = 1;
}

// PROVEN: 46e6b5a1 IDENTICAL/NONE
// OFFSET: 0x008408C0/0x14056C620
void GxDrawLockedElements() {
    if (!s_lockVertexCount) {
        return;
    }

    CGxBatch batch;
    batch.m_primType = s_lockPrimType;
    batch.m_start = 0;
    batch.m_count = s_lockIndexCount;
    batch.m_minIndex = 0;
    batch.m_maxIndex = static_cast<uint16_t>(s_lockVertexCount - 1);
    g_theGxDevicePtr->Draw(batch);
}

// PROVEN: ebd53e0f IDENTICAL/DIFFERENT
// OFFSET: 0x00840EB0/0x14056CE90
void GxXformSetViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    float cleanMinX = minX > 0.0f ? minX : 0.0f;
    float cleanMaxX = maxX < 1.0f ? maxX : 1.0f;
    float cleanMinY = minY > 0.0f ? minY : 0.0f;
    float cleanMaxY = maxY < 1.0f ? maxY : 1.0f;

    g_theGxDevicePtr->XformSetViewport(cleanMinX, cleanMaxX, cleanMinY, cleanMaxY, minZ, maxZ);
}

// OFFSET: 0x00840F60/0x14056CF50
void GxXformViewProj(C44Matrix& matrix) {
    CGxDevice* device = g_theGxDevicePtr;
    C44Matrix view;
    C44Matrix proj;

    view = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];
    proj = device->m_projection;
    matrix = view * proj;
}

// PROVEN: 67242db9 IDENTICAL/NONE
// OFFSET: 0x004DFB80
CGxBuf* GxBufStream(EGxPoolTarget target, uint32_t itemSize, uint32_t itemCount) {
    return g_theGxDevicePtr->BufStream(target, itemSize, itemCount);
}

// INLINED
void GxXformPush(EGxXform xf, const C44Matrix& matrix) {
    g_theGxDevicePtr->XformPushAndSet(xf, matrix);
}

// PROVEN: 6ed57f00 IDENTICAL/DIFFERENT
// OFFSET: 0x00840260/0x14056C0B0
void GxTexSetCannotUpdate(CGxTex* texId) {
    g_theGxDevicePtr->TexSetCannotUpdate(texId);
}

// PROVEN: a2bdc28c IDENTICAL/NONE
// OFFSET: 0x00840280
void GxTexSetUserData(CGxTex* texId, GxTexUserFunc userFunc, void* userArg) {
    g_theGxDevicePtr->TexSetUserData(texId, userFunc, userArg);
}

// PROVEN: 7f5a64e7 IDENTICAL/DIFFERENT
// OFFSET: 0x00840300/0x14056C140
void GxTexParameters(const CGxTex* texId, CGxTexParms& parms) {
    g_theGxDevicePtr->TexParameters(texId, parms);
}

// PROVEN: 8f86faee IDENTICAL/DIFFERENT
// OFFSET: 0x00840320/0x14056C0C0
void GxTexSetDataFormat(CGxTex* texId, EGxTexFormat dataFormat) {
    g_theGxDevicePtr->TexSetDataFormat(texId, dataFormat);
}

// PROVEN: 9917479e IDENTICAL/IDENTICAL
// OFFSET: 0x00840C40/0x14056CB20
void GxPrimIndexPtr(uint32_t indexCount, const uint16_t* indices) {
    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), indexCount);
    g_theGxDevicePtr->BufData(buf, indices, buf->m_itemSize * buf->m_itemCount, 0);
    buf->unk_001C = 1;
    g_theGxDevicePtr->PrimIndexPtr(buf);
}

// PROVEN: bafaed16 IDENTICAL/IDENTICAL
// OFFSET: 0x00840E70/0x14056CE50
void GxTexUpdate(CGxTex* texId, int32_t minX, int32_t minY, int32_t maxX, int32_t maxY, int32_t immediate) {
    CiRect rect;
    rect.minY = minY;
    rect.minX = minX;
    rect.maxY = maxY;
    rect.maxX = maxX;
    g_theGxDevicePtr->TexMarkForUpdate(texId, rect, immediate);
}

// PROVEN: 07d313cf IDENTICAL/NONE
// OFFSET: 0x00841540
void GxPrimLockIndexPtr(EGxPrim primType, uint32_t indexCount, const uint16_t* indices) {
    s_lockPrimType = primType;
    s_lockIndexCount = indexCount;

    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), indexCount);
    g_theGxDevicePtr->BufData(buf, indices, buf->m_itemSize * buf->m_itemCount, 0);
    buf->unk_001C = 1;
    g_theGxDevicePtr->PrimIndexPtr(buf);
}

// PROVEN: 1a91e93b IDENTICAL/IDENTICAL
// OFFSET: 0x00840240/0x14056C090
void GxTexUpdate(CGxTex* texId, CiRect& updateRect, int32_t immediate) {
    g_theGxDevicePtr->TexMarkForUpdate(texId, updateRect, immediate);
}

// PROVEN: 2ae29a11 DIFFERENT/IDENTICAL
// OFFSET: 0x00840C90/0x14056CB90
int32_t GxTexCreate(uint32_t width, uint32_t height, EGxTexFormat format, CGxTexFlags flags, void* userArg, GxTexUserFunc userFunc, CGxTex*& texId) {
    texId = NULL;

    if (width > g_theGxDevicePtr->m_caps.m_texMaxSize[GxTex_2d] || height > g_theGxDevicePtr->m_caps.m_texMaxSize[GxTex_2d] || (width & (width - 1)) || (height & (height - 1)) || format > GxTexFormats_Last || (flags.m_filter == GxTex_Anisotropic && !g_theGxDevicePtr->m_caps.m_texFilterAnisotropic) || !userFunc || width < 8 || height < 8) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return g_theGxDevicePtr->TexCreate(GxTex_2d, width, height, 0, format, format, flags, userArg, userFunc, "Unknown", texId);
}

// OFFSET: 0x00840D30/0x14056CC80
int32_t GxTexCreate(EGxTexTarget target, uint32_t width, uint32_t height, uint32_t depth, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags flags, void* userArg, GxTexUserFunc userFunc, const char* name, CGxTex*& texId) {
    texId = NULL;

    if (target > GxTexTargets_Last || g_theGxDevicePtr->m_caps.m_texTarget[target] != 1 || width < 8 || height < 8 || width > g_theGxDevicePtr->m_caps.m_texMaxSize[target] || height > g_theGxDevicePtr->m_caps.m_texMaxSize[target]) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (target == GxTex_Rectangle) {
        if (flags.m_filter > GxTex_Linear) {
            SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
            return 0;
        }
    } else if (target != GxTex_NonPow2) {
        if ((width & (width - 1)) || (height & (height - 1))) {
            SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
            return 0;
        }
    }

    if (format > GxTexFormats_Last || (flags.m_filter == GxTex_Anisotropic && !g_theGxDevicePtr->m_caps.m_texFilterAnisotropic) || dataFormat > GxTexFormats_Last || !userFunc) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return g_theGxDevicePtr->TexCreate(target, width, height, depth, format, dataFormat, flags, userArg, userFunc, name, texId);
}

// OFFSET: 0x00840E30/0x14056CDF0
int32_t GxTexCreate(const CGxTexParms& parms, CGxTex*& texId) {
    return GxTexCreate(parms.m_target, parms.m_width, parms.m_height, parms.m_depth, parms.m_format, parms.m_dataFormat, parms.m_flags, parms.m_userArg, parms.m_userFunc, "", texId);
}

// INLINED
void GxPrimVertexPtr(CGxBuf* buf, EGxVertexBufferFormat format) {
    g_theGxDevicePtr->PrimVertexPtr(buf, format);
}

// OFFSET: 0x00840910/0x14056C670
void GxPrimVertexPtr(uint32_t vertexCount, const C3Vector* pos, uint32_t posStride, const C3Vector* normal, uint32_t normalStride, const CImVector* color, uint32_t colorStride, const C2Vector* tex0, uint32_t tex0Stride, const C2Vector* tex1, uint32_t tex1Stride) {
    EGxVertexBufferFormat format = GxVBF_PN;

    if (pos) {
        if (normal) {
            if (color) {
                if (!tex0 && !tex1) {
                    format = GxVBF_PNC;
                } else if (tex0 && !tex1) {
                    format = GxVBF_PNCT;
                } else if (tex0 && tex1) {
                    format = GxVBF_PNCT2;
                }
            } else {
                if (!tex0 && !tex1) {
                    format = GxVBF_PN;
                } else if (tex0 && !tex1) {
                    format = GxVBF_PNT;
                } else if (tex0 && tex1) {
                    format = GxVBF_PNT2;
                }
            }
        } else if (color) {
            if (tex0) {
                format = tex1 ? GxVBF_PCT2 : GxVBF_PCT;
            } else if (!tex1) {
                format = GxVBF_PC;
            }
        } else if (tex0) {
            format = tex1 ? GxVBF_PT2 : GxVBF_PT;
        } else if (!tex1) {
            format = GxVBF_P;
        }
    }

    const VertexBufDesc& desc = Buffer::s_vertexBufDesc[format];
    uint32_t vertexSize = desc.size;
    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, vertexSize, vertexCount);
    char* data = g_theGxDevicePtr->BufLock(buf);
    char* posDst = data + desc.attribPtrs[GxVA_Position]->m_offset;

    C3Vector emptyNormal(0.0f, 0.0f, 0.0f);
    CImVector emptyColor(0, 0, 0, 0);
    C2Vector emptyTex0(0.0f, 0.0f);
    C2Vector emptyTex1(0.0f, 0.0f);

    char* normalDst = normal ? data + desc.attribPtrs[GxVA_Normal]->m_offset : reinterpret_cast<char*>(&emptyNormal);
    uint32_t normalDstStride = normal ? vertexSize : 0;
    char* colorDst = color ? data + desc.attribPtrs[GxVA_Color0]->m_offset : reinterpret_cast<char*>(&emptyColor);
    uint32_t colorDstStride = color ? vertexSize : 0;
    char* tex0Dst = tex0 ? data + desc.attribPtrs[GxVA_TexCoord0]->m_offset : reinterpret_cast<char*>(&emptyTex0);
    uint32_t tex0DstStride = tex0 ? vertexSize : 0;
    char* tex1Dst = tex1 ? data + desc.attribPtrs[GxVA_TexCoord1]->m_offset : reinterpret_cast<char*>(&emptyTex1);
    uint32_t tex1DstStride = tex1 ? vertexSize : 0;

    if (!normal) {
        normal = &emptyNormal;
        normalStride = 0;
    }

    if (!color) {
        color = &emptyColor;
        colorStride = 0;
    }

    if (!tex0) {
        tex0 = &emptyTex0;
        tex0Stride = 0;
    }

    if (!tex1) {
        tex1 = &emptyTex1;
        tex1Stride = 0;
    }

    for (uint32_t i = 0; i < vertexCount; i++) {
        *reinterpret_cast<C3Vector*>(posDst) = *pos;
        pos = reinterpret_cast<const C3Vector*>(reinterpret_cast<const char*>(pos) + posStride);
        posDst += vertexSize;

        *reinterpret_cast<C3Vector*>(normalDst) = *normal;
        normal = reinterpret_cast<const C3Vector*>(reinterpret_cast<const char*>(normal) + normalStride);
        normalDst += normalDstStride;

        CImVector c;

        if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
            c.b = color->r;
            c.g = color->g;
            c.r = color->b;
            c.a = color->a;
        } else {
            c = *color;
        }

        *reinterpret_cast<CImVector*>(colorDst) = c;
        color = reinterpret_cast<const CImVector*>(reinterpret_cast<const char*>(color) + colorStride);
        colorDst += colorDstStride;

        *reinterpret_cast<C2Vector*>(tex0Dst) = *tex0;
        tex0Dst += tex0DstStride;
        tex0 = reinterpret_cast<const C2Vector*>(reinterpret_cast<const char*>(tex0) + tex0Stride);

        *reinterpret_cast<C2Vector*>(tex1Dst) = *tex1;
        tex1 = reinterpret_cast<const C2Vector*>(reinterpret_cast<const char*>(tex1) + tex1Stride);
        tex1Dst += tex1DstStride;
    }

    g_theGxDevicePtr->BufUnlock(buf, vertexCount * vertexSize);
    buf->unk_001C = 1;
    g_theGxDevicePtr->PrimVertexPtr(buf, format);
}

// PROVEN: ac687f8c IDENTICAL/IDENTICAL
// OFFSET: 0x008414A0/0x14056D7E0
void GxPrimLockVertexPtrs(uint32_t vertexCount, const C3Vector* pos, uint32_t posStride, const C3Vector* normal, uint32_t normalStride, const CImVector* color, uint32_t colorStride, const uint8_t* bone, uint32_t boneStride, const C2Vector* tex0, uint32_t tex0Stride, const C2Vector* tex1, uint32_t tex1Stride) {
    s_lockVertexCount = vertexCount;
    GxPrimVertexPtr(vertexCount, pos, posStride, normal, normalStride, color, colorStride, tex0, tex0Stride, tex1, tex1Stride);
}

// PROVEN: c0677a59 IDENTICAL/NONE
// OFFSET: 0x0040A240
void GxRsSet(EGxRenderState which, void* value) {
    g_theGxDevicePtr->RsSet(which, value);
}

// PROVEN: 6e1bbc79 IDENTICAL/NONE
// OFFSET: 0x0040B070
void GxRsSet(EGxRenderState which, CGxShader* shader, uint32_t permute) {
    g_theGxDevicePtr->RsSet(which, &shader->m_permutes[permute]);
}

// OFFSET: 0x0040B010
int32_t GxShaderValid(CGxShader* shader, uint32_t permute) {
    return shader ? shader->m_permutes[permute].Valid() : 0;
}

// PROVEN: 2f443e3a IDENTICAL/DIFFERENT
// OFFSET: 0x0040B030/0x14000DA10
void GxRsSet(EGxRenderState which, int32_t value) {
    if (!g_theGxDevicePtr->m_context) {
        return;
    }

    CGxAppRenderState& state = g_theGxDevicePtr->m_appRenderStates[which];

    if (state.m_value.m_data.p != reinterpret_cast<void*>(static_cast<intptr_t>(value))) {
        g_theGxDevicePtr->IRsDirty(which);
        state.m_value.m_data.p = reinterpret_cast<void*>(static_cast<intptr_t>(value));
    }
}

// OFFSET: 0x0040B0A0/0x14000D9A0
void GxUpdateAlphaRef(EGxRenderState which) {
    if (which != GxRs_AlphaRef || !g_theGxDevicePtr->m_context) {
        return;
    }

    int32_t value = CGxDevice::s_alphaRef[g_theGxDevicePtr->m_appRenderStates[GxRs_BlendingMode].m_value.m_data.i[0]];
    CGxAppRenderState& state = g_theGxDevicePtr->m_appRenderStates[GxRs_AlphaRef];

    if (state.m_value.m_data.i[0] != value) {
        g_theGxDevicePtr->IRsDirty(GxRs_AlphaRef);
        state.m_value.m_data.i[0] = value;
    }
}

// OFFSET: 0x00430C20
void GxRsSet(EGxRenderState which, float value) {
    if (!g_theGxDevicePtr->m_context) {
        return;
    }

    CGxAppRenderState& state = g_theGxDevicePtr->m_appRenderStates[which];

    if (value != state.m_value.m_data.f[0]) {
        g_theGxDevicePtr->IRsDirty(which);
        state.m_value.m_data.f[0] = value;
    }
}

// PROVEN: 3f48e3a1 IDENTICAL/UNPROVEN
// OFFSET: 0x008402A0/0x14056C0E0
void GxTexSetFlags(CGxTex* texId, CGxTexFlags flags) {
    g_theGxDevicePtr->TexSetFlags(texId, flags);
}

// PROVEN: 26292652 IDENTICAL/UNPROVEN
// OFFSET: 0x008402C0/0x14056C100
void GxTexSetWrap(CGxTex* texId, EGxTexWrapMode wrapU, EGxTexWrapMode wrapV) {
    g_theGxDevicePtr->TexSetWrap(texId, wrapU, wrapV);
}

// PROVEN: 89f4259f IDENTICAL/NONE
// OFFSET: 0x00840670
void GxLogOpen() {
    CGxDevice::LogOpen();
}

// PROVEN: c00a984f IDENTICAL/NONE
// OFFSET: 0x00840680/0x14056C4A0
void GxLogClose() {
    CGxDevice::LogClose();
}

// PROVEN: ba2cc9bf IDENTICAL/UNPROVEN
// OFFSET: 0x00840690/0x14056C4B0
void GxLog(const char* format, ...) {
    va_list args;
    va_start(args, format);
    CGxDevice::LogV(format, args);
    va_end(args);
}

// OFFSET: 0x008411D0/0x14056D3E0
void GxLogCrashInfo(char* buffer, uint32_t size) {
    SStrPack(buffer, "\r\n----------------------------------------\r\n", STORM_MAX_STR);
    SStrPack(buffer, "               GxInfo\r\n", STORM_MAX_STR);
    SStrPack(buffer, "----------------------------------------\r\n", STORM_MAX_STR);

    if (!g_theGxDevicePtr) {
        SStrPack(buffer, "No GX Device Created\r\n", size);
        CGxDevice::AdapterLogCrashInfo(buffer, size);
        return;
    }

    const CGxCaps& caps = g_theGxDevicePtr->m_caps;

    char line[260];
    SStrPrintf(line, sizeof(line), "GxApi: %s\r\n", g_gxApiNames[g_theGxDevicePtr->DeviceApi()]);
    SStrPack(buffer, line, size);

    const char* model = "Unknown";

    if (static_cast<uint32_t>(caps.m_shaderModel) <= 7) {
        model = g_gxShaderModelNames[caps.m_shaderModel];
    }

    SStrPrintf(line, sizeof(line), "Shader Model: %s\r\n", model);
    SStrPack(buffer, line, size);

    for (uint32_t i = 0; i < GxShTargets_Last; i++) {
        if (static_cast<uint32_t>(caps.m_shaderTargets[i] - 1) <= GxShProfiles_Last - 2) {
            SStrPrintf(line, sizeof(line), "  %s: %s\r\n", g_gxShaderTargetNames[i], CGxDevice::s_shaderProfileNames[caps.m_shaderTargets[i]]);
            SStrPack(buffer, line, size);
        }
    }

    g_theGxDevicePtr->LogCrashInfo(buffer, size);
    SStrPack(buffer, "\r\n", size);
}

// PROVEN: 6db61e72 DIFFERENT/IDENTICAL
// OFFSET: 0x00620390/0x1402AF700
void GxBufDestroy(CGxBuf* buf) {
    if (buf && buf != g_theGxDevicePtr->BufStream(buf->m_pool->m_target, 0, 0)) {
        g_theGxDevicePtr->BufDestroy(buf);
    }
}

// OFFSET: 0x00740630
void GxFormatColor(CImVector& color) {
    if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
        CImVector formatted(color.r, color.g, color.b, color.a);
        color = formatted;
    }
}

// OFFSET: 0x004DFBA0
void GxRenderTargetGet(EGxBuffer buffer, CGxTex*& texture) {
    g_theGxDevicePtr->DeviceGetRenderTarget(buffer, texture);
}

// PROVEN: cb66fab0 IDENTICAL/DIFFERENT
// OFFSET: 0x004370B0/0x140040C90
void GxRenderTargetSet(EGxBuffer buffer, CGxTex* texture, uint32_t plane) {
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->DeviceSetRenderTarget(buffer, texture, plane);
}

// PROVEN: 096f35ac IDENTICAL/DIFFERENT
// OFFSET: 0x004370B0/0x140040C90
void GxFrameBufferSet(CGxFrameBuffer* frameBuffer) {
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->FrameBufferSet(frameBuffer);
}

// PROVEN: 727d0ff0 IDENTICAL/IDENTICAL
// OFFSET: 0x00435470/0x14003EB90
CGxFrameBuffer* GxFrameBufferCreate(CGxTex* depthTex, CGxTex* colorTex) {
    CGxFrameBuffer* frameBuffer = g_theGxDevicePtr->FrameBufferCreate();
    g_theGxDevicePtr->FrameBufferAttach(frameBuffer, GxBuffers_Depth, depthTex, 0);
    g_theGxDevicePtr->FrameBufferAttach(frameBuffer, GxBuffers_Color, colorTex, 0);

    if (!g_theGxDevicePtr->FrameBufferFinalize(frameBuffer)) {
        g_theGxDevicePtr->FrameBufferDelete(frameBuffer);
    }

    return frameBuffer;
}

// PROVEN: 41b44980 IDENTICAL/NONE
// OFFSET: 0x00431C60
void GxXformSet(EGxXform xf, const C44Matrix& matrix) {
    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[xf];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level] = matrix;
}

// PROVEN: 70dce4f2 IDENTICAL/NONE
// OFFSET: 0x00481C90
void GxReadPixels(CiRect& rect, TSGrowableArray<CImVector>& pixels) {
    g_theGxDevicePtr->DeviceReadPixels(rect, pixels);
}

// PROVEN: d7f158e1 IDENTICAL/NONE
// OFFSET: 0x008403C0/0x14056C210
int32_t GxTexStretch(CGxTex* source, CGxTex* dest, const CiRect* destRect, const CiRect* sourceRect, uint32_t level, uint32_t face) {
    return g_theGxDevicePtr->TexStretch(source, dest, destRect, sourceRect, level, face);
}

// PROVEN: 14e624c4 IDENTICAL/NONE
// OFFSET: 0x008404F0/0x14056C330
int32_t GxQueryBegin(CGxQuery* query) {
    return g_theGxDevicePtr->QueryBegin(query);
}

// PROVEN: f82379f0 IDENTICAL/NONE
// OFFSET: 0x00840510/0x14056C350
int32_t GxQueryEnd(CGxQuery* query) {
    return g_theGxDevicePtr->QueryEnd(query);
}

// PROVEN: 66f16c38 IDENTICAL/NONE
// OFFSET: 0x00840530/0x14056C370
int32_t GxQueryGetParam(CGxQuery* query, EGxQueryParam param, uint32_t* data) {
    return g_theGxDevicePtr->QueryGetParam(query, param, data);
}

// PROVEN: 8b269a24 IDENTICAL/NONE
// OFFSET: 0x00840560/0x14056C390
int32_t GxQueryGetData(CGxQuery* query, uint32_t* data) {
    return g_theGxDevicePtr->QueryGetData(query, data);
}

// PROVEN: 8c922b3d IDENTICAL/IDENTICAL
// OFFSET: 0x008414E0/0x14056D850
void GxDrawLockedElements(EGxPrim primType, uint32_t indexCount, const uint16_t* indices) {
    if (!s_lockVertexCount) {
        return;
    }

    GxPrimIndexPtr(indexCount, indices);

    CGxBatch batch;
    batch.m_primType = primType;
    batch.m_start = 0;
    batch.m_count = indexCount;
    batch.m_minIndex = 0;
    batch.m_maxIndex = static_cast<uint16_t>(s_lockVertexCount - 1);
    g_theGxDevicePtr->Draw(batch);
}

// OFFSET: 0x006129D0/0x14029DC40
static void GxuPreprocessDrawList(EGxuDrawListCategory category, EGxuDrawListSorting sorting, const C3Vector& viewPos) {
    for (uint32_t i = 0; i < s_drawLists[category].Count(); i++) {
        s_drawListPreprocess[sorting](&s_drawLists[category][i], viewPos);
    }
}

// OFFSET: 0x00612A20/0x14029DCD0
void GxuSortDrawList(EGxuDrawListCategory category, EGxuDrawListSorting sorting, const C3Vector& viewPos) {
    GxuPreprocessDrawList(category, sorting, viewPos);
    qsort(s_drawLists[category].Ptr(), s_drawLists[category].Count(), sizeof(GxDrawListEntry), s_drawListSorters[sorting]);
}

// PROVEN: e52cfee1 IDENTICAL/NONE
// OFFSET: 0x00612F80
void GxuClearDrawList(EGxuDrawListCategory category) {
    s_drawListBusy = true;

    for (uint32_t i = 0; i < s_drawLists[category].Count(); i++) {
        s_drawLists[category][i].m_item->Discard();
    }

    s_drawLists[category].SetCount(0);
    s_drawListBusy = false;
}

// PROVEN: 67a59497 IDENTICAL/DIFFERENT
// OFFSET: 0x00613040/0x14029E570
void GxuFlushDrawList(EGxuDrawListCategory category, const C3Vector& viewPos) {
    GxuDrawListState* current = NULL;

    for (uint32_t i = 0; i < s_drawLists[category].Count(); i++) {
        GxuDrawListItem* item = s_drawLists[category][i].m_item;
        GxuDrawListState* state = item->GetState();

        if (state && state != current) {
            if (current) {
                current->End();
            }

            state->Begin();
            current = state;
        }

        item->Draw(viewPos);
    }

    if (current) {
        current->End();
    }

    GxuClearDrawList(category);
}

// PROVEN: e22774e3 IDENTICAL/IDENTICAL
// OFFSET: 0x008401D0/0x14056C000
void GxScenePresent(void* window) {
    g_theGxDevicePtr->ScenePresent(window);
}

// PROVEN: d1f6e7b8 IDENTICAL/IDENTICAL
// OFFSET: 0x00840160/0x14056BF80
void GxDevDestroy() {
    g_theGxDevicePtr->DeviceDestroy();
    delete g_theGxDevicePtr;
    g_theGxDevicePtr = NULL;
}

// PROVEN: 373b8e21 IDENTICAL/IDENTICAL
// OFFSET: 0x008401B0/0x14056BFE0
void GxPrimUnlockVertexPtrs() {
    s_lockVertexCount = 0;
}

// INLINED
GxDrawListEntry::GxDrawListEntry() {
    this->m_item = NULL;
    this->m_depth = 0.0f;
    this->m_vtxStream = 0;
}

// OFFSET: 0x004DFBE0
CImVector GxSwizzleColor(const CImVector& color) {
    CImVector result;

    if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
        result.b = color.r;
        result.g = color.g;
        result.r = color.b;
        result.a = color.a;
    } else {
        result = color;
    }

    return result;
}
