#ifndef WOW_SOURCE_DB_REC_TRANSPORTPHYSICSREC_H
#define WOW_SOURCE_DB_REC_TRANSPORTPHYSICSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TransportPhysicsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_waveAmp;
    /* 0008 */ float m_waveTimeScale;
    /* 000C */ float m_rollAmp;
    /* 0010 */ float m_rollTimeScale;
    /* 0014 */ float m_pitchAmp;
    /* 0018 */ float m_pitchTimeScale;
    /* 001C */ float m_maxBank;
    /* 0020 */ float m_maxBankTurnSpeed;
    /* 0024 */ float m_speedDampThresh;
    /* 0028 */ float m_speedDamp;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(TransportPhysicsRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
