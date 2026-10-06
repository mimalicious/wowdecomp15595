#ifndef WOW_SOURCE_DB_REC_ZONEMUSICREC_H
#define WOW_SOURCE_DB_REC_ZONEMUSICREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ZoneMusicRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_setName;
    /* 0008 */ int32_t m_silenceIntervalMin [2];
    /* 0010 */ int32_t m_silenceIntervalMax [2];
    /* 0018 */ int32_t m_sounds [2];
};

#endif
