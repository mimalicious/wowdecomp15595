#ifndef ENGINE_SOURCE_GX_CGXVERTEXPNCT_H
#define ENGINE_SOURCE_GX_CGXVERTEXPNCT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

class CGxVertexPNCT {
    public:
    /* 0000 */ C3Vector m_position;
    /* 000C */ C3Vector m_normal;
    /* 0018 */ CImVector m_color;
    /* 001C */ C2Vector m_texture;
};

#endif
