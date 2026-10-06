#ifndef WOW_SOURCE_DB_REC_SKILLLINECATEGORYREC_H
#define WOW_SOURCE_DB_REC_SKILLLINECATEGORYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SkillLineCategoryRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_sortIndex;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(SkillLineCategoryRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
