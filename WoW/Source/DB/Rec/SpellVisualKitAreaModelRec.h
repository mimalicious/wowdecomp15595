#ifndef WOW_SOURCE_DB_REC_SPELLVISUALKITAREAMODELREC_H
#define WOW_SOURCE_DB_REC_SPELLVISUALKITAREAMODELREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellVisualKitAreaModelRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ int32_t m_enumID;
    /* 000C */ int32_t unk_000C;
    /* 0010 */ float unk_0010;
    /* 0014 */ float unk_0014;
    /* 0018 */ float unk_0018;
};

#endif
