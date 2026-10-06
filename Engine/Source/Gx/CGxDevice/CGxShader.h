#ifndef ENGINE_SOURCE_GX_CGXDEVICE_CGXSHADER_H
#define ENGINE_SOURCE_GX_CGXDEVICE_CGXSHADER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxShaderPermute.h"
#include "Engine/Source/Storm/H/stpl.h"

struct CGxShaderHeader {
    /* 0000 */ uint32_t m_magic;
    /* 0004 */ uint32_t m_version;
    /* 0008 */ uint32_t m_permuteOffsetsOffset;
    /* 000C */ uint32_t m_permuteCount;
    /* 0010 */ uint32_t m_blockOffsetsOffset;
    /* 0014 */ uint32_t m_blockCount;
    /* 0018 */ uint32_t m_blockDataOffset;
};

class CGxShader : public TSHashObject<CGxShader, HASHKEY_STRI> {
    public:
    /* 0018 */ int32_t m_target;
    /* 001C */ uint32_t m_refCount;
    /* 0020 */ uint8_t* m_data;
    /* 0024 */ const uint32_t* m_permuteOffsets;
    /* 0028 */ const uint32_t* m_blockOffsets;
    /* 002C */ const uint8_t* m_blockData;
    /* 0030 */ TSGrowableArray<CGxShaderPermute> m_permutes;

    ~CGxShader();
    bool Load(uint8_t* data, uint32_t size);
};

#endif
