#ifndef WOW_SOURCE_DB_REC_CREATUREIMMUNITIESREC_H
#define WOW_SOURCE_DB_REC_CREATUREIMMUNITIESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CreatureImmunitiesRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_school;
    /* 0008 */ int32_t m_dispelType;
    /* 000C */ int32_t m_mechanicsInverted;
    /* 0010 */ int32_t m_mechanics;
    /* 0014 */ int32_t m_effectsInverted;
    /* 0018 */ int32_t m_effects [6];
    /* 0030 */ int32_t m_statesInverted;
    /* 0034 */ int32_t m_states [12];
    /* 0064 */ int32_t m_flags;
};

#endif
