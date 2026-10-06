#include <cmath>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

// PROVEN: 528faad5 IDENTICAL/IDENTICAL
// OFFSET: 0x0040A260/0x1408F4FD0
void C3Vector::Zero() {
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;
}

// PROVEN: 48c17e2e IDENTICAL/NONE
// OFFSET: 0x004E0560
bool operator==(const C3Vector& l, const C3Vector& r) {
    return l.x == r.x && l.y == r.y && l.z == r.z;
}

// PROVEN: 7a09fa8f IDENTICAL/DIFFERENT
// OFFSET: 0x0043B090/0x1401D2F80
bool operator!=(const C3Vector& l, const C3Vector& r) {
    return l.x != r.x || l.y != r.y || l.z != r.z;
}

// PROVEN: a4f9112a IDENTICAL/IDENTICAL
// OFFSET: 0x0043B020/0x140045D00
void C3Vector::NormalizeSafe() {
    float length = this->z * this->z + this->y * this->y + this->x * this->x;

    if (length > 0.00000023841858f) {
        float scale = 1.0f / sqrt(length);
        this->x = this->x * scale;
        this->y = scale * this->y;
        this->z = scale * this->z;
    }
}

// PROVEN: d95f8ea2 IDENTICAL/NONE
// OFFSET: 0x0043B1B0
C3Vector C3Vector::Cross(const C3Vector& a, const C3Vector& b) {
    C3Vector result;
    result.x = b.z * a.y - a.z * b.y;
    result.y = a.z * b.x - b.z * a.x;
    result.z = b.y * a.x - b.x * a.y;
    return result;
}

// PROVEN: 1f5f3e1f IDENTICAL/IDENTICAL
// OFFSET: 0x004CD1C0/0x140108F40
C3Vector::C3Vector(const C4Vector& v) {
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
}

// PROVEN: 5f199b6f IDENTICAL/IDENTICAL
// OFFSET: 0x004CD1E0/0x140108F60
C3Vector::C3Vector(const CImVector& color) {
    this->x = color.r * 0.0039215689f;
    this->y = color.g * 0.0039215689f;
    this->z = 0.0039215689f * color.b;
}

// PROVEN: 3a5eb7bb IDENTICAL/IDENTICAL
// OFFSET: 0x004CD230/0x140108FB0
C3Vector::EAxis C3Vector::MajorAxis() const {
    float x = fabs(this->x);
    float y = fabs(this->y);
    float z = fabs(this->z);

    if (x > y) {
        if (x > z) {
            return AXIS_X;
        }
    } else if (y > z) {
        return AXIS_Y;
    }

    return AXIS_Z;
}

// PROVEN: 8365ef80 IDENTICAL/DIFFERENT
// OFFSET: 0x004CD280/0x140108FF0
C3Vector::EAxis C3Vector::MinorAxis() const {
    float x = fabs(this->x);
    float y = fabs(this->y);
    float z = fabs(this->z);

    if (x < y) {
        if (x < z) {
            return AXIS_X;
        }

        return AXIS_Z;
    }

    return y > z ? AXIS_Z : AXIS_Y;
}

// OFFSET: #0x004C3650
C3Vector operator%(const C3Vector& l, const C2Vector& r) {
    C3Vector result;
    result.x = -(r.y * l.z);
    result.y = r.x * l.z;
    result.z = r.y * l.x - l.y * r.x;
    return result;
}

// PROVEN: 56611fbb IDENTICAL/NONE
// OFFSET: 0x0043B690
float C3Vector::Distance(const C3Vector& a, const C3Vector& b) {
    return sqrt((a - b).SquaredMag());
}

// OFFSET: #0x007E2CE0
void C3Vector::Minimize(const C3Vector& v) {
    *this = C3Vector::Min(*this, v);
}

// OFFSET: #0x007E2D20
void C3Vector::Maximize(const C3Vector& v) {
    *this = C3Vector::Max(*this, v);
}

// PROVEN: e5827dc1 IDENTICAL/NONE
// OFFSET: 0x00B147B0
void C3Vector::FlushDenormals() {
    if (this->x != 0.0f && fabs(this->x) < 0.0000000099999999f) {
        this->x = 0.0f;
    }

    if (this->y != 0.0f && fabs(this->y) < 0.0000000099999999f) {
        this->y = 0.0f;
    }

    if (this->z != 0.0f && fabs(this->z) < 0.0000000099999999f) {
        this->z = 0.0f;
    }
}
