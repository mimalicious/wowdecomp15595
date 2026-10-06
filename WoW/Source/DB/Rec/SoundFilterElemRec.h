#ifndef WOW_SOURCE_DB_REC_SOUNDFILTERELEMREC_H
#define WOW_SOURCE_DB_REC_SOUNDFILTERELEMREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SoundFilterElemRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundFilterID;
    /* 0008 */ int32_t m_orderIndex;
    /* 000C */ int32_t m_filterType;
    /* 0010 */ float m_params [9];

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(SoundFilterElemRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
