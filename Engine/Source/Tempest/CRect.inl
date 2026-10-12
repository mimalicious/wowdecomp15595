#include "Engine/Source/Tempest/C2Vector.h"

// INLINED
inline CRect::CRect() {
}

// INLINED
inline CRect::CRect(float minY, float minX, float maxY, float maxX) {
    this->minY = minY;
    this->minX = minX;
    this->maxY = maxY;
    this->maxX = maxX;
}

// INLINED
inline bool CRect::ContainsPoint(const C2Vector& point) const {
    return this->minX < point.x && this->maxX > point.x && this->minY < point.y && this->maxY > point.y;
}

// INLINED
inline bool CRect::IsPointInside(const C2Vector& point) const {
    return this->minX <= point.x && this->maxX >= point.x && this->minY <= point.y && this->maxY >= point.y;
}

// INLINED
inline CRect operator|(const CRect& l, const CRect& r) {
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

// INLINED
inline CRect operator+(const CRect& l, const CRect& r) {
    CRect result;
    result.minY = l.minY + r.minY;
    result.minX = l.minX + r.minX;
    result.maxY = l.maxY + r.maxY;
    result.maxX = l.maxX + r.maxX;
    return result;
}
