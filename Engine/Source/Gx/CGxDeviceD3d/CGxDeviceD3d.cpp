#include <cmath>
#include <cstdint>
#include <cstring>
#include <malloc.h>
#include <new>
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"
#include "Engine/Source/Gx/CGxDevice/Win32/GxWindow.h"
#include "Engine/Source/Gx/Blit.h"
#include "Engine/Source/Gx/Buffer.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Os/Error.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "ThirdParty/nvapi/nvapi.h"

typedef IDirect3D9* (WINAPI* Direct3DCreate9Proc)(UINT sdkVersion);

static void* NvQueryInterface(NvU32 id) {
    typedef void* (__cdecl* QueryInterfaceProc)(NvU32 id);

    HMODULE nvapi = GetModuleHandleA("nvapi.dll");

    if (!nvapi) {
        return NULL;
    }

    QueryInterfaceProc queryInterface = reinterpret_cast<QueryInterfaceProc>(GetProcAddress(nvapi, "nvapi_QueryInterface"));

    return queryInterface ? queryInterface(id) : NULL;
}

static NvAPI_Status NvD3d9GetTextureHandle(IDirect3DBaseTexture9* texture, NVDX_ObjectHandle* handle) {
    typedef NvAPI_Status (__cdecl* Proc)(IDirect3DBaseTexture9*, NVDX_ObjectHandle*);

    Proc proc = reinterpret_cast<Proc>(NvQueryInterface(0xC7985ED5));

    return proc ? proc(texture, handle) : NVAPI_NO_IMPLEMENTATION;
}

static NvAPI_Status NvD3d9GetCurrentZBufferHandle(IDirect3DDevice9* device, NVDX_ObjectHandle* handle) {
    typedef NvAPI_Status (__cdecl* Proc)(IDirect3DDevice9*, NVDX_ObjectHandle*);

    Proc proc = reinterpret_cast<Proc>(NvQueryInterface(0xB380F218));

    return proc ? proc(device, handle) : NVAPI_NO_IMPLEMENTATION;
}

static NvAPI_Status NvD3d9StretchRect(IDirect3DDevice9* device, NVDX_ObjectHandle source, const RECT* sourceRect, NVDX_ObjectHandle dest, const RECT* destRect, D3DTEXTUREFILTERTYPE filter) {
    typedef NvAPI_Status (__cdecl* Proc)(IDirect3DDevice9*, NVDX_ObjectHandle, const RECT*, NVDX_ObjectHandle, const RECT*, D3DTEXTUREFILTERTYPE);

    Proc proc = reinterpret_cast<Proc>(NvQueryInterface(0xAEAECD41));

    return proc ? proc(device, source, sourceRect, dest, destRect, filter) : NVAPI_NO_IMPLEMENTATION;
}

// STATIC: %0x00AFD658
const D3DFORMAT CGxDeviceD3d::s_GxFormatToD3dFormat[CGxFormat::Formats_Last] = { D3DFMT_R5G6B5, D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_A2R10G10B10, D3DFMT_D16, D3DFMT_D24X8, D3DFMT_D24S8, D3DFMT_D32 };

// STATIC: %0x00AFD678
const uint32_t CGxDeviceD3d::s_gxAttribToD3dAttribSize[7] = { 4, 4, 4, 8, 12, 4, 4 };

// STATIC: %0x00AFD698
static const D3DQUERYTYPE s_d3dQueryTypes[1] = { D3DQUERYTYPE_OCCLUSION };

// STATIC: %0x00AFE010
const D3DFORMAT CGxDeviceD3d::s_GxTexFmtToD3dFmt[GxTexFormats_Last] = { D3DFMT_UNKNOWN, D3DFMT_A8B8G8R8, D3DFMT_A8R8G8B8, D3DFMT_A4R4G4B4, D3DFMT_A1R5G5B5, D3DFMT_R5G6B5, D3DFMT_V8U8, D3DFMT_DXT1, D3DFMT_DXT3, D3DFMT_DXT5, D3DFMT_L16, D3DFMT_G16R16F, D3DFMT_A16B16G16R16F, D3DFMT_UNKNOWN, D3DFMT_R32F, D3DFMT_A32B32G32R32F, static_cast<D3DFORMAT>(MAKEFOURCC('N', 'U', 'L', 'L')), D3DFMT_D24X8, D3DFMT_D16, D3DFMT_D32, static_cast<D3DFORMAT>(MAKEFOURCC('I', 'N', 'T', 'Z')), static_cast<D3DFORMAT>(MAKEFOURCC('R', 'A', 'W', 'Z')), static_cast<D3DFORMAT>(MAKEFOURCC('R', 'E', 'S', 'Z')), static_cast<D3DFORMAT>(MAKEFOURCC('D', 'F', '2', '4')) };

// STATIC: %0x00AFE070
const D3DCUBEMAP_FACES CGxDeviceD3d::s_faceTypes[6] = { D3DCUBEMAP_FACE_POSITIVE_X, D3DCUBEMAP_FACE_NEGATIVE_X, D3DCUBEMAP_FACE_POSITIVE_Y, D3DCUBEMAP_FACE_NEGATIVE_Y, D3DCUBEMAP_FACE_POSITIVE_Z, D3DCUBEMAP_FACE_NEGATIVE_Z };

// STATIC: %0x00AFE118
const D3DPRIMITIVETYPE CGxDeviceD3d::s_primitiveConversion[5] = { D3DPT_POINTLIST, D3DPT_LINELIST, D3DPT_LINESTRIP, D3DPT_TRIANGLELIST, D3DPT_TRIANGLESTRIP };

// STATIC: %0x00AFE12C
const D3DDECLUSAGE CGxDeviceD3d::s_gxAttribToD3dAttribUsage[GxVAs_Last] = { D3DDECLUSAGE_POSITION, D3DDECLUSAGE_BLENDWEIGHT, D3DDECLUSAGE_BLENDINDICES, D3DDECLUSAGE_NORMAL, D3DDECLUSAGE_COLOR, D3DDECLUSAGE_COLOR, D3DDECLUSAGE_TEXCOORD, D3DDECLUSAGE_TEXCOORD, D3DDECLUSAGE_TEXCOORD, D3DDECLUSAGE_TEXCOORD, D3DDECLUSAGE_TEXCOORD, D3DDECLUSAGE_TEXCOORD, D3DDECLUSAGE_TEXCOORD, D3DDECLUSAGE_TEXCOORD };

// STATIC: %0x00AFE164
const uint32_t CGxDeviceD3d::s_gxAttribToD3dAttribUsageIndex[GxVAs_Last] = { 0, 0, 0, 0, 0, 1, 0, 1, 2, 3, 4, 5, 6, 7 };

// STATIC: %0x00AFE1A4
const D3DBLEND CGxDeviceD3d::s_srcBlend[13] = { D3DBLEND_ONE, D3DBLEND_ONE, D3DBLEND_SRCALPHA, D3DBLEND_SRCALPHA, D3DBLEND_DESTCOLOR, D3DBLEND_DESTCOLOR, D3DBLEND_DESTCOLOR, D3DBLEND_INVSRCALPHA, D3DBLEND_INVSRCALPHA, D3DBLEND_SRCALPHA, D3DBLEND_ONE, D3DBLEND_BLENDFACTOR, D3DBLEND_INVDESTCOLOR };

// STATIC: %0x00AFE1D8
const D3DBLEND CGxDeviceD3d::s_dstBlend[13] = { D3DBLEND_ZERO, D3DBLEND_ZERO, D3DBLEND_INVSRCALPHA, D3DBLEND_ONE, D3DBLEND_ZERO, D3DBLEND_SRCCOLOR, D3DBLEND_ONE, D3DBLEND_ONE, D3DBLEND_ZERO, D3DBLEND_ZERO, D3DBLEND_ONE, D3DBLEND_INVBLENDFACTOR, D3DBLEND_ONE };

// STATIC: %0x00AFE20C
const D3DTEXTUREADDRESS CGxDeviceD3d::s_wrapModes[3] = { D3DTADDRESS_CLAMP, D3DTADDRESS_WRAP, D3DTADDRESS_MIRROR };

// STATIC: %0x00AFE218
static const D3DTEXTUREOP s_texOp[6] = { D3DTOP_MODULATE, D3DTOP_MODULATE2X, D3DTOP_ADD, D3DTOP_SELECTARG2, D3DTOP_BLENDCURRENTALPHA, D3DTOP_BLENDDIFFUSEALPHA };

// STATIC: %0x00AFE230
static const uint32_t s_texArgs[6][2] = { { D3DTA_TEXTURE, D3DTA_CURRENT }, { D3DTA_TEXTURE, D3DTA_CURRENT }, { D3DTA_TEXTURE, D3DTA_CURRENT }, { D3DTA_TEXTURE, D3DTA_CURRENT }, { D3DTA_CURRENT, D3DTA_TEXTURE }, { D3DTA_TEXTURE, D3DTA_CURRENT } };

// STATIC: %0x00AFE260
const D3DCMPFUNC CGxDeviceD3d::s_cmpFunc[4] = { D3DCMP_LESSEQUAL, D3DCMP_EQUAL, D3DCMP_GREATEREQUAL, D3DCMP_LESS };

// STATIC: %0x00AFE270
const D3DCULL CGxDeviceD3d::s_cullMode[3] = { D3DCULL_NONE, D3DCULL_CW, D3DCULL_CCW };

// STATIC: %0x00C0E928
EGxTexFormat CGxDeviceD3d::s_GxTexFormatToUse[GxTexFormats_Last] = { GxTex_Unknown, GxTex_Argb8888, GxTex_Argb8888, GxTex_Argb4444, GxTex_Argb1555, GxTex_Rgb565, GxTex_Uv88, GxTex_Dxt1, GxTex_Dxt3, GxTex_Dxt5, GxTex_R16, GxTex_Gr1616F, GxTex_Abgr16161616F, GxTex_R24X8, GxTex_R32F, GxTex_Abgr32323232F, GxTex_Null, GxTex_D24X8, GxTex_D16, GxTex_D32, GxTex_Intz, GxTex_Rawz, GxTex_Resz, GxTex_AtiD24 };

// STATIC: %0x00C0E988
D3DDECLTYPE CGxDeviceD3d::s_gxAttribToD3dAttribType[7] = { D3DDECLTYPE_D3DCOLOR, D3DDECLTYPE_UBYTE4, D3DDECLTYPE_UBYTE4N, D3DDECLTYPE_FLOAT2, D3DDECLTYPE_FLOAT3, D3DDECLTYPE_SHORT2, D3DDECLTYPE_FLOAT1 };

// STATIC: %0x00C0E9A8
D3DTEXTUREFILTERTYPE CGxDeviceD3d::s_filterModes[GxTexFilters_Last][3] = { { D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE }, { D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_NONE }, { D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT }, { D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_POINT }, { D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR }, { D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR } };

// STATIC: %0x00C0E9F0
static uint32_t s_normalizeNormals = 5;

// STATIC: %0x00D931B8
static D3DLIGHT9 s_d3dLight;

// STATIC: %0x00D93220
static int32_t s_isIdent;

// OFFSET: %0x007C9020
void CGxDeviceD3d::IBufUnlock(CGxBuf* buf) {
    CGxPool* pool = buf->m_pool;
    EmergencyMem& emergencyMem = this->m_emergencyMem[pool->m_target];

    if (emergencyMem.m_lock) {
        emergencyMem.Unlock();
        buf->unk_001D = 0;
        this->m_intF5C = 1;
        return;
    }

    HRESULT result = S_OK;

    switch (pool->m_target) {
        case GxPoolTarget_Vertex:
            result = static_cast<LPDIRECT3DVERTEXBUFFER9>(pool->m_apiSpecific)->Unlock();
            break;

        case GxPoolTarget_Index:
            result = static_cast<LPDIRECT3DINDEXBUFFER9>(pool->m_apiSpecific)->Unlock();
            break;

        default:
            break;
    }

    buf->unk_001D = SUCCEEDED(result);
}

// PROVEN: f1fc1769 IDENTICAL/IDENTICAL
// OFFSET: 0x0084CC40/0x14057B5E0
int32_t CGxDeviceD3d::BufUnlock(CGxBuf* buf, uint32_t size) {
    CGxDevice::BufUnlock(buf, size);
    this->IBufUnlock(buf);

    return 1;
}

// OFFSET: %0x007C90D0
char* CGxDeviceD3d::IBufLock(CGxBuf* buf) {
    CGxPool* pool = buf->m_pool;

    if (!this->m_context) {
        return static_cast<char*>(this->m_emergencyMem[pool->m_target].Lock(buf->m_size));
    }

    DWORD flags = 0;

    if (pool->m_usage == GxPoolUsage_Stream) {
        uint32_t index = (buf->m_itemSize + pool->unk_001C - 1) - (buf->m_itemSize + pool->unk_001C - 1) % buf->m_itemSize;

        if (index + buf->m_size <= static_cast<uint32_t>(pool->m_size)) {
            flags = D3DLOCK_NOOVERWRITE;
        } else {
            flags = D3DLOCK_DISCARD;
            pool->Discard();
            index = 0;
        }

        buf->m_index = index / buf->m_itemSize;
        pool->unk_001C = index + buf->m_size;
    } else if (pool->m_usage == GxPoolUsage_Dynamic) {
        flags = D3DLOCK_NOOVERWRITE;
    }

    char* data = NULL;

    if (!pool->m_apiSpecific) {
        this->CreatePoolAPI(pool);
    }

    if (!pool->m_apiSpecific) {
        return static_cast<char*>(this->m_emergencyMem[pool->m_target].Lock(buf->m_size));
    }

    HRESULT result = S_OK;

    switch (pool->m_target) {
        case GxPoolTarget_Vertex:
            result = static_cast<LPDIRECT3DVERTEXBUFFER9>(pool->m_apiSpecific)->Lock(buf->m_itemSize * buf->m_index, buf->m_size, reinterpret_cast<void**>(&data), flags);
            break;

        case GxPoolTarget_Index:
            result = static_cast<LPDIRECT3DINDEXBUFFER9>(pool->m_apiSpecific)->Lock(buf->m_itemSize * buf->m_index, buf->m_size, reinterpret_cast<void**>(&data), flags);
            break;

        default:
            break;
    }

    if (SUCCEEDED(result) && buf->m_size) {
        __try {
            volatile char* probe = data;

            if (pool->m_usage == GxPoolUsage_Stream) {
                *probe = 0;
            } else {
                *probe = *probe;
            }
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            result = E_FAIL;
        }
    }

    if (FAILED(result)) {
        this->IBufUnlock(buf);
        return static_cast<char*>(this->m_emergencyMem[pool->m_target].Lock(buf->m_size));
    }

    return data;
}

// PROVEN: 93c0b8b7 IDENTICAL/IDENTICAL
// OFFSET: 0x0084CE40/0x14057B800
char* CGxDeviceD3d::BufLock(CGxBuf* buf) {
    CGxDevice::BufLock(buf);

    return this->IBufLock(buf);
}

// PROVEN: 3de90d6d IDENTICAL/NONE
// OFFSET: 0x0084CE60
void CGxDeviceD3d::BufData(CGxBuf* buf, const void* data, uint32_t size, uint32_t offset) {
    CGxDevice::BufData(buf, data, size, offset);

    char* dst = this->IBufLock(buf);
    memcpy(dst + offset, data, size);
    this->IBufUnlock(buf);
}

// PROVEN: 4edf6dd1 IDENTICAL/IDENTICAL
// OFFSET: 0x0084AF40/0x1405795E0
void CGxDeviceD3d::IUnloadD3dLib(HINSTANCE& d3dLib, LPDIRECT3D9& d3d) {
    if (d3d) {
        d3d->Release();
        d3d = NULL;
    }

    if (d3dLib) {
        FreeLibrary(d3dLib);
        d3dLib = NULL;
    }
}

// PROVEN: dafcdb33 IDENTICAL/NONE
// OFFSET: 0x0084B060
LPDIRECT3DINDEXBUFFER9 CGxDeviceD3d::ICreateD3dIB(EGxPoolUsage usage, uint32_t size) {
    D3DPOOL d3dPool = D3DPOOL_MANAGED;
    DWORD d3dUsage = this->m_d3dIsHwDevice ? D3DUSAGE_WRITEONLY : D3DUSAGE_SOFTWAREPROCESSING;

    if (usage == GxPoolUsage_Stream || usage == GxPoolUsage_Dynamic) {
        d3dUsage |= D3DUSAGE_DYNAMIC;
        d3dPool = D3DPOOL_DEFAULT;
    }

    LPDIRECT3DINDEXBUFFER9 indexBuffer = NULL;
    HRESULT hr = this->m_d3dDevice->CreateIndexBuffer(size, d3dUsage, D3DFMT_INDEX16, d3dPool, &indexBuffer, NULL);

    return SUCCEEDED(hr) ? indexBuffer : NULL;
}

// PROVEN: 132cee7b IDENTICAL/NONE
// OFFSET: 0x0084B120
void CGxDeviceD3d::IReleaseD3dPool(CGxPool* pool) {
    if (pool->m_usage == GxPoolUsage_Stream) {
        pool->unk_001C = 0;
    }

    pool->Invalidate();

    if (pool->m_apiSpecific) {
        switch (pool->m_target) {
            case GxPoolTarget_Vertex:
                static_cast<LPDIRECT3DVERTEXBUFFER9>(pool->m_apiSpecific)->Release();
                break;

            case GxPoolTarget_Index:
                static_cast<LPDIRECT3DINDEXBUFFER9>(pool->m_apiSpecific)->Release();
                break;

            default:
                break;
        }

        pool->m_apiSpecific = NULL;
    }
}

// OFFSET: %0x007C77A0
void CGxDeviceD3d::DeviceEvictResources() {
    if (!this->m_context || !this->m_d3dDevice) {
        return;
    }

    HRESULT hr = this->m_d3dDevice->EvictManagedResources();

    if (FAILED(hr) && hr == D3DERR_DRIVERINTERNALERROR) {
        SErrDisplayError(STORM_ERROR_FATAL_CONDITION, __FILE__, __LINE__, "Driver internal error on EvictManagedResources().", 0, 1, 0, 0x11111111);
    }
}

// PROVEN: 6ee5017b IDENTICAL/IDENTICAL
// OFFSET: 0x0084B450/0x140579AB0
void CGxDeviceD3d::DeviceSetGamma(float gamma) {
    CGxDevice::DeviceSetGamma(gamma);

    if ((this->m_d3dCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA) && !this->IDevIsWindowed()) {
        this->m_d3dDevice->SetGammaRamp(0, D3DSGR_NO_CALIBRATION, reinterpret_cast<const D3DGAMMARAMP*>(&this->m_gammaRamp));
    }
}

// PROVEN: 1de44a45 IDENTICAL/NONE
// OFFSET: 0x0084B8E0
void CGxDeviceD3d::PoolDestroy(CGxPool* pool) {
    if (pool) {
        this->IReleaseD3dPool(pool);
        CGxDevice::PoolDestroy(pool);
    }
}

// PROVEN: 801562a5 IDENTICAL/UNPROVEN
// OFFSET: 0x0084B910/0x14057A0B0
void CGxDeviceD3d::CursorSetVisible(int32_t visible) {
    CGxDevice::CursorSetVisible(visible);

    if (this->m_hwCursor && this->m_context) {
        POINT point;
        GetCursorPos(&point);
        ScreenToClient(this->m_hwnd, &point);

        RECT rect;
        GetClientRect(this->m_hwnd, &rect);

        if (point.x >= rect.left && point.x < rect.right && point.y >= rect.top && point.y < rect.bottom) {
            this->m_d3dDevice->ShowCursor(CGxDevice::s_cursorVisible);
        }
    }
}

// PROVEN: ff58da27 IDENTICAL/NONE
// OFFSET: 0x0084B9A0
int32_t CGxDeviceD3d::ISetCursor() {
    if (!this->m_d3dDevice) {
        return 0;
    }

    SetCursor(NULL);
    this->m_d3dDevice->ShowCursor(CGxDevice::s_cursorVisible && this->m_hwCursor);

    return 1;
}

// PROVEN: b4211607 IDENTICAL/NONE
// OFFSET: 0x0084B9F0
void CGxDeviceD3d::CursorUnlock(uint32_t x, uint32_t y) {
    CGxDevice::CursorUnlock(x, y);
    this->m_hwCursorDirty = 1;
}

// OFFSET: %0x007C7E30
void CGxDeviceD3d::ICursorDraw() {
    if (!this->m_hwCursor) {
        this->ISceneBegin();
    }

    CGxDevice::ICursorDraw();

    if (!this->m_hwCursor) {
        this->ISceneEnd();
    }

    if (!this->m_hwCursor || !this->m_hwCursorDirty || !this->m_hwCursorBitmap) {
        return;
    }

    if (this->m_context) {
        D3DLOCKED_RECT lockedRect;

        if (SUCCEEDED(this->m_hwCursorBitmap->LockRect(&lockedRect, NULL, 0))) {
            for (uint32_t y = 0; y < 32; y++) {
                memcpy(static_cast<uint8_t*>(lockedRect.pBits) + y * lockedRect.Pitch, &CGxDevice::s_cursor[y * 32], 32 * sizeof(uint32_t));
            }

            this->m_hwCursorBitmap->UnlockRect();
            this->m_d3dDevice->SetCursorProperties(CGxDevice::s_cursorHotspotX, CGxDevice::s_cursorHotspotY, this->m_hwCursorBitmap);
        }
    }

    this->m_hwCursorDirty = 0;
}

// PROVEN: 9efc0c8d IDENTICAL/DIFFERENT
// OFFSET: 0x0084BB00/0x14057A300
void CGxDeviceD3d::ICursorCreate(const CGxFormat& format) {
    CGxDevice::ICursorCreate(format);

    if (this->m_hwCursor && !this->m_hwCursorTexture) {
        this->m_d3dDevice->CreateTexture(32, 32, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &this->m_hwCursorTexture, NULL);

        if (this->m_hwCursorTexture) {
            this->m_hwCursorTexture->GetSurfaceLevel(0, &this->m_hwCursorBitmap);
        }

        this->m_hwCursorDirty = 1;
        this->ICursorDraw();
    }
}

// OFFSET: %0x007C7FE0
void CGxDeviceD3d::StereoSetConvergence(float convergence) {
    this->m_d3dStereoDirty = 1;
    this->m_d3dStereoConvergence = convergence;
}

