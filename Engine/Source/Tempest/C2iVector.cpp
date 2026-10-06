#include <cstdlib>
#include "Engine/Source/Tempest/C2iVector.h"

// OFFSET: %0x00A48D20
uint32_t C2iVector::MajorAxis() const {
    return abs(this->x) <= abs(this->y);
}

// OFFSET: %0x006F3D60
bool operator<(const C2iVector& l, const C2iVector& r) {
    return l.x < r.x && l.y < r.y;
}
