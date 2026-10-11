#include <cmath>
#include <cstddef>
#include <cstdint>
#include <new>
#include "Engine/Source/Services/Camera.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Services/CameraInt.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CRect.h"

// STATIC: 0x00CA4EF0
static float s_2pi = 6.2831855f;

// STATIC: 0x00CA4EF4
static float s_halfPi = 1.5707964f;

// STATIC: 0x00DD43F0
static float s_inv2Pi = 1.0f / s_2pi;

// INLINED
CAngle::CAngle(float angle) {
    this->m_data = 0.0f;
    this->CAngle::Set_(angle);
}

// OFFSET: 0x0054A640
void CAngle::Set_(const float& value) {
    float turns = static_cast<float>(static_cast<int32_t>(s_inv2Pi * value)) * s_2pi;

    if (value < 0.0f) {
        turns -= s_2pi;
    }

    this->m_data = value - turns;
    this->m_cos = cos(this->m_data);
    this->m_sin = sin(this->m_data);
}

// PROVEN: 7a2f638f IDENTICAL/NONE
// OFFSET: 0x0054A460/0x1401A35C0
void CameraAddRef(CCamera* camera) {
    camera->m_refCount++;
}

// PROVEN: 913dc92b IDENTICAL/NONE
// OFFSET: 0x0054A470/0x1401A35D0
void CameraGetCoord(CCamera* camera, uint32_t index, C3Vector* value) {
    switch (index) {
        case 7:
            *value = camera->m_position;
            break;

        case 8:
            *value = camera->m_target;
            break;
    }
}

// PROVEN: 7bd54c5b IDENTICAL/NONE
// OFFSET: 0x0054A4C0/0x1401A3610
float CameraGetFloat(CCamera* camera, uint32_t index) {
    switch (index) {
        case 0:
            return camera->m_aoa.m_data;

        case 1:
            return camera->m_distance;

        case 2:
            return camera->m_zFar;

        case 3:
            return camera->m_zNear;

        case 4:
            return camera->m_fov.m_data;

        case 5:
            return camera->m_roll.m_data;

        case 6:
            return camera->m_rotation.m_data;

        default:
            return 0.0f;
    }
}

// PROVEN: a5a23e49 IDENTICAL/NONE
// OFFSET: 0x0054A530/0x1401A3680
void CameraSetCoord(CCamera* camera, uint32_t index, const C3Vector& value) {
    switch (index) {
        case 7:
            camera->m_position = value;
            break;

        case 8:
            camera->m_target = value;
            break;
    }
}

// OFFSET: 0x0054A580/0x1401A36C0
void CameraSetFloat(CCamera* camera, uint32_t index, float value) {
    switch (index) {
        case 0:
            camera->m_aoa.Set_(value);
            break;

        case 1:
            camera->m_distance = value;
            break;

        case 2:
            camera->m_zFar = value;
            break;

        case 3:
            camera->m_zNear = value;
            break;

        case 4:
            camera->m_fov.Set_(value);
            break;

        case 5:
            camera->m_roll.Set_(value);
            break;

        case 6:
            camera->m_rotation.Set_(value);
            break;
    }
}

// OFFSET: 0x0054A6B0
CCamera::CCamera() : m_position(100.0f, 0.0f, 0.0f), m_target(0.0f, 0.0f, 0.0f), m_aoa(0.0f), m_fov(s_halfPi), m_roll(0.0f), m_rotation(0.0f) {
    this->m_refCount = 1;
    this->m_distance = 100.0f;
    this->m_zFar = 5000.0f;
    this->m_zNear = 8.0f;
}

// OFFSET: 0x0054A860/0x1401A3910
void CCamera::SetupWorldProjection(const CRect& rect, uint32_t flags) {
    C44Matrix projection;
    GxuXformCreateProjection_SG(this->m_fov.m_data, (rect.maxX - rect.minX) / (rect.maxY - rect.minY), this->m_zNear, this->m_zFar, projection);
    g_theGxDevicePtr->XformSetProjection(projection);

    C44Matrix view;
    C3Vector eye(0.0f, 0.0f, 0.0f);
    C3Vector target(this->m_target.x - this->m_position.x, this->m_target.y - this->m_position.y, this->m_target.z - this->m_position.z);
    C3Vector up(this->m_rotation.m_sin * this->m_roll.m_sin, -(this->m_rotation.m_cos * this->m_roll.m_sin), this->m_roll.m_cos);
    GxuXformCreateLookAtSgCompat(eye, target, up, view);
    g_theGxDevicePtr->XformSetView(view);

    CShaderEffect::UpdateProjMatrix();
}

