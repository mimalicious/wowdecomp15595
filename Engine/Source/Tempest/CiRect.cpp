#include "Engine/Source/Tempest/CiRect.h"

// OFFSET: 0x00722590
CiRect operator&(const CiRect& l, const CiRect& r) {
    int32_t maxX = l.maxX >= r.maxX ? r.maxX : l.maxX;
    int32_t maxY = l.maxY >= r.maxY ? r.maxY : l.maxY;
    int32_t minX = l.minX <= r.minX ? r.minX : l.minX;
    int32_t minY = l.minY > r.minY ? l.minY : r.minY;

    CiRect result;
    result.minX = minX;
    result.minY = minY;
    result.maxY = maxY;
    result.maxX = maxX;
    return result;
}

// PROVEN: 38ae961a IDENTICAL/NONE
// OFFSET: 0x0077CE60
CiRect CiRect::operator&=(const CiRect& r) {
    *this = *this & r;
    return *this;
}
