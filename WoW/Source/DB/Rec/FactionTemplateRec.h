#ifndef WOW_SOURCE_DB_REC_FACTIONTEMPLATEREC_H
#define WOW_SOURCE_DB_REC_FACTIONTEMPLATEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class FactionTemplateRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_faction;
    /* 0008 */ int32_t m_flags;
    /* 000C */ int32_t m_factionGroup;
    /* 0010 */ int32_t m_friendGroup;
    /* 0014 */ int32_t m_enemyGroup;
    /* 0018 */ int32_t m_enemies [4];
    /* 0028 */ int32_t m_friend [4];
};

#endif
