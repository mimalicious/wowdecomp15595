#ifndef ENGINE_SOURCE_TEMPEST_C4PLANE_H
#define ENGINE_SOURCE_TEMPEST_C4PLANE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class C44Matrix;

class C4Plane {
    public:
    /* 0000 */ C3Vector n;
    /* 000C */ float d;

    C4Plane();
    float Distance(const C3Vector& point) const;
    void From3Pos(const C3Vector& p0, const C3Vector& p1, const C3Vector& p2);
    C3Vector& Reflect(const C3Vector& point, C3Vector& result) const;
    void Transform(const C44Matrix& matrix);
};

bool operator!=(const C4Plane& l, const C4Plane& r);

#include "Engine/Source/Tempest/C4Plane.inl"

#endif
