#ifndef WOW_SOURCE_DB_REC_AUCTIONHOUSEREC_H
#define WOW_SOURCE_DB_REC_AUCTIONHOUSEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class AuctionHouseRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_factionID;
    /* 0008 */ int32_t m_depositRate;
    /* 000C */ int32_t m_consignmentRate;
    /* 0010 */ DBCString m_name;
};

#endif