// OFFSET: %0x007C8010
void CGxDeviceD3d::StereoSetSeparation(float separation) {
    this->m_d3dStereoDirty = 1;
    this->m_d3dStereoSeparation = separation;
}

// PROVEN: c53b5934 IDENTICAL/NONE
// OFFSET: 0x0084BC40
void CGxDeviceD3d::QueryCreate(CGxQuery*& query, EGxQueryType type) {
    CGxDevice::QueryCreate(query, type);

    CGxQuery* gxQuery = query;
    LPDIRECT3DQUERY9 d3dQuery;

    if (!gxQuery->m_apiSpecific && this->m_d3dDevice->CreateQuery(s_d3dQueryTypes[gxQuery->m_type], &d3dQuery) == D3D_OK) {
        gxQuery->m_apiSpecific = d3dQuery;
    }
}

// PROVEN: 4c939c1c IDENTICAL/NONE
// OFFSET: 0x0084BC90
int32_t CGxDeviceD3d::QueryBegin(CGxQuery* query) {
    if (!this->m_context) {
        return 0;
    }

    LPDIRECT3DQUERY9 d3dQuery;

    if (!query->m_apiSpecific) {
        if (this->m_d3dDevice->CreateQuery(s_d3dQueryTypes[query->m_type], &d3dQuery) == D3D_OK) {
            query->m_apiSpecific = d3dQuery;
        }

        if (!query->m_apiSpecific) {
            return 0;
        }
    }

    HRESULT hr = D3D_OK;

    if (query->m_type == GxQueryType_Occlusion) {
        hr = static_cast<LPDIRECT3DQUERY9>(query->m_apiSpecific)->Issue(D3DISSUE_BEGIN);
    }

    return hr == D3D_OK;
}

// PROVEN: c65a65ce IDENTICAL/NONE
// OFFSET: 0x0084BD50
int32_t CGxDeviceD3d::QueryGetParam(CGxQuery* query, EGxQueryParam param, uint32_t* data) {
    if (!this->m_context || !query->m_apiSpecific) {
        return 0;
    }

    switch (param) {
        case GxQueryParam_Unk0:
            *data = static_cast<LPDIRECT3DQUERY9>(query->m_apiSpecific)->GetDataSize();
            return 1;

        case GxQueryParam_Unk1:
            *data = static_cast<LPDIRECT3DQUERY9>(query->m_apiSpecific)->GetData(NULL, 0, 0) == D3D_OK;
            return 0;

        default:
            return 0;
    }
}

// OFFSET: %0x007C8250
int32_t CGxDeviceD3d::ILoadD3dLib(HINSTANCE& d3dLib, LPDIRECT3D9& d3d) {
    d3dLib = NULL;
    d3d = NULL;
    d3dLib = LoadLibraryA("d3d9.dll");

    if (d3dLib) {
        Direct3DCreate9Proc d3dCreateProc = reinterpret_cast<Direct3DCreate9Proc>(GetProcAddress(d3dLib, "Direct3DCreate9"));

        if (d3dCreateProc) {
            d3d = d3dCreateProc(D3D_SDK_VERSION);

            if (d3d) {
                return 1;
            }

            CGxDevice::Log("CGxDeviceD3d::ILoadD3dLib(): unable to d3dCreateProc()");
        } else {
            CGxDevice::Log("CGxDeviceD3d::ILoadD3dLib(): unable to GetProcAddress()");
        }
    } else {
        CGxDevice::Log("CGxDeviceD3d::ILoadD3dLib(): unable to LoadLibrary()");
    }

    CGxDeviceD3d::IUnloadD3dLib(d3dLib, d3d);

    return 0;
}

// PROVEN: 4d0a11d4 IDENTICAL/DIFFERENT
// OFFSET: 0x0084B3E0/0x140579A40
void CGxDeviceD3d::ISetWindowFocus(int32_t focus) {
    this->m_windowFocus = focus;

    if (focus) {
        this->m_hwCursorDirty = 1;

        if (!this->m_format.m_window) {
            RECT rect;
            GetWindowRect(this->m_hwnd, &rect);
            ClipCursor(&rect);
        }
    }
}

// PROVEN: cc81e1bd IDENTICAL/NONE
// OFFSET: 0x0084AFF0
LPDIRECT3DVERTEXBUFFER9 CGxDeviceD3d::ICreateD3dVB(EGxPoolUsage usage, uint32_t size) {
    D3DPOOL d3dPool = D3DPOOL_MANAGED;
    DWORD d3dUsage = this->m_d3dIsHwDevice ? D3DUSAGE_WRITEONLY : D3DUSAGE_SOFTWAREPROCESSING;

    if (usage == GxPoolUsage_Stream || usage == GxPoolUsage_Dynamic) {
        d3dUsage |= D3DUSAGE_DYNAMIC;
        d3dPool = D3DPOOL_DEFAULT;
    }

    LPDIRECT3DVERTEXBUFFER9 vertexBuffer = NULL;
    HRESULT hr = this->m_d3dDevice->CreateVertexBuffer(size, d3dUsage, 0, d3dPool, &vertexBuffer, NULL);

    return SUCCEEDED(hr) ? vertexBuffer : NULL;
}

// PROVEN: e1737ac8 IDENTICAL/NONE
// OFFSET: 0x0084B0D0
int32_t CGxDeviceD3d::CreatePoolAPI(CGxPool* pool) {
    switch (pool->m_target) {
        case GxPoolTarget_Vertex:
            pool->m_apiSpecific = this->ICreateD3dVB(pool->m_usage, pool->m_size);
            break;

        case GxPoolTarget_Index:
            pool->m_apiSpecific = this->ICreateD3dIB(pool->m_usage, pool->m_size);
            break;

        default:
            break;
    }

    return 1;
}

// PROVEN: b53a0395 IDENTICAL/IDENTICAL
// OFFSET: 0x0084B4A0/0x140579B00
void CGxDeviceD3d::DeviceSetGamma(const CGxGammaRamp& ramp) {
    CGxDevice::DeviceSetGamma(ramp);

    if ((this->m_d3dCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA) && !this->IDevIsWindowed()) {
        this->m_d3dDevice->SetGammaRamp(0, D3DSGR_NO_CALIBRATION, reinterpret_cast<const D3DGAMMARAMP*>(&this->m_gammaRamp));
    }
}

// PROVEN: 65b9fc6c IDENTICAL/IDENTICAL
// OFFSET: 0x0084B4F0/0x140579B50
void* CGxDeviceD3d::DeviceWindow() {
    return this->m_hwnd;
}

// PROVEN: f3b779a8 IDENTICAL/IDENTICAL
// OFFSET: 0x0084B500/0x140579B60
void CGxDeviceD3d::DeviceOverride(EGxOverride override, uint32_t value) {
    CGxDevice::DeviceOverride(override, value);

    if (override == GxOverride_Unk7) {
        this->m_caps.m_texNonPow2 = value != 0;
    }
}

// PROVEN: 98b92d97 IDENTICAL/NONE
// OFFSET: 0x0084B890
void CGxDeviceD3d::PoolSizeSet(CGxPool* pool, uint32_t size) {
    this->IReleaseD3dPool(pool);
    pool->m_size = size;

    switch (pool->m_target) {
        case GxPoolTarget_Vertex:
            pool->m_apiSpecific = this->ICreateD3dVB(pool->m_usage, size);
            break;

        case GxPoolTarget_Index:
            pool->m_apiSpecific = this->ICreateD3dIB(pool->m_usage, size);
            break;

        default:
            break;
    }
}

// PROVEN: 2ba96d2f IDENTICAL/DIFFERENT
// OFFSET: 0x0084BB80/0x14057A3B0
void CGxDeviceD3d::ICursorDestroy() {
    CGxDevice::ICursorDestroy();

    if (this->m_hwCursorBitmap) {
        this->m_hwCursorBitmap->Release();
        this->m_hwCursorBitmap = NULL;
    }

    if (this->m_hwCursorTexture) {
        this->m_hwCursorTexture->Release();
        this->m_hwCursorTexture = NULL;
    }
}

// PROVEN: e6e9d433 IDENTICAL/NONE
// OFFSET: 0x0084BBF0
float CGxDeviceD3d::StereoGetConvergence() {
    return this->m_d3dStereoConvergence;
}

// PROVEN: d78f8b58 IDENTICAL/NONE
// OFFSET: 0x0084BC20
float CGxDeviceD3d::StereoGetSeparation() {
    return this->m_d3dStereoSeparation;
}

// PROVEN: 7ef24674 IDENTICAL/NONE
// OFFSET: 0x0084BC30
bool CGxDeviceD3d::StereoEnabled() {
    return this->m_d3dStereoEnabled == 1;
}

// PROVEN: 37460b64 IDENTICAL/NONE
// OFFSET: 0x00852AC0
void CGxDeviceD3d::QueryDestroy(CGxQuery*& query) {
    CGxQuery* gxQuery = query;

    if (gxQuery->m_apiSpecific) {
        static_cast<LPDIRECT3DQUERY9>(gxQuery->m_apiSpecific)->Release();
    }

    gxQuery->m_apiSpecific = NULL;
    CGxDevice::QueryDestroy(query);
}

// OFFSET: %0x007C8150
int32_t CGxDeviceD3d::QueryEnd(CGxQuery* query) {
    if (!this->m_context || !query->m_apiSpecific) {
        return 0;
    }

    HRESULT hr = D3D_OK;

    if (query->m_type == GxQueryType_Occlusion) {
        hr = static_cast<LPDIRECT3DQUERY9>(query->m_apiSpecific)->Issue(D3DISSUE_END);
    }

    return hr == D3D_OK;
}

// OFFSET: %0x007C81F0
int32_t CGxDeviceD3d::QueryGetData(CGxQuery* query, uint32_t* data) {
    if (!this->m_context || !query->m_apiSpecific) {
        return 0;
    }

    LPDIRECT3DQUERY9 d3dQuery = static_cast<LPDIRECT3DQUERY9>(query->m_apiSpecific);
    HRESULT hr;

    do {
        hr = d3dQuery->GetData(data, sizeof(uint32_t), D3DGETDATA_FLUSH);
    } while (hr == S_FALSE);

    return hr == D3D_OK;
}

// PROVEN: e6b7519f IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void CGxDeviceD3d::DebugGroupBegin(const char* name, char* buffer) {
}

// PROVEN: b39cee7c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGxDeviceD3d::DebugGroupEnd() {
}

// PROVEN: 8bc5b20d IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void CGxDeviceD3d::DebugEvent(const char* name, char* buffer) {
}

// OFFSET: %0x007D2A00
void CGxDeviceD3d::ISceneEnd() {
    if (!this->m_inScene) {
        return;
    }

    this->m_d3dDevice->EndScene();
    this->m_inScene = 0;

    if (this->m_texCacheUsed <= this->m_texCacheSize) {
        return;
    }

    CGxTex* next;

    for (CGxTex* texId = this->m_texDestroyList.Head(); !this->m_texDestroyList.IsEnd(texId); texId = next) {
        next = this->m_texDestroyList.RawNext(texId);

        if (this->m_texCacheUsed < this->m_texCacheSize) {
            break;
        }

        this->ITexReleaseD3dTexture(texId);
        CGxDevice::TexDestroy(texId);
    }

    for (CGxTex* texId = this->m_texCacheList.Head(); !this->m_texCacheList.IsEnd(texId); texId = next) {
        next = this->m_texCacheList.RawNext(texId);

        if (this->m_frameCount - texId->unk_0050 < 10 || this->m_texCacheUsed < this->m_texCacheSize) {
            break;
        }

        this->ITexReleaseD3dTexture(texId);
    }
}

// PROVEN: 5ea4e21f IDENTICAL/NONE
// OFFSET: 0x00870160
void CGxDeviceD3d::CapsWindowSize(CRect& rect) {
    rect = this->DeviceCurWindow();
}

// OFFSET: %0x007D5E60
void CGxDeviceD3d::IShaderCreate(CGxShaderPermute* permute) {
    permute->m_valid = 0;

    if (!this->m_context) {
        return;
    }

    uint8_t* data = permute->Load();

    if (!*reinterpret_cast<uint32_t*>(data + 28)) {
        SMemFree(data, __FILE__, __LINE__, 0);

        if (permute->m_target) {
            this->unk_29B4 = g_theGxDevicePtr->m_caps.m_shaderTargets[permute->m_target] != 2;
        } else {
            this->unk_29B4 = g_theGxDevicePtr->m_caps.m_shaderTargets[0] != 1;
        }

        return;
    }

    const DWORD* code = reinterpret_cast<const DWORD*>(data + 32);
    permute->m_loaded = 1;

    if (permute->m_target == GxSh_Vertex) {
        LPDIRECT3DVERTEXSHADER9 vertexShader;

        if (SUCCEEDED(this->m_d3dDevice->CreateVertexShader(code, &vertexShader))) {
            permute->m_valid = 1;
            permute->m_apiSpecific = vertexShader;
        }
    } else if (permute->m_target == GxSh_Pixel) {
        LPDIRECT3DPIXELSHADER9 pixelShader;

        if (SUCCEEDED(this->m_d3dDevice->CreatePixelShader(code, &pixelShader))) {
            permute->m_valid = 1;
            permute->m_apiSpecific = pixelShader;
        }
    }

    SMemFree(data, __FILE__, __LINE__, 0);
}

// PROVEN: 658becf9 IDENTICAL/NONE
// OFFSET: 0x00871330
void CGxDeviceD3d::IShaderDestroy(CGxShaderPermute* permute) {
    if (permute->m_apiSpecific) {
        static_cast<IUnknown*>(permute->m_apiSpecific)->Release();
        permute->m_apiSpecific = NULL;
    }

    permute->m_loaded = 0;
}

// OFFSET: %0x007D5440
void CGxDeviceD3d::CapsWindowSizeInScreenCoords(CRect& rect) {
    if (this->IDevIsWindowed()) {
        const CRect& window = this->DeviceCurWindow();
        POINT points[2];
        points[0].x = 0;
        points[0].y = 0;
        points[1].x = static_cast<LONG>(window.maxX);
        points[1].y = static_cast<LONG>(window.maxY);
        MapWindowPoints(this->m_hwnd, NULL, points, 2);
        rect.minY = static_cast<float>(points[0].y);
        rect.minX = static_cast<float>(points[0].x);
        rect.maxY = static_cast<float>(points[1].y);
        rect.maxX = static_cast<float>(points[1].x);
    } else {
        rect = this->DeviceCurWindow();
    }
}

// OFFSET: %0x007D5800
void CGxDeviceD3d::XformSetProjection(const C44Matrix& matrix) {
    CGxDevice::XformSetProjection(matrix);

    C44Matrix projNative = matrix;

    if (!(fabs(projNative.c3 - 1.0f) < 0.00000023841858f) && !(fabs(projNative.c3) < 0.00000023841858f) && !(fabs(projNative.c3 + 1.0f) < 0.00000023841858f)) {
        projNative *= 1.0f / projNative.c3;
    }

    if (projNative.d3 == 0.0f) {
        float minZ = -(projNative.d2 / (projNative.c2 + 1.0f));
        float maxZ = -(projNative.d2 / (projNative.c2 - 1.0f));
        projNative.c2 = maxZ * projNative.c3 / (maxZ - minZ);
        projNative.d2 = maxZ * minZ / (minZ - maxZ);
    } else {
        float invC2 = 1.0f / projNative.c2;
        float minZ = (-1.0f - projNative.d2) * invC2;
        float maxZ = (1.0f - projNative.d2) * invC2;
        projNative.c2 = 1.0f / (maxZ - minZ);
        projNative.d2 = minZ / (minZ - maxZ);
    }

    if (!(this->m_appMasterEnables & (1 << GxMasterEnable_NormalProjection)) && projNative.d3 != 1.0f) {
        C44Matrix scale;
        scale.a0 = 0.2f;
        scale.b1 = 0.2f;
        scale.c2 = 0.2f;
        projNative = projNative * scale;
    }

    this->m_xforms[GxXform_Projection].m_dirty = 1;
    this->m_projNative = projNative;
}

// PROVEN: 9f95e2ee IDENTICAL/NONE
// OFFSET: 0x00870E50
void CGxDeviceD3d::XformSetView(const C44Matrix& matrix) {
    CGxDevice::XformSetView(matrix);
}

