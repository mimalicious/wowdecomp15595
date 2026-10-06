#ifndef WOW_SOURCE_DB_REC_BATTLEMASTERLISTREC_H
#define WOW_SOURCE_DB_REC_BATTLEMASTERLISTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class BattlemasterListRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID [8];
    /* 0024 */ int32_t m_instanceType;
    /* 0028 */ int32_t m_groupsAllowed;
    /* 002C */ const char* m_name;
    /* 0030 */ int32_t m_maxGroupSize;
    /* 0034 */ int32_t m_holidayWorldState;
    /* 0038 */ int32_t unk_0038;
    /* 003C */ int32_t unk_003C;
    /* 0040 */ int32_t unk_0040;
    /* 0044 */ int32_t unk_0044;
    /* 0048 */ int32_t unk_0048;
    /* 004C */ int32_t unk_004C;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(BattlemasterListRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
