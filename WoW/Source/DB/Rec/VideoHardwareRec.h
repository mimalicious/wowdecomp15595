#ifndef WOW_SOURCE_DB_REC_VIDEOHARDWAREREC_H
#define WOW_SOURCE_DB_REC_VIDEOHARDWAREREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class VideoHardwareRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_vendorID;
    /* 0008 */ int32_t m_deviceID;
    /* 000C */ int32_t m_farclipIdx;
    /* 0010 */ int32_t m_terrainLoddistIdx;
    /* 0014 */ int32_t m_terrainShadowLod;
    /* 0018 */ int32_t m_detailDoodadDensityIdx;
    /* 001C */ int32_t m_detailDoodadAlpha;
    /* 0020 */ int32_t m_animatingDoodadIdx;
    /* 0024 */ int32_t m_trilinear;
    /* 0028 */ int32_t m_numLights;
    /* 002C */ int32_t m_specularity;
    /* 0030 */ int32_t m_waterLodidx;
    /* 0034 */ int32_t m_particleDensityIdx;
    /* 0038 */ int32_t m_unitDrawDistIdx;
    /* 003C */ int32_t m_smallCullDistIdx;
    /* 0040 */ int32_t m_resolutionIdx;
    /* 0044 */ int32_t m_baseMipLevel;
    /* 0048 */ DBCString m_oglOverrides;
    /* 004C */ DBCString m_d3DOverrides;
    /* 0050 */ int32_t m_fixLag;
    /* 0054 */ int32_t m_multisample;
    /* 0058 */ int32_t m_atlasdisable;

    static void FixupStrings(void* record, const char* strings);
};

#endif
