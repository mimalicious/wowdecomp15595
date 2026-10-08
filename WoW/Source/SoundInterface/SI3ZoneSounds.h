#ifndef WOW_SOURCE_SOUNDINTERFACE_SI3ZONESOUNDS_H
#define WOW_SOURCE_SOUNDINTERFACE_SI3ZONESOUNDS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/SoundInterface/SI3.h"

class CPhaseShift;
class SE3SoundKitProperties;

struct SI3ZoneSoundInfo {
    /* 0000 */ int32_t m_soundKitID [2];
    /* 0008 */ int32_t m_silenceIntervalMin [2];
    /* 0010 */ int32_t m_silenceIntervalMax [2];
    /* 0018 */ int32_t m_soundAmbienceID;
    /* 001C */ int32_t m_ambienceKitID [2];
    /* 0024 */ int32_t m_reverb;
};

class CZoneSoundManager {
    public:
    /* 0000 */ int32_t m_type;
    /* 0004 */ int32_t m_soundKitID;
    /* 0008 */ uint32_t m_handle;
    /* 000C */ int32_t unk_000C;
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;

    int32_t Play(int32_t soundKitID, int32_t type, SE3SoundKitProperties* properties);
    void Stop();
};

class AmbFlavor : public TSLinkedNode<AmbFlavor> {
    public:
    /* 0008 */ int32_t m_nightSoundKitID;
    /* 000C */ int32_t m_daySoundKitID;
};

class AmbFlavorListHead {
    public:
    /* 0000 */ TSList<AmbFlavor, TSGetLink<AmbFlavor> > m_flavors;
};

class PhasedZoneSoundOverride {
    public:
    /* 0000 */ int32_t m_areaID;
    /* 0004 */ int32_t m_wmoAreaID;
    /* 0008 */ CPhaseShift* m_phaseShift;
    /* 000C */ int32_t m_zoneIntroMusicID;
    /* 0010 */ int32_t m_zoneMusicID;
    /* 0014 */ int32_t m_soundAmbienceID;
    /* 0018 */ int32_t m_soundProviderPreferencesID;
    /* 001C */ int32_t m_uwZoneIntroMusicID;
    /* 0020 */ int32_t m_uwZoneMusicID;
    /* 0024 */ int32_t m_uwSoundAmbienceID;
    /* 0028 */ int32_t m_uwSoundProviderPreferencesID;
};

class WorldStateZoneSoundOverride {
    public:
    /* 0000 */ int32_t m_areaID;
    /* 0004 */ int32_t m_wmoAreaID;
    /* 0008 */ int32_t m_worldStateID;
    /* 000C */ int32_t m_worldStateValue;
    /* 0010 */ int32_t m_zoneIntroMusicID;
    /* 0014 */ int32_t m_zoneMusicID;
    /* 0018 */ int32_t m_soundAmbienceID;
    /* 001C */ int32_t m_soundProviderPreferencesID;
};

#endif
