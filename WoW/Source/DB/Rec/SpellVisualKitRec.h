#ifndef WOW_SOURCE_DB_REC_SPELLVISUALKITREC_H
#define WOW_SOURCE_DB_REC_SPELLVISUALKITREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellVisualKitRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_startAnimID;
    /* 0008 */ int32_t m_animID;
    /* 000C */ int32_t m_animKitID;
    /* 0010 */ int32_t m_headEffect;
    /* 0014 */ int32_t m_chestEffect;
    /* 0018 */ int32_t m_baseEffect;
    /* 001C */ int32_t m_leftHandEffect;
    /* 0020 */ int32_t m_rightHandEffect;
    /* 0024 */ int32_t m_breathEffect;
    /* 0028 */ int32_t m_leftWeaponEffect;
    /* 002C */ int32_t m_rightWeaponEffect;
    /* 0030 */ int32_t m_specialEffect [3];
    /* 003C */ int32_t m_worldEffect;
    /* 0040 */ int32_t m_soundID;
    /* 0044 */ int32_t m_shakeID;
    /* 0048 */ int32_t m_charProc [4];
    /* 0058 */ float m_charParamZero [4];
    /* 0068 */ float m_charParamOne [4];
    /* 0078 */ float m_charParamTwo [4];
    /* 0088 */ float m_charParamThree [4];
    /* 0098 */ int32_t m_flags;
};

#endif
