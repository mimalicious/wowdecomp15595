#ifndef WOW_SOURCE_DB_REC_GTSPELLSCALINGREC_H
#define WOW_SOURCE_DB_REC_GTSPELLSCALINGREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class gtSpellScalingRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ float unk_0004;

    static void FixupStrings(void* record, const char* strings);
};

#endif
