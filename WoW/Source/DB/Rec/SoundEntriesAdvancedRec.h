#ifndef WOW_SOURCE_DB_REC_SOUNDENTRIESADVANCEDREC_H
#define WOW_SOURCE_DB_REC_SOUNDENTRIESADVANCEDREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SoundEntriesAdvancedRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundEntryID;
    /* 0008 */ float m_innerRadius2d;
    /* 000C */ int32_t m_timeA;
    /* 0010 */ int32_t m_timeB;
    /* 0014 */ int32_t m_timeC;
    /* 0018 */ int32_t m_timeD;
    /* 001C */ int32_t m_randomOffsetRange;
    /* 0020 */ int32_t m_usage;
    /* 0024 */ int32_t m_timeIntervalMin;
    /* 0028 */ int32_t m_timeIntervalMax;
    /* 002C */ int32_t m_volumeSliderCategory;
    /* 0030 */ float m_duckToSfx;
    /* 0034 */ float m_duckToMusic;
    /* 0038 */ float m_duckToAmbience;
    /* 003C */ float m_innerRadiusOfInfluence;
    /* 0040 */ float m_outerRadiusOfInfluence;
    /* 0044 */ int32_t m_timeToDuck;
    /* 0048 */ int32_t m_timeToUnduck;
    /* 004C */ float m_insideAngle;
    /* 0050 */ float m_outsideAngle;
    /* 0054 */ float m_outsideVolume;
    /* 0058 */ float m_outerRadius2d;
    /* 005C */ int32_t unk_005C;
    /* 0060 */ int32_t unk_0060;
    /* 0064 */ float unk_0064;
};

#endif
