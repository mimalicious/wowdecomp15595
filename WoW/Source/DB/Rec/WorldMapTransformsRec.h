#ifndef WOW_SOURCE_DB_REC_WORLDMAPTRANSFORMSREC_H
#define WOW_SOURCE_DB_REC_WORLDMAPTRANSFORMSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class WorldMapTransformsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ float m_regionMin [2];
    /* 0010 */ float m_regionMax [2];
    /* 0018 */ int32_t m_newMapID;
    /* 001C */ float m_regionOffset [2];
    /* 0024 */ int32_t m_newDungeonMapID;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ int32_t unk_002C;

    static void FixupStrings(void* record, const char* strings);
};

#endif
