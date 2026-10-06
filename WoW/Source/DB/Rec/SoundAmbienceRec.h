#ifndef WOW_SOURCE_DB_REC_SOUNDAMBIENCEREC_H
#define WOW_SOURCE_DB_REC_SOUNDAMBIENCEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SoundAmbienceRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_ambienceID [2];
};

#endif
