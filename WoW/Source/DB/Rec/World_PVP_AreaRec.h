#ifndef WOW_SOURCE_DB_REC_WORLD_PVP_AREAREC_H
#define WOW_SOURCE_DB_REC_WORLD_PVP_AREAREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class World_PVP_AreaRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_areaID;
    /* 0008 */ int32_t m_nextTimeWorldState;
    /* 000C */ int32_t m_gameTimeWorldState;
    /* 0010 */ int32_t m_battlePopulateTime;
    /* 0014 */ int32_t m_minLevel;
    /* 0018 */ int32_t m_maxLevel;
};

#endif
