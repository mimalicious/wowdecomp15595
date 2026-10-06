#ifndef WOW_SOURCE_DB_REC_CAMERAMODEREC_H
#define WOW_SOURCE_DB_REC_CAMERAMODEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CameraModeRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* unk_0004;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ int32_t unk_000C;
    /* 0010 */ float unk_0010;
    /* 0014 */ float unk_0014;
    /* 0018 */ float unk_0018;
    /* 001C */ float unk_001C;
    /* 0020 */ float unk_0020;
    /* 0024 */ float unk_0024;
    /* 0028 */ float unk_0028;
    /* 002C */ float unk_002C;
    /* 0030 */ float unk_0030;
    /* 0034 */ int32_t unk_0034;
    /* 0038 */ int32_t unk_0038;
    /* 003C */ int32_t unk_003C;
    /* 0040 */ int32_t unk_0040;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(CameraModeRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
