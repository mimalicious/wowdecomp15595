#include <cstdlib>
#include "Engine/Source/Tempest/C2iVector.h"

// OFFSET: 0x00B09B40/0x1408CD280
uint32_t C2iVector::MajorAxis() const {
    return abs(this->x) <= abs(this->y);
}

// OFFSET: 0x0078E3E0
bool operator<(const C2iVector& l, const C2iVector& r) {
    return l.x < r.x && l.y < r.y;
}
