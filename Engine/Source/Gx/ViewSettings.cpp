#include <cstdint>
#include "Engine/Source/Gx/ViewSettings.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"

// STATIC: 0x00D1C90C
static float s_zFar = 946.0f;

// STATIC: 0x00D1C910
static float s_zNear = 0.2f;

// STATIC: 0x00D1C914
static float s_viewportFarClip = 0.94f;

// STATIC: 0x00F803F0
static int32_t s_submerged;

// STATIC: 0x00F803F4
static CAaRect s_viewExtents;

struct ViewSettingsFog {
    float m_start;
    float m_end;
    float m_density;
    CImVector m_color;
};

// STATIC: 0x00F80408
static ViewSettingsFog s_fog[4];

// PROVEN: 0448e23d IDENTICAL/NONE
// OFFSET: 0x00A30110
void ViewSettings::SetZNearFar(float zNear, float zFar) {
    s_zNear = zNear;
    s_zFar = zFar;
}

// PROVEN: bb1e15d2 IDENTICAL/NONE
// OFFSET: 0x00A30130
void ViewSettings::SetViewportFarClip(float farClip) {
    s_viewportFarClip = farClip;
}

// PROVEN: 888189fe IDENTICAL/NONE
// OFFSET: 0x00A30150/0x1407DA020
void ViewSettings::SetSubmerged(int32_t submerged) {
    s_submerged = submerged;
}

// PROVEN: b2b601e4 IDENTICAL/IDENTICAL
// OFFSET: 0x00A30160/0x1407DA030
int32_t ViewSettings::GetSubmerged() {
    return s_submerged;
}

// OFFSET: 0x00A301F0/0x1407DA0C0
void ViewSettings::SetFog(uint32_t index, float start, float end, float density, const CImVector& color) {
    s_fog[index].m_start = start;
    s_fog[index].m_end = end;
    s_fog[index].m_density = density;
    s_fog[index].m_color = color;
}

// OFFSET: 0x00A30230/0x1407DA0F0
void ViewSettings::GetFog(uint32_t index, float& start, float& end, float& density, CImVector& color) {
    start = s_fog[index].m_start;
    end = s_fog[index].m_end;
    density = s_fog[index].m_density;
    color = s_fog[index].m_color;
}

// OFFSET: 0x00A30170/0x1407DA040
void ViewSettings::SetViewExtents(const CAaRect& extents) {
    s_viewExtents = extents;
}

// PROVEN: 6c7ea212 IDENTICAL/IDENTICAL
// OFFSET: 0x00A301A0/0x1407DA070
C4Vector ViewSettings::CalculateParams() {
    C4Vector params;
    params.x = s_zNear;
    params.y = s_zFar;
    params.z = (1.0f / s_zFar - 1.0f / s_zNear) / s_viewportFarClip;
    params.w = 1.0f / s_zNear;

    return params;
}

// OFFSET: 0x00A30260/0x1407DA120
C4Vector ViewSettings::CalculateHomToScreen(float offsetX, float offsetY) {
    float x = ((s_viewExtents.minY + 1.0f) - (1.0f - s_viewExtents.maxY)) * 0.5f;
    float scaleY;
    float y;

    if (g_theGxDevicePtr->m_caps.m_texelCenterOnEdge) {
        scaleY = (s_viewExtents.maxX - s_viewExtents.minX) * 0.5f;
        y = (s_viewExtents.minX + 1.0f) - (1.0f - s_viewExtents.maxY);
    } else {
        scaleY = (s_viewExtents.maxX - s_viewExtents.minX) * -0.5f;
        y = ((1.0f - s_viewExtents.maxX) + 1.0f) - s_viewExtents.minX;
    }

    y = y * 0.5f;

    if (g_theGxDevicePtr->m_caps.m_pixelCenterOnEdge) {
        x = x + offsetX;
        y = y + offsetY;
    }

    C4Vector result;
    result.x = (s_viewExtents.maxY - s_viewExtents.minY) * 0.5f;
    result.y = scaleY;
    result.z = x;
    result.w = y;

    return result;
}
