#ifndef WOW_SOURCE_DB_REC_LFGDUNGEONSREC_H
#define WOW_SOURCE_DB_REC_LFGDUNGEONSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LfgDungeonsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);
    int32_t GetGroupSize() const;
    bool IsHeroic() const;
    bool IsRaid() const;
    bool IsInRandomGroup(int32_t randomID) const;
    bool IsInRandomGroup(int32_t randomID, int32_t expansionLevel) const;
    int32_t GetTopGroupID() const;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
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
    /* 0030 */ DBCString m_textureFilename;
    /* 0034 */ int32_t m_expansionLevel;
    /* 0038 */ int32_t m_orderIndex;
    /* 003C */ int32_t m_groupID;
    /* 0040 */ DBCString m_description;
    /* 0044 */ int32_t m_randomID;
    /* 0048 */ int32_t m_countTank;
    /* 004C */ int32_t m_countHealer;
    /* 0050 */ int32_t m_countDamage;
};

#endif
