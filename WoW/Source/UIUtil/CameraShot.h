#ifndef WOW_SOURCE_UIUTIL_CAMERASHOT_H
#define WOW_SOURCE_UIUTIL_CAMERASHOT_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"

class CCameraShot {
    public:
    /* 0000 */ C3Vector m_position;
    /* 000C */ C3Vector m_target;
    /* 0018 */ float m_roll;
    /* 001C */ float m_fov;
    /* 0020 */ float m_fade;

    CCameraShot();
    void CalcPolarFacing(float& distance, float& yaw, float& pitch) const;
    void CalcYawPitch(float& yaw, float& pitch) const;
    int32_t Sanitize();
    C44Matrix CalcRenderMatrix() const;
    void LerpEuler(const CCameraShot& from, const CCameraShot& to, float t, int32_t flags);
    C44Matrix CalcViewMatrix() const;
};

#endif
