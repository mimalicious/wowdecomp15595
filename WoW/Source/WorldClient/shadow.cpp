#include "shadow.h"
#include <cmath>
#include <xmmintrin.h>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/Game/GameClient/ProjectTex.h"

// STATIC: 0x00E75670/0x140D5F450
static CTexture* s_shadowBlobTexture;

// STATIC: 0x00E75674/0x140D5F458
static CVar* s_cvShadowMode;

// PROVEN: 68b580f3 IDENTICAL/UNPROVEN
// OFFSET: 0x00744F60/0x140422C20
void ShadowDestroy() {
    if (s_shadowBlobTexture) {
        TextureRelease(s_shadowBlobTexture);
        s_shadowBlobTexture = NULL;
    }
}

// OFFSET: 0x00744F90
static float ShadowSqrt(float value) {
    return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(value)));
}

// PROVEN: f06568cd IDENTICAL/IDENTICAL
// OFFSET: 0x00744FD0/0x140422C50
int32_t ShadowBlobsEnabled() {
    return s_cvShadowMode && !s_cvShadowMode->m_intValue;
}

// OFFSET: 0x00744FF0/0x140422C70
static void ShadowRenderBlob(const CAaBox& box, const C44Matrix& transform, float alpha) {
    float halfX = (box.t.x - box.b.x) * 0.5f;
    float halfY = (box.t.y - box.b.y) * 0.5f;
    float modelScale = sqrt(transform.a2 * transform.a2 + transform.a1 * transform.a1 + transform.a0 * transform.a0);

    CAaBox local;
    local.b.x = -halfX;
    local.b.y = -halfY;
    local.b.z = box.b.z;
    local.t.x = halfX;
    local.t.y = halfY;
    local.t.z = box.t.z;
    local *= modelScale;
    local.b.Minimize(C3Vector(5.0f, 5.0f, 5.0f));
    local.b.Maximize(C3Vector(-5.0f, -5.0f, -5.0f));
    local.t.Minimize(C3Vector(5.0f, 5.0f, 5.0f));
    local.t.Maximize(C3Vector(-5.0f, -5.0f, -5.0f));
    float halfZ = (local.t.z - local.b.z) * 0.5f;

    C33Matrix rotation(transform);

    if (fabs(modelScale) >= 0.00000023841858f) {
        if (fabs(modelScale - 1.0f) >= 0.00000023841858f) {
            float inverse = 1.0f / modelScale;
            rotation.Scale(C3Vector(inverse, inverse, inverse));
        }
    } else {
        rotation = C33Matrix(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    C3Vector corners[4];
    corners[0] = C3Vector(local.t.x, local.t.y, 0.0f) * rotation;
    corners[1] = C3Vector(local.t.x, local.b.y, 0.0f) * rotation;
    corners[2] = C3Vector(local.b.x, local.b.y, 0.0f) * rotation;
    corners[3] = C3Vector(local.b.x, local.t.y, 0.0f) * rotation;
    CAaBox bounds = CAaBox::Bounding(corners, 4);
    bounds.b.x += transform.d0;
    bounds.b.y += transform.d1;
    bounds.t.x += transform.d0;
    bounds.t.y += transform.d1;
    bounds.t.z = transform.d2 + halfZ;
    bounds.b.z = transform.d2 - halfZ * 1.6666666f;

    C3Vector scale(1.0f, 1.0f, 1.0f);
    float sizeY = fabs(local.t.y - local.b.y);

    if (sizeY >= 0.00000023841858f) {
        scale.x = 1.0f / sizeY;
    }

    float sizeX = fabs(local.t.x - local.b.x);

    if (sizeX >= 0.00000023841858f) {
        scale.y = 1.0f / sizeX;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, GxBlend_Mod);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(s_shadowBlobTexture));

    if (!CShaderEffect::s_enableShaders) {
        GxRsSet(GxRs_FogColor, static_cast<int32_t>(0xFFFFFFFF));
        GxRsSet(GxRs_ColorOp0, 2);
        GxRsSet(GxRs_AlphaOp0, 3);
    }

    int32_t shade = static_cast<int32_t>(alpha * 127.0f + 0.5f);

    if (shade > 255) {
        shade = 255;
    }

    CImVector color;
    color.b = static_cast<uint8_t>(0x80 - shade);
    color.g = static_cast<uint8_t>(0x80 - shade);
    color.r = static_cast<uint8_t>(0x80 - shade);
    color.a = 0xFF;

    C44Matrix projection(rotation.a0, rotation.b0, rotation.c0, 0.0f, rotation.a1, rotation.b1, rotation.c1, 0.0f, rotation.a2, rotation.b2, rotation.c2, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    ProjectTex2d(bounds, color, &scale, &projection, 0.5f, 0x220122, 0, GxPolygonOffset_Default);
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x00745790/0x140423650
void ShadowRender(const C3Vector& position, float facing, float scale, const CAaBox& box, float alpha, const C3Vector& cameraPos) {
    if (alpha < 0.001f || scale < 0.0099999998f) {
        return;
    }

    if (box.t.x == box.b.x && box.t.y == box.b.y && box.t.z == box.b.z) {
        return;
    }

    if (box.t.x < box.b.x || box.t.y < box.b.y || box.t.z < box.b.z) {
        return;
    }

    if (alpha > 1.0f) {
        alpha = 1.0f;
    }

    float dz = position.z - cameraPos.z;
    float dy = position.y - cameraPos.y;
    float distSq = (position.x - cameraPos.x) * (position.x - cameraPos.x) + dz * dz + dy * dy;

    if (distSq > 900.0f) {
        return;
    }

    float fade = 1.0f;

    if (distSq > 400.0f) {
        fade = 1.0f - (ShadowSqrt(distSq) - 20.0f) * 0.1f;

        if (fade > 1.0f) {
            fade = 1.0f;
        } else if (fade < 0.001f) {
            return;
        }
    }

    C44Matrix transform;
    transform.RotateAroundZ(facing);
    transform.d0 = position.x;
    transform.d1 = position.y;
    transform.d2 = position.z;
    transform.Scale(scale);
    ShadowRenderBlob(box, transform, fade * alpha);
}

// OFFSET: 0x00745970/0x140423860
void ShadowInit() {
    CStatus status;
    s_shadowBlobTexture = TextureCreate("Textures\\ShadowBlob.blp", CGxTexFlags(), &status, 1, NULL, 2);

    if (!s_cvShadowMode) {
        s_cvShadowMode = CVar::Lookup("shadowMode");
    }
}
