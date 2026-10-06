#ifndef WOW_SOURCE_DB_REC_SPELLMISSILEREC_H
#define WOW_SOURCE_DB_REC_SPELLMISSILEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellMissileRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ float m_defaultPitchMin;
    /* 000C */ float m_defaultPitchMax;
    /* 0010 */ float m_defaultSpeedMin;
    /* 0014 */ float m_defaultSpeedMax;
    /* 0018 */ float m_randomizeFacingMin;
    /* 001C */ float m_randomizeFacingMax;
    /* 0020 */ float m_randomizePitchMin;
    /* 0024 */ float m_randomizePitchMax;
    /* 0028 */ float m_randomizeSpeedMin;
    /* 002C */ float m_randomizeSpeedMax;
    /* 0030 */ float m_gravity;
    /* 0034 */ float m_maxDuration;
    /* 0038 */ float m_collisionRadius;

    static void FixupStrings(void* record, const char* strings);
};

#endif
