#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "WoW/Source/SoundInterface/SI3ZoneSounds.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Game/GameTime.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Game/GameClient/PhaseShift_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00CC8CFC
static int32_t s_soundEmitterMapID = -1;

// STATIC: 0x00CC8D00
int32_t SI3::sm_eDayOrNight = 1;

// STATIC: 0x00CC8D14
CZoneSoundManager SI3::sm_ZoneMusic = { SI3ZONESOUNDTYPE_NONE, 0, 0, SI3ZONESOUNDTYPE_NONE, 0, 0 };

// STATIC: 0x00CC8D2C
CZoneSoundManager SI3::sm_ZoneAmbience = { SI3ZONESOUNDTYPE_NONE, 0, 0, SI3ZONESOUNDTYPE_NONE, 0, 0 };

// STATIC: 0x00CC8E28
static int32_t s_phaseShiftWmoGroupAreaID = -1;

// STATIC: 0x00CC8E2C
static int32_t s_phaseShiftWmoAreaID = -1;

// STATIC: 0x00CC8E30
static int32_t s_phaseShiftZoneID = -1;

// STATIC: 0x00CC8E34
static int32_t s_phaseShiftAreaID = -1;

// STATIC: 0x00CC8E38
static int32_t s_overrideAreaY = -1;

// STATIC: 0x00CC8E3C
static int32_t s_overrideMapID = -1;

// STATIC: 0x00CC8E40
static int32_t s_overrideAreaX = -1;

// STATIC: 0x00CC8E44
static int32_t s_overrideChunkY = -1;

// STATIC: 0x00CC8E48
static int32_t s_overrideChunkX = -1;

// STATIC: 0x00E28398
int32_t SI3::sm_bCameraIsUnderwater;

// STATIC: 0x00E2839C
int32_t SI3::sm_bCameraIsUnderwater_ChangedThisFrame;

// STATIC: 0x00E283A4
int32_t SI3::sm_bPlayerIsIndoors;

// STATIC: 0x00E283A8
uint32_t SI3::sm_uNextZoneMusicPlayTime;

// STATIC: 0x00E283AC
uint32_t SI3::sm_nSilenceIntervalMin;

// STATIC: 0x00E283B0
uint32_t SI3::sm_nSilenceIntervalMax;

// STATIC: 0x00E283EC
TSGrowableArray<PhasedZoneSoundOverride*> SI3::sm_PhaseShiftZoneSoundOverridesArray;

// STATIC: 0x00E283FC
TSGrowableArray<WorldStateZoneSoundOverride*> SI3::sm_WorldStateZoneSoundOverridesArray;

// STATIC: 0x00E2840C
TSHashTable<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI> SI3::sm_ZoneSoundOverridesHashTable;

// STATIC: 0x00E28438
SI3ZoneSoundInfo SI3::sm_aZoneSoundInfo[SI3ZONESOUNDTYPE_LAST];

// STATIC: 0x00E286B8
TSGrowableArray<uint32_t> SI3::sm_aIntroMusicNextValidPlayTime;

// STATIC: 0x00E286DC
static uint32_t s_ambienceFlavorSounds[10];

// STATIC: 0x00E28704
static int32_t s_ambienceFlavorSoundAmbienceID;

// STATIC: 0x00E28708
static uint32_t s_luaMusic;

// STATIC: 0x00E2870C
static TSGrowableArray<AmbFlavorListHead> s_ambienceFlavors;

// STATIC: 0x00E28728
static int32_t s_phaseShiftInside;

// PROVEN: 8e3b4799 IDENTICAL/NONE
// OFFSET: 0x0062CD10/0x1402C0710
bool SI3::IsIntroZoneSoundType(int32_t type) {
    return type == SI3ZONESOUNDTYPE_4 || type == SI3ZONESOUNDTYPE_6 || type == SI3ZONESOUNDTYPE_5 || type == SI3ZONESOUNDTYPE_7 || type == SI3ZONESOUNDTYPE_11 || type == SI3ZONESOUNDTYPE_12;
}

// PROVEN: 57437432 IDENTICAL/NONE
// OFFSET: 0x0062CD40
bool SI3::IsUnderwaterZoneSoundType(int32_t type) {
    return type == SI3ZONESOUNDTYPE_9 || type == SI3ZONESOUNDTYPE_10 || type == SI3ZONESOUNDTYPE_11 || type == SI3ZONESOUNDTYPE_12;
}

// PROVEN: 159a1522 IDENTICAL/NONE
// OFFSET: 0x0062CD70
bool SI3::IsZoneMusicZoneSoundType(uint32_t type) {
    return type == SI3ZONESOUNDTYPE_0 || type == SI3ZONESOUNDTYPE_1 || type == SI3ZONESOUNDTYPE_2 || type == SI3ZONESOUNDTYPE_3;
}

// PROVEN: 205c03cd IDENTICAL/NONE
// OFFSET: 0x0062CDA0/0x1402C0760
void SI3::SetWeatherKitID(int32_t soundKitID) {
    SI3::sm_aZoneSoundInfo[SI3ZONESOUNDTYPE_8].m_ambienceKitID[0] = soundKitID;
    SI3::sm_aZoneSoundInfo[SI3ZONESOUNDTYPE_8].m_ambienceKitID[1] = soundKitID;
}

// PROVEN: 99da92b9 IDENTICAL/NONE
// OFFSET: 0x0062CDC0/0x1402C0770
static int32_t CompareSoundEmitters(const void* a, const void* b) {
    return static_cast<const SoundEmittersRec*>(a)->m_mapID - static_cast<const SoundEmittersRec*>(b)->m_mapID;
}

// PROVEN: 9f8f4130 IDENTICAL/NONE
// OFFSET: 0x0062CDE0/0x1402C0780
void SI3::SetPlayerInside(bool inside) {
    SI3::sm_bPlayerIsIndoors = inside;
}

// PROVEN: c7cb4d4d IDENTICAL/NONE
// OFFSET: 0x0062CDF0/0x1402C0790
void SI3::SetZoneReverb(int32_t type, int32_t reverb) {
    SI3::sm_aZoneSoundInfo[type].m_reverb = reverb;
    SI3::UpdateReverb();
}

// PROVEN: 1886a8c9 IDENTICAL/NONE
// OFFSET: 0x0062CE10/0x1402C07B0
void SI3::SetZoneUnderwaterReverb(int32_t reverb) {
    SI3::sm_aZoneSoundInfo[SI3ZONESOUNDTYPE_9].m_reverb = reverb;
    SI3::UpdateReverb();
}

