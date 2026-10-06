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
    /* 0024 */ DBCString m_name;
    /* 0028 */ int32_t unk_0028;

    static void FixupStrings(void* record, const char* strings);
};

#endif
