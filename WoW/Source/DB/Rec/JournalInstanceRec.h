#ifndef WOW_SOURCE_DB_REC_JOURNALINSTANCEREC_H
#define WOW_SOURCE_DB_REC_JOURNALINSTANCEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class JournalInstanceRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_areaID;
    /* 000C */ int32_t m_buttonFileDataID;
    /* 0010 */ int32_t m_backgroundFileDataID;
    /* 0014 */ int32_t m_loreFileDataID;
    /* 0018 */ int32_t m_orderIndex;
    /* 001C */ DBCString m_name;
    /* 0020 */ DBCString m_description;
};

#endif