// OFFSET: 0x0062CE30
int32_t CZoneSoundManager::Play(int32_t soundKitID, int32_t type, SE3SoundKitProperties* properties) {
    int32_t result = SE3::PlaySoundKit(soundKitID, NULL, properties, &this->m_handle, __FILE__, __LINE__, "ZoneSoundManager", 1, NULL);

    if (!result) {
        this->m_soundKitID = soundKitID;
        this->m_type = type;
    }

    return result;
}

// INLINED
void CZoneSoundManager::Stop() {
    SE3::StopSound(this->m_handle, -1, __FILE__, __LINE__, "CZoneSoundManager::Stop");
}

// PROVEN: 1da25354 IDENTICAL/NONE
// OFFSET: 0x0062CE80
void SI3::SetUnderwaterStatus(int32_t underwater) {
    SI3::sm_bCameraIsUnderwater_ChangedThisFrame = underwater != SI3::sm_bCameraIsUnderwater;
    SI3::sm_bCameraIsUnderwater = underwater;
    SI3::ReverbUnderwaterChanged();
}

// PROVEN: c44a512d IDENTICAL/NONE
// OFFSET: 0x0062CEB0/0x1402C07E0
int32_t SI3::IsUnderwater() {
    return SI3::sm_bCameraIsUnderwater;
}

// PROVEN: 44a230e4 IDENTICAL/NONE
// OFFSET: 0x0062CEC0/0x1402C07F0
void SI3::StopLuaMusic() {
    SE3::StopSound(s_luaMusic, 500, __FILE__, __LINE__, "SI3::StopLuaMusic");
}

// OFFSET: 0x0062D030/0x1402C0A10
void SI3::SetZoneIntroMusicID(int32_t type, int32_t zoneIntroMusicID) {
    ZoneIntroMusicTableRec* rec = g_zoneIntroMusicTableDB.GetRecord(zoneIntroMusicID);
    SI3ZoneSoundInfo& info = SI3::sm_aZoneSoundInfo[type];

    if (rec) {
        info.m_soundKitID[0] = rec->m_soundID;
        info.m_soundKitID[1] = rec->m_soundID;
        info.m_silenceIntervalMin[0] = rec->m_minDelayMinutes;
        info.m_silenceIntervalMin[1] = rec->m_minDelayMinutes;
    } else {
        info.m_soundKitID[0] = -1;
        info.m_soundKitID[1] = -1;
        info.m_silenceIntervalMin[0] = 0;
        info.m_silenceIntervalMin[1] = 0;
        info.m_silenceIntervalMax[0] = 0;
        info.m_silenceIntervalMax[1] = 0;
    }
}

// OFFSET: 0x0062D0C0/0x1402C0AA0
void SI3::SetZoneAmbienceID(int32_t type, int32_t soundAmbienceID) {
    SoundAmbienceRec* rec = g_soundAmbienceDB.GetRecord(soundAmbienceID);
    SI3ZoneSoundInfo& info = SI3::sm_aZoneSoundInfo[type];

    if (rec) {
        info.m_ambienceKitID[0] = rec->m_ambienceID[0];
        info.m_ambienceKitID[1] = rec->m_ambienceID[1];
        info.m_soundAmbienceID = soundAmbienceID;
    } else {
        info.m_ambienceKitID[0] = -1;
        info.m_ambienceKitID[1] = -1;
        info.m_soundAmbienceID = 0;
    }
}

// OFFSET: 0x0062D130/0x1402C0B20
static bool ZoneMusicDelayElapsed(uint32_t type) {
    return !SI3::IsZoneMusicZoneSoundType(type) || (SI3::sm_pCVar_Sound_ZoneMusicNoDelay && SI3::sm_pCVar_Sound_ZoneMusicNoDelay->m_intValue) || OsGetAsyncTimeMs() >= SI3::sm_uNextZoneMusicPlayTime;
}

// OFFSET: 0x0062D170/0x1402C0B70
bool SI3::IntroMusicOkToPlay(int32_t soundKitID, int32_t type) {
    if ((soundKitID == SI3::sm_ZoneMusic.m_soundKitID && SE3::IsPlaying(SI3::sm_ZoneMusic.m_handle)) || !SI3::IsIntroZoneSoundType(type)) {
        return true;
    }

    if (SI3::IsIntroZoneSoundType(SI3::sm_ZoneMusic.m_type) && SE3::IsPlaying(SI3::sm_ZoneMusic.m_handle)) {
        return false;
    }

    uint32_t now = OsGetAsyncTimeMs();

    if (static_cast<uint32_t>(soundKitID) >= SI3::sm_aIntroMusicNextValidPlayTime.Count()) {
        return false;
    }

    return now >= SI3::sm_aIntroMusicNextValidPlayTime[soundKitID];
}

// OFFSET: 0x0062D220/0x1402C0C30
int32_t SI3::GetDesiredMusicKitID(int32_t* type, int32_t* silenceMin, int32_t* silenceMax, int32_t* soundKitID) {
    if ((SI3::sm_pCVar_Sound_EnableAllSound && !SI3::sm_pCVar_Sound_EnableAllSound->m_intValue) || (SI3::sm_pCVar_Sound_EnableMusic && !SI3::sm_pCVar_Sound_EnableMusic->m_intValue) || CGGameUI::m_inMovie) {
        if (type) {
            *type = SI3ZONESOUNDTYPE_DISABLED;
        }

        if (silenceMin) {
            *silenceMin = 0;
        }

        if (silenceMax) {
            *silenceMax = 0;
        }

        if (soundKitID) {
            *soundKitID = -3333;
        }

        return -3333;
    }

    if (SE3::IsPlaying(s_luaMusic)) {
        if (soundKitID) {
            *soundKitID = -2222;
        }

        if (type) {
            *type = SI3ZONESOUNDTYPE_14;
        }

        return -2222;
    }

    int32_t current;

    for (current = SI3ZONESOUNDTYPE_LAST - 1; current >= 0; current--) {
        SI3ZoneSoundInfo& info = SI3::sm_aZoneSoundInfo[current];

        if (info.m_soundKitID[SI3::sm_eDayOrNight] < 0) {
            continue;
        }

        if (current == SI3ZONESOUNDTYPE_8 && SI3::sm_bPlayerIsIndoors) {
            continue;
        }

        if (SI3::IsUnderwaterZoneSoundType(current) && !SI3::sm_bCameraIsUnderwater) {
            continue;
        }

        if (SI3::IsIntroZoneSoundType(current)) {
            if (SE3::IsPlaying(SI3::sm_ZoneMusic.m_handle) && SI3::IsIntroZoneSoundType(SI3::sm_ZoneMusic.m_type)) {
                if (type) {
                    *type = SI3::sm_ZoneMusic.m_type;
                }

                if (silenceMin) {
                    *silenceMin = 0;
                }

                if (silenceMax) {
                    *silenceMax = 0;
                }

                if (soundKitID) {
                    *soundKitID = SI3::sm_ZoneMusic.m_soundKitID;
                }

                return SI3::sm_ZoneMusic.m_soundKitID;
            }

            if (!SI3::IntroMusicOkToPlay(info.m_soundKitID[SI3::sm_eDayOrNight], current)) {
                continue;
            }
        }

        break;
    }

    if (current < 0) {
        if (soundKitID) {
            *soundKitID = 0;
        }

        return 0;
    }

    SI3ZoneSoundInfo& info = SI3::sm_aZoneSoundInfo[current];

    if (current <= SI3ZONESOUNDTYPE_3 && !ZoneMusicDelayElapsed(current)) {
        if (type) {
            *type = SI3ZONESOUNDTYPE_DELAYED;
        }

        if (silenceMin) {
            *silenceMin = 0;
        }

        if (silenceMax) {
            *silenceMax = 0;
        }

        if (soundKitID) {
            *soundKitID = info.m_soundKitID[SI3::sm_eDayOrNight];
        }

        return -1111;
    }

    int32_t result = info.m_soundKitID[SI3::sm_eDayOrNight];

    if (type) {
        *type = current;
    }

    if (silenceMin) {
        *silenceMin = info.m_silenceIntervalMin[SI3::sm_eDayOrNight];
    }

    if (silenceMax) {
        *silenceMax = info.m_silenceIntervalMax[SI3::sm_eDayOrNight];
    }

    if (soundKitID) {
        *soundKitID = result;
    }

    return result;
}

