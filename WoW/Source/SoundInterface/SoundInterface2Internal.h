#ifndef WOW_SOURCE_SOUNDINTERFACE_SOUNDINTERFACE2INTERNAL_H
#define WOW_SOURCE_SOUNDINTERFACE_SOUNDINTERFACE2INTERNAL_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

struct ZONESOUNDOVERRIDELOOKUP : public TSHashObject<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI> {
    /* 0018 */ int32_t m_zoneIntroMusicId;
    /* 001C */ int32_t m_zoneMusicId;
    /* 0020 */ int32_t m_soundAmbienceId;
    /* 0024 */ int32_t m_soundProviderPreferencesId;
};

class WorldStateZoneSoundOverride {
    public:
    /* 0000 */ int32_t m_areaId;
    /* 0004 */ int32_t m_wmoAreaId;
    /* 0008 */ int32_t m_worldStateId;
    /* 000C */ int32_t m_worldStateValue;
    /* 0010 */ int32_t m_zoneIntroMusicId;
    /* 0014 */ int32_t m_zoneMusicId;
    /* 0018 */ int32_t m_soundAmbienceId;
    /* 001C */ int32_t m_soundProviderPreferencesId;
};

#endif
