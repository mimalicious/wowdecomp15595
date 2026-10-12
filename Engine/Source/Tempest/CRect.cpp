#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/CRect.h"

// PROVEN: 697ef5ac IDENTICAL/NONE
// OFFSET: 0x00B14340
CRect::CRect(const C2Vector& v) {
    this->minY = v.y;
    this->minX = v.x;
    this->maxY = v.y;
    this->maxX = v.x;
}

// OFFSET: [0x004FEFA0, 0x0061DD50]
CRect CRect::operator&=(const CRect& r) {
    *this = *this & r;
    return *this;
}

// OFFSET: 0x0061DCA0
CRect operator&(const CRect& l, const CRect& r) {
    float maxX = r.maxX <= l.maxX ? r.maxX : l.maxX;
    float maxY = r.maxY <= l.maxY ? r.maxY : l.maxY;
    float minX = r.minX >= l.minX ? r.minX : l.minX;

    CRect result;
    result.minY = r.minY >= l.minY ? r.minY : l.minY;
    result.minX = minX;
    result.maxY = maxY;
    result.maxX = maxX;
    return result;
}

// INLINED
CRect operator/(const CRect& l, const CRect& r) {
    CRect result;
    result.minY = l.minY / r.minY;
    result.minX = l.minX / r.minX;
    result.maxY = l.maxY / r.maxY;
    result.maxX = l.maxX / r.maxX;
    return result;
}

// PROVEN: d95b3d05 IDENTICAL/IDENTICAL
// OFFSET: 0x00828420/0x140546270
bool operator!=(const CRect& l, const CRect& r) {
    return l.minY != r.minY || l.minX != r.minX || l.maxY != r.maxY || l.maxX != r.maxX;
}

