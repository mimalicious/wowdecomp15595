#ifndef WOW_SOURCE_DB_REC_MATERIALREC_H
#define WOW_SOURCE_DB_REC_MATERIALREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class MaterialRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ int32_t m_foleySoundID;
    /* 000C */ int32_t m_sheatheSoundID;
    /* 0010 */ int32_t m_unsheatheSoundID;
};

#endif
