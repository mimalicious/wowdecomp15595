#ifndef ENGINE_SOURCE_TEMPEST_CFRUSTUM_INL
#define ENGINE_SOURCE_TEMPEST_CFRUSTUM_INL

#include "Engine/Source/Tempest/CAaSphere.h"

// INLINED
inline uint32_t CFrustum::Cull(const C3Vector& center, float radius) {
    CAaSphere sphere;
    sphere.c = center;
    sphere.r = radius;
    return this->Cull(sphere);
}

#endif
