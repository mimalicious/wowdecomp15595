#ifndef WOW_SOURCE_DB_REC_WORLDSTATEZONESOUNDSREC_H
#define WOW_SOURCE_DB_REC_WORLDSTATEZONESOUNDSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class WorldStateZoneSoundsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_worldStateID;
    /* 0008 */ int32_t m_worldStateValue;
    /* 000C */ int32_t m_areaID;
    /* 0010 */ int32_t m_wmoareaID;
    /* 0014 */ int32_t m_zoneIntroMusicID;
    /* 0018 */ int32_t m_zoneMusicID;
    /* 001C */ int32_t m_soundAmbienceID;
    /* 0020 */ int32_t m_soundProviderPreferencesID;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(WorldStateZoneSoundsRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