// PROVEN: 4447fb70 IDENTICAL/IDENTICAL
// OFFSET: 0x00858630/0x140589AD0
void CGxDeviceD3d::IStateSyncEnables() {
    if (this->m_appMasterEnables == this->m_hwMasterEnables) {
        return;
    }

    int32_t enable;

    if (this->NeedsUpdate(this->m_appMasterEnables, this->m_hwMasterEnables, 0, 0, GxMasterEnable_PolygonFill, enable)) {
        this->m_d3dDevice->SetRenderState(D3DRS_FILLMODE, enable ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
    }

    this->m_hwMasterEnables = this->m_appMasterEnables;
}

// PROVEN: 830cdff6 IDENTICAL/IDENTICAL
// OFFSET: 0x00858690/0x140589B50
void CGxDeviceD3d::IStateSyncClipPlanes() {
    if (!this->m_clipPlaneMask) {
        return;
    }

    for (uint32_t i = 0; i < 6; i++) {
        if (this->m_clipPlaneMask & (1 << i)) {
            this->m_d3dDevice->SetClipPlane(i, reinterpret_cast<const float*>(&this->m_clipPlane[i]));
        }
    }

    this->m_clipPlaneMask = 0;
}

// PROVEN: 9a870dc8 IDENTICAL/IDENTICAL
// OFFSET: 0x008586F0/0x140589BC0
void CGxDeviceD3d::IStateSyncScissorRect() {
    if (!this->m_scissorRectDirty) {
        return;
    }

    const CRect& window = this->DeviceCurWindow();
    RECT rect;

    rect.left = static_cast<LONG>(this->m_scissorRect.minX * window.maxX + 0.5f);
    rect.right = static_cast<LONG>(this->m_scissorRect.maxX * window.maxX + 1.0f);

    if (this->m_textureTarget[GxBuffers_Color].m_apiSpecific || this->m_textureTarget[GxBuffers_Depth].m_apiSpecific) {
        rect.top = static_cast<LONG>(this->m_scissorRect.minY * window.maxY + 0.5f);
        rect.bottom = static_cast<LONG>(this->m_scissorRect.maxY * window.maxY + 1.0f);
    } else {
        rect.top = static_cast<LONG>((1.0f - this->m_scissorRect.maxY) * window.maxY + 0.5f);
        rect.bottom = static_cast<LONG>((1.0f - this->m_scissorRect.minY) * window.maxY + 1.0f);
    }

    if (rect.left < 0) {
        rect.left = 0;
    }

    if (rect.top < 0) {
        rect.top = 0;
    }

    LONG width = static_cast<LONG>(window.maxX);

    if (rect.right > width) {
        rect.right = width;
    }

    LONG height = static_cast<LONG>(window.maxY);

    if (rect.bottom > height) {
        rect.bottom = height;
    }

    this->m_d3dDevice->SetScissorRect(&rect);
    this->m_scissorRectDirty = 0;
}

// OFFSET: %0x007D3240
LPDIRECT3DVERTEXDECLARATION9 CGxDeviceD3d::IGetVertexDecl() {
    LPDIRECT3DVERTEXDECLARATION9 decl = this->m_d3dVertexDecl[this->m_primVertexFormat];

    if (decl) {
        return decl;
    }

    const VertexBufDesc& desc = Buffer::s_vertexBufDesc[this->m_primVertexFormat];
    D3DVERTEXELEMENT9 elements[GxVAs_Last + 1];

    for (uint32_t i = 0; i < desc.attribCount; i++) {
        const CGxVertexAttrib& attrib = desc.attribs[i];
        D3DVERTEXELEMENT9& element = elements[i];
        element.Stream = attrib.m_stream != 0;
        element.Offset = static_cast<WORD>(attrib.m_offset);
        element.Type = static_cast<BYTE>(s_gxAttribToD3dAttribType[attrib.m_type]);
        element.Method = D3DDECLMETHOD_DEFAULT;
        element.Usage = static_cast<BYTE>(s_gxAttribToD3dAttribUsage[attrib.m_attrib]);
        element.UsageIndex = static_cast<BYTE>(s_gxAttribToD3dAttribUsageIndex[attrib.m_attrib]);
    }

    D3DVERTEXELEMENT9 end = D3DDECL_END();
    elements[desc.attribCount] = end;

    this->m_d3dDevice->CreateVertexDeclaration(elements, &decl);
    this->m_d3dVertexDecl[this->m_primVertexFormat] = decl;

    return decl;
}

// OFFSET: %0x007D3340
void CGxDeviceD3d::ISetVertexDecl() {
    LPDIRECT3DVERTEXDECLARATION9 decl = this->IGetVertexDecl();

    if (this->m_d3dCurrentVertexDecl == decl) {
        return;
    }

    this->m_d3dDevice->SetVertexDeclaration(decl);
    this->m_d3dCurrentVertexDecl = decl;

    uint32_t stride = Buffer::s_vertexBufDesc[this->m_primVertexFormats[0]].size;

    if (this->m_d3dVertexStreamStride[0] != stride) {
        this->m_d3dVertexStreamBuf[0] = NULL;
        this->m_d3dVertexStreamStride[0] = stride;
    }
}

// OFFSET: %0x007D33A0
void CGxDeviceD3d::ISetVertexDeclInstanced() {
    LPDIRECT3DVERTEXDECLARATION9 decl = this->IGetVertexDecl();

    if (this->m_d3dCurrentVertexDecl == decl) {
        return;
    }

    this->m_d3dDevice->SetVertexDeclaration(decl);
    this->m_d3dCurrentVertexDecl = decl;

    uint32_t stride = Buffer::s_vertexBufDesc[this->m_primVertexFormats[0]].size;

    if (this->m_d3dVertexStreamStride[0] != stride) {
        this->m_d3dVertexStreamBuf[0] = NULL;
        this->m_d3dVertexStreamStride[0] = stride;
    }

    if (this->m_primVertexFormats[1] < GxVertexBufferFormats_Last) {
        stride = Buffer::s_vertexBufDesc[this->m_primVertexFormats[1]].size;

        if (this->m_d3dVertexStreamStride[1] != stride) {
            this->m_d3dVertexStreamBuf[1] = NULL;
            this->m_d3dVertexStreamStride[1] = stride;
        }
    }
}

// PROVEN: 579e86fe IDENTICAL/NONE
// OFFSET: 0x00858A20
void CGxDeviceD3d::ISetIndexBuffer() {
    LPDIRECT3DINDEXBUFFER9 indexBuffer = static_cast<LPDIRECT3DINDEXBUFFER9>(this->m_primIndexBuf->m_pool->m_apiSpecific);

    if (this->m_d3dCurrentIndexBuf != indexBuffer) {
        this->m_d3dDevice->SetIndices(indexBuffer);
        this->m_d3dCurrentIndexBuf = indexBuffer;
    }
}

// PROVEN: 03a2fd7b IDENTICAL/DIFFERENT
// OFFSET: 0x00858A60/0x14058A000
void CGxDeviceD3d::ISetVertexStream() {
    LPDIRECT3DVERTEXBUFFER9 vertexBuffer = static_cast<LPDIRECT3DVERTEXBUFFER9>(this->m_primVertexBufs[0]->m_pool->m_apiSpecific);

    if (this->m_d3dVertexStreamBuf[0] != vertexBuffer) {
        this->m_d3dDevice->SetStreamSource(0, vertexBuffer, 0, this->m_d3dVertexStreamStride[0]);
        this->m_d3dVertexStreamBuf[0] = vertexBuffer;
    }
}

// PROVEN: 897e8281 IDENTICAL/NONE
// OFFSET: 0x00858AA0
void CGxDeviceD3d::ISetVertexStreams() {
    LPDIRECT3DVERTEXBUFFER9 vertexBuffer0 = static_cast<LPDIRECT3DVERTEXBUFFER9>(this->m_primVertexBufs[0]->m_pool->m_apiSpecific);
    LPDIRECT3DVERTEXBUFFER9 vertexBuffer1 = this->m_primVertexBufs[1] ? static_cast<LPDIRECT3DVERTEXBUFFER9>(this->m_primVertexBufs[1]->m_pool->m_apiSpecific) : NULL;

    if (this->m_d3dVertexStreamBuf[0] != vertexBuffer0) {
        this->m_d3dDevice->SetStreamSource(0, vertexBuffer0, 0, this->m_d3dVertexStreamStride[0]);
        this->m_d3dVertexStreamBuf[0] = vertexBuffer0;
    }

    if (this->m_d3dVertexStreamBuf[1] != vertexBuffer1) {
        this->m_d3dDevice->SetStreamSource(1, vertexBuffer1, 0, this->m_d3dVertexStreamStride[1]);
        this->m_d3dVertexStreamBuf[1] = vertexBuffer1;
    }
}

// OFFSET: %0x007D35E0
void CGxDeviceD3d::DsSet(EDeviceState state, uint32_t value) {
    if (this->m_deviceStates[state] == value) {
        return;
    }

    switch (state) {
        case Ds_SrcBlend:
            this->m_d3dDevice->SetRenderState(D3DRS_SRCBLEND, value);
            break;

        case Ds_DstBlend:
            this->m_d3dDevice->SetRenderState(D3DRS_DESTBLEND, value);
            break;

        case Ds_TssMagFilter0:
        case Ds_TssMagFilter1:
        case Ds_TssMagFilter2:
        case Ds_TssMagFilter3:
        case Ds_TssMagFilter4:
        case Ds_TssMagFilter5:
        case Ds_TssMagFilter6:
        case Ds_TssMagFilter7:
        case Ds_TssMagFilter8:
        case Ds_TssMagFilter9:
        case Ds_TssMagFilter10:
        case Ds_TssMagFilter11:
        case Ds_TssMagFilter12:
        case Ds_TssMagFilter13:
        case Ds_TssMagFilter14:
        case Ds_TssMagFilter15:
            this->m_d3dDevice->SetSamplerState(state - Ds_TssMagFilter0, D3DSAMP_MAGFILTER, value);
            break;

        case Ds_TssMinFilter0:
        case Ds_TssMinFilter1:
        case Ds_TssMinFilter2:
        case Ds_TssMinFilter3:
        case Ds_TssMinFilter4:
        case Ds_TssMinFilter5:
        case Ds_TssMinFilter6:
        case Ds_TssMinFilter7:
        case Ds_TssMinFilter8:
        case Ds_TssMinFilter9:
        case Ds_TssMinFilter10:
        case Ds_TssMinFilter11:
        case Ds_TssMinFilter12:
        case Ds_TssMinFilter13:
        case Ds_TssMinFilter14:
        case Ds_TssMinFilter15:
            this->m_d3dDevice->SetSamplerState(state - Ds_TssMinFilter0, D3DSAMP_MINFILTER, value);
            break;

        case Ds_TssMipFilter0:
        case Ds_TssMipFilter1:
        case Ds_TssMipFilter2:
        case Ds_TssMipFilter3:
        case Ds_TssMipFilter4:
        case Ds_TssMipFilter5:
        case Ds_TssMipFilter6:
        case Ds_TssMipFilter7:
        case Ds_TssMipFilter8:
        case Ds_TssMipFilter9:
        case Ds_TssMipFilter10:
        case Ds_TssMipFilter11:
        case Ds_TssMipFilter12:
        case Ds_TssMipFilter13:
        case Ds_TssMipFilter14:
        case Ds_TssMipFilter15:
            this->m_d3dDevice->SetSamplerState(state - Ds_TssMipFilter0, D3DSAMP_MIPFILTER, value);
            break;

        case Ds_TssWrapU0:
        case Ds_TssWrapU1:
        case Ds_TssWrapU2:
        case Ds_TssWrapU3:
        case Ds_TssWrapU4:
        case Ds_TssWrapU5:
        case Ds_TssWrapU6:
        case Ds_TssWrapU7:
        case Ds_TssWrapU8:
        case Ds_TssWrapU9:
        case Ds_TssWrapU10:
        case Ds_TssWrapU11:
        case Ds_TssWrapU12:
        case Ds_TssWrapU13:
        case Ds_TssWrapU14:
        case Ds_TssWrapU15:
            this->m_d3dDevice->SetSamplerState(state - Ds_TssWrapU0, D3DSAMP_ADDRESSU, value);
            break;

        case Ds_TssWrapV0:
        case Ds_TssWrapV1:
        case Ds_TssWrapV2:
        case Ds_TssWrapV3:
        case Ds_TssWrapV4:
        case Ds_TssWrapV5:
        case Ds_TssWrapV6:
        case Ds_TssWrapV7:
        case Ds_TssWrapV8:
        case Ds_TssWrapV9:
        case Ds_TssWrapV10:
        case Ds_TssWrapV11:
        case Ds_TssWrapV12:
        case Ds_TssWrapV13:
        case Ds_TssWrapV14:
        case Ds_TssWrapV15:
            this->m_d3dDevice->SetSamplerState(state - Ds_TssWrapV0, D3DSAMP_ADDRESSV, value);
            break;

        case Ds_TssWrapW0:
        case Ds_TssWrapW1:
        case Ds_TssWrapW2:
        case Ds_TssWrapW3:
        case Ds_TssWrapW4:
        case Ds_TssWrapW5:
        case Ds_TssWrapW6:
        case Ds_TssWrapW7:
        case Ds_TssWrapW8:
        case Ds_TssWrapW9:
        case Ds_TssWrapW10:
        case Ds_TssWrapW11:
        case Ds_TssWrapW12:
        case Ds_TssWrapW13:
        case Ds_TssWrapW14:
        case Ds_TssWrapW15:
            this->m_d3dDevice->SetSamplerState(state - Ds_TssWrapW0, D3DSAMP_ADDRESSW, value);
            break;

        case Ds_VtxWrapU0:
        case Ds_VtxWrapU1:
        case Ds_VtxWrapU2:
        case Ds_VtxWrapU3:
            this->m_d3dDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0 + (state - Ds_VtxWrapU0), D3DSAMP_ADDRESSU, value);
            break;

        case Ds_VtxWrapV0:
        case Ds_VtxWrapV1:
        case Ds_VtxWrapV2:
        case Ds_VtxWrapV3:
            this->m_d3dDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0 + (state - Ds_VtxWrapV0), D3DSAMP_ADDRESSV, value);
            break;

        case Ds_VtxWrapW0:
        case Ds_VtxWrapW1:
        case Ds_VtxWrapW2:
        case Ds_VtxWrapW3:
            this->m_d3dDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0 + (state - Ds_VtxWrapW0), D3DSAMP_ADDRESSW, value);
            break;

        case Ds_TssTexTransformFlags0:
        case Ds_TssTexTransformFlags1:
        case Ds_TssTexTransformFlags2:
        case Ds_TssTexTransformFlags3:
        case Ds_TssTexTransformFlags4:
        case Ds_TssTexTransformFlags5:
        case Ds_TssTexTransformFlags6:
        case Ds_TssTexTransformFlags7:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssTexTransformFlags0, D3DTSS_TEXTURETRANSFORMFLAGS, value);
            break;

        case Ds_TssMaxAnisotropy0:
        case Ds_TssMaxAnisotropy1:
        case Ds_TssMaxAnisotropy2:
        case Ds_TssMaxAnisotropy3:
        case Ds_TssMaxAnisotropy4:
        case Ds_TssMaxAnisotropy5:
        case Ds_TssMaxAnisotropy6:
        case Ds_TssMaxAnisotropy7:
        case Ds_TssMaxAnisotropy8:
        case Ds_TssMaxAnisotropy9:
        case Ds_TssMaxAnisotropy10:
        case Ds_TssMaxAnisotropy11:
        case Ds_TssMaxAnisotropy12:
        case Ds_TssMaxAnisotropy13:
        case Ds_TssMaxAnisotropy14:
        case Ds_TssMaxAnisotropy15:
            this->m_d3dDevice->SetSamplerState(state - Ds_TssMaxAnisotropy0, D3DSAMP_MAXANISOTROPY, value);
            break;

        case Ds_TssTexCoordIndex0:
        case Ds_TssTexCoordIndex1:
        case Ds_TssTexCoordIndex2:
        case Ds_TssTexCoordIndex3:
        case Ds_TssTexCoordIndex4:
        case Ds_TssTexCoordIndex5:
        case Ds_TssTexCoordIndex6:
        case Ds_TssTexCoordIndex7:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssTexCoordIndex0, D3DTSS_TEXCOORDINDEX, value);
            break;

        case Ds_TssColorOp0:
        case Ds_TssColorOp1:
        case Ds_TssColorOp2:
        case Ds_TssColorOp3:
        case Ds_TssColorOp4:
        case Ds_TssColorOp5:
        case Ds_TssColorOp6:
        case Ds_TssColorOp7:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssColorOp0, D3DTSS_COLOROP, value);
            break;

        case Ds_TssAlphaOp0:
        case Ds_TssAlphaOp1:
        case Ds_TssAlphaOp2:
        case Ds_TssAlphaOp3:
        case Ds_TssAlphaOp4:
        case Ds_TssAlphaOp5:
        case Ds_TssAlphaOp6:
        case Ds_TssAlphaOp7:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssAlphaOp0, D3DTSS_ALPHAOP, value);
            break;

        case Ds_TssColorArg10:
        case Ds_TssColorArg11:
        case Ds_TssColorArg12:
        case Ds_TssColorArg13:
        case Ds_TssColorArg14:
        case Ds_TssColorArg15:
        case Ds_TssColorArg16:
        case Ds_TssColorArg17:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssColorArg10, D3DTSS_COLORARG1, value);
            break;

        case Ds_TssColorArg20:
        case Ds_TssColorArg21:
        case Ds_TssColorArg22:
        case Ds_TssColorArg23:
        case Ds_TssColorArg24:
        case Ds_TssColorArg25:
        case Ds_TssColorArg26:
        case Ds_TssColorArg27:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssColorArg20, D3DTSS_COLORARG2, value);
            break;

        case Ds_TssAlphaArg10:
        case Ds_TssAlphaArg11:
        case Ds_TssAlphaArg12:
        case Ds_TssAlphaArg13:
        case Ds_TssAlphaArg14:
        case Ds_TssAlphaArg15:
        case Ds_TssAlphaArg16:
        case Ds_TssAlphaArg17:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssAlphaArg10, D3DTSS_ALPHAARG1, value);
            break;

        case Ds_TssAlphaArg20:
        case Ds_TssAlphaArg21:
        case Ds_TssAlphaArg22:
        case Ds_TssAlphaArg23:
        case Ds_TssAlphaArg24:
        case Ds_TssAlphaArg25:
        case Ds_TssAlphaArg26:
        case Ds_TssAlphaArg27:
            this->m_d3dDevice->SetTextureStageState(state - Ds_TssAlphaArg20, D3DTSS_ALPHAARG2, value);
            break;

        case Ds_AmbientMaterialSource:
            this->m_d3dDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, value);
            break;

        case Ds_DiffuseMaterialSource:
            this->m_d3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, value);
            break;

        case Ds_SpecularMaterialSource:
            this->m_d3dDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, value);
            break;

        case Ds_EmissiveMaterialSource:
            this->m_d3dDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, value);
            break;

        case Ds_Ambient:
            this->m_d3dDevice->SetRenderState(D3DRS_AMBIENT, value);
            break;

        case Ds_AlphaBlendEnable:
            this->m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, value);
            break;

        case Ds_AlphaTestEnable:
            this->m_d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, value);
            break;

        case Ds_AlphaRef:
            this->m_d3dDevice->SetRenderState(D3DRS_ALPHAREF, value);
            break;

        case Ds_FogEnable:
            this->m_d3dDevice->SetRenderState(D3DRS_FOGENABLE, value);
            break;

        case Ds_ZWriteEnable:
            this->m_d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, value);
            break;

        case Ds_ColorWriteEnable: {
            DWORD mask = (value & 0x1) != 0;

            if (value & 0x4) {
                mask |= D3DCOLORWRITEENABLE_GREEN;
            }

            if (value & 0x2) {
                mask |= D3DCOLORWRITEENABLE_BLUE;
            }

            if (value & 0x8) {
                mask |= D3DCOLORWRITEENABLE_ALPHA;
            }

            this->m_d3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, mask);
            break;
        }

        case Ds_Lighting:
            this->m_d3dDevice->SetRenderState(D3DRS_LIGHTING, value);
            break;

        case Ds_SpecularEnable:
            this->m_d3dDevice->SetRenderState(D3DRS_SPECULARENABLE, value);
            break;

        case Ds_CullMode:
            this->m_d3dDevice->SetRenderState(D3DRS_CULLMODE, value);
            break;

        case Ds_ClipPlaneEnable:
            this->m_d3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, value);
            break;

        case Ds_ZFunc:
            this->m_d3dDevice->SetRenderState(D3DRS_ZFUNC, value);
            break;

        case Ds_PointScaleA:
            this->m_d3dDevice->SetRenderState(D3DRS_POINTSCALE_A, value);
            break;

        case Ds_PointScaleB:
            this->m_d3dDevice->SetRenderState(D3DRS_POINTSCALE_B, value);
            break;

        case Ds_PointScaleC:
            this->m_d3dDevice->SetRenderState(D3DRS_POINTSCALE_C, value);
            break;

        case Ds_PointScaleEnable:
            this->m_d3dDevice->SetRenderState(D3DRS_POINTSCALEENABLE, value);
            break;
        default:
            break;
    }

    this->m_deviceStates[state] = value;
}

// OFFSET: %0x007D3C60
void CGxDeviceD3d::ISetTexCoordIndex(uint32_t tmu, int32_t texGen, uint32_t coord) {
    EDeviceState state = static_cast<EDeviceState>(Ds_TssTexCoordIndex0 + tmu);

    switch (texGen) {
        case GxTexGen_Disabled:
            this->DsSet(state, coord);
            break;

        case GxTexGen_ObjectLinear:
        case GxTexGen_EyeLinear:
        case GxTexGen_Unk3:
            this->DsSet(state, tmu | D3DTSS_TCI_CAMERASPACEPOSITION);
            break;

        case GxTexGen_ReflectionMap:
        case GxTexGen_SphereMap:
            this->DsSet(state, tmu | D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
            break;

        case GxTexGen_Normal:
            this->DsSet(state, tmu | D3DTSS_TCI_CAMERASPACENORMAL);
            break;

        default:
            break;
    }
}

// PROVEN: dacfd069 IDENTICAL/DIFFERENT
// OFFSET: 0x00859240/0x14058A830
void CGxDeviceD3d::ISetColorOp(uint32_t tmu, uint32_t op) {
    if (tmu < static_cast<uint32_t>(this->m_caps.m_numTmus)) {
        this->DsSet(static_cast<EDeviceState>(Ds_TssColorOp0 + tmu), s_texOp[op]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssColorArg10 + tmu), s_texArgs[op][0]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssColorArg20 + tmu), s_texArgs[op][1]);
    }
}

// PROVEN: 3b752018 IDENTICAL/DIFFERENT
// OFFSET: 0x008592A0/0x14058A8B0
void CGxDeviceD3d::ISetAlphaOp(uint32_t tmu, uint32_t op) {
    if (tmu < static_cast<uint32_t>(this->m_caps.m_numTmus)) {
        this->DsSet(static_cast<EDeviceState>(Ds_TssAlphaOp0 + tmu), s_texOp[op]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssAlphaArg10 + tmu), s_texArgs[op][0]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssAlphaArg20 + tmu), s_texArgs[op][1]);
    }
}

// OFFSET: %0x007D3B70
void CGxDeviceD3d::ISetVertexTexture(uint32_t tmu, CGxTex* texId) {
    if (!texId) {
        this->m_d3dDevice->SetTexture(D3DVERTEXTEXTURESAMPLER0 + tmu, NULL);
        return;
    }

    bool acquired = false;

    if (!texId->m_flags.m_renderTarget && !texId->m_apiSpecificData) {
        this->ITexAcquire(texId);
        acquired = true;
    }

    this->ITexMarkAsUpdated(texId);

    if (acquired) {
        this->ITexUploadAll(texId);
    }

    this->ITexTouch(texId);
    this->m_d3dDevice->SetTexture(D3DVERTEXTEXTURESAMPLER0 + tmu, static_cast<IDirect3DBaseTexture9*>(texId->m_apiSpecificData));

    this->DsSet(static_cast<EDeviceState>(Ds_VtxWrapU0 + tmu), s_wrapModes[texId->m_flags.m_wrapU]);
    this->DsSet(static_cast<EDeviceState>(Ds_VtxWrapV0 + tmu), s_wrapModes[texId->m_flags.m_wrapV]);
    this->DsSet(static_cast<EDeviceState>(Ds_VtxWrapW0 + tmu), s_wrapModes[texId->m_flags.m_wrapW]);
}

// PROVEN: 900b1549 IDENTICAL/IDENTICAL
// OFFSET: 0x00859300/0x14058A930
void CGxDeviceD3d::ISetMaterial(CImVector diffuse, CImVector emissive, CImVector specular, float specularExp) {
    D3DMATERIAL9 material;
    material.Diffuse.r = diffuse.r * (1.0f / 255.0f);
    material.Diffuse.g = diffuse.g * (1.0f / 255.0f);
    material.Diffuse.b = diffuse.b * (1.0f / 255.0f);
    material.Diffuse.a = diffuse.a * (1.0f / 255.0f);
    material.Ambient = material.Diffuse;
    material.Emissive.r = emissive.r * (1.0f / 255.0f);
    material.Emissive.g = emissive.g * (1.0f / 255.0f);
    material.Emissive.b = emissive.b * (1.0f / 255.0f);
    material.Emissive.a = emissive.a * (1.0f / 255.0f);
    material.Specular.r = specular.r * (1.0f / 255.0f);
    material.Specular.g = specular.g * (1.0f / 255.0f);
    material.Specular.b = specular.b * (1.0f / 255.0f);
    material.Specular.a = specular.a * (1.0f / 255.0f);
    material.Power = specularExp;
    this->m_d3dDevice->SetMaterial(&material);
}

// OFFSET: %0x007D3F00
void CGxDeviceD3d::IStateSyncLights() {
    if (this->m_appRenderStates[GxRs_Lighting].m_value.m_data.i[0]) {
        if (this->m_ambientOnlyMode) {
            this->m_ambientOnlyMode = 0;
        }

        this->DsSet(Ds_Lighting, this->m_appMasterEnables & 0x1);
        this->DsSet(Ds_Ambient, 0);

        for (uint32_t i = 0; i < 4; i++) {
            CGxApiLight& light = this->m_lights[i];

            if (!light.m_flags) {
                continue;
            }

            if (light.m_flags & 0x1) {
                if (!light.m_enable) {
                    light.m_flags &= ~0x1;
                    this->m_d3dDevice->LightEnable(i, FALSE);
                    continue;
                }

                this->m_d3dDevice->LightEnable(i, TRUE);
            } else if (!light.m_enable) {
                continue;
            }

            if (light.m_dir.w == 1.0f) {
                s_d3dLight.Type = D3DLIGHT_POINT;
                s_d3dLight.Position.x = light.m_dir.x;
                s_d3dLight.Position.y = light.m_dir.y;
                s_d3dLight.Position.z = light.m_dir.z;
            } else {
                s_d3dLight.Type = D3DLIGHT_DIRECTIONAL;
                s_d3dLight.Direction.x = light.m_dir.x;
                s_d3dLight.Direction.y = light.m_dir.y;
                s_d3dLight.Direction.z = light.m_dir.z;
            }

            s_d3dLight.Diffuse.r = light.m_dirColor.x;
            s_d3dLight.Diffuse.g = light.m_dirColor.y;
            s_d3dLight.Diffuse.b = light.m_dirColor.z;
            s_d3dLight.Diffuse.a = 1.0f;
            s_d3dLight.Ambient.r = light.m_ambColor.x;
            s_d3dLight.Ambient.g = light.m_ambColor.y;
            s_d3dLight.Ambient.b = light.m_ambColor.z;
            s_d3dLight.Ambient.a = 0.0f;
            s_d3dLight.Specular.r = light.m_specColor.x;
            s_d3dLight.Specular.g = light.m_specColor.y;
            s_d3dLight.Specular.b = light.m_specColor.z;
            s_d3dLight.Specular.a = 0.0f;
            s_d3dLight.Range = 10000.0f;
            s_d3dLight.Falloff = 1.0f;
            s_d3dLight.Attenuation0 = light.m_constantAttenuation;
            s_d3dLight.Attenuation1 = light.m_linearAttenuation;
            s_d3dLight.Attenuation2 = light.m_quadraticAttenuation;
            this->m_d3dDevice->SetLight(i, &s_d3dLight);

            light.m_flags &= ~light.m_flags;
        }

        return;
    }

    if ((Buffer::s_vertexBufDesc[this->m_primVertexFormat].mask & (1 << GxVA_Color0)) || this->m_appRenderStates[GxRs_MatDiffuse].m_value.m_data.u[0] == 0xFFFFFFFF) {
        if (this->m_ambientOnlyMode) {
            this->m_ambientOnlyMode = 0;
        }

        this->DsSet(Ds_Lighting, 0);
        this->DsSet(Ds_Ambient, 0);
    } else {
        if (!this->m_ambientOnlyMode) {
            this->m_ambientOnlyMode = 1;

            for (uint32_t i = 0; i < 4; i++) {
                this->m_d3dDevice->LightEnable(i, FALSE);

                if (this->m_lights[i].m_enable) {
                    this->m_lights[i].m_flags |= 0x1;
                } else {
                    this->m_lights[i].m_flags &= ~0x1;
                }
            }
        }

        this->DsSet(Ds_Lighting, 1);
        this->DsSet(Ds_Ambient, 0xFFFFFFFF);
    }
}

