#include <cmath>
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C34Matrix.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/C4Quaternion.h"

// PROVEN: be022127 IDENTICAL/NONE
// OFFSET: 0x005AA4C0
C44Matrix::C44Matrix(float scale) {
    this->a0 = scale;
    this->a1 = 0.0f;
    this->a2 = 0.0f;
    this->a3 = 0.0f;
    this->b0 = 0.0f;
    this->b1 = scale;
    this->b2 = 0.0f;
    this->b3 = 0.0f;
    this->c0 = 0.0f;
    this->c1 = 0.0f;
    this->c2 = scale;
    this->c3 = 0.0f;
    this->d0 = 0.0f;
    this->d1 = 0.0f;
    this->d2 = 0.0f;
    this->d3 = 1.0f;
}

// INLINED
C44Matrix::C44Matrix(float a0, float a1, float a2, float a3, float b0, float b1, float b2, float b3, float c0, float c1, float c2, float c3, float d0, float d1, float d2, float d3) {
    this->a0 = a0;
    this->a1 = a1;
    this->a2 = a2;
    this->a3 = a3;
    this->b0 = b0;
    this->b1 = b1;
    this->b2 = b2;
    this->b3 = b3;
    this->c0 = c0;
    this->c1 = c1;
    this->c2 = c2;
    this->c3 = c3;
    this->d0 = d0;
    this->d1 = d1;
    this->d2 = d2;
    this->d3 = d3;
}

// OFFSET: #0x0057C2A0
void C44Matrix::Identity() {
    this->d3 = 1.0f;
    this->c2 = 1.0f;
    this->b1 = 1.0f;
    this->a0 = 1.0f;
    this->d2 = 0.0f;
    this->d1 = 0.0f;
    this->d0 = 0.0f;
    this->c3 = 0.0f;
    this->c1 = 0.0f;
    this->c0 = 0.0f;
    this->b3 = 0.0f;
    this->b2 = 0.0f;
    this->b0 = 0.0f;
    this->a3 = 0.0f;
    this->a2 = 0.0f;
    this->a1 = 0.0f;
}

// OFFSET: #0x00407F40
C44Matrix::C44Matrix() {
    this->a0 = 1.0f;
    this->a1 = 0.0f;
    this->a2 = 0.0f;
    this->a3 = 0.0f;
    this->b0 = 0.0f;
    this->b2 = 0.0f;
    this->b3 = 0.0f;
    this->c0 = 0.0f;
    this->c1 = 0.0f;
    this->c3 = 0.0f;
    this->d0 = 0.0f;
    this->d1 = 0.0f;
    this->d2 = 0.0f;
    this->b1 = 1.0f;
    this->c2 = 1.0f;
    this->d3 = 1.0f;
}

// PROVEN: 924b82a2 IDENTICAL/DIFFERENT
// OFFSET: 0x00409A60/0x14000C650
C44Matrix& C44Matrix::operator=(const C44Matrix& m) {
    this->a0 = m.a0;
    this->a1 = m.a1;
    this->a2 = m.a2;
    this->a3 = m.a3;
    this->b0 = m.b0;
    this->b1 = m.b1;
    this->b2 = m.b2;
    this->b3 = m.b3;
    this->c0 = m.c0;
    this->c1 = m.c1;
    this->c2 = m.c2;
    this->c3 = m.c3;
    this->d0 = m.d0;
    this->d1 = m.d1;
    this->d2 = m.d2;
    this->d3 = m.d3;
    return *this;
}

// PROVEN: 32b2aef8 IDENTICAL/IDENTICAL
// OFFSET: 0x004E82F0/0x14012A900
bool operator!=(const C44Matrix& l, const C44Matrix& r) {
    return l.a0 != r.a0 || l.a1 != r.a1 || l.a2 != r.a2 || l.a3 != r.a3 || l.b0 != r.b0 || l.b1 != r.b1 || l.b2 != r.b2 || l.b3 != r.b3 || l.c0 != r.c0 || l.c1 != r.c1 || l.c2 != r.c2 || l.c3 != r.c3 || l.d0 != r.d0 || l.d1 != r.d1 || l.d2 != r.d2 || l.d3 != r.d3;
}

