#include <cmath>
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Quaternion.h"

// PROVEN: 212934b3 IDENTICAL/IDENTICAL
// OFFSET: 0x004CD2E0/0x140109040
C33Matrix::C33Matrix(const C4Quaternion& q) {
    float x2 = q.x * 2.0f;
    float y2 = q.y * 2.0f;
    float z2 = 2.0f * q.z;
    float wx = q.w * x2;
    float wy = q.w * y2;
    float wz = q.w * z2;
    float xx = x2 * q.x;
    float xy = q.x * y2;
    float xz = q.x * z2;
    float yy = y2 * q.y;
    float yz = q.y * z2;
    float zz = z2 * q.z;

    this->a0 = 1.0f - (zz + yy);
    this->a1 = xy + wz;
    this->a2 = xz - wy;
    this->b0 = xy - wz;
    this->b1 = 1.0f - (zz + xx);
    this->b2 = yz + wx;
    this->c0 = wy + xz;
    this->c1 = yz - wx;
    this->c2 = 1.0f - (xx + yy);
}

// PROVEN: 6ee1216e IDENTICAL/IDENTICAL
// OFFSET: 0x004CD3F0/0x140109190
C33Matrix::C33Matrix(const C44Matrix& m) {
    this->a0 = m.a0;
    this->a1 = m.a1;
    this->a2 = m.a2;
    this->b0 = m.b0;
    this->b1 = m.b1;
    this->b2 = m.b2;
    this->c0 = m.c0;
    this->c1 = m.c1;
    this->c2 = m.c2;
}

// PROVEN: e5d44559 IDENTICAL/DIFFERENT
// OFFSET: 0x004CD430/0x14028B250
float C33Matrix::Determinant() const {
    return this->c0 * this->a1 * this->b2 + this->a2 * this->b0 * this->c1 + this->c2 * this->a0 * this->b1 - this->c0 * this->a2 * this->b1 - this->c2 * this->a1 * this->b0 - this->a0 * this->c1 * this->b2;
}

// PROVEN: 245adc3b IDENTICAL/IDENTICAL
// OFFSET: 0x004CD4A0/0x140109280
void C33Matrix::Scale(float scale) {
    this->a0 = this->a0 * scale;
    this->a1 = this->a1 * scale;
    this->a2 = this->a2 * scale;
    this->b0 = this->b0 * scale;
    this->b1 = this->b1 * scale;
    this->b2 = this->b2 * scale;
    this->c0 = this->c0 * scale;
    this->c1 = this->c1 * scale;
    this->c2 = scale * this->c2;
}

// OFFSET: %0x004D3700
void C33Matrix::Scale(float x, float y, float z) {
    this->a0 = this->a0 * x;
    this->a1 = this->a1 * x;
    this->a2 = x * this->a2;
    this->b0 = this->b0 * y;
    this->b1 = this->b1 * y;
    this->b2 = y * this->b2;
    this->c0 = this->c0 * z;
    this->c1 = this->c1 * z;
    this->c2 = z * this->c2;
}

// OFFSET: %0x004D37A0
void C33Matrix::Scale(const C3Vector& scale) {
    this->a0 = scale.x * this->a0;
    this->a1 = this->a1 * scale.x;
    this->a2 = this->a2 * scale.x;
    this->b0 = this->b0 * scale.y;
    this->b1 = scale.y * this->b1;
    this->b2 = scale.y * this->b2;
    this->c0 = scale.z * this->c0;
    this->c1 = this->c1 * scale.z;
    this->c2 = this->c2 * scale.z;
}

