#ifndef ENGINE_SOURCE_TEMPEST_C4VECTOR_H
#define ENGINE_SOURCE_TEMPEST_C4VECTOR_H

#include <cstddef>
#include <cstdint>

class CImVector;

class C4Vector {
    public:
    /* 0000 */ float x;
    /* 0004 */ float y;
    /* 0008 */ float z;
    /* 000C */ float w;

    C4Vector();
    C4Vector(const CImVector& color);
    C4Vector(float x, float y, float z, float w);
    void NormalizeSafe();
};

#include "Engine/Source/Tempest/C4Vector.inl"

#endif
