#ifndef WOW_SOURCE_DB_REC_TAXINODESREC_H
#define WOW_SOURCE_DB_REC_TAXINODESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TaxiNodesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_continentID;
    /* 0008 */ float m_pos [3];
    /* 0014 */ const char* m_name;
    /* 0018 */ int32_t m_mountCreatureID [2];

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(TaxiNodesRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
