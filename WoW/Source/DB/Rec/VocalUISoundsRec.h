#ifndef WOW_SOURCE_DB_REC_VOCALUISOUNDSREC_H
#define WOW_SOURCE_DB_REC_VOCALUISOUNDSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class VocalUISoundsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_vocalUienum;
    /* 0008 */ int32_t m_raceID;
    /* 000C */ int32_t m_normalSoundID [2];
    /* 0014 */ int32_t m_pissedSoundID [2];

    static void FixupStrings(void* record, const char* strings);
};

#endif
