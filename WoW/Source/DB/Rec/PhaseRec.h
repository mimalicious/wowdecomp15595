#ifndef WOW_SOURCE_DB_REC_PHASEREC_H
#define WOW_SOURCE_DB_REC_PHASEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class PhaseRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* unk_0004;
    /* 0008 */ int32_t unk_0008;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(PhaseRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
