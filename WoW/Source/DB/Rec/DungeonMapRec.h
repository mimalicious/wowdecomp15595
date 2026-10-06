#ifndef WOW_SOURCE_DB_REC_DUNGEONMAPREC_H
#define WOW_SOURCE_DB_REC_DUNGEONMAPREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class DungeonMapRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_floorIndex;
    /* 000C */ float m_minX;
    /* 0010 */ float m_minY;
    /* 0014 */ float m_maxX;
    /* 0018 */ float m_maxY;
    /* 001C */ int32_t m_parentWorldMapID;

    static void FixupStrings(void* record, const char* strings);
};

#endif