// OFFSET: %0x007D4290
void CGxDeviceD3d::IStateSyncMaterial() {
    uint32_t emissiveSource;

    if (Buffer::s_vertexBufDesc[this->m_primVertexFormat].mask & (1 << GxVA_Color0)) {
        int32_t colorMaterial = this->m_appRenderStates[GxRs_ColorMaterial].m_value.m_data.i[0];
        this->DsSet(Ds_AmbientMaterialSource, colorMaterial == 0);
        this->DsSet(Ds_DiffuseMaterialSource, colorMaterial == 0);
        this->DsSet(Ds_SpecularMaterialSource, colorMaterial == 1);
        emissiveSource = colorMaterial == 2;
    } else {
        emissiveSource = 0;
        this->DsSet(Ds_AmbientMaterialSource, 0);
        this->DsSet(Ds_DiffuseMaterialSource, 0);
        this->DsSet(Ds_SpecularMaterialSource, 0);
    }

    this->DsSet(Ds_EmissiveMaterialSource, emissiveSource);
}

// PROVEN: 59f40302 IDENTICAL/UNPROVEN
// OFFSET: 0x00859920/0x14058B020
void CGxDeviceD3d::IStateSyncXforms() {
    if (this->m_xforms[GxXform_Projection].m_dirty) {
        this->m_d3dDevice->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&this->m_projNative));
        this->m_xforms[GxXform_Projection].m_dirty = 0;
    }

    if (this->m_xforms[GxXform_View].m_dirty) {
        CGxMatrixStack& view = this->m_xforms[GxXform_View];
        this->m_d3dDevice->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&view.m_mtx[view.m_level]));
        view.m_dirty = 0;
    }

    if (this->m_xforms[GxXform_World].m_dirty) {
        this->IXformSetWorld();
    }

    for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_caps.m_numTmus); i++) {
        if (this->m_xforms[i].m_dirty || this->m_texGen[i].m_dirty) {
            this->IXformSetTex(i);
        }
    }
}

// OFFSET: %0x007D4490
void CGxDeviceD3d::ISetTexture(uint32_t tmu, CGxTex* texId) {
    if (texId) {
        bool upload = false;

        if (!texId->m_flags.m_renderTarget && !texId->m_apiSpecificData) {
            this->ITexAcquire(texId);

            if (texId->m_format != GxTex_Intz && texId->m_format != GxTex_Rawz) {
                upload = true;
            }
        }

        this->ITexMarkAsUpdated(texId);

        if (upload) {
            this->ITexUploadAll(texId);
        }

        this->ITexTouch(texId);
        this->m_d3dDevice->SetTexture(tmu, static_cast<IDirect3DBaseTexture9*>(texId->m_apiSpecificData));

        const D3DTEXTUREFILTERTYPE* filter = s_filterModes[texId->m_flags.m_filter];
        this->DsSet(static_cast<EDeviceState>(Ds_TssMinFilter0 + tmu), filter[0]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssMagFilter0 + tmu), filter[1]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssMipFilter0 + tmu), filter[2]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssWrapU0 + tmu), s_wrapModes[texId->m_flags.m_wrapU]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssWrapV0 + tmu), s_wrapModes[texId->m_flags.m_wrapV]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssWrapW0 + tmu), s_wrapModes[texId->m_flags.m_wrapW]);
        this->DsSet(static_cast<EDeviceState>(Ds_TssMaxAnisotropy0 + tmu), texId->m_flags.m_maxAnisotropy);

        if (tmu < 8) {
            if (!this->m_appRenderStates[GxRs_PixelShader].m_value.m_data.i[0] && !this->m_texEnabled[tmu]) {
                this->ISetColorOp(tmu, this->m_appRenderStates[GxRs_ColorOp0 + tmu].m_value.m_data.i[0]);
                this->ISetAlphaOp(tmu, this->m_appRenderStates[GxRs_AlphaOp0 + tmu].m_value.m_data.i[0]);
                this->m_appRenderStates[GxRs_ColorOp0 + tmu].m_dirty = 0;
                this->m_appRenderStates[GxRs_AlphaOp0 + tmu].m_dirty = 0;
            }

            this->m_texEnabled[tmu] = true;
        }
    } else {
        this->m_d3dDevice->SetTexture(tmu, NULL);

        if (tmu < 8) {
            if (!this->m_appRenderStates[GxRs_PixelShader].m_value.m_data.i[0] && this->m_texEnabled[tmu]) {
                this->DsSet(static_cast<EDeviceState>(Ds_TssColorOp0 + tmu), D3DTOP_DISABLE);
                this->DsSet(static_cast<EDeviceState>(Ds_TssAlphaOp0 + tmu), D3DTOP_DISABLE);
            }

            this->m_texEnabled[tmu] = false;
        }
    }
}

// PROVEN: 1682a0ed IDENTICAL/DIFFERENT
// OFFSET: 0x00859BE0/0x14058B360
void CGxDeviceD3d::ISetTexCoord(uint32_t tmu, uint32_t coord) {
    if (tmu < static_cast<uint32_t>(this->m_caps.m_numTmus)) {
        this->ISetTexCoordIndex(tmu, this->m_appRenderStates[GxRs_TexGen0 + tmu].m_value.m_data.i[0], coord);
    }
}

// OFFSET: %0x007D4900
void CGxDeviceD3d::ISetTexGen(uint32_t tmu, int32_t texGen) {
    if (tmu >= static_cast<uint32_t>(this->m_caps.m_numTmus)) {
        return;
    }

    this->ISetTexCoordIndex(tmu, texGen, this->m_appRenderStates[GxRs_TextureCoord0 + tmu].m_value.m_data.i[0]);

    C44Matrix matrix;

    if (texGen == GxTexGen_ObjectLinear || texGen == GxTexGen_EyeLinear) {
        CGxMatrixStack& view = this->m_xforms[GxXform_View];
        matrix = view.m_mtx[view.m_level];
        matrix.AffineInvertInPlace();

        if (texGen == GxTexGen_ObjectLinear) {
            CGxMatrixStack& world = this->m_xforms[GxXform_World];
            matrix = matrix * world.m_mtx[world.m_level].Inverse();
        }
    } else if (texGen == GxTexGen_SphereMap) {
        matrix.a0 = 0.5f;
        matrix.b1 = 0.5f;
        matrix.d0 = 0.5f;
        matrix.d1 = 0.5f;
    } else {
        this->m_texGen[tmu].Identity();
        return;
    }

    CGxMatrixStack& stack = this->m_texGen[tmu];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level] = matrix;
}

// OFFSET: %0x007D4B70
void CGxDeviceD3d::IRsSendToHw(EGxRenderState which) {
    CGxStateBom& value = this->m_appRenderStates[which].m_value;

    switch (which) {
        case GxRs_PolygonOffset:
            if (this->m_caps.m_depthBias) {
                float bias = -CGxDevice::s_polygonOffsetValues[value.m_data.i[0]];
                this->m_d3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *reinterpret_cast<DWORD*>(&bias));
            }

            break;

        case GxRs_MatDiffuse:
        case GxRs_MatEmissive:
        case GxRs_MatSpecular:
        case GxRs_MatSpecularExp: {
            CImVector diffuse;
            CImVector emissive;
            CImVector specular;
            *reinterpret_cast<uint32_t*>(&diffuse) = this->m_appRenderStates[GxRs_MatDiffuse].m_value.m_data.u[0];
            *reinterpret_cast<uint32_t*>(&emissive) = this->m_appRenderStates[GxRs_MatEmissive].m_value.m_data.u[0];
            *reinterpret_cast<uint32_t*>(&specular) = this->m_appRenderStates[GxRs_MatSpecular].m_value.m_data.u[0];
            this->ISetMaterial(diffuse, emissive, specular, this->m_appRenderStates[GxRs_MatSpecularExp].m_value.m_data.f[0]);

            this->m_appRenderStates[GxRs_MatDiffuse].m_dirty = 0;
            this->m_appRenderStates[GxRs_MatEmissive].m_dirty = 0;
            this->m_appRenderStates[GxRs_MatSpecular].m_dirty = 0;
            this->m_appRenderStates[GxRs_MatSpecularExp].m_dirty = 0;

            uint32_t specularEnable = 0;

            if (this->m_appRenderStates[GxRs_MatSpecularExp].m_value.m_data.f[0] > 0.0f && this->m_appRenderStates[GxRs_MatSpecular].m_value.m_data.i[0]) {
                specularEnable = 1;
            }

            this->DsSet(Ds_SpecularEnable, specularEnable);
            break;
        }

        case GxRs_NormalizeNormals:
            if (s_normalizeNormals != value.m_data.u[0]) {
                this->m_d3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, value.m_data.u[0]);
                s_normalizeNormals = value.m_data.u[0];
            }

            break;

        case GxRs_BlendingMode: {
            uint32_t mode = value.m_data.u[0];

            if (mode < 2) {
                this->DsSet(Ds_AlphaBlendEnable, 0);
            } else {
                this->DsSet(Ds_AlphaBlendEnable, 1);
                this->DsSet(Ds_SrcBlend, s_srcBlend[mode]);
                this->DsSet(Ds_DstBlend, s_dstBlend[mode]);
            }

            break;
        }

        case GxRs_AlphaRef: {
            int32_t alphaRef = value.m_data.i[0];

            if (alphaRef > 0) {
                this->DsSet(Ds_AlphaRef, alphaRef);
                this->DsSet(Ds_AlphaTestEnable, 1);
            } else {
                this->DsSet(Ds_AlphaTestEnable, 0);
            }

            break;
        }

        case GxRs_FogStart:
            this->m_d3dDevice->SetRenderState(D3DRS_FOGSTART, value.m_data.u[0]);
            break;

        case GxRs_FogEnd:
            this->m_d3dDevice->SetRenderState(D3DRS_FOGEND, value.m_data.u[0]);
            break;

        case GxRs_FogColor:
            this->m_d3dDevice->SetRenderState(D3DRS_FOGCOLOR, value.m_data.u[0]);
            break;

        case GxRs_Lighting: {
            uint32_t enable = 0;

            if (this->m_appMasterEnables & 0x1) {
                enable = value.m_data.i[0] != 0;
            }

            this->DsSet(Ds_Lighting, enable);
            break;
        }

        case GxRs_Fog: {
            uint32_t enable = 0;

            if (this->m_appMasterEnables & 0x2) {
                enable = value.m_data.i[0] != 0;
            }

            this->DsSet(Ds_FogEnable, enable);
            break;
        }

        case GxRs_DepthTest:
        case GxRs_DepthFunc: {
            uint32_t func;

            if ((this->m_appMasterEnables & 0x4) && this->m_appRenderStates[GxRs_DepthTest].m_value.m_data.i[0]) {
                func = s_cmpFunc[this->m_appRenderStates[GxRs_DepthFunc].m_value.m_data.i[0]];
            } else {
                func = D3DCMP_ALWAYS;
            }

            this->DsSet(Ds_ZFunc, func);
            this->m_appRenderStates[GxRs_DepthTest].m_dirty = 0;
            this->m_appRenderStates[GxRs_DepthFunc].m_dirty = 0;
            break;
        }

        case GxRs_DepthWrite: {
            uint32_t enable = 0;

            if (this->m_appMasterEnables & 0x8) {
                enable = value.m_data.i[0] != 0;
            }

            this->DsSet(Ds_ZWriteEnable, enable);
            break;
        }

        case GxRs_ColorWrite: {
            uint32_t mask = 0;

            if (this->m_appMasterEnables & 0x10) {
                mask = value.m_data.u[0];
            }

            this->DsSet(Ds_ColorWriteEnable, mask);
            break;
        }

        case GxRs_Culling: {
            uint32_t cull = 0;

            if (this->m_appMasterEnables & 0x20) {
                cull = value.m_data.i[0];

                if (value.m_data.i[0] > 2) {
                    cull = 2;
                }
            }

            this->DsSet(Ds_CullMode, s_cullMode[cull]);
            break;
        }

        case GxRs_ClipPlaneMask:
            this->DsSet(Ds_ClipPlaneEnable, value.m_data.u[0]);
            break;

        case GxRs_Multisample:
            this->m_d3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, value.m_data.i[0] != 0);
            break;

        case GxRs_ScissorTest:
            this->m_d3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, value.m_data.i[0] != 0);
            break;

        case GxRs_Texture0:
        case GxRs_Texture1:
        case GxRs_Texture2:
        case GxRs_Texture3:
        case GxRs_Texture4:
        case GxRs_Texture5:
        case GxRs_Texture6:
        case GxRs_Texture7:
        case GxRs_Texture8:
        case GxRs_Texture9:
        case GxRs_Texture10:
        case GxRs_Texture11:
        case GxRs_Texture12:
        case GxRs_Texture13:
        case GxRs_Texture14:
        case GxRs_Texture15:
            this->ISetTexture(which - GxRs_Texture0, static_cast<CGxTex*>(value.m_data.p));
            break;

        case GxRs_VertexTexture0:
        case GxRs_VertexTexture1:
        case GxRs_VertexTexture2:
        case GxRs_VertexTexture3:
            this->ISetVertexTexture(which - GxRs_VertexTexture0, static_cast<CGxTex*>(value.m_data.p));
            break;

        case GxRs_ColorOp0:
        case GxRs_ColorOp1:
        case GxRs_ColorOp2:
        case GxRs_ColorOp3:
        case GxRs_ColorOp4:
        case GxRs_ColorOp5:
        case GxRs_ColorOp6:
        case GxRs_ColorOp7:
            this->ISetColorOp(which - GxRs_ColorOp0, value.m_data.u[0]);
            break;

        case GxRs_AlphaOp0:
        case GxRs_AlphaOp1:
        case GxRs_AlphaOp2:
        case GxRs_AlphaOp3:
        case GxRs_AlphaOp4:
        case GxRs_AlphaOp5:
        case GxRs_AlphaOp6:
        case GxRs_AlphaOp7:
            this->ISetAlphaOp(which - GxRs_AlphaOp0, value.m_data.u[0]);
            break;

        case GxRs_TexGen0:
        case GxRs_TexGen1:
        case GxRs_TexGen2:
        case GxRs_TexGen3:
        case GxRs_TexGen4:
        case GxRs_TexGen5:
        case GxRs_TexGen6:
        case GxRs_TexGen7:
            this->ISetTexGen(which - GxRs_TexGen0, value.m_data.i[0]);
            break;

        case GxRs_TextureCoord0:
        case GxRs_TextureCoord1:
        case GxRs_TextureCoord2:
        case GxRs_TextureCoord3:
        case GxRs_TextureCoord4:
        case GxRs_TextureCoord5:
        case GxRs_TextureCoord6:
        case GxRs_TextureCoord7:
            this->ISetTexCoord(which - GxRs_TextureCoord0, value.m_data.u[0]);
            break;

        case GxRs_VertexShader:
            this->IBindVertexShader(static_cast<CGxShaderPermute*>(value.m_data.p));
            break;

        case GxRs_PixelShader:
            this->IBindPixelShader(static_cast<CGxShaderPermute*>(value.m_data.p));
            break;

        case GxRs_PointScale: {
            float pointScale = value.m_data.f[0];
            const C3Vector& attenuation = *reinterpret_cast<const C3Vector*>(this->m_appRenderStates[GxRs_PointScaleAttenuation].m_value.m_data.f);

            if (attenuation != CGxDevice::s_pointScaleAttenuation) {
                const CRect& window = this->DeviceCurWindow();
                pointScale = pointScale / ((1.0f - this->m_viewport.m_y.m_l) * window.maxY - (1.0f - this->m_viewport.m_y.m_h) * window.maxY);
            }

            this->m_d3dDevice->SetRenderState(D3DRS_POINTSIZE, *reinterpret_cast<DWORD*>(&pointScale));
            break;
        }

        case GxRs_PointScaleAttenuation: {
            const C3Vector& attenuation = *reinterpret_cast<const C3Vector*>(value.m_data.f);
            uint32_t enable = attenuation != CGxDevice::s_pointScaleAttenuation;

            this->DsSet(Ds_PointScaleEnable, enable);

            if (enable) {
                this->DsSet(Ds_PointScaleA, value.m_data.u[0]);
                this->DsSet(Ds_PointScaleB, value.m_data.u[1]);
                this->DsSet(Ds_PointScaleC, value.m_data.u[2]);
            }

            break;
        }

        case GxRs_PointScaleMin:
            this->m_d3dDevice->SetRenderState(D3DRS_POINTSIZE_MIN, value.m_data.u[0]);
            break;

        case GxRs_PointScaleMax:
            this->m_d3dDevice->SetRenderState(D3DRS_POINTSIZE_MAX, value.m_data.u[0]);
            break;

        case GxRs_PointSprite:
            this->m_d3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, value.m_data.i[0] != 0);
            break;

        case GxRs_BlendFactor: {
            uint32_t factor = static_cast<uint32_t>(value.m_data.f[0] * 255.0f);
            this->m_d3dDevice->SetRenderState(D3DRS_BLENDFACTOR, (((((factor << 8) | factor) << 8) | factor) << 8) | factor);
            break;
        }

        default:
            break;
    }
}

// PROVEN: ccc82acf IDENTICAL/DIFFERENT
// OFFSET: 0x00859CA0/0x14058B440
void CGxDeviceD3d::IStateSync() {
    int32_t hwVertexShader = this->m_hwRenderStates[GxRs_VertexShader].m_data.i[0];

    this->IShaderConstantsFlush();
    this->IRsSync(0);

    if (!hwVertexShader || !this->m_appRenderStates[GxRs_VertexShader].m_value.m_data.i[0]) {
        this->IStateSyncLights();
        this->IStateSyncMaterial();
        this->IStateSyncXforms();
    }

    this->IStateSyncEnables();
    this->IStateSyncClipPlanes();
    this->IStateSyncScissorRect();

    if (this->m_viewportDirty) {
        this->IXformSetViewport();
    }
}

// PROVEN: af3140de DIFFERENT/IDENTICAL
// OFFSET: 0x0085B140/0x14058CBD0
void CGxDeviceD3d::IXformSetWorld() {
    CGxMatrixStack& world = this->m_xforms[GxXform_World];

    if (!s_isIdent || !(world.m_flags[world.m_level] & CGxMatrixStack::F_Identity)) {
        this->m_d3dDevice->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&world.m_mtx[world.m_level]));
    }

    int32_t isIdent = world.m_flags[world.m_level] & CGxMatrixStack::F_Identity;
    world.m_dirty = 0;
    s_isIdent = isIdent;
}

// OFFSET: %0x007D5BA0
void CGxDeviceD3d::IXformSetTex(uint32_t tmu) {
    uint32_t flags = D3DTTFF_DISABLE;
    D3DTRANSFORMSTATETYPE state = static_cast<D3DTRANSFORMSTATETYPE>(D3DTS_TEXTURE0 + tmu);
    CGxMatrixStack& texGen = this->m_texGen[tmu];
    CGxMatrixStack& xform = this->m_xforms[tmu];

    switch (g_theGxDevicePtr->m_appRenderStates[GxRs_TextureShader0 + tmu].m_value.m_data.i[0]) {
        case 0:
            if (texGen.m_flags[texGen.m_level] & CGxMatrixStack::F_Identity) {
                this->m_d3dDevice->SetTransform(state, reinterpret_cast<const D3DMATRIX*>(&texGen.m_mtx[texGen.m_level]));
            } else {
                this->m_d3dDevice->SetTransform(state, reinterpret_cast<const D3DMATRIX*>(&texGen.m_mtx[texGen.m_level]));
                flags = D3DTTFF_COUNT3;
            }

            break;

        case 1: {
            C44Matrix product = texGen.m_mtx[texGen.m_level] * xform.m_mtx[xform.m_level];
            C44Matrix matrix = product;

            if (this->m_appRenderStates[GxRs_TexGen0 + tmu].m_value.m_data.i[0]) {
                flags = D3DTTFF_COUNT3;
            } else {
                flags = D3DTTFF_COUNT2;
                matrix.c0 = product.d0;
                matrix.c1 = product.d1;
            }

            this->m_d3dDevice->SetTransform(state, reinterpret_cast<const D3DMATRIX*>(&matrix));
            break;
        }

        case 2: {
            C44Matrix matrix = texGen.m_mtx[texGen.m_level] * xform.m_mtx[xform.m_level];
            this->m_d3dDevice->SetTransform(state, reinterpret_cast<const D3DMATRIX*>(&matrix));
            flags = D3DTTFF_COUNT3 | D3DTTFF_PROJECTED;
            break;
        }

        default:
            break;
    }

    this->DsSet(static_cast<EDeviceState>(Ds_TssTexTransformFlags0 + tmu), flags);
    xform.m_dirty = 0;
    texGen.m_dirty = 0;
}

// OFFSET: %0x007D5F40
void CGxDeviceD3d::IBindPixelShader(CGxShaderPermute* permute) {
    if (permute) {
        if (!permute->m_loaded) {
            this->IShaderCreate(permute);
        }

        this->m_d3dDevice->SetPixelShader(static_cast<LPDIRECT3DPIXELSHADER9>(permute->m_apiSpecific));
        return;
    }

    this->m_d3dDevice->SetPixelShader(NULL);

    for (uint32_t i = 0; i < 8; i++) {
        if (this->m_texEnabled[i]) {
            this->ISetColorOp(i, this->m_appRenderStates[GxRs_ColorOp0 + i].m_value.m_data.u[0]);
            this->ISetAlphaOp(i, this->m_appRenderStates[GxRs_AlphaOp0 + i].m_value.m_data.u[0]);
        } else {
            this->DsSet(static_cast<EDeviceState>(Ds_TssColorOp0 + i), D3DTOP_DISABLE);
            this->DsSet(static_cast<EDeviceState>(Ds_TssAlphaOp0 + i), D3DTOP_DISABLE);
        }

        this->m_appRenderStates[GxRs_ColorOp0 + i].m_dirty = 0;
        this->m_appRenderStates[GxRs_AlphaOp0 + i].m_dirty = 0;
    }
}

