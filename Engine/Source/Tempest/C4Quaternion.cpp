#include <cmath>
#include <cstdint>
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "Engine/Source/Tempest/CMath.h"

// STATIC: #0x00AA2E4C
static const uint32_t s_nextAxis[3] = { 1, 2, 0 };

// PROVEN: 64f52896 IDENTICAL/NONE
// OFFSET: [0x004CBA80/0x140107760<3>, 0x004CBBD0/0x1401078F0<4>]
template <uint32_t N>
static void FromRotationMatrixImpl(float trace, C4Quaternion& q, const float* m) {
    float* out = &q.x;

    if (trace > 0.0f) {
        trace = sqrt(trace + 1.0f);
        q.w = trace * 0.5f;
        float s = 0.5f / trace;
        q.x = (m[2 * N + 1] - m[1 * N + 2]) * s;
        q.y = (m[0 * N + 2] - m[2 * N + 0]) * s;
        q.z = (m[1 * N + 0] - m[0 * N + 1]) * s;
        return;
    }

    uint32_t i = 0;

    if (m[N + 1] > m[0]) {
        i = 1;
    }

    if (m[i * N + i] < m[2 * N + 2]) {
        i = 2;
    }

    uint32_t j = s_nextAxis[i];
    uint32_t k = s_nextAxis[j];

    float s = sqrt(m[i * N + i] - m[j * N + j] - m[k * N + k] + 1.0f);
    out[i] = s * 0.5f;
    s = 0.5f / s;
    q.w = (m[k * N + j] - m[j * N + k]) * s;
    out[j] = (m[i * N + j] + m[j * N + i]) * s;
    out[k] = (m[i * N + k] + m[k * N + i]) * s;
}

// PROVEN: 8f6cfd43 DIFFERENT/IDENTICAL
// OFFSET: 0x004CB5A0/0x1401071F0
C4Quaternion::C4Quaternion(const C4QuaternionCompressed& compressed) {
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;
    this->w = 0.0f;

    this->x = static_cast<int32_t>(compressed.m_packed >> 42) * 0.00000047683716f;
    this->y = static_cast<int32_t>(((compressed.m_packed << 22) >> 32) >> 11) * 0.00000095367432f;
    this->z = static_cast<int32_t>((compressed.m_packed << 43) >> 43) * 0.00000095367432f;

    float length = (this->x * this->x + this->y * this->y) + this->z * this->z;
    this->w = fabs(length - 1.0f) < 0.00000095367432f ? 0.0f : sqrt(1.0f - length);
}

// OFFSET: 0x004CB6A0/0x1401072A0
void C4Quaternion::FromAngleAxis(float angle, const C3Vector& axis) {
    float half = angle * 0.5f;
    float c = cos(half);
    float s = sin(half);
    this->w = c;
    this->x = axis.x * s;
    this->y = axis.y * s;
    this->z = s * axis.z;
}

// OFFSET: 0x004CB720
C4Quaternion C4Quaternion::Slerp(float t, const C4Quaternion& a, const C4Quaternion& b) {
    float cosine = a.z * b.z + a.y * b.y + a.x * b.x + b.w * a.w;
    float sign = 1.0f;

    if (cosine < 0.0f) {
        sign = -1.0f;
        cosine = -cosine;
    }

    float sine = sqrt(fabs(1.0f - cosine * cosine));

    C4Quaternion result;

    if (fabs(sine) < 0.00000047683716f) {
        result.x = a.x;
        result.y = a.y;
        result.z = a.z;
        result.w = a.w;
        return result;
    }

    float angle = atan2(sine, cosine);
    float scale = 1.0f / sine;
    float wa = sin((1.0f - t) * angle) * scale;
    float wb = scale * sin(t * angle) * sign;

    result.x = b.x * wb + a.x * wa;
    result.y = wb * b.y + a.y * wa;
    result.z = a.z * wa + wb * b.z;
    result.w = wb * b.w + wa * a.w;
    return result;
}

// OFFSET: 0x004CB8B0/0x140107560
void C4Quaternion::NormalizeApprox() {
    float length = this->w * this->w + this->z * this->z + this->y * this->y + this->x * this->x;
    float scale = 1.021435f - (length - 0.95906597f) * 0.532516f;

    if (length <= 0.91521198f) {
        scale = scale * (1.021435f - (scale * scale * length - 0.95906597f) * 0.532516f);

        if (length <= 0.6521197f) {
            scale = (1.021435f - (length * (scale * scale) - 0.95906597f) * 0.532516f) * scale;
        }
    }

    this->x = this->x * scale;
    this->y = this->y * scale;
    this->z = scale * this->z;
    this->w = scale * this->w;
}

// OFFSET: #0x00982630
C4Quaternion C4Quaternion::Nlerp(float t, const C4Quaternion& a, const C4Quaternion& b) {
    C4Quaternion result;
    result.x = (b.x - a.x) * t + a.x;
    result.y = (b.y - a.y) * t + a.y;
    result.z = (b.z - a.z) * t + a.z;
    result.w = (b.w - a.w) * t + a.w;
    result.NormalizeApprox();
    return result;
}

// PROVEN: c40ecf29 IDENTICAL/DIFFERENT
// OFFSET: 0x004CBD10/0x140107A90
void C4Quaternion::FromRotationMatrix(const C33Matrix& m) {
    C33Matrix transposed;
    transposed.a0 = m.a0;
    transposed.a1 = m.b0;
    transposed.a2 = m.c0;
    transposed.b0 = m.a1;
    transposed.b1 = m.b1;
    transposed.b2 = m.c1;
    transposed.c0 = m.a2;
    transposed.c1 = m.b2;
    transposed.c2 = m.c2;
    FromRotationMatrixImpl<3>(transposed.c2 + transposed.b1 + transposed.a0, *this, &transposed.a0);
}

// INLINED
static void FromTransposedRotationMatrix(C4Quaternion& q, const C44Matrix& transposed) {
    FromRotationMatrixImpl<4>(transposed.c2 + transposed.b1 + transposed.a0, q, &transposed.a0);
}

// PROVEN: 0de85930 IDENTICAL/DIFFERENT
// OFFSET: 0x004CBDA0/0x140107B10
void C4Quaternion::FromRotationMatrix(const C44Matrix& m) {
    FromTransposedRotationMatrix(*this, m.Transpose());
}

// OFFSET: #0x004F4320
C4Quaternion operator*(const C4Quaternion& l, const C4Quaternion& r) {
    C4Quaternion result;
    result.x = l.w * r.x + r.z * l.y + r.w * l.x - l.z * r.y;
    result.y = r.w * l.y + l.w * r.y + l.z * r.x - r.z * l.x;
    result.z = r.z * l.w + r.w * l.z + l.x * r.y - r.x * l.y;
    result.w = r.w * l.w - l.x * r.x - l.y * r.y - r.z * l.z;
    return result;
}

// OFFSET: 0x0041EE80
C4QuaternionCompressed& C4QuaternionCompressed::operator=(const C4Quaternion& q) {
    int32_t sign = q.w < 0.0f ? -1 : 1;
    int32_t y = (sign * FloatToInt64(q.y * 1048576.0f)) & 0x1FFFFF;
    int64_t packed = (static_cast<int64_t>(sign * FloatToInt64(q.x * 2097152.0f)) << 21) | y;
    int32_t z = (sign * FloatToInt64(q.z * 1048576.0f)) & 0x1FFFFF;
    this->m_packed = (packed << 21) | z;
    return *this;
}
