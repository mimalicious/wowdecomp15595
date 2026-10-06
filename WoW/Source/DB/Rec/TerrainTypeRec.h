#ifndef WOW_SOURCE_DB_REC_TERRAINTYPEREC_H
#define WOW_SOURCE_DB_REC_TERRAINTYPEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TerrainTypeRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_terrainID;
    /* 0008 */ DBCString m_terrainDesc;
    /* 000C */ int32_t m_footstepSprayRun;
    /* 0010 */ int32_t m_footstepSprayWalk;
    /* 0014 */ int32_t m_soundID;
    /* 0018 */ int32_t m_flags;
};

#endif
