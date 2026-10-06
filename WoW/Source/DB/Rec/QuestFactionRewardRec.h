#ifndef WOW_SOURCE_DB_REC_QUESTFACTIONREWARDREC_H
#define WOW_SOURCE_DB_REC_QUESTFACTIONREWARDREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class QuestFactionRewardRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_difficulty [10];
};

#endif
