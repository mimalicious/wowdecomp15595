#ifndef WOW_SOURCE_DB_REC_DUNGEONMAPCHUNKREC_H
#define WOW_SOURCE_DB_REC_DUNGEONMAPCHUNKREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class DungeonMapChunkRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_wmogroupID;
    /* 000C */ int32_t m_dungeonMapID;
    /* 0010 */ float m_minZ;

    static void FixupStrings(void* record, const char* strings);
};

#endif
