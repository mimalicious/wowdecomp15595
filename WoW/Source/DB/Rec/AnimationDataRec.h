#ifndef WOW_SOURCE_DB_REC_ANIMATIONDATAREC_H
#define WOW_SOURCE_DB_REC_ANIMATIONDATAREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class AnimationDataRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ int32_t m_flags;
    /* 000C */ int32_t m_fallback;
    /* 0010 */ int32_t m_behaviorID;
    /* 0014 */ int32_t m_behaviorTier;
};

#endif
