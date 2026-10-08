#ifndef WOW_SOURCE_DB_REC_ANIMKITSEGMENTREC_H
#define WOW_SOURCE_DB_REC_ANIMKITSEGMENTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class AnimKitSegmentRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_parentAnimKitID;
    /* 0008 */ int32_t m_animID;
    /* 000C */ int32_t m_animStartTime;
    /* 0010 */ int32_t m_animKitConfigID;
    /* 0014 */ int32_t m_startCondition;
    /* 0018 */ int32_t m_startConditionParam;
    /* 001C */ int32_t m_startConditionDelay;
    /* 0020 */ int32_t m_endCondition;
    /* 0024 */ int32_t m_endConditionParam;
    /* 0028 */ int32_t m_endConditionDelay;
    /* 002C */ float m_speed;
    /* 0030 */ int32_t m_segmentFlags;
    /* 0034 */ int32_t m_forcedVariation;
    /* 0038 */ int32_t m_overrideConfigFlags;
    /* 003C */ int32_t m_loopToSegmentIndex;
};

#endif
