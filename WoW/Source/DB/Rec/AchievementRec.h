#ifndef WOW_SOURCE_DB_REC_ACHIEVEMENTREC_H
#define WOW_SOURCE_DB_REC_ACHIEVEMENTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class AchievementRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_faction;
    /* 0008 */ int32_t m_instanceID;
    /* 000C */ int32_t m_supercedes;
    /* 0010 */ DBCString m_title;
    /* 0014 */ DBCString m_description;
    /* 0018 */ int32_t m_category;
    /* 001C */ int32_t m_points;
    /* 0020 */ int32_t m_uiOrder;
    /* 0024 */ int32_t m_flags;
    /* 0028 */ int32_t m_iconID;
    /* 002C */ DBCString m_reward;
    /* 0030 */ int32_t m_minimumCriteria;
    /* 0034 */ int32_t m_sharesCriteria;

    static void FixupStrings(void* record, const char* strings);
};

#endif