// OFFSET: 0x0062D4C0
int32_t SI3::GetDesiredAmbienceKitID(int32_t* type) {
    if ((SI3::sm_pCVar_Sound_EnableAllSound && !SI3::sm_pCVar_Sound_EnableAllSound->m_intValue) || (SI3::sm_pCVar_Sound_EnableAmbience && !SI3::sm_pCVar_Sound_EnableAmbience->m_intValue) || CGGameUI::m_inMovie) {
        if (type) {
            *type = SI3ZONESOUNDTYPE_DISABLED;
        }

        return 0;
    }

    int32_t current = SI3ZONESOUNDTYPE_LAST - 1;

    while (SI3::sm_aZoneSoundInfo[current].m_ambienceKitID[SI3::sm_eDayOrNight] < 0 || (SI3::IsUnderwaterZoneSoundType(current) && !SI3::sm_bCameraIsUnderwater) || (current == SI3ZONESOUNDTYPE_8 && SI3::sm_bPlayerIsIndoors)) {
        if (--current < 0) {
            return 0;
        }
    }

    if (type) {
        *type = current;
    }

    return SI3::sm_aZoneSoundInfo[current].m_ambienceKitID[SI3::sm_eDayOrNight];
}

// OFFSET: 0x0062D580
void SI3::IntroMusicFinished(SE3SoundKitObject* sko) {
    uint32_t soundKitID = SI3::sm_ZoneMusic.m_soundKitID;
    uint32_t now = OsGetAsyncTimeMs();

    if (soundKitID < SI3::sm_aIntroMusicNextValidPlayTime.Count()) {
        SI3::sm_aIntroMusicNextValidPlayTime[soundKitID] = now + 60000 * SI3::sm_nSilenceIntervalMin;
    }
}

// OFFSET: 0x0062D5B0/0x1402C1020
void SI3::ZoneMusicFinished(SE3SoundKitObject* sko) {
    uint32_t now = OsGetAsyncTimeMs();

    if (SE3::IsPlaying(SI3::sm_ZoneMusic.m_handle)) {
        SI3::sm_uNextZoneMusicPlayTime = 0;
        return;
    }

    uint32_t delay = SI3::sm_nSilenceIntervalMax;

    if (SI3::sm_nSilenceIntervalMax > SI3::sm_nSilenceIntervalMin) {
        // STATIC: 0x00E2871C
        static CRndSeed s_seed(0);

        delay = SI3::sm_nSilenceIntervalMin + static_cast<uint32_t>((static_cast<uint64_t>(SI3::sm_nSilenceIntervalMax - SI3::sm_nSilenceIntervalMin) * CRandom::uint32_(s_seed)) >> 32);
    }

    SI3::sm_uNextZoneMusicPlayTime = now + delay;
}

// PROVEN: 670317cd IDENTICAL/NONE
// OFFSET: 0x0062D630/0x1402C10C0
void SI3::StopZoneAmbience() {
    SI3::sm_ZoneAmbience.Stop();
}

// OFFSET: 0x0062D650/0x1402C10F0
void SI3::UpdateSoundEmitters(int32_t mapID) {
    if (mapID == s_soundEmitterMapID) {
        return;
    }

    SE3::EmitterManagerDestroyAllEmitters();
    s_soundEmitterMapID = mapID;

    if (mapID == -1) {
        return;
    }

    SoundEmittersRec* records = g_soundEmittersDB.m_numRecords > 0 ? reinterpret_cast<SoundEmittersRec*>(g_soundEmittersDB.m_records) : NULL;
    int32_t count = g_soundEmittersDB.m_numRecords;

    SoundEmittersRec key;
    memset(&key, 0, sizeof(key));
    key.m_mapID = mapID;

    SoundEmittersRec* rec = static_cast<SoundEmittersRec*>(bsearch(&key, records, count, sizeof(SoundEmittersRec), CompareSoundEmitters));

    if (!rec) {
        return;
    }

    while (rec >= records && rec->m_mapID == mapID) {
        rec--;
    }

    for (rec++; rec && rec->m_mapID == mapID && rec < records + count; rec++) {
        SE3SoundKitProperties properties;
        properties.m_bit0 = 1;
        properties.m_maxAudibleBehavior = 1;
        properties.m_coneOrientation = rec->m_direction;
        properties.m_fadeInTime = 3000;
        properties.m_priority = 128;

        uint32_t handle;
        SE3::CreateSoundEmitter(&handle, rec, &properties);
    }
}

