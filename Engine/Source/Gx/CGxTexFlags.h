#ifndef ENGINE_SOURCE_GX_CGXTEXFLAGS_H
#define ENGINE_SOURCE_GX_CGXTEXFLAGS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"

class CGxTexFlags {
    public:
    uint32_t m_filter : 3;
    uint32_t m_bit3 : 1;
    uint32_t m_wrapU : 2;
    uint32_t m_wrapV : 2;
    uint32_t m_wrapW : 2;
    uint32_t m_renderTarget : 1;
    uint32_t m_bit8 : 1;
    uint32_t m_maxAnisotropy : 5;
    uint32_t m_bit14 : 1;
    uint32_t m_bit15 : 1;
    uint32_t m_bit19 : 1;
    uint32_t m_bit20 : 1;
    uint32_t m_unk21 : 10;
    uint32_t m_bit31 : 1;

    bool operator!=(const CGxTexFlags& flags) const;
    bool operator==(const CGxTexFlags& flags) const;
    CGxTexFlags();
    CGxTexFlags(EGxTexFilter filter, uint32_t wrapU = 0, uint32_t wrapV = 0, uint32_t wrapW = 0, uint32_t renderTarget = 0, uint32_t maxAnisotropy = 1, uint32_t bit8 = 0, uint32_t bit14 = 0, uint32_t bit15 = 0);
};

#endif
