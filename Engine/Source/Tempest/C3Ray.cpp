#include <cmath>
#include "Engine/Source/Tempest/C3Ray.h"

// OFFSET: %0x0069C0F0
C3Ray::C3Ray(const C3Vector& origin, const C3Vector& dir) {
    this->origin = origin;
    this->dir = dir;
}

// OFFSET: %0x00A48D40
C3Ray C3Ray::FromStartEnd(const C3Vector& start, const C3Vector& end, bool normalize) {
    C3Vector dir;
    dir.x = end.x - start.x;
    dir.y = end.y - start.y;
    dir.z = end.z - start.z;

    if (normalize) {
        float scale = 1.0f / sqrt(dir.z * dir.z + dir.y * dir.y + dir.x * dir.x);
        dir.x = dir.x * scale;
        dir.y = dir.y * scale;
        dir.z = scale * dir.z;
    }

    C3Ray result;
    result.origin = start;
    result.dir = dir;
    return result;
}
