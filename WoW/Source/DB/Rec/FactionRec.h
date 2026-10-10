#ifndef WOW_SOURCE_DB_REC_FACTIONREC_H
#define WOW_SOURCE_DB_REC_FACTIONREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class FactionRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_reputationIndex;
    /* 0008 */ int32_t m_reputationRaceMask [4];
    /* 0018 */ int32_t m_reputationClassMask [4];
    /* 0028 */ int32_t m_reputationBase [4];
    /* 0038 */ int32_t m_reputationFlags [4];
    /* 0048 */ int32_t m_parentFactionID;
    /* 004C */ float m_parentFactionMod [2];
    /* 0054 */ int32_t m_parentFactionCap [2];
    /* 005C */ DBCString m_name;
    /* 0060 */ DBCString m_description;
    /* 0064 */ int32_t m_sortOrder;
};

#endif
