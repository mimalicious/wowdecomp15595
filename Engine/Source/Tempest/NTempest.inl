#ifndef ENGINE_SOURCE_TEMPEST_NTEMPEST_INL
#define ENGINE_SOURCE_TEMPEST_NTEMPEST_INL

#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"

// INLINED
inline float NTempest::DistanceSquared(const CAaBox& box, const C2Vector& point) {
    float x = box.b.x <= point.x ? (box.t.x >= point.x ? point.x : box.t.x) : box.b.x;
    float y = box.b.y <= point.y ? (box.t.y >= point.y ? point.y : box.t.y) : box.b.y;
    float dy = y - point.y;
    return (x - point.x) * (x - point.x) + dy * dy;
}

// INLINED
inline float NTempest::DistanceSquared(const CAaBox& box, const C3Vector& point) {
    float x = box.b.x <= point.x ? (box.t.x >= point.x ? point.x : box.t.x) : box.b.x;
    float y = box.b.y <= point.y ? (box.t.y >= point.y ? point.y : box.t.y) : box.b.y;
    float z = box.b.z <= point.z ? (box.t.z >= point.z ? point.z : box.t.z) : box.b.z;
    float dy = y - point.y;
    float dz = z - point.z;
    return dy * dy + (x - point.x) * (x - point.x) + dz * dz;
}

#endif
