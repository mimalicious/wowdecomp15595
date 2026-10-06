#ifndef WOW_SOURCE_DB_REC_ACHIEVEMENT_CRITERIAREC_H
#define WOW_SOURCE_DB_REC_ACHIEVEMENT_CRITERIAREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class Achievement_CriteriaRec {
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
    /* 0024 */ const char* m_description;
    /* 0028 */ int32_t m_flags;
    /* 002C */ int32_t m_timerStartEvent;
    /* 0030 */ int32_t m_timerAssetID;
    /* 0034 */ int32_t m_timerTime;
    /* 0038 */ int32_t unk_0038;
    /* 003C */ int32_t unk_003C;
    /* 0040 */ int32_t unk_0040;
    /* 0044 */ int32_t unk_0044;
    /* 0048 */ int32_t unk_0048;
    /* 004C */ int32_t unk_004C;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ int32_t unk_0054;
    /* 0058 */ int32_t unk_0058;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(Achievement_CriteriaRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
