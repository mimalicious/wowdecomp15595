#ifndef WOW_SOURCE_DB_REC_SPELLFLYOUTITEMREC_H
#define WOW_SOURCE_DB_REC_SPELLFLYOUTITEMREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellFlyoutItemRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_spellFlyoutID;
    /* 0008 */ int32_t m_spellID;
    /* 000C */ int32_t m_slot;
};

#endif
