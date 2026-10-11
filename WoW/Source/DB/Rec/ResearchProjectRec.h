#ifndef WOW_SOURCE_DB_REC_RESEARCHPROJECTREC_H
#define WOW_SOURCE_DB_REC_RESEARCHPROJECTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ResearchProjectRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ DBCString m_description;
    /* 000C */ int32_t m_rarity;
    /* 0010 */ int32_t m_researchBranchID;
    /* 0014 */ int32_t m_spellID;
    /* 0018 */ int32_t m_numSockets;
    /* 001C */ DBCString m_texture;
    /* 0020 */ int32_t m_requiredWeight;
};

#endif
