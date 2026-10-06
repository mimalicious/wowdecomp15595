#ifndef WOW_SOURCE_DB_REC_SPELLVISUALEFFECTNAMEREC_H
#define WOW_SOURCE_DB_REC_SPELLVISUALEFFECTNAMEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellVisualEffectNameRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ const char* m_fileName;
    /* 000C */ float m_areaEffectSize;
    /* 0010 */ float m_scale;
    /* 0014 */ float m_minAllowedScale;
    /* 0018 */ float m_maxAllowedScale;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(SpellVisualEffectNameRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
