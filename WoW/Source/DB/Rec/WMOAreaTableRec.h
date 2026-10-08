#ifndef WOW_SOURCE_DB_REC_WMOAREATABLEREC_H
#define WOW_SOURCE_DB_REC_WMOAREATABLEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class WMOAreaTableRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_wmoid;
    /* 0008 */ int32_t m_nameSetID;
    /* 000C */ int32_t m_wmogroupID;
    /* 0010 */ int32_t m_soundProviderPref;
    /* 0014 */ int32_t m_soundProviderPrefUnderwater;
    /* 0018 */ int32_t m_ambienceID;
    /* 001C */ int32_t m_zoneMusic;
    /* 0020 */ int32_t m_introSound;
    /* 0024 */ int32_t m_flags;
    /* 0028 */ int32_t m_areaTableID;
    /* 002C */ DBCString m_areaName;
    /* 0030 */ int32_t m_uwIntroSound;
    /* 0034 */ int32_t m_uwZoneMusic;
    /* 0038 */ int32_t m_uwAmbienceID;
};

#endif
