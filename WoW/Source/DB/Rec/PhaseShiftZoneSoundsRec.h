#ifndef WOW_SOURCE_DB_REC_PHASESHIFTZONESOUNDSREC_H
#define WOW_SOURCE_DB_REC_PHASESHIFTZONESOUNDSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class PhaseShiftZoneSoundsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_areaID;
    /* 0008 */ int32_t m_wmoAreaID;
    /* 000C */ int32_t m_phaseID;
    /* 0010 */ int32_t m_phaseGroupID;
    /* 0014 */ int32_t m_phaseUseFlags;
    /* 0018 */ int32_t m_zoneIntroMusicID;
    /* 001C */ int32_t m_zoneMusicID;
    /* 0020 */ int32_t m_soundAmbienceID;
    /* 0024 */ int32_t m_soundProviderPreferencesID;
    /* 0028 */ int32_t m_uwZoneIntroMusicID;
    /* 002C */ int32_t m_uwZoneMusicID;
    /* 0030 */ int32_t m_uwSoundAmbienceID;
    /* 0034 */ int32_t m_uwSoundProviderPreferencesID;
};

#endif
