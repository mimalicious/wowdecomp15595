#ifndef WOW_SOURCE_DB_REC_SPELLREC_H
#define WOW_SOURCE_DB_REC_SPELLREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_attributes;
    /* 0008 */ int32_t m_attributesEx;
    /* 000C */ int32_t m_attributesExB;
    /* 0010 */ int32_t m_attributesExC;
    /* 0014 */ int32_t m_attributesExD;
    /* 0018 */ int32_t m_attributesExE;
    /* 001C */ int32_t m_attributesExF;
    /* 0020 */ int32_t m_attributesExG;
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ float m_effectChainAmplitude2;
    /* 0030 */ int32_t m_castingTimeIndex;
    /* 0034 */ int32_t m_durationIndex;
    /* 0038 */ int32_t m_powerType;
    /* 003C */ int32_t m_rangeIndex;
    /* 0040 */ float m_speed;
    /* 0044 */ int32_t m_spellVisualID;
    /* 0048 */ float m_effectChainAmplitude1;
    /* 004C */ int32_t m_spellIconID;
    /* 0050 */ int32_t m_activeIconID;
    /* 0054 */ DBCString m_name;
    /* 0058 */ DBCString m_nameSubtext;
    /* 005C */ DBCString m_description;
    /* 0060 */ DBCString m_auraDescription;
    /* 0064 */ int32_t m_schoolMask;
    /* 0068 */ int32_t m_runeCostID;
    /* 006C */ int32_t m_spellMissileID;
    /* 0070 */ int32_t unk_0070;
    /* 0074 */ int32_t m_difficulty;
    /* 0078 */ float unk_0078;
    /* 007C */ int32_t unk_007C;
    /* 0080 */ int32_t unk_0080;
    /* 0084 */ int32_t unk_0084;
    /* 0088 */ int32_t unk_0088;
    /* 008C */ int32_t unk_008C;
    /* 0090 */ int32_t unk_0090;
    /* 0094 */ int32_t unk_0094;
    /* 0098 */ int32_t unk_0098;
    /* 009C */ int32_t unk_009C;
    /* 00A0 */ int32_t unk_00A0;
    /* 00A4 */ int32_t unk_00A4;
    /* 00A8 */ int32_t unk_00A8;
    /* 00AC */ int32_t unk_00AC;
    /* 00B0 */ int32_t unk_00B0;
    /* 00B4 */ int32_t unk_00B4;
    /* 00B8 */ int32_t unk_00B8;
    /* 00BC */ int32_t unk_00BC;

    static void FixupStrings(void* record, const char* strings);
};

#endif
