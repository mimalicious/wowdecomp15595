#ifndef WOW_SOURCE_DB_REC_ANIMATIONDATAREC_H
#define WOW_SOURCE_DB_REC_ANIMATIONDATAREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class AnimationDataRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_weaponflags;
    /* 000C */ int32_t m_bodyflags;
    /* 0010 */ int32_t m_flags;
    /* 0014 */ int32_t m_fallback;
    /* 0018 */ int32_t m_behaviorID;
    /* 001C */ int32_t m_behaviorTier;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(AnimationDataRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
