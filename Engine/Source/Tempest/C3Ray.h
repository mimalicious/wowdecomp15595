#ifndef ENGINE_SOURCE_TEMPEST_C3RAY_H
#define ENGINE_SOURCE_TEMPEST_C3RAY_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class C3Ray {
    public:
    static C3Ray FromStartEnd(const C3Vector& start, const C3Vector& end, bool normalize);

    /* 0000 */ C3Vector origin;
    /* 000C */ C3Vector dir;

    C3Ray();
    C3Ray(const C3Vector& origin, const C3Vector& dir);
};

#include "Engine/Source/Tempest/C3Ray.inl"

#endif
