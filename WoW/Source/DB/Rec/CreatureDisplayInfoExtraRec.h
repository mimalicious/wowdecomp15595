#ifndef WOW_SOURCE_DB_REC_CREATUREDISPLAYINFOEXTRAREC_H
#define WOW_SOURCE_DB_REC_CREATUREDISPLAYINFOEXTRAREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CreatureDisplayInfoExtraRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_displayRaceID;
    /* 0008 */ int32_t m_displaySexID;
    /* 000C */ int32_t m_skinID;
    /* 0010 */ int32_t m_faceID;
    /* 0014 */ int32_t m_hairStyleID;
    /* 0018 */ int32_t m_hairColorID;
    /* 001C */ int32_t m_facialHairID;
    /* 0020 */ int32_t m_npcitemDisplay [11];
    /* 004C */ int32_t m_flags;
    /* 0050 */ DBCString m_bakeName;
};

#endif
