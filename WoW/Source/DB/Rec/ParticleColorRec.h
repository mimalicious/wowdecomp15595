#ifndef WOW_SOURCE_DB_REC_PARTICLECOLORREC_H
#define WOW_SOURCE_DB_REC_PARTICLECOLORREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ParticleColorRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_start [3];
    /* 0010 */ int32_t m_mid [3];
    /* 001C */ int32_t m_end [3];

    static void FixupStrings(void* record, const char* strings);
};

#endif
