#ifndef WOW_SOURCE_DB_REC_ITEMEXTENDEDCOSTREC_H
#define WOW_SOURCE_DB_REC_ITEMEXTENDEDCOSTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB2.h"

class ItemExtendedCostRec {
    public:
    static const DB2Meta s_meta;

    static void FixupRecord(void* record);

    /* 0000 */ int32_t m_ID;
    /* 0004 */ int32_t m_honorPoints;
    /* 0008 */ int32_t m_arenaPoints;
    /* 000C */ int32_t m_arenaBracket;
    /* 0010 */ int32_t m_itemID[5];
    /* 0024 */ int32_t m_itemCount[5];
    /* 0038 */ int32_t m_requiredArenaRating;
    /* 003C */ int32_t m_itemPurchaseGroup;
    /* 0040 */ int32_t m_currencyID[5];
    /* 0054 */ int32_t m_currencyCount[5];
    /* 0068 */ int32_t m_requiredFactionID;
    /* 006C */ int32_t m_requiredFactionStanding;
    /* 0070 */ int32_t m_flags;
    /* 0074 */ int32_t m_requiredGuildLevel;
    /* 0078 */ int32_t m_requiredAchievement;
};

#endif
