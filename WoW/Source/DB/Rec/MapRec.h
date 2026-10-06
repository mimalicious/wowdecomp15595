#ifndef WOW_SOURCE_DB_REC_MAPREC_H
#define WOW_SOURCE_DB_REC_MAPREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class MapRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_directory;
    /* 0008 */ int32_t m_instanceType;
    /* 000C */ int32_t m_flags;
    /* 0010 */ int32_t m_pvp;
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ DBCString m_mapName;
    /* 001C */ int32_t m_areaTableID;
    /* 0020 */ DBCString m_mapDescription0;
    /* 0024 */ DBCString m_mapDescription1;
    /* 0028 */ int32_t m_loadingScreenID;
    /* 002C */ float m_minimapIconScale;
    /* 0030 */ int32_t m_corpseMapID;
    /* 0034 */ float m_corpse [2];
    /* 003C */ int32_t m_timeOfDayOverride;
    /* 0040 */ int32_t m_expansionID;
    /* 0044 */ int32_t m_raidOffset;
    /* 0048 */ int32_t m_maxPlayers;
    /* 004C */ int32_t unk_004C;

    static void FixupStrings(void* record, const char* strings);
};

#endif
