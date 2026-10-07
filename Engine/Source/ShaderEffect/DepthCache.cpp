#include <cstdint>
#include "Engine/Source/ShaderEffect/DepthCache.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Tempest/CRect.h"

// STATIC: %0x00E4576C
static CTexture* s_depthResolveTex;

// STATIC: %0x00E45770
static CTexture* s_depthTex;

// STATIC: %0x00E45774
static int32_t s_refCount;

// STATIC: %0x00E45778
static DepthCache::EDepthMode s_depthMode;

// STATIC: %0x00E4577C
static int32_t s_needsCreate;

// STATIC: 0x00D1C904
static float s_depthRangeMin = -1.0f;

// STATIC: 0x00D1C908
static float s_depthRangeMax = -1.0f;

// STATIC: 0x00F803D0
static CGxShader* s_depthRangeVS;

// STATIC: 0x00F803CC
static CGxShader* s_depthRangePS;

// OFFSET: %0x00970860
void __fastcall DepthCache::Reset() {
    if (s_depthResolveTex) {
        TextureRelease(s_depthResolveTex);
        s_depthResolveTex = NULL;
    }

    if (s_depthTex) {
        TextureRelease(s_depthTex);
        s_depthTex = NULL;
    }

    s_depthMode = DM_Unknown;
    s_needsCreate = 1;
}

// PROVEN: 3239f9e4 IDENTICAL/IDENTICAL
// OFFSET: 0x00A2F850/0x1407D9540
bool DepthCache::IsValid() {
    return s_depthMode != DM_Unknown;
}

// OFFSET: 0x00A2F860/0x1407D9550
CTexture* DepthCache::GetDepthTex() {
    return s_depthTex;
}

// PROVEN: 3fd63246 IDENTICAL/IDENTICAL
// OFFSET: 0x00A2F870/0x1407D9560
CTexture* DepthCache::GetDepthSR() {
    return s_depthResolveTex ? s_depthResolveTex : s_depthTex;
}

// PROVEN: 49770cf6 IDENTICAL/IDENTICAL
// OFFSET: 0x00A2F880/0x1407D9580
DepthCache::EDepthMode DepthCache::GetDepthMode() {
    return s_depthMode;
}

// OFFSET: 0x00A2F8C0/0x1407D95C0
int32_t DepthCache::IsSupported(EDepthMode* mode, EGxTexFormat* format, EGxTexFormat* resolveFormat) {
    EGxTexFormat resolve = GxTex_Unknown;
    EDepthMode depthMode = DM_Unknown;
    EGxTexFormat depth = GxTex_Unknown;

    if (g_theGxDevicePtr->m_caps.m_texNonPow2 || !g_theGxDevicePtr->m_caps.unk_0300) {
        return 0;
    }

    if (g_theGxDevicePtr->DeviceApi() == GxApi_D3d11) {
        switch (g_theGxDevicePtr->DeviceFormat().m_depthFormat) {
            case CGxFormat::Fmt_Ds160:
                depth = GxTex_R16;
                break;

            case CGxFormat::Fmt_Ds24X:
            case CGxFormat::Fmt_Ds248:
                depth = GxTex_R24X8;
                break;

            case CGxFormat::Fmt_Ds320:
                depth = GxTex_R32F;
                break;

            default:
                break;
        }

        depthMode = DM_Texture;

        if (g_theGxDevicePtr->DeviceFormat().m_multisampleCount > 1) {
            switch (g_theGxDevicePtr->DeviceFormat().m_depthFormat) {
                case CGxFormat::Fmt_Ds160:
                    resolve = GxTex_R16;
                    break;

                case CGxFormat::Fmt_Ds24X:
                case CGxFormat::Fmt_Ds248:
                case CGxFormat::Fmt_Ds320:
                    resolve = GxTex_R32F;
                    break;

                default:
                    break;
            }
        }
    } else if (g_theGxDevicePtr->m_caps.m_texFmt[GxTex_Intz]) {
        depth = GxTex_Intz;
        depthMode = DM_Texture;
    } else if (g_theGxDevicePtr->m_caps.m_texFmt[GxTex_AtiD24]) {
        depth = GxTex_AtiD24;
        depthMode = DM_Texture;
    }

    if (mode) {
        *mode = depthMode;
    }

    if (format) {
        *format = depth;
    }

    if (resolveFormat) {
        *resolveFormat = resolve;
    }

    return depthMode != DM_Unknown;
}

// OFFSET: 0x00A2FA40/0x1407D9780
void DepthCache::Release() {
    if (--s_refCount) {
        return;
    }

    g_theGxDevicePtr->RemoveDeviceRestoredCallback(DepthCache::Reset);
    g_theGxDevicePtr->RemoveTextureRecreationCallback(DepthCache::Reset);

    if (s_depthResolveTex) {
        TextureRelease(s_depthResolveTex);
        s_depthResolveTex = NULL;
    }

    if (s_depthTex) {
        TextureRelease(s_depthTex);
        s_depthTex = NULL;
    }

    s_depthMode = DM_Unknown;
}

