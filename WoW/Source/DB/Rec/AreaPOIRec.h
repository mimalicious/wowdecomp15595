#ifndef WOW_SOURCE_DB_REC_AREAPOIREC_H
#define WOW_SOURCE_DB_REC_AREAPOIREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class AreaPOIRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_importance;
    /* 0008 */ int32_t m_icon [9];
    /* 002C */ int32_t m_factionID;
    /* 0030 */ float m_pos [2];
    /* 0038 */ int32_t m_continentID;
    /* 003C */ int32_t m_flags;
    /* 0040 */ int32_t m_areaID;
    /* 0044 */ DBCString m_name;
    /* 0048 */ DBCString m_description;
    /* 004C */ int32_t m_worldStateID;
    /* 0050 */ int32_t m_worldMapLink;
    /* 0054 */ int32_t m_graveyardID;
};

#endif