// OFFSET: 0x0062D7A0/0x1402C12B0
void SI3::SetZoneMusicID(int32_t type, int32_t zoneMusicID) {
    ZoneMusicRec* rec = g_zoneMusicDB.GetRecord(zoneMusicID);
    SI3ZoneSoundInfo& info = SI3::sm_aZoneSoundInfo[type];

    if (rec) {
        info.m_soundKitID[0] = rec->m_sounds[0];
        info.m_soundKitID[1] = rec->m_sounds[1];
        info.m_silenceIntervalMin[0] = rec->m_silenceIntervalMin[0];
        info.m_silenceIntervalMin[1] = rec->m_silenceIntervalMin[1];
        info.m_silenceIntervalMax[0] = rec->m_silenceIntervalMax[0];
        info.m_silenceIntervalMax[1] = rec->m_silenceIntervalMax[1];

        int32_t soundKitID = 0;
        SI3::GetDesiredMusicKitID(NULL, NULL, NULL, &soundKitID);

        if (soundKitID != SI3::sm_ZoneMusic.m_soundKitID) {
            SI3::sm_uNextZoneMusicPlayTime = 0;
        }
    } else {
        info.m_soundKitID[0] = -1;
        info.m_soundKitID[1] = -1;
        info.m_silenceIntervalMin[0] = 0;
        info.m_silenceIntervalMin[1] = 0;
        info.m_silenceIntervalMax[0] = 0;
        info.m_silenceIntervalMax[1] = 0;

        if (zoneMusicID > 0) {
            SI3::sm_uNextZoneMusicPlayTime = 0;
        }
    }
}

// PROVEN: 4342e8c1 IDENTICAL/NONE
// OFFSET: 0x0062D870
void SI3::SetScreenEffect(int32_t soundAmbienceID, int32_t zoneMusicID) {
    SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_15, soundAmbienceID);
    SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_15, zoneMusicID);
}

// OFFSET: 0x0062D890/0x1402C13E0
int32_t SI3::GetPhaseShiftZoneSoundOverride(int32_t areaID, int32_t zoneID, int32_t wmoAreaID, int32_t wmoGroupAreaID, int32_t inside) {
    uint32_t count = SI3::sm_PhaseShiftZoneSoundOverridesArray.Count();
    int32_t found = 0;

    if (areaID == -1 && zoneID == -1 && wmoAreaID == -1 && wmoGroupAreaID == -1) {
        wmoAreaID = s_phaseShiftWmoAreaID;
        areaID = s_phaseShiftAreaID;
        wmoGroupAreaID = s_phaseShiftWmoGroupAreaID;
        inside = s_phaseShiftInside;
        zoneID = s_phaseShiftZoneID;
    } else {
        s_phaseShiftAreaID = areaID;
        s_phaseShiftWmoGroupAreaID = wmoGroupAreaID;
        s_phaseShiftZoneID = zoneID;
        s_phaseShiftWmoAreaID = wmoAreaID;
        s_phaseShiftInside = inside;
    }

    if (areaID == -1 && zoneID == -1 && wmoAreaID == -1 && wmoGroupAreaID == -1) {
        return 0;
    }

    if (inside) {
        for (uint32_t i = 0; i < count; i++) {
            PhasedZoneSoundOverride* override = SI3::sm_PhaseShiftZoneSoundOverridesArray[i];

            if (!((wmoAreaID > 0 && wmoAreaID == override->m_wmoAreaID) || (wmoGroupAreaID > 0 && wmoGroupAreaID == override->m_wmoAreaID))) {
                continue;
            }

            if (!PhaseShift_C_HasSharedPhase(override->m_phaseShift)) {
                continue;
            }

            SI3::SetZoneReverb(SI3ZONESOUNDTYPE_3, override->m_soundProviderPreferencesID);
            SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_3, override->m_soundAmbienceID);
            SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_7, override->m_zoneIntroMusicID);
            SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_3, override->m_zoneMusicID);
            SI3::SetZoneReverb(SI3ZONESOUNDTYPE_10, override->m_uwSoundProviderPreferencesID);
            SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_10, override->m_uwSoundAmbienceID);
            SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_12, override->m_uwZoneIntroMusicID);
            SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_10, override->m_uwZoneMusicID);
            found = 1;

            if (wmoGroupAreaID == SI3::sm_PhaseShiftZoneSoundOverridesArray[i]->m_wmoAreaID) {
                return 1;
            }
        }
    } else if (wmoAreaID > 0 || wmoGroupAreaID > 0) {
        int32_t best = 4;

        for (uint32_t i = 0; i < count; i++) {
            PhasedZoneSoundOverride* override = SI3::sm_PhaseShiftZoneSoundOverridesArray[i];
            int32_t priority = 4;

            if (areaID > 0 && areaID == override->m_areaID) {
                priority = 3;
            }

            if (zoneID > 0 && zoneID == override->m_areaID) {
                priority = 2;
            }

            if (wmoGroupAreaID > 0 && wmoGroupAreaID == override->m_wmoAreaID) {
                priority = 0;
            }

            if (priority >= best || !PhaseShift_C_HasSharedPhase(override->m_phaseShift)) {
                continue;
            }

            SI3::SetZoneReverb(SI3ZONESOUNDTYPE_3, override->m_soundProviderPreferencesID);
            SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_3, override->m_soundAmbienceID);
            SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_7, override->m_zoneIntroMusicID);
            SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_3, override->m_zoneMusicID);
            SI3::SetZoneReverb(SI3ZONESOUNDTYPE_10, override->m_uwSoundProviderPreferencesID);
            SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_10, override->m_uwSoundAmbienceID);
            SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_12, override->m_uwZoneIntroMusicID);
            SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_10, override->m_uwZoneMusicID);
            found = 1;
            best = priority;

            if (!priority) {
                return 1;
            }
        }
    } else {
        for (uint32_t i = 0; i < count; i++) {
            PhasedZoneSoundOverride* override = SI3::sm_PhaseShiftZoneSoundOverridesArray[i];

            if (!((areaID > 0 && areaID == override->m_areaID) || (zoneID > 0 && zoneID == override->m_areaID))) {
                continue;
            }

            if (!PhaseShift_C_HasSharedPhase(override->m_phaseShift)) {
                continue;
            }

            SI3::SetZoneReverb(SI3ZONESOUNDTYPE_3, override->m_soundProviderPreferencesID);
            SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_3, override->m_soundAmbienceID);
            SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_7, override->m_zoneIntroMusicID);
            SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_3, override->m_zoneMusicID);
            SI3::SetZoneReverb(SI3ZONESOUNDTYPE_10, override->m_uwSoundProviderPreferencesID);
            SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_10, override->m_uwSoundAmbienceID);
            SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_12, override->m_uwZoneIntroMusicID);
            SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_10, override->m_uwZoneMusicID);
            found = 1;

            if (zoneID == SI3::sm_PhaseShiftZoneSoundOverridesArray[i]->m_areaID) {
                return 1;
            }
        }
    }

    if (found) {
        return 1;
    }

    SI3::SetZoneReverb(SI3ZONESOUNDTYPE_3, -1);
    SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_3, -1);
    SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_7, -1);
    SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_3, -1);
    SI3::SetZoneReverb(SI3ZONESOUNDTYPE_10, -1);
    SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_10, -1);
    SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_12, -1);
    SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_10, -1);
    return 0;
}

