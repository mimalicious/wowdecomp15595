#ifndef WOW_SOURCE_DB_REC_FOOTSTEPTERRAINLOOKUPREC_H
#define WOW_SOURCE_DB_REC_FOOTSTEPTERRAINLOOKUPREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class FootstepTerrainLookupRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_creatureFootstepID;
    /* 0008 */ int32_t m_terrainSoundID;
    /* 000C */ int32_t m_soundID;
    /* 0010 */ int32_t m_soundIdsplash;
};

#endif
