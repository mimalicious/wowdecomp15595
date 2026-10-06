#ifndef WOW_SOURCE_DB_REC_LFGDUNGEONSREC_H
#define WOW_SOURCE_DB_REC_LFGDUNGEONSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LfgDungeonsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_minLevel;
    /* 000C */ int32_t m_maxLevel;
    /* 0010 */ int32_t m_targetLevel;
    /* 0014 */ int32_t m_targetLevelMin;
    /* 0018 */ int32_t m_targetLevelMax;
    /* 001C */ int32_t m_mapID;
    /* 0020 */ int32_t m_difficulty;
    /* 0024 */ int32_t m_flags;
    /* 0028 */ int32_t m_typeID;
    /* 002C */ int32_t m_faction;
    /* 0030 */ const char* m_textureFilename;
    /* 0034 */ int32_t m_expansionLevel;
    /* 0038 */ int32_t m_orderIndex;
    /* 003C */ int32_t m_groupID;
    /* 0040 */ const char* m_description;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(LfgDungeonsRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