// OFFSET: %0x007D5500
int32_t CGxDeviceD3d::XformIsValidViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    const CRect& window = this->DeviceCurWindow();
    uint32_t x = static_cast<uint32_t>(minX * window.maxX + 0.5f);
    uint32_t y = static_cast<uint32_t>((1.0f - maxY) * window.maxY + 0.5f);
    uint32_t width = static_cast<uint32_t>(window.maxX * maxX - x + 0.5f);
    uint32_t height = static_cast<uint32_t>(window.maxY * (1.0f - minY) - y + 0.5f);

    if (this->m_textureTarget[GxBuffers_Color].m_apiSpecific || this->m_textureTarget[GxBuffers_Depth].m_apiSpecific) {
        y = static_cast<uint32_t>(minY * window.maxY);
    }

    return width && height && minZ >= 0.0f && maxZ <= 1.0f && minZ <= maxZ && x + width <= window.maxX && y + height <= window.maxY;
}

// OFFSET: %0x007D5690
void CGxDeviceD3d::IXformSetViewport() {
    const CRect& window = this->DeviceCurWindow();
    D3DVIEWPORT9 viewport;

    viewport.X = static_cast<DWORD>(this->m_viewport.m_x.m_l * window.maxX + 0.5f);
    viewport.Y = static_cast<DWORD>((1.0f - this->m_viewport.m_y.m_h) * window.maxY + 0.5f);
    viewport.Width = static_cast<DWORD>(this->m_viewport.m_x.m_h * window.maxX - viewport.X + 0.5f);
    viewport.Height = static_cast<DWORD>((1.0f - this->m_viewport.m_y.m_l) * window.maxY - viewport.Y + 0.5f);
    viewport.MinZ = this->m_viewport.m_z.m_l;
    viewport.MaxZ = this->m_viewport.m_z.m_h;

    if (this->m_textureTarget[GxBuffers_Color].m_apiSpecific || this->m_textureTarget[GxBuffers_Depth].m_apiSpecific) {
        viewport.Y = static_cast<DWORD>(window.maxY * this->m_viewport.m_y.m_l);
    }

    this->m_d3dDevice->SetViewport(&viewport);
    this->m_viewportDirty = 0;
}

// OFFSET: %0x007D5DD0
void CGxDeviceD3d::IShaderConstantsFlush() {
    ShadowConstants& vs = CGxDevice::s_shadowConstants[GxSh_Vertex];

    if (vs.dirtyMin <= vs.dirtyMax) {
        this->m_d3dDevice->SetVertexShaderConstantF(vs.dirtyMin, &vs.constants[vs.dirtyMin * 4], vs.dirtyMax - vs.dirtyMin + 1);
        vs.dirtyMin = 255;
        vs.dirtyMax = 0;
    }

    ShadowConstants& ps = CGxDevice::s_shadowConstants[GxSh_Pixel];

    if (ps.dirtyMin <= ps.dirtyMax) {
        this->m_d3dDevice->SetPixelShaderConstantF(ps.dirtyMin, &ps.constants[ps.dirtyMin * 4], ps.dirtyMax - ps.dirtyMin + 1);
        ps.dirtyMin = 255;
        ps.dirtyMax = 0;
    }
}

// OFFSET: %0x007D5D70
void CGxDeviceD3d::IBindVertexShader(CGxShaderPermute* permute) {
    if (!permute) {
        this->m_d3dDevice->SetVertexShader(NULL);
        return;
    }

    if (!permute->m_loaded) {
        this->IShaderCreate(permute);
    }

    this->m_d3dDevice->SetVertexShader(static_cast<LPDIRECT3DVERTEXSHADER9>(permute->m_apiSpecific));
}

// OFFSET: %0x007C7590
void CGxDeviceD3d::ISetPresentParms(uint32_t adapter, D3DDEVTYPE deviceType, D3DPRESENT_PARAMETERS& d3dpp, const CGxFormat& format) {
    memset(&d3dpp, 0, sizeof(d3dpp));

    if (format.m_window) {
        D3DDISPLAYMODE displayMode;

        if (FAILED(this->m_d3d->GetAdapterDisplayMode(adapter, &displayMode))) {
            displayMode.Format = this->m_desktopDisplayMode.Format;
        }

        const CRect& window = this->DeviceCurWindow();
        d3dpp.Windowed = TRUE;
        d3dpp.BackBufferWidth = static_cast<UINT>(window.maxX);
        d3dpp.BackBufferFormat = displayMode.Format;
        d3dpp.BackBufferHeight = static_cast<UINT>(window.maxY);
        d3dpp.BackBufferCount = format.m_vsync ? format.m_backBufferCount : 1;
        d3dpp.FullScreen_RefreshRateInHz = 0;
    } else {
        d3dpp.BackBufferWidth = format.m_size.x;
        d3dpp.BackBufferHeight = format.m_size.y;
        d3dpp.BackBufferFormat = s_GxFormatToD3dFormat[format.m_colorFormat];
        d3dpp.BackBufferCount = format.m_vsync ? format.m_backBufferCount : 1;
        d3dpp.FullScreen_RefreshRateInHz = format.m_refreshRate / format.m_refreshRateDenom;
    }

    d3dpp.hDeviceWindow = this->m_hwnd;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = s_GxFormatToD3dFormat[format.m_depthFormat];

    switch (format.m_vsync) {
        case 1:
            d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
            break;

        case 2:
            d3dpp.PresentationInterval = format.m_window ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_TWO;
            break;

        case 3:
            d3dpp.PresentationInterval = format.m_window ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_THREE;
            break;

        case 4:
            d3dpp.PresentationInterval = format.m_window ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_FOUR;
            break;

        default:
            d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
            break;
    }

    if (format.m_multisampleCount <= 1) {
        d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        return;
    }

    d3dpp.MultiSampleType = static_cast<D3DMULTISAMPLE_TYPE>(format.m_multisampleCount);

    DWORD qualityLevels;

    if (FAILED(this->m_d3d->CheckDeviceMultiSampleType(adapter, deviceType, d3dpp.BackBufferFormat, format.m_window, d3dpp.MultiSampleType, &qualityLevels)) || qualityLevels <= 1) {
        qualityLevels = 1;
    }

    d3dpp.MultiSampleQuality = static_cast<DWORD>((qualityLevels - 1) * format.m_multisampleQuality);
}

// OFFSET: %0x007C82F0
void CGxDeviceD3d::ISetCaps(const CGxFormat& format) {
    this->m_caps.m_numTmus = this->m_d3dCaps.MaxTextureBlendStages >= 8 ? 8 : this->m_d3dCaps.MaxTextureBlendStages;
    this->m_caps.m_numVertexTmus = 0;
    this->m_caps.m_numShaderTmus = this->m_d3dCaps.MaxSimultaneousTextures >= 16 ? 16 : this->m_d3dCaps.MaxSimultaneousTextures;
    this->m_caps.m_pixelCenterOnEdge = 1;
    this->m_caps.m_texelCenterOnEdge = 0;

    uint32_t maxTextureSize = this->m_d3dCaps.MaxTextureWidth;
    this->m_caps.m_texMaxSize[GxTex_2d] = maxTextureSize >= 256 ? maxTextureSize : 256;
    this->m_caps.m_texMaxSize[GxTex_CubeMap] = maxTextureSize >= 256 ? maxTextureSize : 256;
    this->m_caps.m_texMaxSize[GxTex_Rectangle] = maxTextureSize >= 256 ? maxTextureSize : 256;
    this->m_caps.m_texMaxSize[GxTex_NonPow2] = maxTextureSize >= 256 ? maxTextureSize : 256;
    this->m_caps.m_texMaxSize[GxTex_Volume] = this->m_d3dCaps.MaxVolumeExtent >= 256 ? this->m_d3dCaps.MaxVolumeExtent : 256;
    this->m_caps.m_maxIndex = this->m_d3dCaps.MaxVertexIndex;

    uint32_t filterCaps = this->m_d3dCaps.TextureFilterCaps;
    this->m_caps.m_texFilterTrilinear = (filterCaps & D3DPTFILTERCAPS_MIPFLINEAR) != 0;
    this->m_caps.m_texFilterAnisotropic = (filterCaps & (D3DPTFILTERCAPS_MINFANISOTROPIC | D3DPTFILTERCAPS_MAGFANISOTROPIC)) != 0;

    if (filterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) {
        s_filterModes[GxTex_Anisotropic][0] = D3DTEXF_ANISOTROPIC;
    }

    if (this->m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) {
        s_filterModes[GxTex_Anisotropic][1] = D3DTEXF_ANISOTROPIC;
    }

    this->m_caps.m_maxTexAnisotropy = this->m_d3dCaps.MaxAnisotropy;

    if (this->m_caps.m_texFilterAnisotropic && this->m_d3dCaps.MaxAnisotropy < 2) {
        this->m_caps.m_texFilterAnisotropic = 0;
    }

    this->m_caps.m_numStreams = this->m_d3dCaps.MaxStreams;
    this->m_caps.m_depthBias = (this->m_d3dCaps.RasterCaps & D3DPRASTERCAPS_DEPTHBIAS) != 0;

    for (uint32_t i = 0; i < GxShTargets_Last; i++) {
        this->m_caps.m_shaderTargets[i] = GxShProfile_None;
        this->m_caps.m_shaderConstants[i] = 0;
    }

    for (uint32_t i = 0; i < GxShProfiles_Last; i++) {
        this->m_caps.m_shaderProfiles[i] = 0;
    }

    uint32_t pixelShaderVersion = this->m_d3dCaps.PixelShaderVersion;

    if (pixelShaderVersion >= D3DPS_VERSION(3, 0)) {
        this->m_caps.m_shaderProfiles[GxShProfile_Ps30] = 1;
    }

    if (pixelShaderVersion >= D3DPS_VERSION(2, 0)) {
        this->m_caps.m_shaderProfiles[GxShProfile_Ps20] = 1;
    }

    if (pixelShaderVersion >= D3DPS_VERSION(1, 4)) {
        this->m_caps.m_shaderProfiles[GxShProfile_Ps14] = 1;
    }

    if (pixelShaderVersion >= D3DPS_VERSION(1, 1)) {
        this->m_caps.m_shaderProfiles[GxShProfile_Ps11] = 1;
    }

    uint32_t vertexShaderVersion = this->m_d3dCaps.VertexShaderVersion;
    this->m_caps.m_shaderProfiles[GxShProfile_PsFf] = 1;

    if (vertexShaderVersion >= D3DVS_VERSION(3, 0)) {
        this->m_caps.m_shaderProfiles[GxShProfile_Vs30] = 1;
    }

    if (vertexShaderVersion >= D3DVS_VERSION(2, 0)) {
        this->m_caps.m_shaderProfiles[GxShProfile_Vs20] = 1;
    }

    if (vertexShaderVersion >= D3DVS_VERSION(1, 1)) {
        this->m_caps.m_shaderProfiles[GxShProfile_Vs11] = 1;
    }

    this->m_caps.m_shaderProfiles[GxShProfile_VsFf] = 1;

    this->ChooseShaderModel(format.m_shaderModel);

    if (this->m_caps.m_shaderTargets[GxSh_Vertex] != GxShProfile_VsFf) {
        this->m_caps.m_shaderConstants[GxSh_Vertex] = this->m_d3dCaps.MaxVertexShaderConst;
    }

    if (this->m_caps.m_shaderTargets[GxSh_Vertex] == GxShProfile_Vs30) {
        this->m_caps.m_numVertexTmus = 4;
    }

    switch (this->m_caps.m_shaderTargets[GxSh_Pixel]) {
        case GxShProfile_Ps11:
            this->m_caps.m_numShaderTmus = 4;
            break;

        case GxShProfile_Ps14:
            this->m_caps.m_numShaderTmus = 6;
            break;

        case GxShProfile_Ps20:
        case GxShProfile_Ps30:
            this->m_caps.m_numShaderTmus = 16;
            break;

        default:
            break;
    }

    D3DDEVICE_CREATION_PARAMETERS creationParameters;
    this->m_d3dDevice->GetCreationParameters(&creationParameters);

    D3DADAPTER_IDENTIFIER9 identifier;
    this->m_d3d->GetAdapterIdentifier(creationParameters.AdapterOrdinal, 0, &identifier);

    if (identifier.VendorId == 0x10DE && identifier.DeviceId - 0x300 <= 0xFF && this->m_caps.m_shaderConstants[GxSh_Vertex] > 192) {
        this->m_caps.m_shaderConstants[GxSh_Vertex] = 192;
    }

    for (uint32_t i = 0; i < GxTexFormats_Last; i++) {
        D3DFORMAT d3dFormat = s_GxTexFmtToD3dFmt[i];
        this->m_caps.m_vertexTexFmt[i] = d3dFormat && this->m_d3d->CheckDeviceFormat(this->m_d3dCaps.AdapterOrdinal, this->m_d3dCaps.DeviceType, this->m_devAdapterFormat, D3DUSAGE_QUERY_VERTEXTEXTURE, D3DRTYPE_TEXTURE, d3dFormat) == D3D_OK;
        this->m_caps.m_texFmt[i] = d3dFormat && this->m_d3d->CheckDeviceFormat(this->m_d3dCaps.AdapterOrdinal, this->m_d3dCaps.DeviceType, this->m_devAdapterFormat, 0, D3DRTYPE_TEXTURE, d3dFormat) == D3D_OK;
        this->m_caps.m_rttFormat[i] = d3dFormat && this->m_d3d->CheckDeviceFormat(this->m_d3dCaps.AdapterOrdinal, this->m_d3dCaps.DeviceType, this->m_devAdapterFormat, CGxDevice::s_texFormatIsDepth[i] ? D3DUSAGE_DEPTHSTENCIL : D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, d3dFormat) == D3D_OK;
        this->m_caps.m_pcfFormat[i] = 0;
    }

    if (this->m_caps.m_rttFormat[GxTex_D24X8]) {
        this->m_caps.m_pcfFormat[GxTex_D24X8] = 1;
    }

    if (identifier.VendorId == 0x1002 && !this->m_caps.m_texFmt[GxTex_Resz]) {
        this->m_caps.m_texFmt[GxTex_Intz] = 0;
        this->m_caps.m_texFmt[GxTex_AtiD24] = 0;
    }

    if (this->m_caps.m_shaderTargets[GxSh_Vertex] == GxShProfile_Vs11) {
        s_gxAttribToD3dAttribType[1] = D3DDECLTYPE_D3DCOLOR;
        s_gxAttribToD3dAttribType[2] = D3DDECLTYPE_D3DCOLOR;
    } else {
        s_gxAttribToD3dAttribType[1] = D3DDECLTYPE_UBYTE4;
        s_gxAttribToD3dAttribType[2] = D3DDECLTYPE_UBYTE4N;
    }

    uint32_t miscCaps = this->m_d3dCaps.PrimitiveMiscCaps;

    if (miscCaps & D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING) {
        this->m_caps.m_numRenderTargets = this->m_d3dCaps.NumSimultaneousRTs;
    }

    this->m_caps.m_colorWrite = (miscCaps & D3DPMISCCAPS_COLORWRITEENABLE) != 0;
    this->m_caps.m_maxClipPlanes = this->m_d3dCaps.MaxUserClipPlanes != 0;
    this->m_caps.unk_02BC = 0;
    this->m_caps.m_hwCursor = this->m_d3dCaps.CursorCaps & D3DCURSORCAPS_COLOR;

    uint32_t textureCaps = this->m_d3dCaps.TextureCaps;
    this->m_caps.m_texTarget[GxTex_2d] = 1;
    this->m_caps.m_texTarget[GxTex_CubeMap] = (textureCaps & D3DPTEXTURECAPS_CUBEMAP) != 0;
    this->m_caps.m_texTarget[GxTex_Rectangle] = 0;
    this->m_caps.m_texTarget[GxTex_NonPow2] = (textureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) || !(textureCaps & D3DPTEXTURECAPS_POW2);
    this->m_caps.m_texTarget[GxTex_Volume] = (textureCaps & D3DPTEXTURECAPS_VOLUMEMAP) != 0;
    this->m_caps.m_texNonPow2 = 1;

    if (this->m_caps.m_texTarget[GxTex_NonPow2]) {
        this->m_caps.m_texNonPow2 = (textureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) != 0;

        if (identifier.VendorId == 0x10DE) {
            this->m_caps.m_texNonPow2 = 0;
        }
    }

    LPDIRECT3DQUERY9 query = NULL;
    this->m_caps.m_occlusionQuery = this->m_d3dDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &query) == D3D_OK;

    if (query) {
        query->Release();
    }

    float maxPointSize = this->m_d3dCaps.MaxPointSize;
    this->m_caps.m_pointParameters = maxPointSize > 1.0f;
    this->m_caps.m_pointScaleMax = maxPointSize;
    this->m_caps.m_pointSprite = maxPointSize > 1.0f;
    this->m_caps.m_blendFactor = (this->m_d3dCaps.SrcBlendCaps & D3DPBLENDCAPS_BLENDFACTOR) != 0;
    this->m_caps.unk_02D8[1] = 0;
    this->m_caps.unk_02D8[0] = 0;
    this->m_caps.unk_02D8[2] = 0;
    this->m_caps.unk_02D8[3] = 0;
    this->m_caps.unk_02D8[4] = 0;
    this->m_caps.unk_0300 = 0;
    this->m_caps.unk_02F4 = 1;
    this->m_caps.unk_02F8 = this->m_caps.m_shaderTargets[GxSh_Pixel] != GxShProfile_Ps30;
    this->m_caps.unk_02FC = this->m_caps.m_shaderTargets[GxSh_Pixel] != GxShProfile_Ps30;
    this->m_caps.unk_0304 = this->m_caps.m_shaderTargets[GxSh_Vertex] == GxShProfile_Vs30;
}

// OFFSET: %0x007C8990
int32_t CGxDeviceD3d::ICreateD3dDevice(const CGxFormat& format) {
    bool hwTnL = format.m_hwTnL;

    if (hwTnL && !(this->m_d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)) {
        hwTnL = false;
    }

    uint32_t adapter = this->IGetAdapterIndex(format.m_monitor);

    if (NvAPI_Initialize() == NVAPI_OK) {
        CGxDevice::Log("Detected NvAPI and checking if it's valid...");

        NvPhysicalGpuHandle gpus[NVAPI_MAX_PHYSICAL_GPUS];
        NvU32 gpuCount = 0;
        bool hasGpus = NvAPI_EnumPhysicalGPUs(gpus, &gpuCount) == NVAPI_OK && gpuCount;
        int32_t nvDisplay = 0;

        if (hasGpus) {
            CGxDevice::Log("    Detected %d Nvidia Physical GPU(s)", gpuCount);
            CGxDevice::Log("    Using D3D adapter %d", adapter);

            D3DADAPTER_IDENTIFIER9 identifier;
            this->m_d3d->GetAdapterIdentifier(adapter, 0, &identifier);
            CGxDevice::Log("    Adapter Identifier DeviceName: %s", identifier.DeviceName);

            NvDisplayHandle display;

            if (NvAPI_GetAssociatedNvidiaDisplayHandle(identifier.DeviceName, &display) == NVAPI_OK) {
                CGxDevice::Log("    Nvidia associated display handle GPUs detected");

                if (NvAPI_GetPhysicalGPUsFromDisplay(display, gpus, &gpuCount) != NVAPI_OK || !gpuCount) {
                    nvDisplay = 0;
                } else {
                    nvDisplay = 1;
                    CGxDevice::Log("    Display driven by %d Nvidia GPU(s)", gpuCount);
                }
            }

            NvAPI_EnumPhysicalGPUs(gpus, &gpuCount);

            if (nvDisplay) {
                this->m_d3dNVAPI = 1;
                this->m_d3dStereoEnabled = 0;

                NvU8 stereoEnabled;

                switch (NvAPI_Stereo_IsEnabled(&stereoEnabled)) {
                    case NVAPI_NOT_SUPPORTED:
                        CGxDevice::Log("No stereoscopic device present.");
                        break;

                    case NVAPI_NO_IMPLEMENTATION:
                        CGxDevice::Log("No stereoscopic driver present.");
                        break;

                    case NVAPI_OK:
                        this->m_d3dStereoRestore = this->m_d3dStereoEnabled;
                        this->m_caps.m_stereoAvailable = 1;

                        if (format.m_stereoEnabled) {
                            if (!stereoEnabled) {
                                NvAPI_Status status = NvAPI_Stereo_Enable();

                                if (status != NVAPI_OK) {
                                    NvAPI_ShortString message;
                                    NvAPI_GetErrorMessage(status, message);
                                    CGxDevice::Log("Failed to enable stereoscopic mode: %s", message);
                                }
                            }
                        } else if (stereoEnabled) {
                            NvAPI_Stereo_Disable();
                        }

                        break;

                    default:
                        break;
                }
            }
        }
    }

    this->m_d3dIsHwDevice = hwTnL;

    D3DPRESENT_PARAMETERS d3dpp;
    this->ISetPresentParms(adapter, D3DDEVTYPE_HAL, d3dpp, format);
    D3DPERF_SetOptions(1);

    DWORD behaviorFlags = hwTnL ? D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE : D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;

    if (FAILED(this->m_d3d->CreateDevice(adapter, D3DDEVTYPE_HAL, this->m_hwnd, behaviorFlags, &d3dpp, &this->m_d3dDevice))) {
        this->m_d3dDevice = NULL;
        return 0;
    }

    if (this->m_d3dNVAPI) {
        NvU8 stereoEnabled = 0;

        if (NvAPI_Stereo_IsEnabled(&stereoEnabled) == NVAPI_OK && stereoEnabled) {
            NvAPI_Stereo_CreateConfigurationProfileRegistryKey(NVAPI_STEREO_DEFAULT_REGISTRY_PROFILE);
            NvAPI_Stereo_CreateHandleFromIUnknown(this->m_d3dDevice, reinterpret_cast<StereoHandle*>(&this->m_d3dStereoHandle));

            if (this->m_d3dStereoHandle) {
                this->m_d3dStereoEnabled = 1;
                this->m_d3dStereoDirty = 1;
            }
        }
    }

    this->m_devAdapterFormat = d3dpp.BackBufferFormat;
    this->m_context = 1;
    this->m_devDepthFormat = d3dpp.AutoDepthStencilFormat;

    this->ISetCaps(format);
    this->Log(this->m_caps);
    CGxDevice::Log("NVAPI: %d", this->m_d3dNVAPI);

    this->IStateSetD3DDefaults();
    this->DeviceSetGamma(this->m_gammaRamp);
    this->ICursorCreate(format);

    CGxDevice::s_uiVertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "UI");
    CGxDevice::s_uiPixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "UI");

    return 1;
}

