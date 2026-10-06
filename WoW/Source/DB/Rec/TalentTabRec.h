#ifndef WOW_SOURCE_DB_REC_TALENTTABREC_H
#define WOW_SOURCE_DB_REC_TALENTTABREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TalentTabRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ int32_t m_classMask;
    /* 0010 */ int32_t m_categoryEnumID;
    /* 0014 */ int32_t m_orderIndex;
    /* 0018 */ const char* m_backgroundFile;
    /* 001C */ const char* unk_001C;
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(TalentTabRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
