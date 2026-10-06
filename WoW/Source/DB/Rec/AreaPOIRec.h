#ifndef WOW_SOURCE_DB_REC_AREAPOIREC_H
#define WOW_SOURCE_DB_REC_AREAPOIREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class AreaPOIRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_importance;
    /* 0008 */ int32_t m_icon [9];
    /* 002C */ int32_t m_factionID;
    /* 0030 */ float m_pos [2];
    /* 0038 */ int32_t m_continentID;
    /* 003C */ int32_t m_flags;
    /* 0040 */ int32_t m_areaID;
    /* 0044 */ const char* m_name;
    /* 0048 */ const char* m_description;
    /* 004C */ int32_t m_worldStateID;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ int32_t unk_0054;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(AreaPOIRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
