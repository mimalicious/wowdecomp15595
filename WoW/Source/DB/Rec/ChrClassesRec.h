#ifndef WOW_SOURCE_DB_REC_CHRCLASSESREC_H
#define WOW_SOURCE_DB_REC_CHRCLASSESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ChrClassesRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ DBCString m_petNameToken;
    /* 000C */ DBCString m_name;
    /* 0010 */ DBCString m_nameFemale;
    /* 0014 */ DBCString m_nameMale;
    /* 0018 */ DBCString m_filename;
    /* 001C */ int32_t m_spellClassSet;
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ int32_t m_cinematicSequenceID;
    /* 0028 */ int32_t m_requiredExpansion;
    /* 002C */ int32_t unk_002C;
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ int32_t unk_0034;
};

#endif
