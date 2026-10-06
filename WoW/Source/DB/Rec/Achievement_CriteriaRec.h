#ifndef WOW_SOURCE_DB_REC_ACHIEVEMENT_CRITERIAREC_H
#define WOW_SOURCE_DB_REC_ACHIEVEMENT_CRITERIAREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class Achievement_CriteriaRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_achievementID;
    /* 0008 */ int32_t m_type;
    /* 000C */ int32_t m_assetID;
    /* 0010 */ int32_t m_quantity;
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ int32_t m_startEvent;
    /* 001C */ int32_t m_startAsset;
    /* 0020 */ int32_t m_failEvent;
    /* 0024 */ int32_t m_failAsset;
    /* 0028 */ DBCString m_description;
    /* 002C */ int32_t m_flags;
    /* 0030 */ int32_t m_timerStartEvent;
    /* 0034 */ int32_t m_timerAssetID;
    /* 0038 */ int32_t m_timerTime;
    /* 003C */ int32_t unk_003C;
    /* 0040 */ int32_t unk_0040;
    /* 0044 */ int32_t unk_0044;
    /* 0048 */ int32_t unk_0048;
    /* 004C */ int32_t unk_004C;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ int32_t unk_0054;
    /* 0058 */ int32_t unk_0058;
    /* 005C */ int32_t unk_005C;

    static void FixupStrings(void* record, const char* strings);
};

#endif