// PROVEN: 12ec340e IDENTICAL/IDENTICAL
// OFFSET: 0x004CD600/0x140109390
C33Matrix operator*(const C33Matrix& l, const C33Matrix& r) {
    C33Matrix result;
    result.a0 = l.a0 * r.a0 + l.a1 * r.b0 + l.a2 * r.c0;
    result.a1 = l.a0 * r.a1 + l.a1 * r.b1 + l.a2 * r.c1;
    result.a2 = l.a0 * r.a2 + l.a1 * r.b2 + l.a2 * r.c2;
    result.b0 = l.b0 * r.a0 + l.b1 * r.b0 + l.b2 * r.c0;
    result.b1 = l.b0 * r.a1 + l.b1 * r.b1 + l.b2 * r.c1;
    result.b2 = l.b0 * r.a2 + (l.b1 * r.b2 + l.b2 * r.c2);
    result.c0 = l.c0 * r.a0 + l.c1 * r.b0 + l.c2 * r.c0;
    result.c1 = l.c0 * r.a1 + (l.c1 * r.b1 + l.c2 * r.c1);
    result.c2 = l.c0 * r.a2 + l.c1 * r.b2 + l.c2 * r.c2;
    return result;
}

// OFFSET: #0x004C5430
C33Matrix C33Matrix::Adjoint() const {
    C33Matrix result;
    result.a0 = this->b1 * this->c2 - this->c1 * this->b2;
    result.a1 = -(this->c2 * this->a1 - this->c1 * this->a2);
    result.a2 = this->b2 * this->a1 - this->b1 * this->a2;
    result.b0 = -(this->b0 * this->c2 - this->b2 * this->c0);
    result.b1 = this->a0 * this->c2 - this->c0 * this->a2;
    result.b2 = -(this->b2 * this->a0 - this->b0 * this->a2);
    result.c0 = this->c1 * this->b0 - this->b1 * this->c0;
    result.c1 = -(this->c1 * this->a0 - this->c0 * this->a1);
    result.c2 = this->b1 * this->a0 - this->b0 * this->a1;
    return result;
}

// OFFSET: #0x004C54E0
C33Matrix C33Matrix::Inverse(float determinant) const {
    float scale = 1.0f / determinant;
    C33Matrix adjoint = this->Adjoint();

    C33Matrix result;
    result.a0 = adjoint.a0 * scale;
    result.a1 = adjoint.a1 * scale;
    result.a2 = adjoint.a2 * scale;
    result.b0 = adjoint.b0 * scale;
    result.b1 = adjoint.b1 * scale;
    result.b2 = adjoint.b2 * scale;
    result.c0 = adjoint.c0 * scale;
    result.c1 = adjoint.c1 * scale;
    result.c2 = scale * adjoint.c2;
    return result;
}

// OFFSET: #0x004C5550
C33Matrix C33Matrix::Rotation(float angle) {
    float c = cos(angle);
    float s = sin(angle);

    C33Matrix result;
    result.a0 = c;
    result.a1 = s;
    result.a2 = 0.0f;
    result.b0 = -s;
    result.b1 = c;
    result.b2 = 0.0f;
    result.c0 = 0.0f;
    result.c1 = 0.0f;
    result.c2 = 1.0f;
    return result;
}

// OFFSET: #0x004C5630
bool C33Matrix::ToEulerAnglesZYX(float& yaw, float& pitch, float& roll) const {
    if (this->a2 >= 1.0f) {
        yaw = atan2(-this->b0, -this->c0);
        pitch = -1.5707964f;
        roll = 0.0f;
        return false;
    }

    if (this->a2 <= -1.0f) {
        yaw = -atan2(this->b0, this->c0);
        pitch = 1.5707964f;
        roll = 0.0f;
        return false;
    }

    yaw = atan2(this->a1, this->a0);
    pitch = asin(-this->a2);
    roll = atan2(this->b2, this->c2);

    return true;
}

