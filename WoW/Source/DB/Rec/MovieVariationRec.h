#ifndef WOW_SOURCE_DB_REC_MOVIEVARIATIONREC_H
#define WOW_SOURCE_DB_REC_MOVIEVARIATIONREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class MovieVariationRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_movieID;
    /* 0008 */ int32_t m_fileDataID;

    static void FixupStrings(void* record, const char* strings);
};

#endif
