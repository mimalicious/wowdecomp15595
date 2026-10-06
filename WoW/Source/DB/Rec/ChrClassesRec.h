#ifndef WOW_SOURCE_DB_REC_CHRCLASSESREC_H
#define WOW_SOURCE_DB_REC_CHRCLASSESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ChrClassesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ const char* m_petNameToken;
    /* 000C */ const char* m_name;
    /* 0010 */ const char* m_nameFemale;
    /* 0014 */ const char* m_nameMale;
    /* 0018 */ const char* m_filename;
    /* 001C */ int32_t m_spellClassSet;
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ int32_t unk_002C;
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ int32_t unk_0034;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(ChrClassesRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
