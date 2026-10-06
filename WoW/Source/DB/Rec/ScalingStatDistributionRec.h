#ifndef WOW_SOURCE_DB_REC_SCALINGSTATDISTRIBUTIONREC_H
#define WOW_SOURCE_DB_REC_SCALINGSTATDISTRIBUTIONREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ScalingStatDistributionRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_statID [10];
    /* 002C */ int32_t m_bonus [5];
    /* 0040 */ int32_t unk_0040;
    /* 0044 */ int32_t unk_0044;
    /* 0048 */ int32_t unk_0048;
    /* 004C */ int32_t unk_004C;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ int32_t unk_0054;
    /* 0058 */ int32_t m_maxlevel;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(ScalingStatDistributionRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
