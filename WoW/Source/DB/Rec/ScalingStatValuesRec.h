#ifndef WOW_SOURCE_DB_REC_SCALINGSTATVALUESREC_H
#define WOW_SOURCE_DB_REC_SCALINGSTATVALUESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ScalingStatValuesRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_level;
    /* 0008 */ int32_t m_dpsMod [6];
    /* 0020 */ int32_t m_spellPower;
    /* 0024 */ int32_t m_statMultiplier [5];
    /* 0038 */ int32_t m_armorShoulder [4];
    /* 0048 */ int32_t m_armorChest [4];
    /* 0058 */ int32_t m_armorHead [4];
    /* 0068 */ int32_t m_armorLegs [4];
    /* 0078 */ int32_t m_armorFeet [4];
    /* 0088 */ int32_t m_armorWaist [4];
    /* 0098 */ int32_t m_armorHands [4];
    /* 00A8 */ int32_t m_armorWrist [4];
    /* 00B8 */ int32_t m_armorCloak;
};

#endif