// OFFSET: 0x0062DD50/0x1402C1AF0
int32_t SI3::GetWorldStateZoneSoundOverride(int32_t areaID, int32_t zoneID, int32_t wmoAreaID, int32_t wmoGroupAreaID, int32_t inside) {
    uint32_t count = SI3::sm_WorldStateZoneSoundOverridesArray.Count();
    int32_t found = 0;

    if (inside) {
        for (uint32_t i = 0; i < count; i++) {
            WorldStateZoneSoundOverride* override = SI3::sm_WorldStateZoneSoundOverridesArray[i];

            if (!((wmoAreaID > 0 && wmoAreaID == override->m_wmoAreaID) || (wmoGroupAreaID > 0 && wmoGroupAreaID == override->m_wmoAreaID))) {
                continue;
            }

            if (GetWorldState(override->m_worldStateID) != override->m_worldStateValue) {
                continue;
            }

            SI3::SetZoneReverb(SI3ZONESOUNDTYPE_1, override->m_soundProviderPreferencesID);
            SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_1, override->m_soundAmbienceID);
            SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_5, override->m_zoneIntroMusicID);
            SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_1, override->m_zoneMusicID);
            found = 1;

            if (wmoGroupAreaID == SI3::sm_WorldStateZoneSoundOverridesArray[i]->m_wmoAreaID) {
                return 1;
            }
        }
    } else if (wmoAreaID > 0 || wmoGroupAreaID > 0) {
        int32_t best = 4;

        for (uint32_t i = 0; i < count; i++) {
            WorldStateZoneSoundOverride* override = SI3::sm_WorldStateZoneSoundOverridesArray[i];
            int32_t priority = 4;

            if (areaID > 0 && areaID == override->m_areaID) {
                priority = 3;
            }

            if (zoneID > 0 && zoneID == override->m_areaID) {
                priority = 2;
            }

            if (wmoGroupAreaID > 0 && wmoGroupAreaID == override->m_wmoAreaID) {
                priority = 0;
            }

            if (priority >= best || GetWorldState(override->m_worldStateID) != override->m_worldStateValue) {
                continue;
            }

            SI3::SetZoneReverb(SI3ZONESOUNDTYPE_1, override->m_soundProviderPreferencesID);
            SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_1, override->m_soundAmbienceID);
            SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_5, override->m_zoneIntroMusicID);
            SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_1, override->m_zoneMusicID);
            found = 1;
            best = priority;

            if (!priority) {
                return 1;
            }
        }
    } else {
        for (uint32_t i = 0; i < count; i++) {
            WorldStateZoneSoundOverride* override = SI3::sm_WorldStateZoneSoundOverridesArray[i];

            if (!((areaID > 0 && areaID == override->m_areaID) || (zoneID > 0 && zoneID == override->m_areaID))) {
                continue;
            }

            if (GetWorldState(override->m_worldStateID) != override->m_worldStateValue) {
                continue;
            }

            SI3::SetZoneReverb(SI3ZONESOUNDTYPE_1, override->m_soundProviderPreferencesID);
            SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_1, override->m_soundAmbienceID);
            SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_5, override->m_zoneIntroMusicID);
            SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_1, override->m_zoneMusicID);
            found = 1;

            if (zoneID == SI3::sm_WorldStateZoneSoundOverridesArray[i]->m_areaID) {
                return 1;
            }
        }
    }

    if (found) {
        return 1;
    }

    SI3::SetZoneReverb(SI3ZONESOUNDTYPE_1, -1);
    SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_1, -1);
    SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_5, -1);
    SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_1, -1);
    return 0;
}

// PROVEN: 7e270ab3 IDENTICAL/NONE
// OFFSET: 0x0062E060
void SI3::AIMusicFinished(SE3SoundKitObject* sko) {
    if (sko && sko->m_pSoundKitDef && sko->m_pSoundKitDef->m_pSoundKitRec && sko->m_pSoundKitDef->m_pSoundKitRec->m_id != SI3::sm_aZoneSoundInfo[SI3ZONESOUNDTYPE_13].m_soundKitID[0]) {
        return;
    }

    SI3ZoneSoundInfo& info = SI3::sm_aZoneSoundInfo[SI3ZONESOUNDTYPE_13];
    info.m_soundKitID[0] = -1;
    info.m_soundKitID[1] = -1;
    info.m_silenceIntervalMin[0] = 0;
    info.m_silenceIntervalMin[1] = 0;
    info.m_silenceIntervalMax[0] = 0;
    info.m_silenceIntervalMax[1] = 0;
    info.m_ambienceKitID[0] = -1;
    info.m_ambienceKitID[1] = -1;
    info.m_soundAmbienceID = 0;
    info.m_reverb = -1;
}

// OFFSET: 0x0062E0C0
void SI3::PlayAIMusic(int32_t soundKitID) {
    SI3ZoneSoundInfo& info = SI3::sm_aZoneSoundInfo[SI3ZONESOUNDTYPE_13];
    info.m_silenceIntervalMin[0] = 0;
    info.m_silenceIntervalMin[1] = 0;
    info.m_silenceIntervalMax[0] = 0;
    info.m_silenceIntervalMax[1] = 0;
    info.m_soundAmbienceID = 0;
    info.m_ambienceKitID[0] = -1;
    info.m_ambienceKitID[1] = -1;
    info.m_reverb = -1;
    info.m_soundKitID[0] = soundKitID;
    info.m_soundKitID[1] = soundKitID;
}

// OFFSET: 0x0062E110
void SI3::ZoneSoundsPhaseChanged() {
    SI3::GetPhaseShiftZoneSoundOverride(-1, -1, -1, -1, 0);
}

// OFFSET: 0x0062E130/0x1402C2070
int32_t SI3::PlayLuaMusic(const char* filename) {
    int32_t result = SE3::PlaySoundFile(filename, &s_luaMusic, SE3BUS_7, static_cast<SE3USAGE>(1));

    if (!result) {
        SI3::sm_ZoneMusic.Stop();
        SI3::sm_uNextZoneMusicPlayTime = 0;

        SI3ZoneSoundInfo& info = SI3::sm_aZoneSoundInfo[SI3ZONESOUNDTYPE_13];
        info.m_soundKitID[0] = -1;
        info.m_soundKitID[1] = -1;
        info.m_ambienceKitID[0] = -1;
        info.m_ambienceKitID[1] = -1;
        info.m_reverb = -1;
        info.m_silenceIntervalMin[0] = 0;
        info.m_silenceIntervalMin[1] = 0;
        info.m_silenceIntervalMax[0] = 0;
        info.m_silenceIntervalMax[1] = 0;
        info.m_soundAmbienceID = 0;
    }

    return result;
}

