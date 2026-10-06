#ifndef WOW_SOURCE_DB_REC_TRANSPORTROTATIONREC_H
#define WOW_SOURCE_DB_REC_TRANSPORTROTATIONREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TransportRotationRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_gameObjectsID;
    /* 0008 */ int32_t m_timeIndex;
    /* 000C */ float m_rot [4];

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(TransportRotationRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
