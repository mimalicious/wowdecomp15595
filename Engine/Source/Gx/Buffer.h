#ifndef ENGINE_SOURCE_GX_BUFFER_H
#define ENGINE_SOURCE_GX_BUFFER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/Types.h"

class VertexBufDesc {
    public:
    /* 0000 */ const char* name;
    /* 0004 */ const CGxVertexAttrib* attribs;
    /* 0008 */ uint32_t attribCount;
    /* 000C */ uint32_t size;
    /* 0010 */ const CGxVertexAttrib* attribPtrs [GxVAs_Last];
    /* 0048 */ uint32_t mask;
};

namespace Buffer {
    extern VertexBufDesc s_vertexBufDesc[GxVertexBufferFormats_Last];
}

#endif
