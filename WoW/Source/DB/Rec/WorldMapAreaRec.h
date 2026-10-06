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
    /* 000C */ DBCString m_areaName;
    /* 0010 */ float m_locLeft;
    /* 0014 */ float m_locRight;
    /* 0018 */ float m_locTop;
    /* 001C */ float m_locBottom;
    /* 0020 */ int32_t m_displayMapID;
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ int32_t unk_002C;
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ int32_t unk_0034;

    static void FixupStrings(void* record, const char* strings);
};

#endif
