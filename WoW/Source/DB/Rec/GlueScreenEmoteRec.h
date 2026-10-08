#ifndef WOW_SOURCE_DB_REC_GLUESCREENEMOTEREC_H
#define WOW_SOURCE_DB_REC_GLUESCREENEMOTEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class GlueScreenEmoteRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_classID;
    /* 0008 */ int32_t m_raceID;
    /* 000C */ int32_t m_screen;
    /* 0010 */ int32_t m_offHandItemType;
    /* 0014 */ int32_t m_mainHandItemType;
    /* 0018 */ int32_t m_animKitID;
    /* 001C */ int32_t m_spellVisualKitID;
};

#endif
