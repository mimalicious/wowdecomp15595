#ifndef WOW_SOURCE_DB_REC_DEATHTHUDLOOKUPSREC_H
#define WOW_SOURCE_DB_REC_DEATHTHUDLOOKUPSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class DeathThudLookupsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_sizeClass;
    /* 0008 */ int32_t m_terrainTypeSoundID;
    /* 000C */ int32_t m_soundEntryID;
    /* 0010 */ int32_t m_soundEntryIdwater;
};

#endif