// OFFSET: 0x004C6F90/0x1401022A0
float C44Matrix::Determinant() const {
    float b0 = this->b0;
    float b1 = this->b1;
    float b2 = this->b2;
    float b3 = this->b3;
    float c0 = this->c0;
    float c1 = this->c1;
    float c2 = this->c2;
    float c3 = this->c3;
    float d0 = this->d0;
    float d1 = this->d1;
    float d2 = this->d2;
    float d3 = this->d3;

    float result = (b1 * c2 * d3 + c3 * b2 * d1 + b3 * c1 * d2 - c2 * b3 * d1 - d3 * (c1 * b2) - b1 * c3 * d2) * this->a0
        - (c2 * b0 * d3 + c3 * b2 * d0 + b3 * c0 * d2 - b3 * c2 * d0 - d3 * (c0 * b2) - b0 * c3 * d2) * this->a1
        + (c1 * b0 * d3 + c3 * b1 * d0 + b3 * c0 * d1 - b3 * c1 * d0 - d3 * (c0 * b1) - b0 * c3 * d1) * this->a2;

    return result - (c1 * b0 * d2 + c2 * b1 * d0 + b2 * c0 * d1 - b2 * c1 * d0 - d2 * (c0 * b1) - b0 * c2 * d1) * this->a3;
}

// PROVEN: 085b5f42 IDENTICAL/IDENTICAL
// OFFSET: 0x004C7200/0x140102500
void C44Matrix::Translate(const C3Vector& move) {
    this->d0 = this->c0 * move.z + this->b0 * move.y + move.x * this->a0 + this->d0;
    this->d1 = this->c1 * move.z + this->b1 * move.y + this->a1 * move.x + this->d1;
    this->d2 = this->c2 * move.z + this->b2 * move.y + this->a2 * move.x + this->d2;
}