// OFFSET: 0x0062E1B0
int32_t SI3::GetChunkLocation(int32_t& mapID, int32_t& areaX, int32_t& areaY, int32_t& chunkX, int32_t& chunkY) {
    mapID = World::QueryMapId();

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    C3Vector position = player->GetPosition();
    World::GetChunkCoords(position, areaX, areaY, chunkX, chunkY);
    return 1;
}

// OFFSET: 0x0062E220/0x1402C2120
void SI3::StartAmbienceFlavorSounds(int32_t type) {
    for (uint32_t* sound = s_ambienceFlavorSounds; sound < s_ambienceFlavorSounds + 10; sound++) {
        SE3::StopSound(*sound, 3000, __FILE__, __LINE__, "SI3::StartAmbienceFlavorSounds");
        *sound = 0;
    }

    if (type >= SI3ZONESOUNDTYPE_LAST) {
        return;
    }

    int32_t soundAmbienceID = SI3::sm_aZoneSoundInfo[type].m_soundAmbienceID;
    C3Vector listener;
    SE3::GetListenerPosition(&listener);

    uint32_t* sound = s_ambienceFlavorSounds;
    AmbFlavorListHead& head = s_ambienceFlavors[soundAmbienceID];

    for (AmbFlavor* flavor = head.m_flavors.Head(); flavor; sound++) {
        AmbFlavor* next = head.m_flavors.Next(flavor);

        if (sound == s_ambienceFlavorSounds + 10) {
            break;
        }

        int32_t soundKitID = SI3::sm_eDayOrNight ? flavor->m_nightSoundKitID : flavor->m_daySoundKitID;

        SE3SoundKitProperties properties;
        properties.m_bus = SE3BUS_AMBIENCE;
        SE3::PlaySoundKit(soundKitID, &listener, &properties, sound, __FILE__, __LINE__, "SI3::StartAmbienceFlavorSounds", 1, NULL);
        flavor = next;
    }
}

// OFFSET: 0x0062E350/0x1402C22D0
void SI3::InitZoneIntros() {
    uint32_t count = g_soundEntriesDB.m_maxID + 1;
    SI3::sm_aIntroMusicNextValidPlayTime.SetCount(count);

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        SI3::sm_aIntroMusicNextValidPlayTime[i] = 0;
    }
}

// PROVEN: 3d60c503 IDENTICAL/NONE
// OFFSET: 0x0062E3D0/0x1402C2380
void SI3::StopAllMusic() {
    SI3::sm_ZoneMusic.Stop();
    SI3::sm_uNextZoneMusicPlayTime = 0;
    SI3::StopLuaMusic();

    SI3ZoneSoundInfo& info = SI3::sm_aZoneSoundInfo[SI3ZONESOUNDTYPE_13];
    info.m_soundKitID[0] = -1;
    info.m_soundKitID[1] = -1;
    info.m_ambienceKitID[0] = -1;
    info.m_ambienceKitID[1] = -1;
    info.m_reverb = -1;
    info.m_silenceIntervalMin[0] = 0;
    info.m_silenceIntervalMin[1] = 0;
    info.m_silenceIntervalMax[0] = 0;
    info.m_silenceIntervalMax[1] = 0;
    info.m_soundAmbienceID = 0;
}

// OFFSET: 0x0062E460/0x1402C2440
void SI3::InitPhaseShiftZoneSoundOverrides() {
    SI3::sm_PhaseShiftZoneSoundOverridesArray.SetCount(g_phaseShiftZoneSoundsDB.m_numRecords);
    memset(SI3::sm_PhaseShiftZoneSoundOverridesArray.Ptr(), 0, sizeof(PhasedZoneSoundOverride*) * g_phaseShiftZoneSoundsDB.m_numRecords);

    for (uint32_t i = 0; i < SI3::sm_PhaseShiftZoneSoundOverridesArray.Count(); i++) {
        SI3::sm_PhaseShiftZoneSoundOverridesArray[i] = static_cast<PhasedZoneSoundOverride*>(SMemAlloc(sizeof(PhasedZoneSoundOverride), __FILE__, __LINE__, 0));
        PhaseShiftZoneSoundsRec* rec = g_phaseShiftZoneSoundsDB.GetRecordByIndex(i);

        SI3::sm_PhaseShiftZoneSoundOverridesArray[i]->m_areaID = rec->m_areaID;
        SI3::sm_PhaseShiftZoneSoundOverridesArray[i]->m_wmoAreaID = rec->m_wmoAreaID;
        SI3::sm_PhaseShiftZoneSoundOverridesArray[i]->m_phaseShift = new (SMemAlloc(sizeof(CPhaseShift), __FILE__, __LINE__, 0)) CPhaseShift();
        SI3::sm_PhaseShiftZoneSoundOverridesArray[i]->m_phaseShift->SetPhasesFromDB(rec->m_phaseUseFlags, rec->m_phaseID, rec->m_phaseGroupID);
        SI3::sm_PhaseShiftZoneSoundOverridesArray[i]->m_zoneIntroMusicID = rec->m_zoneIntroMusicID;
        SI3::sm_PhaseShiftZoneSoundOverridesArray[i]->m_zoneMusicID = rec->m_zoneMusicID;
        SI3::sm_PhaseShiftZoneSoundOverridesArray[i]->m_soundAmbienceID = rec->m_soundAmbienceID;
        SI3::sm_PhaseShiftZoneSoundOverridesArray[i]->m_soundProviderPreferencesID = rec->m_soundProviderPreferencesID;
        SI3::sm_PhaseShiftZoneSoundOverridesArray[i]->m_uwZoneIntroMusicID = rec->m_uwZoneIntroMusicID;
        SI3::sm_PhaseShiftZoneSoundOverridesArray[i]->m_uwZoneMusicID = rec->m_uwZoneMusicID;
        SI3::sm_PhaseShiftZoneSoundOverridesArray[i]->m_uwSoundAmbienceID = rec->m_uwSoundAmbienceID;
        SI3::sm_PhaseShiftZoneSoundOverridesArray[i]->m_uwSoundProviderPreferencesID = rec->m_uwSoundProviderPreferencesID;
    }
}

