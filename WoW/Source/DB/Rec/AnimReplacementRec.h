#ifndef WOW_SOURCE_DB_REC_ANIMREPLACEMENTREC_H
#define WOW_SOURCE_DB_REC_ANIMREPLACEMENTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class AnimReplacementRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ int32_t unk_000C;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(AnimReplacementRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
