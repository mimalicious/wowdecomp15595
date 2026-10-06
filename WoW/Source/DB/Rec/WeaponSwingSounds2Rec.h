#ifndef WOW_SOURCE_DB_REC_WEAPONSWINGSOUNDS2REC_H
#define WOW_SOURCE_DB_REC_WEAPONSWINGSOUNDS2REC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class WeaponSwingSounds2Rec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_swingType;
    /* 0008 */ int32_t m_crit;
    /* 000C */ int32_t m_soundID;

    static void FixupStrings(void* record, const char* strings);
};

#endif
