#ifndef WOW_SOURCE_DB_REC_RESEARCHPROJECTREC_H
#define WOW_SOURCE_DB_REC_RESEARCHPROJECTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ResearchProjectRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString unk_0004;
    /* 0008 */ DBCString unk_0008;
    /* 000C */ int32_t unk_000C;
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ int32_t unk_0018;
    /* 001C */ DBCString unk_001C;
    /* 0020 */ int32_t unk_0020;
};

#endif