// PROVEN: 7a3dc296 IDENTICAL/NONE
// OFFSET: 0x00A2FA50
void DepthCache::Resolve() {
    CGxTex* depth = TextureGetGxTex(s_depthTex);

    if (depth) {
        g_theGxDevicePtr->DeviceResolveDepthBuffer(depth);
    }

    if (s_depthResolveTex) {
        GxTexResolve(TextureGetGxTex(s_depthTex), TextureGetGxTex(s_depthResolveTex));
    }
}

// OFFSET: 0x00A2FAA0/0x1407D9810
static void CreateTextures() {
    CRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_theGxDevicePtr->CapsWindowSize(rect);

    GxLog("Preparing to create sample-able depth buffer texture:");

    EGxTexFormat format = GxTex_Unknown;
    EGxTexFormat resolveFormat = GxTex_Unknown;
    DepthCache::EDepthMode mode = DepthCache::DM_Unknown;
    s_depthMode = DepthCache::DM_Unknown;

    if (DepthCache::IsSupported(&mode, &format, &resolveFormat) && (g_theGxDevicePtr->m_caps.unk_0314 || g_theGxDevicePtr->DeviceFormat().m_multisampleCount == 1)) {
        GxLog("    DepthCache is supported!");
        GxLog("    Format:        %s", CGxDevice::s_texFormatNames[format]);
        GxLog("    FormatResolve: %s", CGxDevice::s_texFormatNames[resolveFormat]);

        CGxTexFlags flags;

        if (g_theGxDevicePtr->DeviceApi() != GxApi_D3d11) {
            flags.m_renderTarget = 1;
        }

        if (format) {
            flags.m_filter = 0;
            flags.m_bit8 = resolveFormat != GxTex_Unknown;
            s_depthTex = TextureCreate(GxTex_NonPow2, static_cast<uint32_t>(rect.maxX), static_cast<uint32_t>(rect.maxY), 1, format, format, flags, NULL, GxTexNullUserFunc, "DepthCache", 0);
        }

        if (resolveFormat) {
            flags.m_renderTarget = 1;
            flags.m_bit8 = 0;
            s_depthResolveTex = TextureCreate(GxTex_NonPow2, static_cast<uint32_t>(rect.maxX), static_cast<uint32_t>(rect.maxY), 1, resolveFormat, resolveFormat, flags, NULL, GxTexNullUserFunc, "DepthCache Resolve", 0);
        }

        s_needsCreate = 0;
        s_depthMode = mode;
    } else {
        GxLog("    DepthCache is NOT supported!");
        s_needsCreate = 0;
    }
}

// OFFSET: 0x00A2FCA0/0x1407D9A60
void DepthCache::AddRef() {
    if (s_refCount++) {
        return;
    }

    CreateTextures();
    g_theGxDevicePtr->AddDeviceRestoredCallback(DepthCache::Reset);
    g_theGxDevicePtr->AddTextureRecreationCallback(DepthCache::Reset);
}

// PROVEN: ac54ae27 IDENTICAL/UNPROVEN
// OFFSET: 0x00A2FCE0/0x1407D9AB0
void DepthCache::Validate() {
    if (s_needsCreate && s_refCount) {
        CreateTextures();
    }
}

// OFFSET: 0x00A2F890/0x1407D9590
int32_t DepthCache::DepthRangeDumpEnabled() {
    return s_depthRangeMin >= 0.0f || s_depthRangeMax >= 0.0f;
}

// OFFSET: 0x00A2FD00/0x1407D9AD0
void DepthCache::SetDepthRangeDump(float rangeMin, float rangeMax) {
    int32_t enabled = s_depthRangeMin >= 0.0f || s_depthRangeMax >= 0.0f;
    int32_t enable = rangeMin >= 0.0f || rangeMax >= 0.0f;

    if (enabled != enable) {
        if (enable) {
            DepthCache::AddRef();
            s_depthRangeVS = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "DepthRange");
            s_depthRangePS = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "DepthRange");
        } else {
            DepthCache::Release();

            if (s_depthRangeVS) {
                g_theGxDevicePtr->ShaderRelease(s_depthRangeVS);
                s_depthRangeVS = NULL;
            }

            if (s_depthRangePS) {
                g_theGxDevicePtr->ShaderRelease(s_depthRangePS);
                s_depthRangePS = NULL;
            }
        }
    }

    s_depthRangeMin = rangeMin;
    s_depthRangeMax = rangeMax;
}
