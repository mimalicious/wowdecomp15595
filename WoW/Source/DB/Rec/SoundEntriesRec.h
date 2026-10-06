#ifndef WOW_SOURCE_DB_REC_SOUNDENTRIESREC_H
#define WOW_SOURCE_DB_REC_SOUNDENTRIESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SoundEntriesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundType;
    /* 0008 */ const char* m_name;
    /* 000C */ const char* m_file [10];
    /* 0034 */ int32_t m_freq [10];
    /* 005C */ const char* m_directoryBase;
    /* 0060 */ float m_volumeFloat;
    /* 0064 */ int32_t m_flags;
    /* 0068 */ float m_minDistance;
    /* 006C */ float m_distanceCutoff;
    /* 0070 */ int32_t m_eaxdef;
    /* 0074 */ int32_t m_soundEntriesAdvancedID;
    /* 0078 */ float m_volumeVariationPlus;
    /* 007C */ float m_volumeVariationMinus;
    /* 0080 */ float m_pitchVariationPlus;
    /* 0084 */ float m_pitchVariationMinus;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(SoundEntriesRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
