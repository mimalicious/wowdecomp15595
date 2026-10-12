#include "CSimpleCamera.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/Storm/H/storm.h"
#include <cmath>

// PROVEN: cd7c7771 IDENTICAL/IDENTICAL
// OFFSET: 0x0043BA30/0x140046460
float CSimpleCamera::FOV() const {
    return this->m_fov;
}

// PROVEN: 49e00a28 IDENTICAL/IDENTICAL
// OFFSET: 0x0043BA40/0x140046470
C3Vector CSimpleCamera::Forward() const {
    return C3Vector(this->m_facing.a0, this->m_facing.a1, this->m_facing.a2);
}

// PROVEN: 8529d28e IDENTICAL/IDENTICAL
// OFFSET: 0x0043BA60/0x140046490
C3Vector CSimpleCamera::Right() const {
    return C3Vector(this->m_facing.b0, this->m_facing.b1, this->m_facing.b2);
}

// PROVEN: 15ef82d6 IDENTICAL/IDENTICAL
// OFFSET: 0x0043BA80/0x1400464B0
C3Vector CSimpleCamera::Up() const {
    return C3Vector(this->m_facing.c0, this->m_facing.c1, this->m_facing.c2);
}

// OFFSET: 0x00445E80/0x140051EE0
CSimpleCamera::CSimpleCamera(float fov) {
    this->m_scene = NULL;
    this->m_facing.a0 = 1.0f;
    this->m_facing.a1 = 0.0f;
    this->m_facing.a2 = 0.0f;
    this->m_facing.b0 = 0.0f;
    this->m_facing.b1 = 1.0f;
    this->m_facing.b2 = 0.0f;
    this->m_facing.c0 = 0.0f;
    this->m_facing.c1 = 0.0f;
    this->m_facing.c2 = 1.0f;
    this->m_fov = fov;
    this->m_facing.FromEulerAnglesZYX(0.0f, 0.0f, 0.0f);
}

// INLINED
CSimpleCamera::~CSimpleCamera() {
    if (this->m_scene) {
        this->m_scene->Release();
    }
}

// PROVEN: 1ffddff3 IDENTICAL/IDENTICAL
// OFFSET: 0x00445E60/0x140051EB0
CM2Scene* CSimpleCamera::GetScene() {
    if (!this->m_scene) {
        this->m_scene = M2CreateScene();
    }

    return this->m_scene;
}

// PROVEN: c48d9a8a IDENTICAL/IDENTICAL
// OFFSET: 0x00445E30/0x14004CDA0
void CSimpleCamera::SetFacing(float yaw, float pitch, float roll) {
    this->m_facing.FromEulerAnglesZYX(yaw, pitch, roll);
}

// OFFSET: 0x00445F10
static void FaceDirection(const C3Vector& direction, C3Vector& right, C3Vector& forward, C3Vector& up) {
    if (fabs(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z) < 0.00000023841858f) {
        SErrSetLastError(87);
        return;
    }

    forward = direction;

    if (fabs(forward.x * forward.x + forward.y * forward.y) < 0.00000023841858f) {
        right.x = 1.0f;
        right.y = 0.0f;
        right.z = 0.0f;
    } else {
        right.x = -forward.y;
        right.y = forward.x;
        right.z = 0.0f;

        float scale = 1.0f / sqrt(right.x * right.x + right.y * right.y);
        right.x *= scale;
        right.y *= scale;
    }

    up = C3Vector(-(right.y * forward.z), forward.z * right.x, right.y * forward.x - forward.y * right.x);
}

// OFFSET: 0x00446040/0x140051F50
static void BuildBillboardMatrix(C33Matrix& matrix, const C3Vector& direction) {
    C3Vector forward(0.0f, 0.0f, 0.0f);
    C3Vector right(0.0f, 0.0f, 0.0f);
    C3Vector up(0.0f, 0.0f, 0.0f);
    FaceDirection(direction, right, forward, up);

    matrix.a0 = forward.x;
    matrix.a1 = forward.y;
    matrix.a2 = forward.z;
    matrix.b0 = right.x;
    matrix.b1 = right.y;
    matrix.b2 = right.z;
    matrix.c0 = up.x;
    matrix.c1 = up.y;
    matrix.c2 = up.z;
}

// OFFSET: 0x004460D0/0x140052100
static void FaceDirectionWithRoll(const C3Vector& direction, const C3Vector& upHint, C3Vector& right, C3Vector& forward, C3Vector& up) {
    if (fabs(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z) < 0.00000023841858f) {
        SErrSetLastError(87);
        return;
    }

    forward = direction;

    if (fabs(fabs(upHint.y * forward.y + upHint.x * forward.x + upHint.z * forward.z) - 1.0f) < 0.00000023841858f) {
        right.x = 1.0f;
        right.y = 0.0f;
        right.z = 0.0f;
    } else {
        right = C3Vector(upHint.y * forward.z - upHint.z * forward.y, upHint.z * forward.x - upHint.x * forward.z, upHint.x * forward.y - forward.x * upHint.y);

        float scale = 1.0f / sqrt(right.x * right.x + right.z * right.z + right.y * right.y);
        right.x *= scale;
        right.y *= scale;
        right.z *= scale;
    }

    up = C3Vector(right.z * forward.y - forward.z * right.y, forward.z * right.x - right.z * forward.x, forward.x * right.y - right.x * forward.y);
}

// OFFSET: 0x004462B0/0x140052310
static void BuildBillboardMatrixWithRoll(C33Matrix& matrix, const C3Vector& direction, const C3Vector& upHint) {
    C3Vector forward(0.0f, 0.0f, 0.0f);
    C3Vector right(0.0f, 0.0f, 0.0f);
    C3Vector up(0.0f, 0.0f, 0.0f);
    FaceDirectionWithRoll(direction, upHint, right, forward, up);

    matrix.a0 = forward.x;
    matrix.a1 = forward.y;
    matrix.a2 = forward.z;
    matrix.b0 = right.x;
    matrix.b1 = right.y;
    matrix.b2 = right.z;
    matrix.c0 = up.x;
    matrix.c1 = up.y;
    matrix.c2 = up.z;
}

// OFFSET: 0x00446340/0x1400523B0
void CSimpleCamera::SetFacing(const C3Vector& forward) {
    BuildBillboardMatrix(this->m_facing, forward);
}

// OFFSET: 0x00446360/0x1400523C0
void CSimpleCamera::SetFacing(const C3Vector& forward, const C3Vector& up) {
    BuildBillboardMatrixWithRoll(this->m_facing, forward, up);
}
