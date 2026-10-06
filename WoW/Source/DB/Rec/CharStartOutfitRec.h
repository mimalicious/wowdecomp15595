#ifndef WOW_SOURCE_DB_REC_CHARSTARTOUTFITREC_H
#define WOW_SOURCE_DB_REC_CHARSTARTOUTFITREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CharStartOutfitRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int8_t m_raceID;
    /* 0005 */ int8_t m_classID;
    /* 0006 */ int8_t m_sexID;
    /* 0007 */ int8_t m_outfitID;
    /* 0008 */ int32_t m_itemID [24];
    /* 0068 */ int32_t m_displayItemID [24];
    /* 00C8 */ int32_t m_inventoryType [24];
    /* 0128 */ int32_t unk_0128;
    /* 012C */ int32_t unk_012C;

    static void FixupStrings(void* record, const char* strings);
};

#endif