// OFFSET: 0x0062E610/0x1402C2660
void SI3::InitWorldStateZoneSoundOverrides() {
    SI3::sm_WorldStateZoneSoundOverridesArray.SetCount(g_worldStateZoneSoundsDB.m_numRecords);
    memset(SI3::sm_WorldStateZoneSoundOverridesArray.Ptr(), 0, sizeof(WorldStateZoneSoundOverride*) * g_worldStateZoneSoundsDB.m_numRecords);

    for (uint32_t i = 0; i < SI3::sm_WorldStateZoneSoundOverridesArray.Count(); i++) {
        SI3::sm_WorldStateZoneSoundOverridesArray[i] = static_cast<WorldStateZoneSoundOverride*>(SMemAlloc(sizeof(WorldStateZoneSoundOverride), __FILE__, __LINE__, 0));
        WorldStateZoneSoundsRec* rec = g_worldStateZoneSoundsDB.GetRecordByIndex(i);

        SI3::sm_WorldStateZoneSoundOverridesArray[i]->m_areaID = rec->m_areaID;
        SI3::sm_WorldStateZoneSoundOverridesArray[i]->m_wmoAreaID = rec->m_wmoareaID;
        SI3::sm_WorldStateZoneSoundOverridesArray[i]->m_worldStateID = rec->m_worldStateID;
        SI3::sm_WorldStateZoneSoundOverridesArray[i]->m_worldStateValue = rec->m_worldStateValue;
        SI3::sm_WorldStateZoneSoundOverridesArray[i]->m_zoneIntroMusicID = rec->m_zoneIntroMusicID;
        SI3::sm_WorldStateZoneSoundOverridesArray[i]->m_zoneMusicID = rec->m_zoneMusicID;
        SI3::sm_WorldStateZoneSoundOverridesArray[i]->m_soundAmbienceID = rec->m_soundAmbienceID;
        SI3::sm_WorldStateZoneSoundOverridesArray[i]->m_soundProviderPreferencesID = rec->m_soundProviderPreferencesID;
    }
}

// PROVEN: 776291af IDENTICAL/NONE
// OFFSET: 0x0062E750
void SI3::FreeZoneIntros() {
    SI3::sm_aIntroMusicNextValidPlayTime.Clear();
}

// OFFSET: 0x0062E7A0
void SI3::FreeWorldStateZoneSoundOverrides() {
    for (uint32_t i = 0; i < SI3::sm_WorldStateZoneSoundOverridesArray.Count(); i++) {
        delete SI3::sm_WorldStateZoneSoundOverridesArray[i];
    }

    SI3::sm_WorldStateZoneSoundOverridesArray.Clear();
}

// OFFSET: 0x0062E820
int32_t SI3::GetZoneSoundOverride(int32_t mapID, int32_t areaX, int32_t areaY, int32_t chunkX, int32_t chunkY, int32_t* zoneIntroMusicID, int32_t* zoneMusicID, int32_t* soundAmbienceID, int32_t* soundProviderPreferencesID) {
    char key[64];
    SStrPrintf(key, sizeof(key), "%010d_%010d_%010d_%010d_%010d", mapID, areaX, areaY, chunkX, chunkY);

    ZONESOUNDOVERRIDELOOKUP* lookup = SI3::sm_ZoneSoundOverridesHashTable.Ptr(key);

    if (!lookup) {
        return 0;
    }

    *zoneIntroMusicID = lookup->m_zoneIntroMusicID;
    *zoneMusicID = lookup->m_zoneMusicID;
    *soundAmbienceID = lookup->m_soundAmbienceID;
    *soundProviderPreferencesID = lookup->m_soundProviderPreferencesID;
    return 1;
}

// PROVEN: 620ba03e IDENTICAL/NONE
// OFFSET: 0x0062E890
void SI3::OverrideZoneSounds() {
    int32_t mapID;
    int32_t areaX;
    int32_t areaY;
    int32_t chunkX;
    int32_t chunkY;

    if (!SI3::GetChunkLocation(mapID, areaX, areaY, chunkX, chunkY)) {
        return;
    }

    if (chunkX != s_overrideChunkX || chunkY != s_overrideChunkY) {
        int32_t zoneIntroMusicID;
        int32_t zoneMusicID;
        int32_t soundAmbienceID;
        int32_t soundProviderPreferencesID;

        if (SI3::GetZoneSoundOverride(mapID, areaX, areaY, chunkX, chunkY, &zoneIntroMusicID, &zoneMusicID, &soundAmbienceID, &soundProviderPreferencesID)) {
            if (soundProviderPreferencesID >= 0) {
                SI3::SetZoneReverb(SI3ZONESOUNDTYPE_2, soundProviderPreferencesID);
            }

            if (soundAmbienceID >= 0) {
                SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_2, soundAmbienceID);
            }

            if (zoneIntroMusicID >= 0) {
                SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_6, zoneIntroMusicID);
            }

            if (zoneMusicID >= 0) {
                SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_2, zoneMusicID);
            }
        } else {
            SI3::SetZoneReverb(SI3ZONESOUNDTYPE_2, -1);
            SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_2, -1);
            SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_6, -1);
            SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_2, -1);
        }
    }

    s_overrideAreaX = areaX;
    s_overrideMapID = mapID;
    s_overrideAreaY = areaY;
    s_overrideChunkX = chunkX;
    s_overrideChunkY = chunkY;
}

// OFFSET: 0x0062E9B0
void SI3::FreeZoneSoundOverrides() {
    SI3::sm_ZoneSoundOverridesHashTable.Clear();
}

// OFFSET: 0x0062ED10
void SI3::ShutDownAmbienceFlavor() {
    s_ambienceFlavorSoundAmbienceID = 0;

    for (uint32_t i = 0; i < s_ambienceFlavors.Count(); i++) {
        s_ambienceFlavors[i].m_flavors.Clear();
    }
}

// OFFSET: 0x0062ED50
void SI3::FreePhaseShiftZoneSoundOverrides() {
    for (uint32_t i = 0; i < SI3::sm_PhaseShiftZoneSoundOverridesArray.Count(); i++) {
        PhasedZoneSoundOverride* override = SI3::sm_PhaseShiftZoneSoundOverridesArray[i];

        if (!override) {
            continue;
        }

        delete override->m_phaseShift;
        delete override;
    }

    SI3::sm_PhaseShiftZoneSoundOverridesArray.Clear();
}

