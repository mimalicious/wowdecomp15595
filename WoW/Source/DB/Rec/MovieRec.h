#ifndef WOW_SOURCE_DB_REC_MOVIEREC_H
#define WOW_SOURCE_DB_REC_MOVIEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class MovieRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_filename;
    /* 0008 */ int32_t m_volume;
    /* 000C */ int32_t m_keyID;

    static void FixupStrings(void* record, const char* strings);
};

#endif
