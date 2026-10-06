#ifndef WOW_SOURCE_DB_REC_TRANSPORTANIMATIONREC_H
#define WOW_SOURCE_DB_REC_TRANSPORTANIMATIONREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TransportAnimationRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_transportID;
    /* 0008 */ int32_t m_timeIndex;
    /* 000C */ float m_pos [3];
    /* 0018 */ int32_t m_sequenceID;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(TransportAnimationRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
