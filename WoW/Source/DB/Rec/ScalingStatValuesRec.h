#ifndef WOW_SOURCE_DB_REC_SCALINGSTATVALUESREC_H
#define WOW_SOURCE_DB_REC_SCALINGSTATVALUESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ScalingStatValuesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ int32_t unk_000C;
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ int32_t unk_0018;
    /* 001C */ int32_t unk_001C;
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ int32_t unk_002C;
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ int32_t unk_0034;
    /* 0038 */ int32_t unk_0038;
    /* 003C */ int32_t unk_003C;
    /* 0040 */ int32_t unk_0040;
    /* 0044 */ int32_t unk_0044;
    /* 0048 */ int32_t unk_0048;
    /* 004C */ int32_t unk_004C;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ int32_t unk_0054;
    /* 0058 */ int32_t unk_0058;
    /* 005C */ int32_t unk_005C;
    /* 0060 */ int32_t unk_0060;
    /* 0064 */ int32_t unk_0064;
    /* 0068 */ int32_t unk_0068;
    /* 006C */ int32_t unk_006C;
    /* 0070 */ int32_t unk_0070;
    /* 0074 */ int32_t unk_0074;
    /* 0078 */ int32_t unk_0078;
    /* 007C */ int32_t unk_007C;
    /* 0080 */ int32_t unk_0080;
    /* 0084 */ int32_t unk_0084;
    /* 0088 */ int32_t unk_0088;
    /* 008C */ int32_t unk_008C;
    /* 0090 */ int32_t unk_0090;
    /* 0094 */ int32_t unk_0094;
    /* 0098 */ int32_t unk_0098;
    /* 009C */ int32_t unk_009C;
    /* 00A0 */ int32_t unk_00A0;
    /* 00A4 */ int32_t unk_00A4;
    /* 00A8 */ int32_t unk_00A8;
    /* 00AC */ int32_t unk_00AC;
    /* 00B0 */ int32_t unk_00B0;
    /* 00B4 */ int32_t unk_00B4;
    /* 00B8 */ int32_t unk_00B8;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(ScalingStatValuesRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
