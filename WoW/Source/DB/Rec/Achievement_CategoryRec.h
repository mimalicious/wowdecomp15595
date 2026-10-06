#ifndef WOW_SOURCE_DB_REC_ACHIEVEMENT_CATEGORYREC_H
#define WOW_SOURCE_DB_REC_ACHIEVEMENT_CATEGORYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class Achievement_CategoryRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_parent;
    /* 0008 */ DBCString m_name;
    /* 000C */ int32_t m_uiOrder;

    static void FixupStrings(void* record, const char* strings);
};

#endif
