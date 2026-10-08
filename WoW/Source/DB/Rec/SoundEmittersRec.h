#ifndef WOW_SOURCE_DB_REC_SOUNDEMITTERSREC_H
#define WOW_SOURCE_DB_REC_SOUNDEMITTERSREC_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/DB/WowClientDB.h"

class SoundEmittersRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ C3Vector m_position;
    /* 0010 */ C3Vector m_direction;
    /* 001C */ int32_t m_soundEntriesID;
    /* 0020 */ int32_t m_mapID;
    /* 0024 */ DBCString m_name;
    /* 0028 */ int32_t m_emitterType;
};

#endif
