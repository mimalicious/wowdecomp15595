#include "WoW/Source/UIUtil/CameraShot.h"
#include <cmath>
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Tempest/CMath.h"

// STATIC: 0x00C99EEC/0x140B326D0
static float s_polarFacingMinDistance = 0.000099999997f;

// OFFSET: 0x00462D60/0x140076120
CCameraShot::CCameraShot() {
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->m_target.y = 0.0f;
    this->m_target.z = 0.0f;
    this->m_target.x = 1.0f;
    this->m_roll = 0.0f;
    this->m_fov = 1.5707964f;
    this->m_fade = 1.0f;
}

// OFFSET: 0x00462DB0/0x140076200
void CCameraShot::CalcPolarFacing(float& distance, float& yaw, float& pitch) const {
    float dx = this->m_target.x - this->m_position.x;
    float dy = this->m_target.y - this->m_position.y;
    float dz = this->m_target.z - this->m_position.z;
    distance = sqrtf(dz * dz + dy * dy + dx * dx);

    if (distance <= s_polarFacingMinDistance) {
        yaw = 0.0f;
        pitch = 0.0f;
    } else {
        yaw = CMath::normalizeAngleNegPiToPi_(atan2f(dy, dx));
        pitch = asinf(dz / distance);
    }
}

// PROVEN: 9665686e IDENTICAL/DIFFERENT
// OFFSET: 0x00462E40/0x1400762B0
void CCameraShot::CalcYawPitch(float& yaw, float& pitch) const {
    float distance;
    this->CalcPolarFacing(distance, yaw, pitch);
}

// OFFSET: 0x00462E60/0x140076350
C44Matrix CCameraShot::CalcRenderMatrix() const {
    C3Vector dir = this->m_target - this->m_position;
    float mag = dir.z * dir.z + dir.y * dir.y + dir.x * dir.x;

    if (mag > 0.00000023841858f) {
        float scale = 1.0f / sqrtf(mag);
        dir.x = dir.x * scale;
        dir.y = dir.y * scale;
        dir.z = scale * dir.z;
    }

    float c = cosf(this->m_roll);
    float s = sinf(this->m_roll);
    C3Vector up(0.0f, s, c);

    if (fabs(fabs(dir.z * c + dir.y * s + dir.x * 0.0f) - 1.0f) < 0.00000011920929f) {
        up = C3Vector(s, c, 0.0f);
    }

    C44Matrix matrix;
    C3Vector eye(0.0f, 0.0f, 0.0f);
    GxuXformCreateLookAtSgCompat(eye, dir, up, matrix);
    return matrix;
}

// OFFSET: 0x00463000/0x140076530
int32_t CCameraShot::Sanitize() {
    this->m_fov = this->m_fov < 0.1f ? 0.1f : (this->m_fov < 3.0415928f ? this->m_fov : 3.0415928f);
    float x = this->m_position.x;
    float y = this->m_position.y;

    if (sqrtf((this->m_target.x - x) * (this->m_target.x - x) + (this->m_target.y - y) * (this->m_target.y - y)) >= 0.001f) {
        return 1;
    }

    this->m_target.x = x + 1.0f;
    this->m_target.y = y;
    this->m_target.z = this->m_position.z;
    return 0;
}

// OFFSET: 0x004630B0/0x1400765E0
void CCameraShot::LerpEuler(const CCameraShot& from, const CCameraShot& to, float t, int32_t flags) {
    float inv = 1.0f - t;
    this->m_roll = from.m_roll * inv + to.m_roll * t;
    this->m_fade = from.m_fade * inv + to.m_fade * t;

    if (flags & 0x2) {
        this->m_fov = inv * from.m_fov + to.m_fov * t;
    } else {
        float half = atan2f(tanf(from.m_fov * 0.5f) * inv + tanf(0.5f * to.m_fov) * t, 1.0f);
        this->m_fov = half + half;
    }

    float fromDistance = 0.0f;
    float toDistance = 0.0f;
    float fromYaw = 0.0f;
    float toYaw = 0.0f;
    float fromPitch = 0.0f;
    float toPitch = 0.0f;
    from.CalcPolarFacing(fromDistance, fromYaw, fromPitch);
    to.CalcPolarFacing(toDistance, toYaw, toPitch);
    float yaw = CMath::normalizeAngleNegPiToPi_(toYaw - fromYaw) * t;
    yaw = CMath::normalizeAngleNegPiToPi_(yaw + fromYaw);
    float pitch = fromPitch * inv + toPitch * t;
    float distance = fromDistance * inv + toDistance * t;
    float flat = cosf(pitch) * distance;
    C3Vector offset(cosf(yaw) * flat, flat * sinf(yaw), distance * sinf(pitch));

    if (flags & 0x4) {
        this->m_target.x = from.m_target.x * inv + t * to.m_target.x;
        this->m_target.y = from.m_target.y * inv + to.m_target.y * t;
        this->m_target.z = from.m_target.z * inv + to.m_target.z * t;
        this->m_position = this->m_target - offset;
    } else {
        this->m_position.x = from.m_position.x * inv + to.m_position.x * t;
        this->m_position.y = from.m_position.y * inv + to.m_position.y * t;
        this->m_position.z = from.m_position.z * inv + to.m_position.z * t;
        this->m_target = this->m_position + offset;
    }
}

// OFFSET: 0x004633B0/0x140076980
C44Matrix CCameraShot::CalcViewMatrix() const {
    C3Vector dir = this->m_target - this->m_position;
    float mag = dir.z * dir.z + dir.y * dir.y + dir.x * dir.x;

    if (mag > 0.00000023841858f) {
        float scale = 1.0f / sqrtf(mag);
        dir.x = dir.x * scale;
        dir.y = dir.y * scale;
        dir.z = scale * dir.z;
    }

    float c = cosf(this->m_roll);
    float s = sinf(this->m_roll);
    C3Vector up(0.0f, s, c);

    if (fabs(fabs(dir.z * c + dir.y * s + dir.x * 0.0f) - 1.0f) < 0.00000011920929f) {
        up = C3Vector(s, c, 0.0f);
    }

    C3Vector side = C3Vector::Cross(up, dir);
    side *= 1.0f / sqrtf(side.z * side.z + side.y * side.y + side.x * side.x);
    C3Vector top = C3Vector::Cross(dir, side);
    top *= 1.0f / sqrtf(top.y * top.y + top.z * top.z + top.x * top.x);

    C44Matrix matrix;
    matrix.a3 = 0.0f;
    matrix.b3 = 0.0f;
    matrix.c3 = 0.0f;
    matrix.d3 = 1.0f;
    matrix.a0 = dir.x;
    matrix.a1 = dir.y;
    matrix.a2 = dir.z;
    matrix.b0 = side.x;
    matrix.b1 = side.y;
    matrix.b2 = side.z;
    matrix.c0 = top.x;
    matrix.c1 = top.y;
    matrix.c2 = top.z;
    matrix.d0 = this->m_position.x;
    matrix.d1 = this->m_position.y;
    matrix.d2 = this->m_position.z;
    return matrix;
}
