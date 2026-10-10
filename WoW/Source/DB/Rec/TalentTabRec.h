#ifndef WOW_SOURCE_DB_REC_TALENTTABREC_H
#define WOW_SOURCE_DB_REC_TALENTTABREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TalentTabRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ int32_t m_spellIconID;
    /* 000C */ int32_t m_classMask;
    /* 0010 */ int32_t m_petTalentMask;
    /* 0014 */ int32_t m_orderIndex;
    /* 0018 */ DBCString m_backgroundFile;
    /* 001C */ DBCString unk_001C;
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ int32_t m_masterySpellID [2];
};

#endif
