#ifndef WOW_SOURCE_DB_REC_RANDPROPPOINTSREC_H
#define WOW_SOURCE_DB_REC_RANDPROPPOINTSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class RandPropPointsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_epic [5];
    /* 0018 */ int32_t m_superior [5];
    /* 002C */ int32_t m_good [5];
};

#endif
