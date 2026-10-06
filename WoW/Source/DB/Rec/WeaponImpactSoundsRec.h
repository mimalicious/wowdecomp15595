#ifndef WOW_SOURCE_DB_REC_WEAPONIMPACTSOUNDSREC_H
#define WOW_SOURCE_DB_REC_WEAPONIMPACTSOUNDSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class WeaponImpactSoundsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_weaponSubClassID;
    /* 0008 */ int32_t m_parrySoundType;
    /* 000C */ int32_t m_impactSoundID [10];
    /* 0034 */ int32_t m_critImpactSoundID [10];

    static void FixupStrings(void* record, const char* strings);
};

#endif