// OFFSET: 0x0062F2E0/0x1402C39C0
void SI3::InitAmbienceFlavor() {
    int32_t count = g_soundAmbienceFlavorDB.m_numRecords;
    s_ambienceFlavorSoundAmbienceID = 0;
    s_ambienceFlavors.SetCount(g_soundAmbienceDB.m_maxID + 1);

    for (int32_t i = 0; i < count; i++) {
        SoundAmbienceFlavorRec* rec = g_soundAmbienceFlavorDB.GetRecordByIndex(i);
        AmbFlavor* flavor = s_ambienceFlavors[rec->m_soundAmbienceID].m_flavors.NewNode(2, 0, 0);
        flavor->m_daySoundKitID = rec->m_soundEntriesIDDay;
        flavor->m_nightSoundKitID = rec->m_soundEntriesIDNight;
    }
}

// OFFSET: 0x0062F3F0/0x1402C3B20
static int32_t ZoneSoundsUpdate(const void* data, void* param) {
    int32_t ambienceType = SI3ZONESOUNDTYPE_NONE;
    SI3::sm_eDayOrNight = !g_clientGameTime.IsDaytime();
    int32_t ambienceKitID = SI3::GetDesiredAmbienceKitID(&ambienceType);

    if (ambienceKitID != s_ambienceFlavorSoundAmbienceID) {
        SI3::StartAmbienceFlavorSounds(ambienceType);
    }

    s_ambienceFlavorSoundAmbienceID = ambienceKitID;

    if (!ambienceKitID && SE3::IsPlaying(SI3::sm_ZoneAmbience.m_handle)) {
        SI3::sm_ZoneAmbience.Stop();
    }

    if (!SE3::IsPlaying(SI3::sm_ZoneAmbience.m_handle) || (ambienceKitID > 0 && ambienceKitID != SI3::sm_ZoneAmbience.m_soundKitID)) {
        SI3::sm_ZoneAmbience.Stop();

        SE3SoundKitProperties properties;
        properties.m_reverb = 0;
        properties.m_stopPrevious = 0;
        properties.m_bit10 = 1;
        properties.m_bus = SE3BUS_AMBIENCE;
        properties.m_usage = 1;
        properties.m_priority = 100;
        properties.m_fadeInTime = SI3::sm_bCameraIsUnderwater_ChangedThisFrame != 0 ? 0 : 5000;
        properties.m_fadeOutTime = 5000;
        SI3::sm_ZoneAmbience.Play(ambienceKitID, ambienceType, &properties);
    }

    int32_t silenceMin = 0;
    int32_t silenceMax = 0;
    int32_t musicType = SI3ZONESOUNDTYPE_NONE;
    int32_t musicKitID = SI3::GetDesiredMusicKitID(&musicType, &silenceMin, &silenceMax, NULL);

    if (musicKitID) {
        if (musicKitID > 0 && !(SI3::sm_ZoneMusic.m_soundKitID == musicKitID && SE3::IsPlaying(SI3::sm_ZoneMusic.m_handle)) && !CGGameUI::CameraMusicIsPlaying() && !CGGameUI::SequenceMusicIsPlaying()) {
            SI3::sm_ZoneMusic.Stop();

            SE3SoundKitProperties properties;
            properties.m_fadeInTime = 4000;
            properties.m_fadeOutTime = 4000;
            properties.m_priority = 100;
            properties.m_bus = SE3BUS_MUSIC;
            properties.m_selectMode = 2;
            properties.m_reverb = 0;
            properties.m_bit10 = 1;

            if (!SI3::sm_ZoneMusic.Play(musicKitID, musicType, &properties)) {
                if (SI3::IsIntroZoneSoundType(musicType)) {
                    SI3::sm_nSilenceIntervalMin = silenceMin;
                    SI3::sm_nSilenceIntervalMax = 0;
                    SE3::SetPlayFinishedCallback(SI3::sm_ZoneMusic.m_handle, SI3::IntroMusicFinished, NULL);
                } else if (SI3::IsZoneMusicZoneSoundType(musicType)) {
                    SI3::sm_nSilenceIntervalMax = silenceMax;
                    SI3::sm_nSilenceIntervalMin = silenceMin;
                    SE3::SetPlayFinishedCallback(SI3::sm_ZoneMusic.m_handle, SI3::ZoneMusicFinished, NULL);
                } else if (musicType == SI3ZONESOUNDTYPE_13) {
                    SI3::sm_nSilenceIntervalMax = silenceMax;
                    SI3::sm_nSilenceIntervalMin = silenceMin;
                    SE3::SetPlayFinishedCallback(SI3::sm_ZoneMusic.m_handle, SI3::AIMusicFinished, NULL);
                }
            }
        }
    } else if (SE3::IsPlaying(SI3::sm_ZoneMusic.m_handle)) {
        SI3::sm_ZoneMusic.Stop();
    }

    C3Vector listener;
    SE3::GetListenerPosition(&listener);

    for (int32_t i = 0; i < 10; i++) {
        if (SE3::IsPlaying(s_ambienceFlavorSounds[i])) {
            SE3::Set3DPosition(s_ambienceFlavorSounds[i], listener);
        } else {
            s_ambienceFlavorSounds[i] = 0;
        }
    }

    return 1;
}

// PROVEN: 6056d7c9 IDENTICAL/NONE
// OFFSET: 0x0062F790/0x1402C3F50
void SI3::InitZoneSoundsHandler() {
    EventRegister(EVENT_ID_IDLE, ZoneSoundsUpdate);
    SI3::WaterInit();
}

// OFFSET: 0x0062F7B0
void SI3::ShutdownZoneSoundsHandler() {
    EventUnregister(EVENT_ID_IDLE, ZoneSoundsUpdate);
    SI3::sm_ZoneAmbience.Stop();
    SI3::StopAllMusic();
    SI3::WaterShutdown();
}

// OFFSET: 0x0062F880/0x1402C3FC0
void SI3::InitZoneSoundOverrides() {
    char key[64];

    for (int32_t i = 0; i < g_worldChunkSoundsDB.m_numRecords; i++) {
        WorldChunkSoundsRec* rec = g_worldChunkSoundsDB.GetRecordByIndex(i);

        SStrPrintf(key, sizeof(key), "%010d_%010d_%010d_%010d_%010d", rec->m_mapID, rec->m_chunkX, rec->m_chunkY, rec->m_subchunkX, rec->m_subchunkY);

        ZONESOUNDOVERRIDELOOKUP* lookup = SI3::sm_ZoneSoundOverridesHashTable.Ptr(key);

        if (!lookup) {
            lookup = SI3::sm_ZoneSoundOverridesHashTable.New(key, 0, 0);
        }

        lookup->m_zoneIntroMusicID = rec->m_zoneIntroMusicID;
        lookup->m_zoneMusicID = rec->m_zoneMusicID;
        lookup->m_soundAmbienceID = rec->m_soundAmbienceID;
        lookup->m_soundProviderPreferencesID = rec->m_soundProviderPreferencesID;
    }
}
