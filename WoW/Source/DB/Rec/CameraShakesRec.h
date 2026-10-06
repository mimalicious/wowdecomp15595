#ifndef WOW_SOURCE_DB_REC_CAMERASHAKESREC_H
#define WOW_SOURCE_DB_REC_CAMERASHAKESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CameraShakesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_shakeType;
    /* 0008 */ int32_t m_direction;
    /* 000C */ float m_amplitude;
    /* 0010 */ float m_frequency;
    /* 0014 */ float m_duration;
    /* 0018 */ float m_phase;
    /* 001C */ float m_coefficient;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(CameraShakesRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
