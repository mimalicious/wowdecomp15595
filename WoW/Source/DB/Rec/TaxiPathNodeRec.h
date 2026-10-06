#ifndef WOW_SOURCE_DB_REC_TAXIPATHNODEREC_H
#define WOW_SOURCE_DB_REC_TAXIPATHNODEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TaxiPathNodeRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_pathID;
    /* 0008 */ int32_t m_nodeIndex;
    /* 000C */ int32_t m_continentID;
    /* 0010 */ float m_loc [3];
    /* 001C */ int32_t m_flags;
    /* 0020 */ int32_t m_delay;
    /* 0024 */ int32_t m_arrivalEventID;
    /* 0028 */ int32_t m_departureEventID;
};

#endif
