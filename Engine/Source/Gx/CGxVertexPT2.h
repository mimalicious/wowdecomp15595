#ifndef ENGINE_SOURCE_GX_CGXVERTEXPT2_H
#define ENGINE_SOURCE_GX_CGXVERTEXPT2_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"

class CGxVertexPT2 {
    public:
    /* 0000 */ C3Vector p;
    /* 000C */ C2Vector tc[2];
};

#endif
