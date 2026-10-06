#ifndef WOW_SOURCE_DB_REC_GROUNDEFFECTTEXTUREREC_H
#define WOW_SOURCE_DB_REC_GROUNDEFFECTTEXTUREREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class GroundEffectTextureRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_doodadID [4];
    /* 0014 */ int32_t m_doodadWeight [4];
    /* 0024 */ int32_t m_density;
    /* 0028 */ int32_t m_sound;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(GroundEffectTextureRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
