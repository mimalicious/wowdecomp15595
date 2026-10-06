#ifndef WOW_SOURCE_DB_REC_SPELLVISUALEFFECTNAMEREC_H
#define WOW_SOURCE_DB_REC_SPELLVISUALEFFECTNAMEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellVisualEffectNameRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ DBCString m_fileName;
    /* 000C */ float m_areaEffectSize;
    /* 0010 */ float m_scale;
    /* 0014 */ float m_minAllowedScale;
    /* 0018 */ float m_maxAllowedScale;
    /* 001C */ int32_t unk_001C;
    /* 0020 */ float unk_0020;
};

#endif
