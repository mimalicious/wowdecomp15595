#ifndef ENGINE_SOURCE_GX_CGXSHADERPERMUTE_H
#define ENGINE_SOURCE_GX_CGXSHADERPERMUTE_H

#include <cstddef>
#include <cstdint>

class CGxShader;

#pragma pack(push, 2)

class CGxShaderPermute {
    public:
    /* 0000 */ void* m_apiSpecific;
    /* 0004 */ CGxShader* m_shader;
    /* 0008 */ uint32_t m_index;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint16_t unk_0014;
    /* 0016 */ uint32_t unk_0016 [4];
    /* 0026 */ uint8_t m_target;
    /* 0028 */ uint32_t m_valid : 1;
    /* 0028 */ uint32_t m_loaded : 1;
    /* 0028 */ uint32_t m_flagsUnused : 30;

    int32_t Valid();
    uint8_t* Load();
};

#pragma pack(pop)

#endif