// OFFSET: %0x007C9BE0
void CGxDeviceD3d::IReleaseD3dResources(int32_t freeTextures) {
    this->ITexForceRecreation(freeTextures);
    this->IReleaseD3dShaders(freeTextures);
    this->IReleaseD3dPools(freeTextures);
    memset(this->m_deviceStates, 0xFF, sizeof(this->m_deviceStates));

    if (this->m_defColorSurface) {
        this->m_defColorSurface->Release();
        this->m_defColorSurface = NULL;
    }

    if (this->m_defDepthSurface) {
        this->m_defDepthSurface->Release();
        this->m_defDepthSurface = NULL;
    }

    if (this->m_defDepthStencilSurface) {
        this->m_defDepthStencilSurface->Release();
        this->m_defDepthStencilSurface = NULL;
    }

    if (this->m_readbackSurface) {
        this->m_readbackSurface->Release();
        this->m_readbackSurface = NULL;
    }

    if (this->m_eventQuery) {
        this->m_eventQuery->Release();
        this->m_eventQuery = NULL;
    }

    this->IReleaseD3dQueries();

    if (this->m_d3dDevice) {
        this->m_d3dDevice->ShowCursor(FALSE);
    }
}

// OFFSET: %0x007C9CC0
int32_t CGxDeviceD3d::DeviceWM(EGxWM wm, uintptr_t param1, uintptr_t param2) {
    switch (wm) {
        case GxWM_Size:
            if (param2 == 1 || param2 == 2) {
                this->m_windowVisible = 0;
                break;
            }

            this->m_windowVisible = 1;
            this->DeviceSetDefWindow(*reinterpret_cast<const CRect*>(param1));

            if (this->m_d3dDevice && this->m_context) {
                this->IReleaseD3dResources(0);

                D3DPRESENT_PARAMETERS d3dpp;
                this->ISetPresentParms(this->m_d3dCaps.AdapterOrdinal, this->m_d3dCaps.DeviceType, d3dpp, this->m_format);

                if (this->m_d3dDevice->Reset(&d3dpp) == D3D_OK) {
                    this->IStateSetD3DDefaults();
                    this->ISetWindowFocus(1);
                    this->m_context = 1;
                    this->m_intF5C = 0;
                    this->NotifyOnDeviceRestored();
                    this->m_viewportDirty = 1;
                    break;
                }

                this->m_context = 0;
            }

            this->m_viewportDirty = 1;
            break;

        case GxWM_DisplayChange:
            if (this->m_windowVisible) {
                this->DeviceSetDefWindow(*reinterpret_cast<const CRect*>(param1));
                this->m_viewportDirty = 1;
            }

            break;

        case GxWM_Destroy:
        case GxWM_KillFocus:
            this->m_windowFocus = 0;
            break;

        case GxWM_SetFocus:
            this->ISetWindowFocus(1);
            break;

        default:
            break;
    }

    return 0;
}

// OFFSET: %0x007C9E40
void CGxDeviceD3d::IDestroyD3d() {
    if (CGxDevice::s_uiVertexShader) {
        g_theGxDevicePtr->ShaderRelease(CGxDevice::s_uiVertexShader);
        CGxDevice::s_uiVertexShader = NULL;
    }

    if (CGxDevice::s_uiPixelShader) {
        g_theGxDevicePtr->ShaderRelease(CGxDevice::s_uiPixelShader);
        CGxDevice::s_uiPixelShader = NULL;
    }

    this->ICursorDestroy();
    this->IReleaseD3dResources(1);
    this->IReleaseD3dVertexDecl();

    if (this->m_eventQuery) {
        this->m_eventQuery->Release();
        this->m_eventQuery = NULL;
    }

    if (this->m_d3dStereoHandle) {
        NvAPI_Stereo_DestroyHandle(static_cast<StereoHandle>(this->m_d3dStereoHandle));
        this->m_d3dStereoHandle = NULL;
    }

    if (this->m_d3dDevice) {
        this->m_d3dDevice->Release();
        this->m_d3dDevice = NULL;
    }

    if (this->m_d3dNVAPI) {
        NvU8 stereoEnabled;

        if (NvAPI_Stereo_IsEnabled(&stereoEnabled) == NVAPI_OK) {
            if (this->m_d3dStereoRestore) {
                if (!stereoEnabled) {
                    NvAPI_Stereo_Enable();
                }
            } else if (stereoEnabled) {
                NvAPI_Stereo_Disable();
            }
        }
    }
}

// OFFSET: %0x007C9F30
int32_t CGxDeviceD3d::DeviceSetFormat(const CGxFormat& format) {
    CGxDevice::Log("CGxDeviceD3d::DeviceSetFormat():");
    this->Log(format);

    if (this->m_hwnd) {
        ShowWindow(this->m_hwnd, SW_HIDE);
    }

    this->IDestroyD3d();
    this->IWindowDestroy(&this->m_hwnd);
    this->m_format = format;

    CGxFormat createFormat = format;

    if (this->IWindowCreate(&this->m_hwnd, createFormat, 1) && this->ICreateD3dDevice(createFormat) && CGxDevice::DeviceSetFormat(createFormat)) {
        this->ISetWindowFocus(1);
        return 1;
    }

    CGxDevice::Log("CGxDeviceD3d::DeviceSetFormat(): unable to set format!");
    this->IDestroyD3d();
    this->IWindowDestroy(&this->m_hwnd);

    return 0;
}

// PROVEN: fff7ccad IDENTICAL/UNPROVEN
// OFFSET: 0x0084D700/0x14057C2E0
void CGxDeviceD3d::DeviceDestroy() {
    CGxDevice::DeviceDestroy();

    if (this->m_hwnd && this->m_ownhwnd) {
        this->IWindowDestroy(&this->m_hwnd);
    }

    if (this->m_hwndClass) {
        this->IWindowClassDestroy(&this->m_hwndClass);
    }

    this->IDestroyD3d();

    if (this->m_d3d) {
        this->m_d3d->Release();
        this->m_d3d = NULL;
    }

    if (this->m_d3dLib) {
        FreeLibrary(this->m_d3dLib);
        this->m_d3dLib = NULL;
    }
}

// OFFSET: %0x007CA0B0
int32_t CGxDeviceD3d::ICreateD3d(uint32_t monitor) {
    if (CGxDeviceD3d::ILoadD3dLib(this->m_d3dLib, this->m_d3d)) {
        uint32_t adapter = this->IGetAdapterIndex(monitor);

        if (SUCCEEDED(this->m_d3d->GetDeviceCaps(adapter, D3DDEVTYPE_HAL, &this->m_d3dCaps))) {
            if (this->m_desktopDisplayMode.Format) {
                return 1;
            }

            D3DDISPLAYMODE displayMode;

            if (SUCCEEDED(this->m_d3d->GetAdapterDisplayMode(adapter, &displayMode))) {
                this->m_desktopDisplayMode = displayMode;
                return 1;
            }
        }
    }

    this->IDestroyD3d();

    if (this->m_d3d) {
        this->m_d3d->Release();
        this->m_d3d = NULL;
    }

    if (this->m_d3dLib) {
        FreeLibrary(this->m_d3dLib);
        this->m_d3dLib = NULL;
    }

    return 0;
}

// OFFSET: %0x007CA190
int32_t CGxDeviceD3d::DeviceCreate(GxWindowProc windowProc, const CGxFormat& format) {
    this->m_ownhwnd = 1;

    HDC dc = GetDC(NULL);

    if (GetDeviceGammaRamp(dc, &this->m_systemGammaRamp) && &this->m_gammaRamp != &this->m_systemGammaRamp) {
        this->m_gammaRamp = this->m_systemGammaRamp;
    }

    ReleaseDC(NULL, dc);

    this->m_hwndClass = this->IWindowClassCreate();

    if (!this->m_hwndClass) {
        char* error = OsGetLastErrorStr();
        CGxDevice::Log("CGxDeviceD3d::DeviceCreate(): IWindowClassCreate() failed: %s", error);
        OsFreeLastErrorStr(error);
        this->DeviceDestroy();
        return 0;
    }

    if (this->ICreateD3d(format.m_monitor) && CGxDevice::DeviceCreate(windowProc, format)) {
        return 1;
    }

    this->DeviceDestroy();
    return 0;
}

// OFFSET: %0x007CA270
int32_t CGxDeviceD3d::DeviceCreate(void* window, const CGxFormat& format) {
    this->m_ownhwnd = 0;
    CGxDevice::DeviceCreate(window, format);
    this->m_hwnd = static_cast<HWND>(window);

    if (this->ICreateD3d(format.m_monitor) && this->ICreateD3dDevice(format) && CGxDevice::DeviceCreate(window, format)) {
        return 1;
    }

    this->DeviceDestroy();
    return 0;
}

// OFFSET: %0x007B3850
void CGxDeviceD3d::IReleaseD3dQueries() {
    for (CGxQuery* query = this->m_queryList.Head(); query; query = this->m_queryList.Next(query)) {
        if (query->m_apiSpecific) {
            static_cast<LPDIRECT3DQUERY9>(query->m_apiSpecific)->Release();
        }

        query->m_apiSpecific = NULL;
    }
}

// OFFSET: %0x007C9B60
void CGxDeviceD3d::IReleaseD3dPools(int32_t freePools) {
    for (CGxPool* pool = CGxDevice::s_poolList.Head(); pool; pool = CGxDevice::s_poolList.Next(pool)) {
        if (freePools || pool->m_usage == GxPoolUsage_Stream || pool->m_usage == GxPoolUsage_Dynamic) {
            this->IReleaseD3dPool(pool);
        }
    }
}

// OFFSET: %0x007D1D00
void CGxDeviceD3d::TexDestroy(CGxTex* texId) {
    texId->m_link2.Unlink();

    if (texId->m_format == GxTex_Intz || texId->m_format == GxTex_Rawz) {
        texId->m_apiSpecificData2 = NULL;
    } else if (texId->m_apiSpecificData2) {
        delete[] static_cast<uint8_t*>(texId->m_apiSpecificData2);
        texId->m_apiSpecificData2 = NULL;
    }

    if (texId->m_apiSpecificData) {
        if (!texId->m_flags.m_renderTarget) {
            this->m_texDestroyList.LinkToTail(texId);
            return;
        }

        this->ITexReleaseD3dTexture(texId);
    }

    CGxDevice::TexDestroy(texId);
}

// OFFSET: %0x007D24D0
void CGxDeviceD3d::ITexCreate(CGxTex* texId) {
    texId->m_format = s_GxTexFormatToUse[texId->m_format];

    if (!texId->m_flags.m_renderTarget) {
        if (texId->m_format != GxTex_Intz && texId->m_format != GxTex_Rawz) {
            texId->m_apiSpecificData2 = SMemAlloc(this->ITexBitsSize(texId, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF), __FILE__, __LINE__, 0);
        }

        texId->m_needsCreation = 0;
    } else {
        this->ITexAcquire(texId);

        if (texId->m_apiSpecificData) {
            texId->m_needsCreation = 0;
        }
    }
}

// PROVEN: bfba044f IDENTICAL/UNPROVEN
// OFFSET: 0x00857B70/0x140588CB0
void CGxDeviceD3d::ITexMarkAsUpdated(CGxTex* texId) {
    if (!texId->m_needsUpdate || !this->m_context) {
        return;
    }

    if (texId->m_needsCreation || (!texId->m_apiSpecificData && !texId->m_apiSpecificData2)) {
        this->ITexCreate(texId);
    }

    if (!texId->m_needsCreation && (texId->m_apiSpecificData || texId->m_apiSpecificData2)) {
        if (texId->m_userFunc) {
            this->ITexUpload(texId);
        }

        CGxDevice::ITexMarkAsUpdated(texId);
    }
}

// OFFSET: %0x007D2870
void CGxDeviceD3d::ISceneBegin() {
    if (!this->m_context) {
        if (this->m_d3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            this->IReleaseD3dResources(0);

            D3DPRESENT_PARAMETERS d3dpp;
            this->ISetPresentParms(this->m_d3dCaps.AdapterOrdinal, this->m_d3dCaps.DeviceType, d3dpp, this->m_format);

            if (this->m_d3dDevice->Reset(&d3dpp) == D3D_OK) {
                this->IStateSetD3DDefaults();
                this->ISetWindowFocus(1);
                this->m_context = 1;
                this->m_intF5C = 0;
                this->m_d3dStereoDirty = 1;
                this->NotifyOnDeviceRestored();
                this->IRetrieveNvDepthHandle();
            }
        }

        if (!this->m_context) {
            return;
        }
    }

    this->ShaderConstantsClear();

    if (this->m_d3dDevice->BeginScene() == D3D_OK) {
        this->m_inScene = 1;
    }
}

// OFFSET: %0x007D46D0
void CGxDeviceD3d::IRetrieveNvDepthHandle() {
    if (!this->m_d3dNVAPI) {
        return;
    }

    this->ISceneBegin();

    CImVector black;
    black.b = 0x00;
    black.g = 0x00;
    black.r = 0x00;
    black.a = 0xFF;
    this->SceneClear(0x10, black);

    NvAPI_Status status = NvD3d9GetCurrentZBufferHandle(this->m_d3dDevice, &this->m_nvDepthSurfaceHandle);
    this->ISceneEnd();

    GxLog("ISceneBegin: Retrieving NvAPI depth surface handle (%p, status == %d)!\n", this->m_nvDepthSurfaceHandle, status);
}

// PROVEN: 6d5ddab6 IDENTICAL/DIFFERENT
// OFFSET: 0x008580E0/0x1405893F0
void CGxDeviceD3d::ScenePresent(void* window) {
    if (this->m_context) {
        int32_t scrShotClick = this->m_scrShotClick;

        CGxDevice::ScenePresent(window);

        if (this->unk_29B4) {
            D3DRECT rect;
            rect.x1 = 0;
            rect.y1 = 0;
            rect.x2 = 10;
            rect.y2 = 10;
            this->m_d3dDevice->Clear(1, &rect, D3DCLEAR_TARGET, 0xFFFF00FF, 1.0f, 0);
            this->unk_29B4 = 0;
        }

        this->ISceneEnd();
        this->ICursorDraw();

        if (this->m_format.m_fixLag) {
            if (this->m_eventQuery) {
                this->m_eventQuery->Issue(D3DISSUE_END);

                while (this->m_eventQuery->GetData(NULL, 0, D3DGETDATA_FLUSH) == S_FALSE) {
                    OsSleep(0);
                }
            } else {
                LPDIRECT3DSURFACE9 backBuffer;

                if (SUCCEEDED(this->m_d3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer))) {
                    D3DSURFACE_DESC desc;
                    backBuffer->GetDesc(&desc);

                    D3DLOCKED_RECT lockedRect;

                    if (SUCCEEDED(backBuffer->LockRect(&lockedRect, NULL, D3DLOCK_READONLY))) {
                        backBuffer->UnlockRect();
                    }

                    backBuffer->Release();
                }
            }
        }

        if (scrShotClick) {
            this->DeviceScreenShot();
        }

        this->WaitForFPSCap();

        HRESULT result;

        if (window) {
            RECT rect;
            GetClientRect(static_cast<HWND>(window), &rect);
            result = this->m_d3dDevice->Present(&rect, &rect, static_cast<HWND>(window), NULL);
        } else {
            result = this->m_d3dDevice->Present(NULL, NULL, NULL, NULL);
        }

        if (FAILED(result)) {
            this->m_context = 0;
        }

        if (this->m_d3dStereoHandle && this->m_d3dStereoDirty) {
            this->m_d3dStereoDirty = 0;

            NvAPI_Status status = NvAPI_Stereo_SetConvergence(static_cast<StereoHandle>(this->m_d3dStereoHandle), this->m_d3dStereoConvergence);
            CGxDevice::Log("NvAPI_Stereo_SetConvergence: status=%d convergence=%g", status, this->m_d3dStereoConvergence);

            status = NvAPI_Stereo_SetSeparation(static_cast<StereoHandle>(this->m_d3dStereoHandle), this->m_d3dStereoSeparation);
            CGxDevice::Log("NvAPI_Stereo_SetSeparation: status=%d separation=%g", status, this->m_d3dStereoSeparation);
        }
    }

    this->ISceneBegin();
}

// OFFSET: %0x007D2CE0
void CGxDeviceD3d::SetInstanceCount(uint32_t count) {
    this->m_d3dDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | count);
    this->m_d3dDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1);
}

// OFFSET: %0x007D2D30
void CGxDeviceD3d::ResetInstancing() {
    this->m_primVertexFormat = GxVertexBufferFormats_Last;
    this->m_primVertexFormats[1] = GxVertexBufferFormats_Last;
    this->m_primVertexBufs[1] = NULL;
    this->m_d3dCurrentVertexDecl = NULL;
    this->m_d3dVertexStreamBuf[0] = NULL;
    this->m_d3dVertexStreamBuf[1] = NULL;
    this->m_d3dVertexStreamStride[0] = 0;
    this->m_d3dVertexStreamStride[1] = 0;
    this->m_d3dDevice->SetStreamSourceFreq(0, 1);
    this->m_d3dDevice->SetStreamSourceFreq(1, 1);
    this->m_d3dDevice->SetStreamSource(1, NULL, 0, 0);
}

// OFFSET: %0x007D2DD0
void CGxDeviceD3d::DrawNonIndexed(const CGxBatch& batch) {
    if (!this->m_context || this->m_intF5C) {
        return;
    }

    this->IStateSync();
    this->ISetVertexDecl();
    this->ISetVertexStream();
    this->m_d3dDevice->DrawPrimitive(s_primitiveConversion[batch.m_primType], this->m_primVertexBufs[0]->m_index, this->PrimCalcCount(batch.m_primType, batch.m_count));
}

// OFFSET: %0x007D2E50
void CGxDeviceD3d::Draw(const CGxBatch& batch) {
    if (!this->m_context || this->m_intF5C) {
        return;
    }

    this->IStateSync();
    this->ISetVertexDecl();
    this->ISetIndexBuffer();
    this->ISetVertexStream();
    this->m_d3dDevice->DrawIndexedPrimitive(s_primitiveConversion[batch.m_primType], this->m_primVertexBufs[0]->m_index, batch.m_minIndex, batch.m_maxIndex - batch.m_minIndex + 1, batch.m_start + this->m_primIndexBuf->m_index, this->PrimCalcCount(batch.m_primType, batch.m_count));
}

// OFFSET: %0x007D2F00
void CGxDeviceD3d::DrawInstancedNonIndexed(const CGxBatch& batch) {
    if (!this->m_context || this->m_intF5C) {
        return;
    }

    this->IStateSync();
    this->ISetVertexDeclInstanced();
    this->ISetVertexStreams();
    this->m_d3dDevice->DrawPrimitive(s_primitiveConversion[batch.m_primType], this->m_primVertexBufs[0]->m_index, this->PrimCalcCount(batch.m_primType, batch.m_count));
}

// OFFSET: %0x007D2F80
void CGxDeviceD3d::DrawInstanced(const CGxBatch& batch) {
    if (!this->m_context || this->m_intF5C) {
        return;
    }

    this->IStateSync();
    this->ISetVertexDeclInstanced();
    this->ISetIndexBuffer();
    this->ISetVertexStreams();
    this->m_d3dDevice->DrawIndexedPrimitive(s_primitiveConversion[batch.m_primType], this->m_primVertexBufs[0]->m_index, batch.m_minIndex, batch.m_maxIndex - batch.m_minIndex + 1, batch.m_start + this->m_primIndexBuf->m_index, this->PrimCalcCount(batch.m_primType, batch.m_count));
}

// OFFSET: %0x007D47B0
void CGxDeviceD3d::IStateSetD3DDefaults() {
    this->m_d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    this->m_d3dDevice->SetRenderState(D3DRS_LOCALVIEWER, TRUE);
    this->m_d3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    this->m_d3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
    this->m_d3dDevice->SetRenderState(D3DRS_FOGDENSITY, 0);

    for (uint32_t i = 0; i < 16; i++) {
        this->m_d3dDevice->SetSamplerState(i, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
    }

    this->IRsForceUpdate();
    this->IRsSync(0);

    this->m_d3dCurrentVertexDecl = NULL;
    this->m_d3dCurrentIndexBuf = NULL;
    this->m_d3dVertexStreamBuf[0] = NULL;
    this->m_d3dVertexStreamBuf[1] = NULL;
    this->m_d3dVertexStreamStride[0] = 0;
    this->m_d3dVertexStreamStride[1] = 0;

    this->m_d3dDevice->GetRenderTarget(0, &this->m_defColorSurface);
    this->m_d3dDevice->GetDepthStencilSurface(&this->m_defDepthSurface);
    this->InitLights();
    this->m_ambientOnlyMode = 0;

    if (!this->m_eventQuery) {
        this->m_d3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, &this->m_eventQuery);
    }

    this->m_nvDepthSurfaceHandle = NULL;

    if (this->m_context) {
        this->IRetrieveNvDepthHandle();
    }

    this->ISceneBegin();
}

// PROVEN: e68271b6 IDENTICAL/IDENTICAL
// OFFSET: 0x00858810/0x140589CF0
void CGxDeviceD3d::IReleaseD3dVertexDecl() {
    for (uint32_t i = 0; i < GxVertexBufferFormats_Last; i++) {
        if (this->m_d3dVertexDecl[i]) {
            this->m_d3dVertexDecl[i]->Release();
            this->m_d3dVertexDecl[i] = NULL;
        }
    }
}

// OFFSET: %0x007AED20
void CGxDeviceD3d::IReleaseD3dShaders(int32_t freeShaders) {
    if (!freeShaders) {
        return;
    }

    for (uint32_t target = 0; target < GxShTargets_Last; target++) {
        for (CGxShader* shader = CGxDevice::s_shaderList[target].Head(); shader; shader = CGxDevice::s_shaderList[target].Next(shader)) {
            for (uint32_t i = 0; i < shader->m_permutes.Count(); i++) {
                this->IShaderDestroy(&shader->m_permutes[i]);
            }
        }
    }
}

