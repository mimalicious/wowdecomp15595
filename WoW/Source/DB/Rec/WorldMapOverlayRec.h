#ifndef WOW_SOURCE_DB_REC_WORLDMAPOVERLAYREC_H
#define WOW_SOURCE_DB_REC_WORLDMAPOVERLAYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class WorldMapOverlayRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapAreaID;
    /* 0008 */ int32_t m_areaID [4];
    /* 0018 */ DBCString unk_0018;
    /* 001C */ int32_t unk_001C;
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ int32_t unk_002C;
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ int32_t unk_0034;
    /* 0038 */ int32_t unk_0038;
};

#endif
