#ifndef WOW_SOURCE_DB_REC_SOUNDENTRIESREC_H
#define WOW_SOURCE_DB_REC_SOUNDENTRIESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SoundEntriesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundType;
    /* 0008 */ DBCString m_name;
    /* 000C */ DBCString m_file [10];
    /* 0034 */ int32_t m_freq [10];
    /* 005C */ DBCString m_directoryBase;
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
    /* 0088 */ int32_t unk_0088;

    static void FixupStrings(void* record, const char* strings);
};

#endif
