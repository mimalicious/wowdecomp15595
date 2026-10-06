#ifndef WOW_SOURCE_DB_REC_WORLDMAPAREAREC_H
#define WOW_SOURCE_DB_REC_WORLDMAPAREAREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class WorldMapAreaRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_areaID;
    /* 000C */ const char* m_areaName;
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ int32_t unk_0018;
    /* 001C */ int32_t unk_001C;
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ int32_t unk_002C;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(WorldMapAreaRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
