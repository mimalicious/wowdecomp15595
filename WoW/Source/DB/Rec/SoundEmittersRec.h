#ifndef WOW_SOURCE_DB_REC_SOUNDEMITTERSREC_H
#define WOW_SOURCE_DB_REC_SOUNDEMITTERSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SoundEmittersRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_position [3];
    /* 0010 */ float m_direction [3];
    /* 001C */ int32_t m_soundEntryAdvancedID;
    /* 0020 */ int32_t m_mapID;
    /* 0024 */ const char* m_name;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(SoundEmittersRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