// PROVEN: 22ea500d IDENTICAL/IDENTICAL
// OFFSET: 0x004C72A0/0x140102590
void C44Matrix::Scale(const C3Vector& scale) {
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

// PROVEN: fdb47380 IDENTICAL/IDENTICAL
// OFFSET: 0x004C7330/0x140102620
void C44Matrix::Scale(float scale) {
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

// OFFSET: #0x004C1C40
void C44Matrix::FromQuaternion(const C4Quaternion& q, C44Matrix& m) {
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

    m.a0 = 1.0f - (zz + yy);
    m.a1 = xy + wz;
    m.a2 = xz - wy;
    m.b0 = xy - wz;
    m.b1 = 1.0f - (zz + xx);
    m.b2 = yz + wx;
    m.c0 = wy + xz;
    m.c1 = yz - wx;
    m.c2 = 1.0f - (xx + yy);
}

// OFFSET: %0x004CD0A0
C44Matrix::C44Matrix(const C33Matrix& m) {
    this->a0 = m.a0;
    this->a1 = m.a1;
    this->a2 = m.a2;
    this->a3 = 0.0f;
    this->b0 = m.b0;
    this->b1 = m.b1;
    this->b2 = m.b2;
    this->b3 = 0.0f;
    this->c0 = m.c0;
    this->c1 = m.c1;
    this->c2 = m.c2;
    this->c3 = 0.0f;
    this->d0 = 0.0f;
    this->d1 = 0.0f;
    this->d2 = 0.0f;
    this->d3 = 1.0f;
}

// PROVEN: 72d9859f IDENTICAL/DIFFERENT
// OFFSET: 0x004C73F0/0x1401028E0
C44Matrix::C44Matrix(const C4Quaternion& q) {
    this->a3 = 0.0f;
    this->b3 = 0.0f;
    this->c3 = 0.0f;
    this->d0 = 0.0f;
    this->d1 = 0.0f;
    this->d2 = 0.0f;
    this->d3 = 1.0f;
    FromQuaternion(q, *this);
}

// OFFSET: #0x004C1E60
C44Matrix operator+(const C44Matrix& l, const C44Matrix& r) {
    C44Matrix result;
    result.a0 = l.a0 + r.a0;
    result.a1 = l.a1 + r.a1;
    result.a2 = l.a2 + r.a2;
    result.a3 = l.a3 + r.a3;
    result.b0 = l.b0 + r.b0;
    result.b1 = l.b1 + r.b1;
    result.b2 = l.b2 + r.b2;
    result.b3 = l.b3 + r.b3;
    result.c0 = l.c0 + r.c0;
    result.c1 = l.c1 + r.c1;
    result.c2 = l.c2 + r.c2;
    result.c3 = l.c3 + r.c3;
    result.d0 = l.d0 + r.d0;
    result.d1 = l.d1 + r.d1;
    result.d2 = l.d2 + r.d2;
    result.d3 = l.d3 + r.d3;
    return result;
}

// PROVEN: bdb0ff7c IDENTICAL/IDENTICAL
// OFFSET: 0x00409AD0/0x14000C240
C44Matrix operator*(const C44Matrix& l, const C44Matrix& r) {
    C44Matrix result;
    result.a0 = l.a0 * r.a0 + l.a1 * r.b0 + l.a2 * r.c0 + l.a3 * r.d0;
    result.a1 = l.a0 * r.a1 + l.a1 * r.b1 + l.a2 * r.c1 + l.a3 * r.d1;
    result.a2 = l.a0 * r.a2 + l.a1 * r.b2 + l.a2 * r.c2 + l.a3 * r.d2;
    result.a3 = l.a0 * r.a3 + l.a1 * r.b3 + l.a2 * r.c3 + l.a3 * r.d3;
    result.b0 = l.b0 * r.a0 + l.b1 * r.b0 + l.b2 * r.c0 + l.b3 * r.d0;
    result.b1 = l.b0 * r.a1 + l.b1 * r.b1 + l.b2 * r.c1 + l.b3 * r.d1;
    result.b2 = l.b0 * r.a2 + l.b1 * r.b2 + l.b2 * r.c2 + l.b3 * r.d2;
    result.b3 = l.b0 * r.a3 + l.b1 * r.b3 + l.b2 * r.c3 + l.b3 * r.d3;
    result.c0 = l.c0 * r.a0 + l.c1 * r.b0 + l.c2 * r.c0 + l.c3 * r.d0;
    result.c1 = l.c0 * r.a1 + l.c1 * r.b1 + l.c2 * r.c1 + l.c3 * r.d1;
    result.c2 = l.c0 * r.a2 + l.c1 * r.b2 + l.c2 * r.c2 + l.c3 * r.d2;
    result.c3 = l.c0 * r.a3 + l.c1 * r.b3 + l.c2 * r.c3 + l.c3 * r.d3;
    result.d0 = l.d0 * r.a0 + l.d1 * r.b0 + l.d2 * r.c0 + l.d3 * r.d0;
    result.d1 = l.d0 * r.a1 + l.d1 * r.b1 + l.d2 * r.c1 + l.d3 * r.d1;
    result.d2 = l.d0 * r.a2 + l.d1 * r.b2 + l.d2 * r.c2 + l.d3 * r.d2;
    result.d3 = l.d2 * r.c3 + (l.d0 * r.a3 + l.d1 * r.b3) + l.d3 * r.d3;
    return result;
}

// OFFSET: #0x004C2120
C44Matrix operator*(const C44Matrix& l, float s) {
    C44Matrix result;
    result.a0 = l.a0 * s;
    result.a1 = l.a1 * s;
    result.a2 = l.a2 * s;
    result.a3 = l.a3 * s;
    result.b0 = l.b0 * s;
    result.b1 = l.b1 * s;
    result.b2 = l.b2 * s;
    result.b3 = l.b3 * s;
    result.c0 = l.c0 * s;
    result.c1 = l.c1 * s;
    result.c2 = l.c2 * s;
    result.c3 = l.c3 * s;
    result.d0 = l.d0 * s;
    result.d1 = l.d1 * s;
    result.d2 = l.d2 * s;
    result.d3 = s * l.d3;
    return result;
}

// PROVEN: 74d50525 IDENTICAL/IDENTICAL
// OFFSET: 0x0041EF40/0x140022C90
C3Vector operator*(const C3Vector& v, const C44Matrix& m) {
    C3Vector result;
    result.x = m.c0 * v.z + m.b0 * v.y + v.x * m.a0 + m.d0;
    result.y = m.c1 * v.z + m.b1 * v.y + m.a1 * v.x + m.d1;
    result.z = m.c2 * v.z + m.b2 * v.y + m.a2 * v.x + m.d2;
    return result;
}

// OFFSET: #0x004C2270
C4Vector operator*(const C4Vector& v, const C44Matrix& m) {
    C4Vector result;
    result.x = m.d0 * v.w + m.b0 * v.y + m.c0 * v.z + v.x * m.a0;
    result.y = m.d1 * v.w + m.b1 * v.y + m.a1 * v.x + m.c1 * v.z;
    result.z = m.d2 * v.w + m.b2 * v.y + m.a2 * v.x + m.c2 * v.z;
    result.w = m.d3 * v.w + m.b3 * v.y + m.a3 * v.x + m.c3 * v.z;
    return result;
}

// PROVEN: 258c073c IDENTICAL/DIFFERENT
// OFFSET: 0x0041EFE0/0x140022D40
C3Vector operator*=(C3Vector& v, const C44Matrix& m) {
    float y = m.c1 * v.z + m.b1 * v.y + m.a1 * v.x + m.d1;
    float z = m.c2 * v.z + m.b2 * v.y + m.a2 * v.x + m.d2;
    v.x = m.c0 * v.z + m.b0 * v.y + v.x * m.a0 + m.d0;
    v.y = y;
    v.z = z;
    return v;
}

// PROVEN: ae67771f IDENTICAL/NONE
// OFFSET: 0x004C7670
C44Matrix& C44Matrix::operator*=(const C44Matrix& m) {
    *this = *this * m;
    return *this;
}

// OFFSET: 0x004C76A0/0x140102A00
C44Matrix& C44Matrix::operator*=(float s) {
    *this = *this * s;
    return *this;
}

// PROVEN: ce905afe IDENTICAL/IDENTICAL
// OFFSET: 0x004C77A0/0x140102B60
C44Matrix C44Matrix::Transpose() const {
    C44Matrix result;
    result.a0 = this->a0;
    result.a1 = this->b0;
    result.a2 = this->c0;
    result.a3 = this->d0;
    result.b0 = this->a1;
    result.b1 = this->b1;
    result.b2 = this->c1;
    result.b3 = this->d1;
    result.c0 = this->a2;
    result.c1 = this->b2;
    result.c2 = this->c2;
    result.c3 = this->d2;
    result.d0 = this->a3;
    result.d1 = this->b3;
    result.d2 = this->c3;
    result.d3 = this->d3;
    return result;
}

// OFFSET: 0x004C7810
C44Matrix C44Matrix::Adjoint() const {
    float a0 = this->a0;
    float a1 = this->a1;
    float a2 = this->a2;
    float a3 = this->a3;
    float b0 = this->b0;
    float b1 = this->b1;
    float b2 = this->b2;
    float b3 = this->b3;
    float c0 = this->c0;
    float c1 = this->c1;
    float c2 = this->c2;
    float c3 = this->c3;
    float d0 = this->d0;
    float d1 = this->d1;
    float d2 = this->d2;
    float d3 = this->d3;

    C44Matrix result;
    result.a0 = b2 * c3 * d1 + b3 * c1 * d2 + b1 * c2 * d3 - b3 * c2 * d1 - b2 * c1 * d3 - b1 * c3 * d2;
    result.a1 = -(a2 * c3 * d1 + a3 * c1 * d2 + a1 * c2 * d3 - a3 * c2 * d1 - a2 * c1 * d3 - a1 * c3 * d2);
    result.a2 = a1 * b2 * d3 + a2 * b3 * d1 + a3 * b1 * d2 - a3 * b2 * d1 - a2 * b1 * d3 - a1 * b3 * d2;
    result.a3 = -(a2 * b3 * c1 + a3 * b1 * c2 + a1 * b2 * c3 - a3 * b2 * c1 - a2 * b1 * c3 - a1 * b3 * c2);
    result.b0 = -(b3 * c0 * d2 + b0 * c2 * d3 + b2 * c3 * d0 - b3 * c2 * d0 - b2 * c0 * d3 - b0 * c3 * d2);
    result.b1 = a2 * c3 * d0 + a3 * c0 * d2 + a0 * c2 * d3 - a3 * c2 * d0 - a2 * c0 * d3 - a0 * c3 * d2;
    result.b2 = -(a2 * b3 * d0 + a3 * b0 * d2 + a0 * b2 * d3 - a3 * b2 * d0 - a2 * b0 * d3 - a0 * b3 * d2);
    result.b3 = a2 * b3 * c0 + a3 * b0 * c2 + a0 * b2 * c3 - a3 * b2 * c0 - a2 * b0 * c3 - a0 * b3 * c2;
    result.c0 = b1 * c3 * d0 + b3 * c0 * d1 + b0 * c1 * d3 - b3 * c1 * d0 - b1 * c0 * d3 - b0 * c3 * d1;
    result.c1 = -(a1 * c3 * d0 + a3 * c0 * d1 + a0 * c1 * d3 - a3 * c1 * d0 - a1 * c0 * d3 - a0 * c3 * d1);
    result.c2 = a1 * b3 * d0 + a3 * b0 * d1 + a0 * b1 * d3 - a3 * b1 * d0 - a1 * b0 * d3 - a0 * b3 * d1;
    result.c3 = -(a1 * b3 * c0 + a3 * b0 * c1 + a0 * b1 * c3 - a3 * b1 * c0 - a1 * b0 * c3 - a0 * b3 * c1);
    result.d0 = -(b2 * c0 * d1 + b0 * c1 * d2 + b1 * c2 * d0 - b2 * c1 * d0 - b1 * c0 * d2 - b0 * c2 * d1);
    result.d1 = a1 * c2 * d0 + a2 * c0 * d1 + a0 * c1 * d2 - a2 * c1 * d0 - a1 * c0 * d2 - a0 * c2 * d1;
    result.d2 = -(a1 * b2 * d0 + a2 * b0 * d1 + a0 * b1 * d2 - a2 * b1 * d0 - a1 * b0 * d2 - a0 * b2 * d1);
    result.d3 = a0 * b1 * c2 + a1 * b2 * c0 + a2 * b0 * c1 - b1 * a2 * c0 - c2 * (b0 * a1) - a0 * b2 * c1;
    return result;
}

// PROVEN: 14aebfe7 IDENTICAL/DIFFERENT
// OFFSET: 0x004C8690/0x140103340
C44Matrix C44Matrix::Inverse(float determinant) const {
    return this->Adjoint() * (1.0f / determinant);
}

// OFFSET: 0x004C87A0/0x140103380
C44Matrix C44Matrix::AffineInverse() const {
    C33Matrix rotation(*this);

    C44Matrix result;
    result.a0 = rotation.a0;
    result.a1 = rotation.b0;
    result.a2 = rotation.c0;
    result.a3 = 0.0f;
    result.b3 = 0.0f;
    result.b0 = rotation.a1;
    result.b1 = rotation.b1;
    result.b2 = rotation.c1;
    result.c0 = rotation.a2;
    result.c1 = rotation.b2;
    result.c2 = rotation.c2;
    result.c3 = 0.0f;
    result.d3 = 1.0f;

    float x = -this->d0;
    float y = -this->d1;
    float z = -this->d2;
    result.d0 = rotation.a0 * x + rotation.a1 * y + rotation.a2 * z;
    result.d1 = rotation.b0 * x + rotation.b2 * z + y * rotation.b1;
    result.d2 = z * rotation.c2 + y * rotation.c1 + x * rotation.c0;

    return result;
}

// PROVEN: 5c609503 IDENTICAL/IDENTICAL
// OFFSET: 0x004C8B30/0x1401037E0
void C44Matrix::InvertTranslation() {
    float x = -this->d0;
    float y = -this->d1;
    float z = -this->d2;
    this->d0 = this->c0 * z + this->b0 * y + this->a0 * x;
    this->d1 = this->c1 * z + this->b1 * y + this->a1 * x;
    this->d2 = this->c2 * z + this->b2 * y + this->a2 * x;
}

// OFFSET: 0x004C88E0/0x140103500
C44Matrix C44Matrix::AffineInverse(float scale) const {
    if (fabs(scale - 1.0f) < 0.00000095367432f) {
        return this->AffineInverse();
    }

    C33Matrix rotation(*this);
    C33Matrix transposed;
    transposed.a0 = rotation.a0;
    transposed.a1 = rotation.b0;
    transposed.a2 = rotation.c0;
    transposed.b0 = rotation.a1;
    transposed.b1 = rotation.b1;
    transposed.b2 = rotation.c1;
    transposed.c0 = rotation.a2;
    transposed.c1 = rotation.b2;
    transposed.c2 = rotation.c2;

    C44Matrix result(transposed);
    float s = 1.0f / (scale * scale);
    result.a0 = result.a0 * s;
    result.a1 = result.a1 * s;
    result.a2 = result.a2 * s;
    result.b0 = result.b0 * s;
    result.b1 = result.b1 * s;
    result.b2 = result.b2 * s;
    result.c0 = result.c0 * s;
    result.c1 = result.c1 * s;
    result.c2 = s * result.c2;

    C3Vector move;
    move.x = -this->d0;
    move.y = -this->d1;
    move.z = -this->d2;
    result.Translate(move);

    return result;
}

// OFFSET: 0x004C8E50/0x140103D20
void C44Matrix::AffineInvertInPlace(float scale) {
    float t = this->a1;
    this->a1 = this->b0;
    this->b0 = t;

    t = this->a2;
    this->a2 = this->c0;
    this->c0 = t;

    t = this->b2;
    this->b2 = this->c1;
    this->c1 = t;

    if (fabs(scale - 1.0f) >= 0.00000095367432f) {
        float s = 1.0f / (scale * scale);
        this->b1 = this->b1 * s;
        this->b2 = s * this->b2;
        this->c0 = this->c0 * s;
        this->c1 = this->c1 * s;
        this->b0 = this->b0 * s;
        this->a0 = this->a0 * s;
        this->a2 = this->a2 * s;
        this->c2 = s * this->c2;
        this->a1 = s * this->a1;
    }

    this->InvertTranslation();
}

// OFFSET: #0x004C31B0
C44Matrix C44Matrix::RotationAroundX(float angle) {
    float c;
    float s;
    CMath::fsincos_(angle, &s, &c);

    C44Matrix result;
    result.a0 = 1.0f;
    result.a1 = 0.0f;
    result.a2 = 0.0f;
    result.a3 = 0.0f;
    result.b0 = 0.0f;
    result.b1 = c;
    result.b2 = s;
    result.b3 = 0.0f;
    result.c0 = 0.0f;
    result.c1 = -s;
    result.c2 = c;
    result.c3 = 0.0f;
    result.d0 = 0.0f;
    result.d1 = 0.0f;
    result.d2 = 0.0f;
    result.d3 = 1.0f;
    return result;
}

// OFFSET: #0x004C3220
C44Matrix C44Matrix::RotationAroundY(float angle) {
    float c;
    float s;
    CMath::fsincos_(angle, &s, &c);

    C44Matrix result;
    result.a0 = c;
    result.a1 = 0.0f;
    result.a2 = -s;
    result.a3 = 0.0f;
    result.b0 = 0.0f;
    result.b1 = 1.0f;
    result.b2 = 0.0f;
    result.b3 = 0.0f;
    result.c0 = s;
    result.c1 = 0.0f;
    result.c2 = c;
    result.c3 = 0.0f;
    result.d0 = 0.0f;
    result.d1 = 0.0f;
    result.d2 = 0.0f;
    result.d3 = 1.0f;
    return result;
}

// PROVEN: 25767a34 IDENTICAL/DIFFERENT
// OFFSET: 0x004C8A90/0x140103750
C44Matrix C44Matrix::RotationAroundZ(float angle) {
    float c;
    float s;
    CMath::fsincos_(angle, &s, &c);

    C44Matrix result;
    result.a0 = c;
    result.a1 = s;
    result.a2 = 0.0f;
    result.a3 = 0.0f;
    result.b0 = -s;
    result.b1 = c;
    result.b2 = 0.0f;
    result.b3 = 0.0f;
    result.c0 = 0.0f;
    result.c1 = 0.0f;
    result.c2 = 1.0f;
    result.c3 = 0.0f;
    result.d0 = 0.0f;
    result.d1 = 0.0f;
    result.d2 = 0.0f;
    result.d3 = 1.0f;
    return result;
}

// PROVEN: d778a6bd IDENTICAL/DIFFERENT
// OFFSET: 0x004C8BD0/0x140103880
void C44Matrix::RotateAroundX(float angle) {
    *this = RotationAroundX(angle) * *this;
}

// PROVEN: 04082aa3 IDENTICAL/DIFFERENT
// OFFSET: 0x004C8C90/0x1401039C0
void C44Matrix::RotateAroundY(float angle) {
    *this = RotationAroundY(angle) * *this;
}

// PROVEN: 1763a98e IDENTICAL/DIFFERENT
// OFFSET: 0x004C8D50/0x140103B00
void C44Matrix::RotateAroundZ(float angle) {
    *this = RotationAroundZ(angle) * *this;
}

// PROVEN: ebbf4bde IDENTICAL/NONE
// OFFSET: 0x004C8E10
void C44Matrix::Rotate(const C4Quaternion& q) {
    *this = C44Matrix(q) * *this;
}

// PROVEN: 92f75560 IDENTICAL/DIFFERENT
// OFFSET: 0x004C9070/0x140103EF0
C44Matrix C44Matrix::Rotation(float angle, const C3Vector& axis, bool normalized) {
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

    C44Matrix result;
    result.a0 = xx * t + c;
    result.a1 = t * xy + zs;
    result.a2 = t * zx - ys;
    result.a3 = 0.0f;
    result.b0 = t * xy - zs;
    result.b1 = (v.y * v.y) * t + c;
    result.b2 = t * zy + xs;
    result.b3 = 0.0f;
    result.c0 = t * zx + ys;
    result.c1 = t * zy - xs;
    result.c2 = (v.z * v.z) * t + c;
    result.c3 = 0.0f;
    result.d0 = 0.0f;
    result.d1 = 0.0f;
    result.d2 = 0.0f;
    result.d3 = 1.0f;
    return result;
}

// OFFSET: #0x0057C2E0
C3Vector TransformDirection(const C3Vector& v, const C44Matrix& m) {
    C3Vector result;
    result.x = m.c0 * v.z + m.b0 * v.y + v.x * m.a0;
    result.y = m.c1 * v.z + m.b1 * v.y + m.a1 * v.x;
    result.z = m.c2 * v.z + m.b2 * v.y + m.a2 * v.x;
    return result;
}

// PROVEN: befca4a3 IDENTICAL/IDENTICAL
// OFFSET: 0x0076CC90/0x140455310
bool operator==(const C44Matrix& l, const C44Matrix& r) {
    return l.a0 == r.a0 && l.a1 == r.a1 && l.a2 == r.a2 && l.a3 == r.a3 && l.b0 == r.b0 && l.b1 == r.b1 && l.b2 == r.b2 && l.b3 == r.b3 && l.c0 == r.c0 && l.c1 == r.c1 && l.c2 == r.c2 && l.c3 == r.c3 && l.d0 == r.d0 && l.d1 == r.d1 && l.d2 == r.d2 && l.d3 == r.d3;
}

// PROVEN: 71e5f822 IDENTICAL/DIFFERENT
// OFFSET: 0x004C7530/0x140102920
C44Matrix& C44Matrix::operator=(const C4Quaternion& q) {
    FromQuaternion(q, *this);
    this->a3 = 0.0f;
    this->b3 = 0.0f;
    this->c3 = 0.0f;
    this->d0 = 0.0f;
    this->d1 = 0.0f;
    this->d2 = 0.0f;
    this->d3 = 1.0f;
    return *this;
}

// OFFSET: #0x004C2210
C3Vector operator*(const C44Matrix& m, const C3Vector& v) {
    C3Vector result;
    result.x = m.a2 * v.z + m.a1 * v.y + m.a0 * v.x + m.a3;
    result.y = m.b2 * v.z + m.b0 * v.x + m.b1 * v.y + m.b3;
    result.z = m.c2 * v.z + m.c0 * v.x + m.c1 * v.y + m.c3;
    return result;
}

// PROVEN: 34b4327c IDENTICAL/NONE
// OFFSET: 0x00777110
C44Matrix::C44Matrix(const C3Vector& x, const C3Vector& y, const C3Vector& z, const C3Vector& w) {
    this->a0 = x.x;
    this->a1 = x.y;
    this->a2 = x.z;
    this->a3 = 0.0f;
    this->b0 = y.x;
    this->b1 = y.y;
    this->b2 = y.z;
    this->b3 = 0.0f;
    this->c0 = z.x;
    this->c1 = z.y;
    this->c2 = z.z;
    this->c3 = 0.0f;
    this->d0 = w.x;
    this->d1 = w.y;
    this->d2 = w.z;
    this->d3 = 1.0f;
}
