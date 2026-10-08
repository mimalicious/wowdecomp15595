#ifndef WOW_SOURCE_DB_REC_SOUNDEMITTERPILLPOINTSREC_H
#define WOW_SOURCE_DB_REC_SOUNDEMITTERPILLPOINTSREC_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/DB/WowClientDB.h"

class SoundEmitterPillPointsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundEmittersID;
    /* 0008 */ C3Vector m_position;
};

#endif
