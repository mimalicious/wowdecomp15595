#ifndef WOW_SOURCE_DB_REC_AREATABLEREC_H
#define WOW_SOURCE_DB_REC_AREATABLEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class AreaTableRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_continentID;
    /* 0008 */ int32_t m_parentAreaID;
    /* 000C */ int32_t m_areaBit;
    /* 0010 */ int32_t m_flags;
    /* 0014 */ int32_t m_soundProviderPref;
    /* 0018 */ int32_t m_soundProviderPrefUnderwater;
    /* 001C */ int32_t m_ambienceID;
    /* 0020 */ int32_t m_zoneMusic;
    /* 0024 */ int32_t m_introSound;
    /* 0028 */ int32_t m_explorationLevel;
    /* 002C */ DBCString m_areaName;
    /* 0030 */ int32_t m_factionGroupMask;
    /* 0034 */ int32_t m_liquidTypeID [4];
    /* 0044 */ float m_minElevation;
    /* 0048 */ float m_ambientMultiplier;
    /* 004C */ int32_t unk_004C;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ int32_t unk_0054;
    /* 0058 */ int32_t unk_0058;
    /* 005C */ int32_t unk_005C;
    /* 0060 */ int32_t unk_0060;
    /* 0064 */ int32_t unk_0064;

    static void FixupStrings(void* record, const char* strings);
};

#endif
