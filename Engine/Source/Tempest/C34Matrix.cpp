#include <cmath>
#include "Engine/Source/Tempest/C34Matrix.h"
#include "Engine/Source/Tempest/CMath.h"

// OFFSET: 0x004CE0C0
C34Matrix operator*(const C34Matrix& l, const C34Matrix& r) {
    C34Matrix result;
    result.a0 = l.a0 * r.a0 + l.a1 * r.b0 + l.a2 * r.c0;
    result.a1 = l.a0 * r.a1 + l.a1 * r.b1 + l.a2 * r.c1;
    result.a2 = l.a0 * r.a2 + l.a1 * r.b2 + l.a2 * r.c2;
    result.b0 = l.b0 * r.a0 + l.b1 * r.b0 + l.b2 * r.c0;
    result.b1 = l.b0 * r.a1 + l.b1 * r.b1 + l.b2 * r.c1;
    result.b2 = l.b0 * r.a2 + l.b1 * r.b2 + l.b2 * r.c2;
    result.c0 = l.c0 * r.a0 + l.c1 * r.b0 + l.c2 * r.c0;
    result.c1 = l.c0 * r.a1 + l.c1 * r.b1 + l.c2 * r.c1;
    result.c2 = l.c0 * r.a2 + l.c1 * r.b2 + l.c2 * r.c2;
    result.d0 = l.d0 * r.a0 + l.d1 * r.b0 + l.d2 * r.c0 + r.d0;
    result.d1 = l.d0 * r.a1 + l.d1 * r.b1 + l.d2 * r.c1 + r.d1;
    result.d2 = l.d0 * r.a2 + l.d1 * r.b2 + l.d2 * r.c2 + r.d2;
    return result;
}

// OFFSET: 0x004CE310
void C34Matrix::RotateAroundZ(float angle) {
    float c;
    float s;
    CMath::fsincos_(angle, &s, &c);

    C34Matrix rotation(c, s, 0.0f, -s, c, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    *this = rotation * *this;
}
