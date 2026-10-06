#ifndef WOW_SOURCE_DB_REC_LOADINGSCREENTAXISPLINESREC_H
#define WOW_SOURCE_DB_REC_LOADINGSCREENTAXISPLINESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LoadingScreenTaxiSplinesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_pathID;
    /* 0008 */ float m_locx [8];
    /* 0028 */ float m_locy [8];
    /* 0048 */ int32_t m_legIndex;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(LoadingScreenTaxiSplinesRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
