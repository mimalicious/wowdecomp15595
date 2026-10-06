#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/CRect.h"

// OFFSET: #0x0047FC40
bool CRect::ContainsPoint(const C2Vector& point) const {
    return this->minX < point.x && this->maxX > point.x && this->minY < point.y && this->maxY > point.y;
}

// OFFSET: #0x00488D40
bool CRect::IsPointInside(const C2Vector& point) const {
    return this->minX <= point.x && this->maxX >= point.x && this->minY <= point.y && this->maxY >= point.y;
}

// OFFSET: #0x0048EDE0
CRect CRect::operator&=(const CRect& r) {
    *this = *this & r;
    return *this;
}

// OFFSET: #0x0048ED60
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

// OFFSET: #0x0078F2F0
CRect operator|(const CRect& l, const CRect& r) {
    float maxX = r.maxX >= l.maxX ? r.maxX : l.maxX;
    float maxY = r.maxY >= l.maxY ? r.maxY : l.maxY;
    float minX = r.minX <= l.minX ? r.minX : l.minX;

    CRect result;
    result.minY = r.minY <= l.minY ? r.minY : l.minY;
    result.minX = minX;
    result.maxY = maxY;
    result.maxX = maxX;
    return result;
}

// OFFSET: #0x007A6DA0
CRect operator+(const CRect& l, const CRect& r) {
    CRect result;
    result.minY = l.minY + r.minY;
    result.minX = l.minX + r.minX;
    result.maxY = l.maxY + r.maxY;
    result.maxX = l.maxX + r.maxX;
    return result;
}

// OFFSET: #0x007A6DD0
CRect operator/(const CRect& l, const CRect& r) {
    CRect result;
    result.minY = l.minY / r.minY;
    result.minX = l.minX / r.minX;
    result.maxY = l.maxY / r.maxY;
    result.maxX = l.maxX / r.maxX;
    return result;
}

// PROVEN: d95b3d05 IDENTICAL
// OFFSET: 0x00828420
bool operator!=(const CRect& l, const CRect& r) {
    return l.minY != r.minY || l.minX != r.minX || l.maxY != r.maxY || l.maxX != r.maxX;
}

