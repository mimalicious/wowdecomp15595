#ifndef WOW_SOURCE_DB_REC_EMOTESTEXTREC_H
#define WOW_SOURCE_DB_REC_EMOTESTEXTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class EmotesTextRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ int32_t m_emoteID;
    /* 000C */ int32_t m_emoteText [16];

    static void FixupStrings(void* record, const char* strings);
};

#endif
