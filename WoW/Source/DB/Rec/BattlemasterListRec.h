#ifndef WOW_SOURCE_DB_REC_BATTLEMASTERLISTREC_H
#define WOW_SOURCE_DB_REC_BATTLEMASTERLISTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class BattlemasterListRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID [8];
    /* 0024 */ int32_t m_instanceType;
    /* 0028 */ int32_t m_groupsAllowed;
    /* 002C */ DBCString m_name;
    /* 0030 */ int32_t m_maxGroupSize;
    /* 0034 */ int32_t m_holidayWorldState;
    /* 0038 */ int32_t m_minLevel;
    /* 003C */ int32_t m_maxLevel;
    /* 0040 */ int32_t m_ratedPlayers;
    /* 0044 */ int32_t m_minPlayers;
    /* 0048 */ int32_t m_maxPlayers;
    /* 004C */ int32_t m_flags;
};

#endif