// OFFSET: %0x004D3EB0
void C33Matrix::FromEulerAnglesZYX(float yaw, float pitch, float roll) {
    float c = cos(yaw);
    float s = sin(yaw);

    C33Matrix z;
    z.a0 = c;
    z.a1 = -s;
    z.a2 = 0.0f;
    z.b2 = 0.0f;
    z.c0 = 0.0f;
    z.c1 = 0.0f;
    z.b0 = s;
    z.b1 = c;
    z.c2 = 1.0f;

    c = cos(pitch);
    s = sin(pitch);

    C33Matrix y;
    y.a0 = c;
    y.a1 = 0.0f;
    y.a2 = s;
    y.b0 = 0.0f;
    y.b1 = 1.0f;
    y.b2 = 0.0f;
    y.c0 = -s;
    y.c1 = 0.0f;
    y.c2 = c;

    c = cos(roll);
    s = sin(roll);

    C33Matrix x;
    x.a0 = 1.0f;
    x.a1 = 0.0f;
    x.a2 = 0.0f;
    x.b0 = 0.0f;
    x.b1 = c;
    x.b2 = -s;
    x.c0 = 0.0f;
    x.c1 = s;
    x.c2 = c;

    C33Matrix product = z * (y * x);

    this->a0 = product.a0;
    this->a1 = product.b0;
    this->a2 = product.c0;
    this->b0 = product.a1;
    this->b1 = product.b1;
    this->b2 = product.c1;
    this->c0 = product.a2;
    this->c1 = product.b2;
    this->c2 = product.c2;
}

// PROVEN: 0798e9ee IDENTICAL/DIFFERENT
// OFFSET: 0x004CDF00/0x140109D80
C33Matrix C33Matrix::Rotation(float angle, const C3Vector& axis, bool normalized) {
    C3Vector v = axis;

    if (!normalized) {
        v.Normalize();
    }

    float c;
    float s;
    CMath::fsincos_(angle, &s, &c);

    float xx = v.x * v.x;
    float xy = v.y * v.x;
    float zy = v.z * v.y;
    float zx = v.z * v.x;
    float xs = v.x * s;
    float ys = v.y * s;
    float zs = v.z * s;
    float t = 1.0f - c;

    C33Matrix result;
    result.a0 = xx * t + c;
    result.a1 = t * xy + zs;
    result.a2 = t * zx - ys;
    result.b0 = t * xy - zs;
    result.b1 = (v.y * v.y) * t + c;
    result.b2 = t * zy + xs;
    result.c0 = t * zx + ys;
    result.c1 = t * zy - xs;
    result.c2 = (v.z * v.z) * t + c;
    return result;
}

// PROVEN: b1ea7faf IDENTICAL/UNPROVEN
// OFFSET: 0x004CE070/0x140109F50
void C33Matrix::Rotate(float angle, const C3Vector& axis, bool normalized) {
    *this = Rotation(angle, axis, normalized) * *this;
}

// OFFSET: #0x004C1D40
C33Matrix::C33Matrix(float a0, float a1, float a2, float b0, float b1, float b2, float c0, float c1, float c2) {
    this->a0 = a0;
    this->a1 = a1;
    this->a2 = a2;
    this->b0 = b0;
    this->b1 = b1;
    this->b2 = b2;
    this->c0 = c0;
    this->c1 = c1;
    this->c2 = c2;
}

// OFFSET: %0x004D3CE0
void C33Matrix::RotateAroundY(float angle) {
    float c = cos(angle);
    float s = sin(angle);

    C33Matrix rotation;
    rotation.a0 = c;
    rotation.a1 = 0.0f;
    rotation.a2 = -s;
    rotation.b0 = 0.0f;
    rotation.b1 = 1.0f;
    rotation.b2 = 0.0f;
    rotation.c0 = s;
    rotation.c1 = 0.0f;
    rotation.c2 = c;

    *this = rotation * *this;
}

// OFFSET: #0x005FED20
C3Vector operator*(const C3Vector& v, const C33Matrix& m) {
    C3Vector result;
    result.x = m.b0 * v.y + m.c0 * v.z + v.x * m.a0;
    result.y = m.b1 * v.y + m.a1 * v.x + m.c1 * v.z;
    result.z = m.b2 * v.y + m.a2 * v.x + m.c2 * v.z;
    return result;
}

// PROVEN: 3c0f5380 IDENTICAL/NONE
// OFFSET: 0x005853E0
C3Vector operator*=(C3Vector& v, const C33Matrix& m) {
    float y = m.c1 * v.z + m.b1 * v.y + m.a1 * v.x;
    float z = m.c2 * v.z + m.b2 * v.y + m.a2 * v.x;
    v.x = m.c0 * v.z + m.b0 * v.y + v.x * m.a0;
    v.y = y;
    v.z = z;
    return v;
}
