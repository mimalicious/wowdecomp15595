#ifndef WOW_SOURCE_DB_REC_AREATRIGGERREC_H
#define WOW_SOURCE_DB_REC_AREATRIGGERREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class AreaTriggerRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_continentID;
    /* 0008 */ float m_pos [3];
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ int32_t unk_0018;
    /* 001C */ int32_t unk_001C;
    /* 0020 */ float m_radius;
    /* 0024 */ float m_boxLength;
    /* 0028 */ float m_boxWidth;
    /* 002C */ float m_boxHeight;
    /* 0030 */ float m_boxYaw;

    static void FixupStrings(void* record, const char* strings);
};

#endif
