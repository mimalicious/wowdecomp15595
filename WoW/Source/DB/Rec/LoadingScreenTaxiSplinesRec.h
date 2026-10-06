#ifndef WOW_SOURCE_DB_REC_LOADINGSCREENTAXISPLINESREC_H
#define WOW_SOURCE_DB_REC_LOADINGSCREENTAXISPLINESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LoadingScreenTaxiSplinesRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_pathID;
    /* 0008 */ float m_locx [8];
    /* 0028 */ float m_locy [8];
    /* 0048 */ int32_t m_legIndex;
};

#endif
