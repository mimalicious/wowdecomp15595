#ifndef WOW_SOURCE_DB_REC_MOUNTCAPABILITYREC_H
#define WOW_SOURCE_DB_REC_MOUNTCAPABILITYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class MountCapabilityRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ int32_t m_requiredRidingSkill;
    /* 000C */ int32_t m_requiredArea;
    /* 0010 */ int32_t m_requiredAura;
    /* 0014 */ int32_t m_requiredSpell;
    /* 0018 */ int32_t m_speedModSpell;
    /* 001C */ int32_t m_requiredMap;
};

#endif
