#ifndef WOW_SOURCE_DB_REC_JOURNALENCOUNTERSECTIONREC_H
#define WOW_SOURCE_DB_REC_JOURNALENCOUNTERSECTIONREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class JournalEncounterSectionRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_journalEncounterID;
    /* 0008 */ int32_t m_nextSiblingSectionID;
    /* 000C */ int32_t m_firstChildSectionID;
    /* 0010 */ int32_t m_parentSectionID;
    /* 0014 */ int32_t m_orderIndex;
    /* 0018 */ int32_t m_type;
    /* 001C */ int32_t m_flags;
    /* 0020 */ int32_t m_iconFlags;
    /* 0024 */ DBCString m_title;
    /* 0028 */ DBCString m_bodyText;
    /* 002C */ int32_t m_difficultyMask;
    /* 0030 */ int32_t m_iconCreatureDisplayInfoID;
    /* 0034 */ int32_t m_iconFileDataID;
};

#endif
