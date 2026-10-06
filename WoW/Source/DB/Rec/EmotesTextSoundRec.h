#ifndef WOW_SOURCE_DB_REC_EMOTESTEXTSOUNDREC_H
#define WOW_SOURCE_DB_REC_EMOTESTEXTSOUNDREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class EmotesTextSoundRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_emotesTextID;
    /* 0008 */ int32_t m_raceID;
    /* 000C */ int32_t m_sexID;
    /* 0010 */ int32_t m_soundID;

    static void FixupStrings(void* record, const char* strings);
};

#endif
