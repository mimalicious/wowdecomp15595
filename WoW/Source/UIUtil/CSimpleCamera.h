#ifndef WOW_SOURCE_UIUTIL_CSIMPLECAMERA_H
#define WOW_SOURCE_UIUTIL_CSIMPLECAMERA_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C33Matrix.h"

class CM2Scene;
class CRect;

class CSimpleCamera {
    public:
    void SetFacing(const C3Vector& forward);
    void SetFacing(const C3Vector& forward, const C3Vector& up);
    /* 0004 */ CM2Scene* m_scene;
    /* 0008 */ C3Vector m_position;
    /* 0014 */ C33Matrix m_facing;
    /* 0038 */ float m_fov;

    virtual float FOV() const;
    virtual C3Vector Forward() const;
    virtual C3Vector Right() const;
    virtual C3Vector Up() const;

    CSimpleCamera(float fov);
    ~CSimpleCamera();
    CM2Scene* GetScene();
    void SetFacing(float yaw, float pitch, float roll);
};

#endif
