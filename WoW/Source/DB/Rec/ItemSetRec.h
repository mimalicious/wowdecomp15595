#ifndef WOW_SOURCE_DB_REC_ITEMSETREC_H
#define WOW_SOURCE_DB_REC_ITEMSETREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ItemSetRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_itemID [17];
    /* 004C */ int32_t m_setSpellID [8];
    /* 006C */ int32_t m_setThreshold [8];
    /* 008C */ int32_t m_requiredSkill;
    /* 0090 */ int32_t m_requiredSkillRank;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(ItemSetRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