// PROVEN: 27f2465f IDENTICAL/NONE
// OFFSET: 0x0054AA40/0x1401A3B30
CCamera* CameraCreate() {
    return new (__FILE__, __LINE__) CCamera;
}

// OFFSET: 0x0054AA60/0x1401A3B70
CCamera* CameraDuplicate(CCamera* source) {
    if (!source) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    CCamera* camera = new (__FILE__, __LINE__) CCamera;

    if (!camera) {
        return NULL;
    }

    camera->m_position = source->m_position;
    camera->m_target = source->m_target;
    camera->m_distance = source->m_distance;
    camera->m_zNear = source->m_zNear;
    camera->m_zFar = source->m_zFar;
    camera->m_aoa.Set_(source->m_aoa.m_data);
    camera->m_fov.Set_(source->m_fov.m_data);
    camera->m_roll.Set_(source->m_roll.m_data);
    camera->m_rotation.Set_(source->m_rotation.m_data);

    return camera;
}

// OFFSET: 0x0054AB40/0x1401A3C80
void CameraRelease(CCamera*& camera) {
    if (camera->m_refCount-- == 1) {
        delete camera;
    }

    camera = NULL;
}

// OFFSET: 0x0054AB70
void CameraSetupScreenProjection(const CRect& rect, const C2Vector& offset, float depth, int32_t forceOrtho) {
    float centerX = (rect.minX + rect.maxX) * 0.5f;
    float centerY = 0.5f * (rect.minY + rect.maxY);
    float left = rect.minX - centerX;
    float right = rect.maxX - centerX;
    float bottom = rect.minY + -centerY;
    float top = -centerY + rect.maxY;

    C44Matrix projection;

    if (forceOrtho || !g_theGxDevicePtr->StereoEnabled()) {
        GxuXformCreateOrtho(left, right, bottom, top, -500.0f, 500.0f, projection);
        projection.Scale(C3Vector(1.0f, 1.0f, -1.0f));
    } else {
        GxuXformCreateOrthoDepth(left, right, bottom, top, -500.0f, 500.0f, projection);
    }

    GxuAdjustProjForHalfPixel(projection);
    GxXformSetProjection(projection);

    C44Matrix view;
    view.Translate(C3Vector(offset.x - centerX, offset.y - centerY, 0.0f));
    GxXformSetView(view);
}

// PROVEN: 1bf5d399 IDENTICAL/UNPROVEN
// OFFSET: 0x0054AE00/0x1401A3FF0
void CameraSetupWorldProjection(CCamera* camera, const CRect& rect, uint32_t flags) {
    if (!camera) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    camera->SetupWorldProjection(rect, flags);
}

// OFFSET: 0x0054AE30
void CameraGetLineSegment(float x, float y, C3Vector* start, C3Vector* end) {
    if (!start || !end || !(x >= 0.0f) || !(x <= 1.0f) || !(y >= 0.0f) || !(y <= 1.0f)) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    C3Vector corners[8];
    C44Matrix view;
    C44Matrix projection;
    CGxDevice* device = g_theGxDevicePtr;
    view = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];
    projection = device->m_projection;
    GxuXformCalcFrustumCorners(view, projection, corners);

    float nearLeftX = corners[0].x + (corners[1].x - corners[0].x) * y;
    float nearLeftY = corners[0].y + (corners[1].y - corners[0].y) * y;
    float nearLeftZ = corners[0].z + (corners[1].z - corners[0].z) * y;
    float nearRightX = corners[3].x + (corners[2].x - corners[3].x) * y;
    float nearRightY = corners[3].y + (corners[2].y - corners[3].y) * y;
    float nearRightZ = corners[3].z + (corners[2].z - corners[3].z) * y;

    start->x = nearLeftX + (nearRightX - nearLeftX) * x;
    start->y = nearLeftY + (nearRightY - nearLeftY) * x;
    start->z = nearLeftZ + (nearRightZ - nearLeftZ) * x;

    float farLeftX = corners[4].x + (corners[5].x - corners[4].x) * y;
    float farLeftY = corners[4].y + (corners[5].y - corners[4].y) * y;
    float farLeftZ = corners[4].z + (corners[5].z - corners[4].z) * y;
    float farRightX = corners[7].x + (corners[6].x - corners[7].x) * y;
    float farRightY = corners[7].y + (corners[6].y - corners[7].y) * y;
    float farRightZ = corners[7].z + (corners[6].z - corners[7].z) * y;

    end->x = farLeftX + (farRightX - farLeftX) * x;
    end->y = farLeftY + (farRightY - farLeftY) * x;
    end->z = farLeftZ + (farRightZ - farLeftZ) * x;
}