// OFFSET: %0x007D2950
void CGxDeviceD3d::SceneClear(uint32_t mask, CImVector color) {
    CGxDevice::SceneClear(mask, color);

    if (!this->m_context) {
        return;
    }

    DWORD flags = (mask & 0xF) != 0;

    if (mask & 0x10) {
        flags |= D3DCLEAR_ZBUFFER;
    }

    if (this->m_viewportDirty) {
        this->IXformSetViewport();
    }

    this->m_d3dDevice->Clear(0, NULL, flags, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b), 1.0f, 0);
}

// PROVEN: c5d45942 IDENTICAL/NONE
// OFFSET: 0x008692C0
void CGxDeviceD3d::SceneClearToViewport(uint32_t mask, CImVector color) {
    this->SceneClear(mask, color);
}

// OFFSET: %0x007C7840
void CGxDeviceD3d::DeviceSetBaseMipLevel(uint32_t level) {
    this->ITexForceRecreation(1);
    CGxDevice::DeviceSetBaseMipLevel(level);
}

// OFFSET: %0x007C7940
void CGxDeviceD3d::LogCrashInfo(char* buffer, uint32_t size) {
    char line[260];

    if (!this->m_d3dDevice) {
        SStrPrintf(line, sizeof(line), "No device initialized.\r\n");
        SStrPack(buffer, line, size);
    }

    D3DDEVICE_CREATION_PARAMETERS parameters;
    this->m_d3dDevice->GetCreationParameters(&parameters);

    uint32_t adapterCount = this->m_d3d->GetAdapterCount();

    SStrPrintf(line, sizeof(line), "Adapter Count: %d\r\n", adapterCount);
    SStrPack(buffer, line, size);

    for (uint32_t i = 0; i < adapterCount; i++) {
        SStrPrintf(line, sizeof(line), "\r\nAdapter %d%s:\r\n", i, parameters.AdapterOrdinal == i ? " (primary)" : "");
        SStrPack(buffer, line, size);

        D3DADAPTER_IDENTIFIER9 identifier;
        this->m_d3d->GetAdapterIdentifier(i, 0, &identifier);

        SStrPrintf(line, sizeof(line), "  Driver: %s\r\n", identifier.Driver);
        SStrPack(buffer, line, size);
        SStrPrintf(line, sizeof(line), "  Version: %d.%d.%04d.%04d\r\n", HIWORD(identifier.DriverVersion.HighPart), LOWORD(identifier.DriverVersion.HighPart), HIWORD(identifier.DriverVersion.LowPart), LOWORD(identifier.DriverVersion.LowPart));
        SStrPack(buffer, line, size);
        SStrPrintf(line, sizeof(line), "  Description: %s\r\n", identifier.Description);
        SStrPack(buffer, line, size);
        SStrPrintf(line, sizeof(line), "  DeviceName: %s\r\n", identifier.DeviceName);
        SStrPack(buffer, line, size);
    }
}

// OFFSET: %0x007C8CE0
void CGxDeviceD3d::ICreateReadbackSurface() {
    if (this->m_readbackSurface) {
        return;
    }

    const CRect& window = this->DeviceDefWindow();
    D3DFORMAT format;

    if (this->m_format.m_window) {
        D3DDISPLAYMODE displayMode;

        if (SUCCEEDED(this->m_d3d->GetAdapterDisplayMode(this->m_d3dCaps.AdapterOrdinal, &displayMode))) {
            format = displayMode.Format;
        } else {
            format = this->m_desktopDisplayMode.Format;
        }
    } else {
        format = s_GxFormatToD3dFormat[this->m_format.m_colorFormat];
    }

    this->m_d3dDevice->CreateRenderTarget(static_cast<UINT>(window.maxX - window.minX), static_cast<UINT>(window.maxY - window.minY), format, D3DMULTISAMPLE_NONE, 0, TRUE, &this->m_readbackSurface, NULL);
}

// OFFSET: %0x007C8DB0
void CGxDeviceD3d::DeviceSetRenderTarget(EGxBuffer buffer, CGxTex* texture, uint32_t plane) {
    if (!this->m_context) {
        return;
    }

    CGxDevice::TextureTarget& target = this->m_textureTarget[buffer];

    if (target.m_texture == texture && target.m_plane == plane) {
        return;
    }

    CGxDevice::DeviceSetRenderTarget(buffer, texture, plane);

    if (target.m_apiSpecific) {
        static_cast<LPDIRECT3DSURFACE9>(target.m_apiSpecific)->Release();
        target.m_apiSpecific = NULL;
    }

    if (texture) {
        if (texture->m_needsCreation || (!texture->m_apiSpecificData && !texture->m_apiSpecificData2)) {
            this->ITexCreate(texture);
        }

        if (!texture->m_needsCreation && (texture->m_apiSpecificData || texture->m_apiSpecificData2)) {
            target.m_apiSpecific = this->ITexGetSurface(texture, 0, plane);
        }
    }

    if (buffer == GxBuffers_Color || buffer == GxBuffers_Depth) {
        LPDIRECT3DSURFACE9 depthSurface = this->m_defDepthSurface;
        LPDIRECT3DSURFACE9 colorSurface = this->m_defColorSurface;

        if (this->m_textureTarget[GxBuffers_Color].m_apiSpecific) {
            colorSurface = static_cast<LPDIRECT3DSURFACE9>(this->m_textureTarget[GxBuffers_Color].m_apiSpecific);

            if (!this->m_textureTarget[GxBuffers_Depth].m_apiSpecific) {
                this->ICreateDepthStencilSurface(texture->m_width, texture->m_height);

                if (this->m_defDepthStencilSurface) {
                    depthSurface = this->m_defDepthStencilSurface;
                }
            }
        }

        if (this->m_textureTarget[GxBuffers_Depth].m_apiSpecific) {
            depthSurface = static_cast<LPDIRECT3DSURFACE9>(this->m_textureTarget[GxBuffers_Depth].m_apiSpecific);
        }

        this->m_d3dDevice->SetRenderTarget(0, colorSurface);
        this->m_d3dDevice->SetDepthStencilSurface(depthSurface);
    } else {
        this->m_d3dDevice->SetRenderTarget(buffer, static_cast<LPDIRECT3DSURFACE9>(target.m_apiSpecific));
    }

    GxXformSetViewport(this->m_viewport.m_x.m_l, this->m_viewport.m_x.m_h, this->m_viewport.m_y.m_l, this->m_viewport.m_y.m_h, this->m_viewport.m_z.m_l, this->m_viewport.m_z.m_h);

    this->m_viewportDirty = 1;
    this->m_scissorRectDirty = 1;
}

// OFFSET: %0x007C96A0
void CGxDeviceD3d::DeviceResolveDepthBuffer(CGxTex* texture) {
    if (!this->m_context) {
        return;
    }

    if (texture->m_needsCreation || (!texture->m_apiSpecificData && !texture->m_apiSpecificData2)) {
        this->ITexCreate(texture);

        if (texture->m_needsCreation || (!texture->m_apiSpecificData && !texture->m_apiSpecificData2)) {
            return;
        }
    }

    if (this->m_d3dNVAPI) {
        NVDX_ObjectHandle textureHandle;

        if (this->m_nvDepthSurfaceHandle && NvD3d9GetTextureHandle(static_cast<IDirect3DBaseTexture9*>(texture->m_apiSpecificData), &textureHandle) == NVAPI_OK) {
            NvAPI_Status status = NvD3d9StretchRect(this->m_d3dDevice, this->m_nvDepthSurfaceHandle, NULL, textureHandle, NULL, D3DTEXF_POINT);

            if (status != NVAPI_OK) {
                DebugLog("NVAPI Error: NvAPI_D3D9_StretchRect - %x(%d)\n", status, status);
            }
        }
    } else if (g_theGxDevicePtr->Caps().m_texFmt[GxTex_Resz]) {
        g_theGxDevicePtr->RsPush();
        g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);
        GxRsSet(GxRs_DepthTest, 0);
        GxRsSet(GxRs_DepthWrite, 0);
        GxRsSet(GxRs_ColorWrite, 0);

        CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(C3Vector), 1);
        C3Vector* vertex = reinterpret_cast<C3Vector*>(g_theGxDevicePtr->BufLock(buf));
        vertex->x = 0.0f;
        vertex->y = 0.0f;
        vertex->z = 0.0f;
        g_theGxDevicePtr->BufUnlock(buf, sizeof(C3Vector));
        buf->unk_001C = 1;
        g_theGxDevicePtr->PrimVertexPtr(buf, GxVBF_P);

        CGxBatch batch;
        batch.m_primType = GxPrim_Points;
        batch.m_start = 0;
        batch.m_count = 1;
        batch.m_minIndex = 0;
        batch.m_maxIndex = 1;
        g_theGxDevicePtr->DrawNonIndexed(batch);

        this->m_d3dDevice->SetRenderState(D3DRS_POINTSIZE, 0x7FA05000);
        this->m_d3dDevice->SetRenderState(D3DRS_POINTSIZE, 0);
        g_theGxDevicePtr->RsPop();
    }
}

// PROVEN: 4461ee72 IDENTICAL/DIFFERENT
// OFFSET: 0x0084CAF0/0x14057B440
void CGxDeviceD3d::DeviceCopyTex(CGxTex* source, uint32_t sourcePlane, CGxTex* dest, uint32_t destPlane) {
    LPDIRECT3DSURFACE9 destSurface = NULL;
    LPDIRECT3DSURFACE9 sourceSurface = NULL;

    if (source) {
        if (source->m_needsCreation || (!source->m_apiSpecificData && !source->m_apiSpecificData2)) {
            this->ITexCreate(source);
        }

        if (!source->m_needsCreation && (source->m_apiSpecificData || source->m_apiSpecificData2)) {
            sourceSurface = this->ITexGetSurface(source, 0, sourcePlane);
        }
    }

    if (dest) {
        if (dest->m_needsCreation || (!dest->m_apiSpecificData && !dest->m_apiSpecificData2)) {
            this->ITexCreate(dest);
        }

        if (!dest->m_needsCreation && (dest->m_apiSpecificData || dest->m_apiSpecificData2)) {
            destSurface = this->ITexGetSurface(dest, 0, destPlane);
        }
    }

    if (sourceSurface) {
        if (destSurface) {
            this->m_d3dDevice->StretchRect(sourceSurface, NULL, destSurface, NULL, D3DTEXF_NONE);
        }

        sourceSurface->Release();
    }

    if (destSurface) {
        destSurface->Release();
    }
}

// OFFSET: %0x007D0DC0
LPDIRECT3DSURFACE9 CGxDeviceD3d::ITexGetSurface(CGxTex* texId, uint32_t level, uint32_t face) {
    LPDIRECT3DSURFACE9 surface = NULL;

    switch (texId->m_target) {
        case GxTex_2d:
        case GxTex_NonPow2:
            static_cast<IDirect3DTexture9*>(texId->m_apiSpecificData)->GetSurfaceLevel(level, &surface);
            return surface;

        case GxTex_CubeMap:
            static_cast<IDirect3DCubeTexture9*>(texId->m_apiSpecificData)->GetCubeMapSurface(s_faceTypes[face], level, &surface);
            return surface;

        default:
            return NULL;
    }
}

// OFFSET: [%0x007C9600, %0x007C9B30]
CGxDeviceD3d::~CGxDeviceD3d() {
}

// OFFSET: %0x007C9310
void CGxDeviceD3d::DeviceReadPixels(CiRect& rect, TSGrowableArray<CImVector>& pixels) {
    this->ClampRectToWindow(rect);

    uint32_t height = rect.maxY - rect.minY;
    uint32_t width = rect.maxX - rect.minX;
    uint32_t count = width * height;
    pixels.SetCount(count);

    LPDIRECT3DSURFACE9 surface = NULL;
    HRESULT result;

    if (this->m_format.m_multisampleCount < 2) {
        result = this->m_d3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &surface);
    } else {
        RECT sourceRect = { rect.minX, rect.minY, static_cast<LONG>(width), static_cast<LONG>(height) };

        this->ICreateReadbackSurface();

        if (!this->m_readbackSurface) {
            return;
        }

        result = this->m_d3dDevice->StretchRect(this->m_defColorSurface, &sourceRect, this->m_readbackSurface, &sourceRect, D3DTEXF_NONE);
        surface = this->m_readbackSurface;
        surface->AddRef();
    }

    if (FAILED(result)) {
        return;
    }

    D3DLOCKED_RECT lockedRect;

    if (SUCCEEDED(surface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY))) {
        D3DSURFACE_DESC desc;
        surface->GetDesc(&desc);

        if (width <= desc.Width && height <= desc.Height) {
            const uint8_t* source = static_cast<const uint8_t*>(lockedRect.pBits);

            switch (desc.Format) {
                case D3DFMT_A8R8G8B8:
                case D3DFMT_X8R8G8B8: {
                    CImVector* dest = pixels.Ptr();

                    for (uint32_t y = height; y; y--) {
                        memcpy(dest, source, sizeof(CImVector) * width);
                        source += lockedRect.Pitch;
                        dest += width;
                    }

                    break;
                }

                case D3DFMT_R5G6B5: {
                    uint8_t* dest = reinterpret_cast<uint8_t*>(pixels.Ptr());

                    for (uint32_t y = height; y; y--) {
                        const uint8_t* row = source;

                        for (uint32_t x = width; x; x--) {
                            dest[3] = 0xFF;
                            dest[2] = row[1] & 0xF8;
                            dest[1] = (*reinterpret_cast<const uint16_t*>(row) >> 3) & 0xFC;
                            dest[0] = row[0] << 3;
                            dest += 4;
                            row += 2;
                        }

                        source += lockedRect.Pitch;
                    }

                    break;
                }

                case D3DFMT_X1R5G5B5:
                case D3DFMT_A1R5G5B5: {
                    uint8_t* dest = reinterpret_cast<uint8_t*>(pixels.Ptr());

                    for (uint32_t y = height; y; y--) {
                        const uint8_t* row = source;

                        for (uint32_t x = width; x; x--) {
                            dest[3] = 0xFF;
                            dest[2] = (*reinterpret_cast<const uint16_t*>(row) >> 7) & 0xF8;
                            dest[1] = (*reinterpret_cast<const uint16_t*>(row) >> 2) & 0xF8;
                            dest[0] = row[0] << 3;
                            dest += 4;
                            row += 2;
                        }

                        source += lockedRect.Pitch;
                    }

                    break;
                }

                default:
                    memset(pixels.Ptr(), 0xFF, sizeof(CImVector) * count);
                    break;
            }
        }

        surface->UnlockRect();
    }

    surface->Release();
}

// OFFSET: %0x007C7380
int32_t CGxDeviceD3d::ICheckDepthStencilFormat(D3DFORMAT format) {
    return SUCCEEDED(this->m_d3d->CheckDeviceFormat(this->m_d3dCaps.AdapterOrdinal, this->m_d3dCaps.DeviceType, this->m_devAdapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, format)) && SUCCEEDED(this->m_d3d->CheckDepthStencilMatch(this->m_d3dCaps.AdapterOrdinal, this->m_d3dCaps.DeviceType, this->m_devAdapterFormat, this->m_devAdapterFormat, format));
}

// OFFSET: %0x007C7310
uint32_t CGxDeviceD3d::IGetAdapterIndex(uint32_t monitor) {
    HMONITOR handle = static_cast<HMONITOR>(CGxDevice::AdapterGetMonitorHandle(monitor));

    if (!handle) {
        return 0;
    }

    uint32_t adapter = 0;

    if (!this->m_d3d->GetAdapterCount()) {
        return 0;
    }

    while (handle != this->m_d3d->GetAdapterMonitor(adapter)) {
        if (++adapter >= this->m_d3d->GetAdapterCount()) {
            return 0;
        }
    }

    return adapter;
}

// OFFSET: %0x007C9580
void CGxDeviceD3d::DeviceReadDepths(CiRect& rect, TSGrowableArray<float>& depths) {
    this->ClampRectToWindow(rect);

    uint32_t count = (rect.maxY - rect.minY) * (rect.maxX - rect.minX);
    depths.SetCount(count);
    memset(depths.Ptr(), 0, sizeof(float) * count);
}

// OFFSET: %0x007D25B0
int32_t CGxDeviceD3d::TexCopy(CGxTex* source, CGxTex* dest, const C2iVector& destPos, const C2iVector& sourcePos, const C2iVector& size, uint32_t level, uint32_t plane) {
    if (!this->m_context) {
        return 0;
    }

    if (dest->m_needsCreation || (!dest->m_apiSpecificData && !dest->m_apiSpecificData2)) {
        this->ITexCreate(dest);
    }

    if (dest->m_needsCreation || (!dest->m_apiSpecificData && !dest->m_apiSpecificData2)) {
        return 0;
    }

    RECT sourceRect = { sourcePos.x, sourcePos.y, sourcePos.x + size.x, sourcePos.y + size.y };
    RECT destRect = { destPos.x, destPos.y, destPos.x + size.x, destPos.y + size.y };

    LPDIRECT3DSURFACE9 sourceSurface = this->m_defColorSurface;

    if (source) {
        sourceSurface = this->ITexGetSurface(source, 0, 0);

        if (!sourceSurface) {
            return 0;
        }
    }

    LPDIRECT3DSURFACE9 destSurface = this->ITexGetSurface(dest, level, plane);

    if (!destSurface) {
        return 0;
    }

    HRESULT result = this->m_d3dDevice->StretchRect(sourceSurface, &sourceRect, destSurface, &destRect, D3DTEXF_NONE);

    if (source) {
        sourceSurface->Release();
    }

    destSurface->Release();

    return SUCCEEDED(result);
}

// OFFSET: %0x007D26D0
int32_t CGxDeviceD3d::TexStretch(CGxTex* source, CGxTex* dest, const CiRect* destRect, const CiRect* sourceRect, uint32_t level, uint32_t face) {
    if (!this->m_context) {
        return 0;
    }

    if (dest->m_needsCreation || (!dest->m_apiSpecificData && !dest->m_apiSpecificData2)) {
        this->ITexCreate(dest);
    }

    if (dest->m_needsCreation || (!dest->m_apiSpecificData && !dest->m_apiSpecificData2)) {
        return 0;
    }

    if (source && (source->m_needsCreation || (!source->m_apiSpecificData && !source->m_apiSpecificData2))) {
        return 0;
    }

    LPDIRECT3DSURFACE9 destSurface = this->ITexGetSurface(dest, level, face);

    if (!destSurface) {
        return 0;
    }

    LPDIRECT3DSURFACE9 sourceSurface = this->m_defColorSurface;

    if (source) {
        sourceSurface = this->ITexGetSurface(source, 0, 0);

        if (!sourceSurface) {
            return 0;
        }
    }

    RECT d3dDestRect = { 0, 0, 0, 0 };

    if (destRect) {
        d3dDestRect.left = destRect->minX;
        d3dDestRect.top = destRect->minY;
        d3dDestRect.right = destRect->maxX;
        d3dDestRect.bottom = destRect->maxY;
    }

    RECT d3dSourceRect = { 0, 0, 0, 0 };

    if (sourceRect) {
        d3dSourceRect.left = sourceRect->minX;
        d3dSourceRect.top = sourceRect->minY;
        d3dSourceRect.right = sourceRect->maxX;
        d3dSourceRect.bottom = sourceRect->maxY;
    }

    D3DTEXTUREFILTERTYPE filter;

    if (destRect && sourceRect && d3dDestRect.right - d3dDestRect.left == d3dSourceRect.right - d3dSourceRect.left && d3dDestRect.bottom - d3dDestRect.top == d3dSourceRect.bottom - d3dSourceRect.top) {
        filter = D3DTEXF_NONE;
    } else {
        filter = D3DTEXF_LINEAR;
    }

    HRESULT result = this->m_d3dDevice->StretchRect(sourceSurface, sourceRect ? &d3dSourceRect : NULL, destSurface, destRect ? &d3dDestRect : NULL, filter);

    if (source) {
        sourceSurface->Release();
    }

    destSurface->Release();

    return SUCCEEDED(result);
}

// OFFSET: %0x007D3520
void CGxDeviceD3d::ICreateDepthStencilSurface(uint32_t width, uint32_t height) {
    if (this->m_defDepthStencilSurface) {
        D3DSURFACE_DESC desc;

        if (SUCCEEDED(this->m_defDepthStencilSurface->GetDesc(&desc)) && desc.Width >= width && desc.Height >= height) {
            return;
        }

        this->m_defDepthStencilSurface->Release();
    }

    D3DFORMAT formats[3] = { D3DFMT_D24S8, D3DFMT_D24X8, D3DFMT_D32 };

    for (uint32_t i = 0; i < 3; i++) {
        D3DFORMAT format = formats[i];

        if (this->ICheckDepthStencilFormat(format)) {
            this->m_d3dDevice->CreateDepthStencilSurface(width, height, format, D3DMULTISAMPLE_NONE, 0, FALSE, &this->m_defDepthStencilSurface, NULL);
        }

        if (this->m_defDepthStencilSurface) {
            break;
        }
    }
}

// OFFSET: %0x007C9870
CGxDeviceD3d::CGxDeviceD3d() {
    this->m_d3dStereoConvergence = 1.0f;
    this->m_api = GxApi_D3d9;
    this->m_hwnd = NULL;
    this->m_d3dLib = NULL;
    this->m_d3d = NULL;
    this->m_d3dDevice = NULL;
    this->m_d3dIsHwDevice = 0;
    this->m_d3dNVAPI = 0;
    this->m_d3dStereoEnabled = 0;
    this->m_d3dStereoRestore = 0;
    this->m_d3dStereoHandle = NULL;
    this->m_d3dStereoSeparation = 25.0f;
    this->m_d3dStereoDirty = 0;
    this->m_caps.m_colorFormat = GxCF_argb;
    this->m_caps.unk_001C = 1;
    memset(&this->m_desktopDisplayMode, 0, sizeof(this->m_desktopDisplayMode));
    memset(this->m_deviceStates, 0xFF, sizeof(this->m_deviceStates));
    memset(this->m_texEnabled, 0, sizeof(this->m_texEnabled));
    this->m_defDepthStencilSurface = NULL;
    this->m_defColorSurface = NULL;
    this->m_defDepthSurface = NULL;
    this->m_nvDepthSurfaceHandle = NULL;
    this->m_readbackSurface = NULL;
    this->m_eventQuery = NULL;
    this->m_hwCursorDirty = 1;
    this->m_hwCursorTexture = NULL;
    this->m_hwCursorBitmap = NULL;
    this->unk_29B4 = 0;
    memset(this->m_d3dVertexDecl, 0, sizeof(this->m_d3dVertexDecl));
    this->m_d3dCurrentVertexDecl = NULL;
    this->m_d3dCurrentIndexBuf = NULL;
    this->m_d3dVertexStreamBuf[0] = NULL;
    this->m_d3dVertexStreamBuf[1] = NULL;
    this->m_d3dVertexStreamStride[0] = 0;
    this->m_d3dVertexStreamStride[1] = 0;

    for (uint32_t i = 0; i < GxTexFormats_Last; i++) {
        this->m_scratchTextures[i] = NULL;
    }

    this->m_texCacheUsed = 0;
    this->m_texCacheSize = 0x1000000;
    this->DeviceCreatePools();
    this->DeviceCreateStreamBufs();
}

