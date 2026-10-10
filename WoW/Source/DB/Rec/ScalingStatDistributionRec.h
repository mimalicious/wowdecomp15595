#ifndef WOW_SOURCE_DB_REC_SCALINGSTATDISTRIBUTIONREC_H
#define WOW_SOURCE_DB_REC_SCALINGSTATDISTRIBUTIONREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ScalingStatDistributionRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_statID [10];
    /* 002C */ int32_t m_bonus [10];
    /* 0054 */ int32_t m_minlevel;
    /* 0058 */ int32_t m_maxlevel;
};

#endif
