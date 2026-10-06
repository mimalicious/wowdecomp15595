#ifndef WOW_SOURCE_DB_REC_TAXINODESREC_H
#define WOW_SOURCE_DB_REC_TAXINODESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TaxiNodesRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_continentID;
    /* 0008 */ float m_pos [3];
    /* 0014 */ DBCString m_name;
    /* 0018 */ int32_t m_mountCreatureID [2];
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ float unk_0024;
    /* 0028 */ float unk_0028;
};

#endif
