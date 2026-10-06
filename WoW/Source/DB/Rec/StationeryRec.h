#ifndef WOW_SOURCE_DB_REC_STATIONERYREC_H
#define WOW_SOURCE_DB_REC_STATIONERYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class StationeryRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_itemID;
    /* 0008 */ const char* m_texture;
    /* 000C */ int32_t m_flags;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(StationeryRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
