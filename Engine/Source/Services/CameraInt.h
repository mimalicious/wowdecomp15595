#ifndef ENGINE_SOURCE_SERVICES_CAMERAINT_H
#define ENGINE_SOURCE_SERVICES_CAMERAINT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class CRect;

class CAngle {
    public:
    /* 0004 */ float m_data;
    /* 0008 */ float m_cos;
    /* 000C */ float m_sin;

    CAngle(float angle);
    virtual void Set_(const float& value);
};

class CCamera {
    public:
    /* 0000 */ int32_t m_refCount;
    /* 0004 */ C3Vector m_position;
    /* 0010 */ C3Vector m_target;
    /* 001C */ float m_distance;
    /* 0020 */ float m_zFar;
    /* 0024 */ float m_zNear;
    /* 0028 */ CAngle m_aoa;
    /* 0038 */ CAngle m_fov;
    /* 0048 */ CAngle m_roll;
    /* 0058 */ CAngle m_rotation;

    CCamera();
    void SetupWorldProjection(const CRect& rect, uint32_t flags);
};

#endif