// OFFSET: %0x007D0C60
int32_t CGxDeviceD3d::ITexAllocD3dTexture(CGxTex* texId) {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t baseMip;
    uint32_t mipCount;
    this->ITexWHDStartEnd(texId, width, height, depth, baseMip, mipCount);

    DWORD usage = 0;

    if (texId->m_flags.m_renderTarget) {
        usage = CGxDevice::s_texFormatIsDepth[texId->m_format] ? D3DUSAGE_DEPTHSTENCIL : D3DUSAGE_RENDERTARGET;
    }

    switch (texId->m_target) {
        case GxTex_2d:
        case GxTex_NonPow2: {
            LPDIRECT3DTEXTURE9 texture = NULL;

            if (FAILED(this->m_d3dDevice->CreateTexture(width, height, mipCount - baseMip, usage, s_GxTexFmtToD3dFmt[texId->m_format], D3DPOOL_DEFAULT, &texture, NULL))) {
                return 0;
            }

            texId->m_apiSpecificData = texture;
            break;
        }

        case GxTex_CubeMap: {
            LPDIRECT3DCUBETEXTURE9 texture = NULL;

            if (FAILED(this->m_d3dDevice->CreateCubeTexture(width, mipCount - baseMip, usage, s_GxTexFmtToD3dFmt[texId->m_format], D3DPOOL_DEFAULT, &texture, NULL))) {
                return 0;
            }

            texId->m_apiSpecificData = texture;
            break;
        }

        case GxTex_Volume: {
            LPDIRECT3DVOLUMETEXTURE9 texture = NULL;

            if (FAILED(this->m_d3dDevice->CreateVolumeTexture(width, height, depth, mipCount - baseMip, usage, s_GxTexFmtToD3dFmt[texId->m_format], D3DPOOL_DEFAULT, &texture, NULL))) {
                return 0;
            }

            texId->m_apiSpecificData = texture;
            break;
        }

        default:
            break;
    }

    if (!texId->m_flags.m_renderTarget) {
        this->m_texCacheUsed += this->ITexBitsSize(texId, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
    }

    return 1;
}

// PROVEN: b03464a5 IDENTICAL/NONE
// OFFSET: 0x00856620
void CGxDeviceD3d::TexSetCacheSize(int32_t size) {
    this->m_texCacheSize = size;
}

// OFFSET: %0x007D0E30
LPDIRECT3DTEXTURE9 CGxDeviceD3d::ITexGetScratch(EGxTexFormat format) {
    LPDIRECT3DTEXTURE9 texture = this->m_scratchTextures[format];

    if (!texture) {
        if (FAILED(this->m_d3dDevice->CreateTexture(1024, 1024, 10, 0, s_GxTexFmtToD3dFmt[format], D3DPOOL_SYSTEMMEM, &texture, NULL))) {
            return NULL;
        }

        this->m_scratchTextures[format] = texture;
    }

    return texture;
}

// OFFSET: 0x00856700/0x140587520
void CGxDeviceD3d::ITexUploadBox(CGxTex* texId, uint32_t level, const D3DBOX* box, const void* bits, uint32_t pitch) {
    LPDIRECT3DVOLUMETEXTURE9 scratch = NULL;

    if (FAILED(this->m_d3dDevice->CreateVolumeTexture(texId->m_width, texId->m_height, texId->m_depth, 1, 0, s_GxTexFmtToD3dFmt[texId->m_format], D3DPOOL_SYSTEMMEM, &scratch, NULL)) || !scratch) {
        return;
    }

    D3DLOCKED_BOX lockedBox;

    if (FAILED(scratch->LockBox(level, &lockedBox, box, 0))) {
        scratch->Release();
        return;
    }

    uint8_t* dest = static_cast<uint8_t*>(lockedBox.pBits);
    const uint8_t* source = static_cast<const uint8_t*>(bits);

    for (uint32_t slice = box->Front; slice < box->Back; slice++) {
        C2iVector size;
        size.x = box->Right - box->Left;
        size.y = box->Bottom - box->Top;
        Blit(size, BlitAlpha_0, source, pitch, GxGetBlitFormat(texId->m_dataFormat), dest, lockedBox.RowPitch, GxGetBlitFormat(texId->m_format));

        dest += lockedBox.SlicePitch;
        source += pitch * (box->Bottom - box->Top);
    }

    scratch->UnlockBox(level);
    this->m_d3dDevice->UpdateTexture(scratch, static_cast<IDirect3DBaseTexture9*>(texId->m_apiSpecificData));
    scratch->Release();
}

// OFFSET: %0x007D0FF0
void CGxDeviceD3d::ITexUploadRect(CGxTex* texId, uint32_t face, uint32_t level, const RECT* rect, const void* bits, uint32_t pitch) {
    LPDIRECT3DSURFACE9 scratchSurface = NULL;
    LPDIRECT3DSURFACE9 surface = NULL;
    LPDIRECT3DTEXTURE9 scratch = this->ITexGetScratch(texId->m_format);

    if (scratch && SUCCEEDED(scratch->GetSurfaceLevel(level, &scratchSurface))) {
        surface = this->ITexGetSurface(texId, level, face);

        D3DLOCKED_RECT lockedRect;

        if (SUCCEEDED(scratchSurface->LockRect(&lockedRect, rect, 0))) {
            C2iVector size;
            size.x = rect->right - rect->left;
            size.y = rect->bottom - rect->top;
            Blit(size, BlitAlpha_0, bits, pitch, GxGetBlitFormat(texId->m_format), lockedRect.pBits, lockedRect.Pitch, GxGetBlitFormat(texId->m_format));

            scratchSurface->UnlockRect();

            POINT point = { rect->left, rect->top };
            this->m_d3dDevice->UpdateSurface(scratchSurface, rect, surface, &point);
        }
    }

    if (scratchSurface) {
        scratchSurface->Release();
    }

    if (surface) {
        surface->Release();
    }
}

// OFFSET: %0x007D1100
void CGxDeviceD3d::ITexUploadAll(CGxTex* texId) {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t baseMip;
    uint32_t mipCount;
    this->ITexWHDStartEnd(texId, width, height, depth, baseMip, mipCount);

    int32_t compressed = texId->m_format >= GxTex_Dxt1 && texId->m_format <= GxTex_Dxt5;
    const uint8_t* bits = static_cast<const uint8_t*>(texId->m_apiSpecificData2);
    uint32_t faceCount = texId->m_target == GxTex_CubeMap ? 6 : 1;

    for (uint32_t face = 0; face < faceCount; face++) {
        uint32_t mipWidth = width;
        uint32_t mipHeight = height;
        uint32_t mipDepth = depth;

        for (uint32_t mip = baseMip; mip != mipCount; mip++) {
            uint32_t rectWidth = mipWidth;
            uint32_t rectHeight = mipHeight;

            if (compressed) {
                rectHeight = (rectHeight + 3) & ~3;
                rectWidth = (rectWidth + 3) & ~3;
            }

            uint32_t pitch = GxCalcTexelStrideInBytes(texId->m_format, mipWidth);

            if (texId->m_apiSpecificData) {
                if (texId->m_target == GxTex_Volume) {
                    D3DBOX box;
                    box.Left = 0;
                    box.Top = 0;
                    box.Front = 0;
                    box.Back = mipDepth;
                    box.Right = rectWidth;
                    box.Bottom = rectHeight;
                    this->ITexUploadBox(texId, mip - baseMip, &box, bits, pitch);
                } else {
                    RECT rect;
                    rect.left = 0;
                    rect.top = 0;
                    rect.right = rectWidth;
                    rect.bottom = rectHeight;
                    this->ITexUploadRect(texId, face, mip - baseMip, &rect, bits, pitch);
                }
            }

            if (compressed) {
                bits += pitch * (mipHeight >> 2);
            } else {
                bits += mipHeight * pitch;
            }

            mipHeight >>= 1;
            mipDepth >>= 1;
            mipWidth >>= 1;

            if (!mipWidth) {
                mipWidth = 1;
            }

            if (!mipHeight) {
                mipHeight = 1;
            }

            if (!mipDepth) {
                mipDepth = 1;
            }
        }
    }
}

// OFFSET: %0x007D12A0
void CGxDeviceD3d::ITexUpload(CGxTex* texId) {
    uint32_t texelStrideInBytes;
    const void* texels = NULL;
    texId->m_userFunc(GxTex_Lock, texId->m_width, texId->m_height, texId->m_depth, 0, texId->m_userArg, texelStrideInBytes, texels);

    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t baseMip;
    uint32_t mipCount;
    this->ITexWHDStartEnd(texId, width, height, depth, baseMip, mipCount);

    int32_t compressed = texId->m_dataFormat >= GxTex_Dxt1 && texId->m_dataFormat <= GxTex_Dxt5;
    uint32_t faceCount = texId->m_target == GxTex_CubeMap ? 6 : 1;
    uint8_t* bits = static_cast<uint8_t*>(texId->m_apiSpecificData2);

    for (uint32_t face = 0; face < faceCount; face++) {
        uint32_t mipWidth = width;
        uint32_t mipHeight = height;
        uint32_t mipDepth = depth;

        for (uint32_t mip = baseMip; mip != mipCount; mip++) {
            texels = NULL;
            texId->m_userFunc(GxTex_Latch, mipWidth, mipHeight, texId->m_target == GxTex_CubeMap ? face : mipDepth, mip, texId->m_userArg, texelStrideInBytes, texels);

            if (texels && !texId->m_flags.m_renderTarget) {
                uint32_t minY = texId->m_updateRect.minY >> mip;
                uint32_t minX = texId->m_updateRect.minX >> mip;
                uint32_t maxY = texId->m_updateRect.maxY >> mip;
                uint32_t maxX = texId->m_updateRect.maxX >> mip;

                if (minY == maxY) {
                    maxY++;
                }

                if (minX == maxX) {
                    maxX++;
                }

                if (compressed) {
                    minY &= ~3;
                    minX &= ~3;
                    maxY = (maxY + 3) & ~3;
                    maxX = (maxX + 3) & ~3;
                }

                const uint8_t* src = static_cast<const uint8_t*>(texels);

                if (!texId->m_flags.m_bit15) {
                    if (compressed) {
                        src += s_texFormatBytesPerBlock[texId->m_dataFormat] * (minX >> 2) + texelStrideInBytes * (minY >> 2);
                    } else {
                        src += ((minX * s_texFormatBitDepth[texId->m_dataFormat]) >> 3) + texelStrideInBytes * minY;
                    }
                }

                uint32_t pitch = GxCalcTexelStrideInBytes(texId->m_format, mipWidth);
                uint8_t* dest;

                if (compressed) {
                    dest = bits + s_texFormatBytesPerBlock[texId->m_dataFormat] * (minX >> 2) + pitch * (minY >> 2);
                } else {
                    dest = bits + ((minX * s_texFormatBitDepth[texId->m_dataFormat]) >> 3) + minY * pitch;
                }

                if (texId->m_target == GxTex_Volume) {
                    uint32_t minZ = texId->m_updatePlaneMin >> mip;
                    uint32_t maxZ = texId->m_updatePlaneMax >> mip;

                    if (minZ == maxZ) {
                        maxZ++;
                    }

                    const uint8_t* sliceSrc = src;
                    uint8_t* sliceDest = dest;

                    for (uint32_t z = minZ; z < maxZ; z++) {
                        C2iVector size;
                        size.x = maxX - minX;
                        size.y = maxY - minY;
                        Blit(size, BlitAlpha_0, sliceSrc, texelStrideInBytes, GxGetBlitFormat(texId->m_dataFormat), sliceDest, pitch, GxGetBlitFormat(texId->m_format));

                        if (compressed) {
                            sliceSrc += texelStrideInBytes * (mipHeight >> 2);
                            sliceDest += pitch * (mipHeight >> 2);
                        } else {
                            sliceSrc += texelStrideInBytes * mipHeight;
                            sliceDest += mipHeight * pitch;
                        }
                    }

                    if (texId->m_apiSpecificData) {
                        D3DBOX box;
                        box.Left = minX;
                        box.Top = minY;
                        box.Right = maxX;
                        box.Bottom = maxY;
                        box.Front = minZ;
                        box.Back = maxZ;
                        this->ITexUploadBox(texId, mip - baseMip, &box, dest, pitch);
                    }

                    if (compressed) {
                        bits += mipDepth * pitch * (mipHeight >> 2);
                    } else {
                        bits += pitch * mipDepth * mipHeight;
                    }
                } else {
                    C2iVector size;
                    size.x = maxX - minX;
                    size.y = maxY - minY;
                    Blit(size, BlitAlpha_0, src, texelStrideInBytes, GxGetBlitFormat(texId->m_dataFormat), dest, pitch, GxGetBlitFormat(texId->m_format));

                    if (texId->m_apiSpecificData) {
                        RECT rect;
                        rect.left = minX;
                        rect.top = minY;
                        rect.right = maxX;
                        rect.bottom = maxY;
                        this->ITexUploadRect(texId, face, mip - baseMip, &rect, dest, pitch);
                    }

                    if (compressed) {
                        bits += pitch * (mipHeight >> 2);
                    } else {
                        bits += mipHeight * pitch;
                    }
                }
            }

            mipHeight >>= 1;
            mipDepth >>= 1;
            mipWidth >>= 1;

            if (!mipWidth) {
                mipWidth = 1;
            }

            if (!mipHeight) {
                mipHeight = 1;
            }

            if (!mipDepth) {
                mipDepth = 1;
            }
        }
    }

    texels = NULL;
    texId->m_userFunc(GxTex_Unlock, texId->m_width, texId->m_height, texId->m_depth, 0, texId->m_userArg, texelStrideInBytes, texels);
}

// OFFSET: %0x007D1700
void CGxDeviceD3d::ITexTakeD3dTexture(CGxTex* dest, CGxTex* source) {
    dest->m_apiSpecificData = source->m_apiSpecificData;
    source->m_apiSpecificData = NULL;
    source->m_link2.Unlink();

    if (!source->m_apiSpecificData2) {
        CGxDevice::TexDestroy(source);
    }
}

// OFFSET: %0x007D1770
void CGxDeviceD3d::ITexUnlinkBound() {
    for (uint32_t i = GxRs_VertexTexture0; i <= GxRs_VertexTexture3; i++) {
        CGxTex* texId = static_cast<CGxTex*>(this->m_appRenderStates[i].m_value.m_data.p);

        if (texId && !texId->m_flags.m_renderTarget) {
            texId->m_link2.Unlink();
        }
    }

    for (uint32_t i = GxRs_Texture0; i <= GxRs_Texture15; i++) {
        CGxTex* texId = static_cast<CGxTex*>(this->m_appRenderStates[i].m_value.m_data.p);

        if (texId && !texId->m_flags.m_renderTarget) {
            texId->m_link2.Unlink();
        }
    }
}

// OFFSET: %0x007D1A00
void CGxDeviceD3d::ITexReleaseD3dTexture(CGxTex* texId) {
    IDirect3DBaseTexture9* texture = static_cast<IDirect3DBaseTexture9*>(texId->m_apiSpecificData);

    if (!texture) {
        return;
    }

    if (!texId->m_flags.m_renderTarget) {
        this->m_texCacheUsed -= this->ITexBitsSize(texId, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
    }

    texture->Release();
    texId->m_apiSpecificData = NULL;
    texId->m_link2.Unlink();
}

// OFFSET: %0x007D1A80
void CGxDeviceD3d::ITexForceRecreation(int32_t all) {
    for (uint32_t i = 0; i < GxTexFormats_Last; i++) {
        if (this->m_scratchTextures[i]) {
            this->m_scratchTextures[i]->Release();
            this->m_scratchTextures[i] = NULL;
        }
    }

    CGxTex* next;

    for (CGxTex* texId = this->m_texDestroyList.Head(); !this->m_texDestroyList.IsEnd(texId); texId = next) {
        next = this->m_texDestroyList.RawNext(texId);
        this->ITexReleaseD3dTexture(texId);
        CGxDevice::TexDestroy(texId);
    }

    for (CGxTex* texId = CGxDevice::s_textureList.Head(); texId; texId = CGxDevice::s_textureList.Next(texId)) {
        this->ITexReleaseD3dTexture(texId);

        if (all || texId->m_flags.m_renderTarget) {
            if (texId->m_format == GxTex_Intz || texId->m_format == GxTex_Rawz) {
                texId->m_apiSpecificData2 = NULL;
            } else if (texId->m_apiSpecificData2) {
                delete[] static_cast<uint8_t*>(texId->m_apiSpecificData2);
                texId->m_apiSpecificData2 = NULL;
            }

            // STATIC: %0x00D931A4
            static CiRect s_updateRect;

            texId->m_needsCreation = 1;
            this->TexMarkForUpdate(texId, s_updateRect, 0);

            uint32_t texelStrideInBytes;
            const void* texels;
            texId->m_userFunc(GxTex_3, texId->m_width, texId->m_height, 0, 0, texId->m_userArg, texelStrideInBytes, texels);
        }
    }

    if (this->m_hwnd && this->m_d3dDevice) {
        this->NotifyOnTextureRecreation();
    }
}

// OFFSET: %0x007D1DC0
int32_t CGxDeviceD3d::ITexEvictFor(CGxTex* texId, uint32_t size) {
    for (CGxTex* cached = this->m_texCacheList.Tail(); cached && cached->unk_0050 == static_cast<uint32_t>(this->m_frameCount); cached = this->m_texCacheList.Prev(cached)) {
        if (texId->m_flags.m_filter == cached->m_flags.m_filter && texId->m_flags.m_renderTarget == cached->m_flags.m_renderTarget && texId->m_format == cached->m_format && texId->m_target == cached->m_target && texId->m_width == cached->m_width && texId->m_height == cached->m_height) {
            this->ITexTakeD3dTexture(texId, cached);
            return 1;
        }
    }

    CGxTex* oldest;

    while ((oldest = this->m_texCacheList.Tail()) != NULL) {
        this->ITexReleaseD3dTexture(oldest);

        if (size + this->m_texCacheUsed < this->m_texCacheSize && this->ITexAllocD3dTexture(texId)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: %0x007D1F20
void CGxDeviceD3d::ITexLinkBound() {
    for (uint32_t i = GxRs_VertexTexture0; i <= GxRs_VertexTexture3; i++) {
        CGxTex* texId = static_cast<CGxTex*>(this->m_appRenderStates[i].m_value.m_data.p);

        if (texId && !texId->m_flags.m_renderTarget && texId->m_apiSpecificData) {
            texId->unk_0050 = this->m_frameCount;
            this->m_texCacheList.LinkToTail(texId);
        }
    }

    for (uint32_t i = GxRs_Texture0; i <= GxRs_Texture15; i++) {
        CGxTex* texId = static_cast<CGxTex*>(this->m_appRenderStates[i].m_value.m_data.p);

        if (texId && !texId->m_flags.m_renderTarget && texId->m_apiSpecificData) {
            texId->unk_0050 = this->m_frameCount;
            this->m_texCacheList.LinkToTail(texId);
        }
    }
}

// OFFSET: %0x007D2190
void CGxDeviceD3d::ITexAcquire(CGxTex* texId) {
    texId->unk_0050 = this->m_frameCount - 1;

    for (CGxTex* destroyed = this->m_texDestroyList.Head(); destroyed; destroyed = this->m_texDestroyList.Next(destroyed)) {
        if (texId->m_flags.m_filter == destroyed->m_flags.m_filter && texId->m_flags.m_renderTarget == destroyed->m_flags.m_renderTarget && texId->m_format == destroyed->m_format && texId->m_target == destroyed->m_target && texId->m_width == destroyed->m_width && texId->m_height == destroyed->m_height) {
            this->ITexTakeD3dTexture(texId, destroyed);
            return;
        }
    }

    uint32_t size = this->ITexBitsSize(texId, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);

    if (size + this->m_texCacheUsed < this->m_texCacheSize && this->ITexAllocD3dTexture(texId)) {
        return;
    }

    for (CGxTex* cached = this->m_texCacheList.Head(); cached; cached = this->m_texCacheList.Next(cached)) {
        if (this->m_frameCount - cached->unk_0050 < 10) {
            break;
        }

        if (texId->m_flags.m_filter == cached->m_flags.m_filter && texId->m_flags.m_renderTarget == cached->m_flags.m_renderTarget && texId->m_format == cached->m_format && texId->m_target == cached->m_target && texId->m_width == cached->m_width && texId->m_height == cached->m_height) {
            this->ITexTakeD3dTexture(texId, cached);
            return;
        }
    }

    while (true) {
        CGxTex* oldest = this->m_texCacheList.Head();

        if (!oldest || this->m_frameCount - oldest->unk_0050 < 10) {
            if (!this->ITexAllocD3dTexture(texId)) {
                this->ITexUnlinkBound();
                int32_t found = this->ITexEvictFor(texId, size);
                this->ITexLinkBound();

                if (!found) {
                    this->ITexAllocD3dTexture(texId);
                }
            }

            return;
        }

        this->ITexReleaseD3dTexture(oldest);

        if (size + this->m_texCacheUsed < this->m_texCacheSize && this->ITexAllocD3dTexture(texId)) {
            return;
        }
    }
}

// OFFSET: %0x007D2490
void CGxDeviceD3d::ITexTouch(CGxTex* texId) {
    if (!texId->m_flags.m_renderTarget && texId->m_apiSpecificData && texId->unk_0050 != static_cast<uint32_t>(this->m_frameCount)) {
        texId->unk_0050 = this->m_frameCount;
        this->m_texCacheList.LinkToTail(texId);
    }
}
