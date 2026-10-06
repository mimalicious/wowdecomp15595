#ifndef WOW_SOURCE_DB_REC_RESEARCHFIELDREC_H
#define WOW_SOURCE_DB_REC_RESEARCHFIELDREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ResearchFieldRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString unk_0004;
    /* 0008 */ int32_t unk_0008;
};

#endif
