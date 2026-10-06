#ifndef WOW_SOURCE_DB_REC_FOOTSTEPTERRAINLOOKUPREC_H
#define WOW_SOURCE_DB_REC_FOOTSTEPTERRAINLOOKUPREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class FootstepTerrainLookupRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_creatureFootstepID;
    /* 0008 */ int32_t m_terrainSoundID;
    /* 000C */ int32_t m_soundID;
    /* 0010 */ int32_t m_soundIdsplash;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(FootstepTerrainLookupRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
