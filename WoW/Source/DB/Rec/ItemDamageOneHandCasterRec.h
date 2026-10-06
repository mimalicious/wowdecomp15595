#ifndef WOW_SOURCE_DB_REC_ITEMDAMAGEONEHANDCASTERREC_H
#define WOW_SOURCE_DB_REC_ITEMDAMAGEONEHANDCASTERREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ItemDamageOneHandCasterRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ float unk_0004;
    /* 0008 */ float unk_0008;
    /* 000C */ float unk_000C;
    /* 0010 */ float unk_0010;
    /* 0014 */ float unk_0014;
    /* 0018 */ float unk_0018;
    /* 001C */ float unk_001C;
    /* 0020 */ int32_t unk_0020;

    static void FixupStrings(void* record, const char* strings);
};

#endif
