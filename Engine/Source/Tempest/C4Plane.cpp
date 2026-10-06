#include <cmath>
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/C44Matrix.h"

// OFFSET: #0x007912C0
void C4Plane::From3Pos(const C3Vector& p0, const C3Vector& p1, const C3Vector& p2) {
    float ex = p2.x - p0.x;
    float ey = p2.y - p0.y;
    float ez = p2.z - p0.z;
    float fx = p1.x - p0.x;
    float fy = p1.y - p0.y;
    float fz = p1.z - p0.z;

    this->n.x = fy * ez - fz * ey;
    this->n.y = fz * ex - ez * fx;
    this->n.z = ey * fx - ex * fy;

    float scale = 1.0f / sqrt(this->n.z * this->n.z + this->n.y * this->n.y + this->n.x * this->n.x);
    this->n.x = this->n.x * scale;
    this->n.y = this->n.y * scale;
    this->n.z = scale * this->n.z;
    this->d = -(this->n.z * p0.z + this->n.x * p0.x + this->n.y * p0.y);
}

// PROVEN: 453ab2b8 IDENTICAL
// OFFSET: 0x00828420
bool operator!=(const C4Plane& l, const C4Plane& r) {
    return l.n.x != r.n.x || l.n.y != r.n.y || l.n.z != r.n.z || l.d != r.d;
}

// OFFSET: %0x00A47800
void C4Plane::Transform(const C44Matrix& matrix) {
    float d = -this->d;
    C3Vector point(this->n.x * d, this->n.y * d, this->n.z * d);

    C3Vector normal;
    normal.x = this->n.x * matrix.a0 + matrix.c0 * this->n.z + matrix.b0 * this->n.y;
    normal.y = this->n.x * matrix.a1 + this->n.z * matrix.c1 + this->n.y * matrix.b1;
    normal.z = matrix.a2 * this->n.x + matrix.c2 * this->n.z + matrix.b2 * this->n.y;
    this->n = normal;
    this->n.NormalizeSafe();

    C3Vector viewPoint = point * matrix;
    this->d = -(this->n.z * viewPoint.z + this->n.y * viewPoint.y + this->n.x * viewPoint.x);
}
