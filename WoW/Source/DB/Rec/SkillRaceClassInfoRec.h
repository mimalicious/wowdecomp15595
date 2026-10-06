#ifndef WOW_SOURCE_DB_REC_SKILLRACECLASSINFOREC_H
#define WOW_SOURCE_DB_REC_SKILLRACECLASSINFOREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SkillRaceClassInfoRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_skillID;
    /* 0008 */ int32_t m_raceMask;
    /* 000C */ int32_t m_classMask;
    /* 0010 */ int32_t m_flags;
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ int32_t unk_0018;
    /* 001C */ int32_t m_skillTierID;
    /* 0020 */ int32_t m_skillCostIndex;
};

#endif
