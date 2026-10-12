#include "Engine/Source/Tempest/C33Matrix.h"

// INLINED
inline C44Matrix::C44Matrix() {
    this->a0 = 1.0f;
    this->a1 = 0.0f;
    this->a2 = 0.0f;
    this->a3 = 0.0f;
    this->b0 = 0.0f;
    this->b1 = 1.0f;
    this->b2 = 0.0f;
    this->b3 = 0.0f;
    this->c0 = 0.0f;
    this->c1 = 0.0f;
    this->c2 = 1.0f;
    this->c3 = 0.0f;
    this->d0 = 0.0f;
    this->d1 = 0.0f;
    this->d2 = 0.0f;
    this->d3 = 1.0f;
}

// OFFSET: 0x0043B6C0
inline C44Matrix::C44Matrix(const C34Matrix& m) {
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
    this->d0 = m.d0;
    this->d1 = m.d1;
    this->d2 = m.d2;
    this->d3 = 1.0f;
}

// OFFSET: 0x00435440
inline C44Matrix C44Matrix::Inverse() const {
    return this->Inverse(this->Determinant());
}

inline void C44Matrix::AffineInvertInPlace() {
    float t = this->a1;
    this->a1 = this->b0;
    this->b0 = t;

    t = this->a2;
    this->a2 = this->c0;
    this->c0 = t;

    t = this->b2;
    this->b2 = this->c1;
    this->c1 = t;

    this->InvertTranslation();
}

// INLINED
inline C44Matrix::C44Matrix(float a0, float a1, float a2, float a3, float b0, float b1, float b2, float b3, float c0, float c1, float c2, float c3, float d0, float d1, float d2, float d3) {
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

// INLINED
inline void C44Matrix::Identity() {
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

// INLINED
inline C44Matrix::C44Matrix(const C33Matrix& m) {
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

// INLINED
inline C44Matrix operator+(const C44Matrix& l, const C44Matrix& r) {
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

// INLINED
inline C44Matrix operator*(const C44Matrix& l, float s) {
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

// INLINED
inline C3Vector TransformDirection(const C3Vector& v, const C44Matrix& m) {
    C3Vector result;
    result.x = m.c0 * v.z + m.b0 * v.y + v.x * m.a0;
    result.y = m.c1 * v.z + m.b1 * v.y + m.a1 * v.x;
    result.z = m.c2 * v.z + m.b2 * v.y + m.a2 * v.x;
    return result;
}

// INLINED
inline C3Vector operator*(const C44Matrix& m, const C3Vector& v) {
    C3Vector result;
    result.x = m.a2 * v.z + m.a1 * v.y + m.a0 * v.x + m.a3;
    result.y = m.b2 * v.z + m.b0 * v.x + m.b1 * v.y + m.b3;
    result.z = m.c2 * v.z + m.c0 * v.x + m.c1 * v.y + m.c3;
    return result;
}

// OFFSET: 0x00409AD0/0x14000C240
inline C44Matrix operator*(const C44Matrix& l, const C44Matrix& r) {
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

// OFFSET: 0x004C73F0/0x1401028E0
inline C44Matrix::C44Matrix(const C4Quaternion& q) {
    this->a3 = 0.0f;
    this->b3 = 0.0f;
    this->c3 = 0.0f;
    this->d0 = 0.0f;
    this->d1 = 0.0f;
    this->d2 = 0.0f;
    this->d3 = 1.0f;
    FromQuaternion(q, *this);
}

// OFFSET: 0x004C72A0/0x140102590
inline void C44Matrix::Scale(const C3Vector& scale) {
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
