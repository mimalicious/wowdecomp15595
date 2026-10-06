#ifndef WOW_SOURCE_DB_REC_CREATURESOUNDDATAREC_H
#define WOW_SOURCE_DB_REC_CREATURESOUNDDATAREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CreatureSoundDataRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundExertionID;
    /* 0008 */ int32_t m_soundExertionCriticalID;
    /* 000C */ int32_t m_soundInjuryID;
    /* 0010 */ int32_t m_soundInjuryCriticalID;
    /* 0014 */ int32_t m_soundInjuryCrushingBlowID;
    /* 0018 */ int32_t m_soundDeathID;
    /* 001C */ int32_t m_soundStunID;
    /* 0020 */ int32_t m_soundStandID;
    /* 0024 */ int32_t m_soundFootstepID;
    /* 0028 */ int32_t m_soundAggroID;
    /* 002C */ int32_t m_soundWingFlapID;
    /* 0030 */ int32_t m_soundWingGlideID;
    /* 0034 */ int32_t m_soundAlertID;
    /* 0038 */ int32_t m_soundFidget [5];
    /* 004C */ int32_t m_customAttack [4];
    /* 005C */ int32_t m_npcsoundID;
    /* 0060 */ int32_t m_loopSoundID;
    /* 0064 */ int32_t m_creatureImpactType;
    /* 0068 */ int32_t m_soundJumpStartID;
    /* 006C */ int32_t m_soundJumpEndID;
    /* 0070 */ int32_t m_soundPetAttackID;
    /* 0074 */ int32_t m_soundPetOrderID;
    /* 0078 */ int32_t m_soundPetDismissID;
    /* 007C */ float m_fidgetDelaySecondsMin;
    /* 0080 */ float m_fidgetDelaySecondsMax;
    /* 0084 */ int32_t m_birthSoundID;
    /* 0088 */ int32_t m_spellCastDirectedSoundID;
    /* 008C */ int32_t m_submergeSoundID;
    /* 0090 */ int32_t unk_0090;
    /* 0094 */ int32_t unk_0094;
    /* 0098 */ int32_t unk_0098;
    /* 009C */ int32_t unk_009C;

    static void FixupStrings(void* record, const char* strings);
};

#endif
