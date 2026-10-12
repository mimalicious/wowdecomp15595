#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/Base/CmdLine.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Frame/CSimpleMovieFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Game/GameTime.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/UnitSound_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/SoundInterface/SI3ZoneSounds.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WorldClient/AreaList.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00CC8E64/0x140B69230
static int32_t s_savedVoiceSessionType = 4;

// STATIC: 0x00E283E0/0x140D0FF68
static int32_t s_restartSignaled;

// STATIC: 0x00E286D0
static uint32_t s_taxiSound;

// STATIC: 0x00E286D4/0x140D10254
static float s_taxiFidgetElapsed;

// STATIC: 0x00E286D8/0x140D1025C
static float s_taxiFidgetDelay;

// STATIC: 0x00E2872C/0x140D102B8
static const char* s_savedVoiceSession;

// OFFSET: 0x0062C2B0/0x1402BF9E0
void SI3::SignalRestart() {
    s_restartSignaled = 1;
}

// OFFSET: 0x0062C2C0/0x1402BF9F0
int32_t SI3::HeartBeat(const void* data, void* param) {
    if (s_restartSignaled) {
        SI3::ShutDown(1);
        OsSleep(200);
        SI3::Init(1);

        if (ClientServices::Connection()->m_inGame) {
            SI3::InitSoundInGame();
        }

        s_restartSignaled = 0;
    }

    return 1;
}

// OFFSET: 0x0062C310/0x1402BFA50
void SI3::PosToWorldChunkCoords(const C3Vector& position, uint32_t* x, uint32_t* y) {
    int32_t areaX;
    int32_t areaY;
    int32_t chunkX;
    int32_t chunkY;
    World::GetChunkCoords(position, areaX, areaY, chunkX, chunkY);
    *x = (chunkX + 16 * areaX) / 4;
    *y = (chunkY + 16 * areaY) / 4;
}

// OFFSET: 0x0062C370/0x1402BFAC0
int32_t SI3::GetFallbackSoundKitID(int32_t soundKitID) {
    return SE3::GetFallbackSoundKitID(soundKitID);
}

// OFFSET: 0x0062C380/0x1402BFAD0
int32_t SI3::PlayUISound(int32_t soundKitID, const SE3BUS* bus, bool forceNoDuplicates) {
    SE3SoundKitProperties properties;
    properties.m_bus = bus ? *bus : SE3BUS_6;
    properties.m_highPriorityDownload = 1;
    properties.m_priority = 105;

    if (forceNoDuplicates) {
        properties.m_uniqueness = 2;
    }

    properties.m_reverb = 0;
    return SE3::PlaySoundKit(soundKitID, NULL, &properties, NULL, __FILE__, __LINE__, "SI3::PlayUISound");
}

// OFFSET: 0x0062C420/0x1402BFBB0
void SI3::PlayItemSound(int32_t type, int32_t displayID) {
    ItemDisplayInfoRec displayInfo;

    if (!g_itemDisplayInfoDB.GetRecord(displayID, &displayInfo)) {
        return;
    }

    ItemGroupSoundsRec* groupSounds = g_itemGroupSoundsDB.GetRecord(displayInfo.m_groupSoundIndex);

    if (!groupSounds) {
        return;
    }

    SE3SoundKitProperties properties;
    SE3::PlaySoundKit(groupSounds->m_sound[type], NULL, &properties, NULL, __FILE__, __LINE__, "PlayItemSound()");
}

// OFFSET: 0x0062C500/0x1402BFCD0
void SI3::PlayFoleySound(uint32_t materialID, const C3Vector& position, int32_t self, void* unit) {
    MaterialRec* material = g_materialDB.GetRecord(materialID);

    if (!material || !material->m_foleySoundID) {
        return;
    }

    C3Vector soundPosition;
    soundPosition.x = position.x;
    soundPosition.y = position.y;
    soundPosition.z = position.z + 2.0f;

    if (self) {
        static CVar* s_listenerAtCharacter = CVar::Lookup("Sound_ListenerAtCharacter");

        int32_t listenerAtCharacter = s_listenerAtCharacter->m_intValue;

        SE3SoundKitProperties properties;
        properties.m_priority = 110;
        SE3::PlaySoundKit(material->m_foleySoundID, listenerAtCharacter ? NULL : &soundPosition, &properties, NULL, __FILE__, __LINE__, "SI3::PlayFoleySound");
    } else {
        SE3SoundKitProperties properties;
        SE3::PlaySoundKit(material->m_foleySoundID, &soundPosition, &properties, NULL, __FILE__, __LINE__, "SI3::PlayFoleySound");
    }
}

// OFFSET: 0x0062C670/0x1402BFEA0
void SI3::PlayItemSound(int32_t type, const CGItem_C* item) {
    SE3SoundKitProperties properties;

    if (item->m_item->m_itemFieldFlags & 0x8) {
        if (type == 1) {
            SE3::PlaySoundKit("(DONOTRENAME)WrappedItemDrop", NULL, &properties, NULL, __FILE__, __LINE__, "PlayItemSound()");
        } else {
            SE3::PlaySoundKit("(DONOTRENAME)WrappedItemPickup", NULL, &properties, NULL, __FILE__, __LINE__, "PlayItemSound()");
        }
    } else if (item->m_groupSounds) {
        SE3::PlaySoundKit(item->m_groupSounds->m_sound[type], NULL, &properties, NULL, __FILE__, __LINE__, "PlayItemSound()");
    }
}

// OFFSET: 0x0062C750/0x1402BFFB0
void SI3::ForceUpdateZoneSounds() {
    CGObject_C* object = ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object) {
        object = ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__);

        if (!object) {
            return;
        }
    }

    uintptr_t worldObject = object->m_worldObject;
    AreaTableRec* area = AreaListGetObjectArea(worldObject);

    if (!area) {
        return;
    }

    AreaTableRec* zone;

    if (area->m_flags & 0x40000000) {
        zone = g_areaTableDB.GetRecord(area->m_parentAreaID);
    } else {
        zone = area;
        area = NULL;
    }

    const WMOAreaTableRec* groupRec = NULL;
    const WMOAreaTableRec* rootRec = NULL;
    uint32_t wmoGroupID = 0;
    int32_t inside = World::QueryObjectInside(worldObject, 1);
    World::QueryMapObjAreaTable(worldObject, groupRec, rootRec, wmoGroupID, 1);
    AreaListUpdateSounds(zone, area, rootRec, groupRec, inside, World::QueryMapId());
}

// OFFSET: 0x0062C840/0x1402C00E0
void SI3::UpdateTaxiSounds(float elapsed) {
    if (elapsed <= 0.0f) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !(player->m_unit->m_unitFieldFlags & 0x100000)) {
        return;
    }

    if (s_taxiFidgetDelay <= 0.0f) {
        float delayMin = CreatureSoundDataGetFidgetDelayMin(player->GetSoundData(0));
        float range = CreatureSoundDataGetFidgetDelayMax(player->GetSoundData(0));
        range = range - CreatureSoundDataGetFidgetDelayMin(player->GetSoundData(0));

        if (!CreatureSoundDataGetFidgetSound(player->GetSoundData(0), 4)) {
            s_taxiFidgetDelay = 5.0f;
            s_taxiFidgetElapsed = 0.0f;
            return;
        }

        if (range <= 0.0f) {
            s_taxiFidgetDelay = 2.0f;
            return;
        }

        uint32_t roll = CRandom::dice_(static_cast<uint32_t>(static_cast<int64_t>(range * 100.0f)), g_rndSeed);
        s_taxiFidgetDelay = static_cast<float>(roll) * 0.0099999998f + delayMin;
    }

    if (!SE3::IsPlaying(s_taxiSound)) {
        s_taxiFidgetElapsed = s_taxiFidgetElapsed + elapsed;
    }

    if (s_taxiFidgetElapsed >= s_taxiFidgetDelay) {
        int32_t soundKitID = CreatureSoundDataGetFidgetSound(player->GetSoundData(0), 4);
        C3Vector position = player->GetPosition();

        SE3SoundKitProperties properties;
        SE3::PlaySoundKit(soundKitID, &position, &properties, &s_taxiSound, __FILE__, __LINE__, "SI3::UpdateTaxiSounds");
        s_taxiFidgetDelay = 0.0f;
        s_taxiFidgetElapsed = 0.0f;
    }
}

// OFFSET: 0x0062F9E0/0x1402C4370
static int32_t _GetDayProgressionCallback() {
    return static_cast<int32_t>(g_clientGameTime.GameTimeGetDayProgression() * 86400000.0);
}

// OFFSET: 0x0062FA00/0x1402C43A0
void SI3::InitSoundInGame() {
    SI3::InitFootstepSounds();
    SI3::InitEmoteSounds();
    SI3::InitWeaponSwingCombatSounds();
    SI3::InitWeaponImpactCombatSounds();
    SI3::InitPhaseShiftZoneSoundOverrides();
    SI3::InitWorldStateZoneSoundOverrides();
    SI3::InitZoneSoundOverrides();
}

// OFFSET: 0x0062FAB0/0x1402C43D0
int32_t SI3::OnFocusChanged(const void* data, void* param) {
    const EVENT_DATA_FOCUS* focus = static_cast<const EVENT_DATA_FOCUS*>(data);

    if (!focus) {
        return 1;
    }

    if (focus->m_focus) {
        SE3::SetFocusScalar(1.0f);

        if (CGGameUI::m_initialized && SI3::sm_pCVar_VoiceChat_EnableVoiceChat && SI3::sm_pCVar_VoiceChat_EnableVoiceChat->m_intValue && s_savedVoiceSession) {
            CGChat::SetActiveVoiceSession(s_savedVoiceSession, s_savedVoiceSessionType, -1);
            s_savedVoiceSession = NULL;
            s_savedVoiceSessionType = 4;
        }
    } else if (SI3::sm_pCVar_Sound_EnableSoundWhenGameIsInBG && SI3::sm_pCVar_Sound_EnableSoundWhenGameIsInBG->m_intValue) {
        SE3::SetFocusScalar(1.0f);
    } else {
        SE3::SetFocusScalar(0.0f);

        if (CGGameUI::m_initialized && SI3::sm_pCVar_VoiceChat_EnableVoiceChat && SI3::sm_pCVar_VoiceChat_EnableVoiceChat->m_intValue && !s_savedVoiceSession) {
            s_savedVoiceSession = CGChat::GetChannelStringForDisplay(CGChat::GetActiveVoiceSession(), &s_savedVoiceSessionType);
            CGChat::SetActiveVoiceSession(NULL, 4, -1);
        }
    }

    return 1;
}

// OFFSET: 0x0062FBB0/0x1402C44E0
void SI3::ShutDown(int32_t restart) {
    SE3::Log_Write(" ");
    SE3::Log_Write("=> Shutting Down SI3");
    EventUnregister(EVENT_ID_POLL, SI3::HeartBeat);
    EventUnregister(EVENT_ID_FOCUS, SI3::OnFocusChanged);
    SI3::ShutDownFootstepSounds();
    SI3::ShutDownEmoteSounds();
    SI3::DSP_ShutDown();
    SE3::ShutDown();
    SI3::FreeZoneSoundOverrides();
    SI3::FreeWorldStateZoneSoundOverrides();
    SI3::FreePhaseShiftZoneSoundOverrides();

    if (!restart) {
        SE3::EmitterManagerShutDown();
    }

    SE3::Log_Write("=> Shutting Down SI3 COMPLETE.");
    SE3::Log_Write(" ");
    SE3::Log_ShutDown();
}

// OFFSET: 0x0062FC30
static void _UpdateGameSoundOutputDevice() {
    char name[256];

    int32_t count = SE3::GetNumOutputDrivers_Cached(SE3::sm_pGameSystem);
    int32_t i;

    for (i = 0; i < count; i++) {
        SE3::GetOutputDriverName_Cached(i, name, sizeof(name), SE3::sm_pGameSystem);

        if (!strcmp(SE3::GetCurrentGameOutputDeviceName(), name)) {
            break;
        }
    }

    if (i < count) {
        if (!SI3::sm_pCVar_Sound_OutputDriverIndex || SI3::sm_pCVar_Sound_OutputDriverIndex->m_intValue) {
            return;
        }

        SE3::GetOutputDriverName_Cached(1, name, sizeof(name), SE3::sm_pGameSystem);

        if (!strcmp(SE3::GetCurrentGameOutputDeviceName(), name)) {
            return;
        }
    }

    if (SI3::sm_pCVar_Sound_OutputDriverIndex) {
        SI3::sm_pCVar_Sound_OutputDriverIndex->Set("0", true, false, false, true);
    }

    SI3::ShutDown(count != 0);
    SI3::Init(count != 0);

    if (ClientServices::Connection()->m_inGame) {
        SI3::InitSoundInGame();
    }
}

// OFFSET: 0x0062FD90
static int32_t _HandleSoundDeviceChange(const void* data, void* param) {
    SI3::VoiceChat_StopPlayingLoopbackSound();
    EventUnregister(EVENT_ID_POLL, _HandleSoundDeviceChange);
    CSimpleMovieFrame::AudioDeviceChanged_ShutDown();
    _UpdateGameSoundOutputDevice();

    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_SOUND_DEVICE_UPDATE, NULL);
    }

    CSimpleMovieFrame::AudioDeviceChanged_Restart();
    return 1;
}

// OFFSET: 0x0062FDE0
static void _RegisterSoundDeviceChange() {
    EventRegister(EVENT_ID_POLL, _HandleSoundDeviceChange);
}

// OFFSET: 0x0062FDF0/0x1402C4740
void SI3::Init(int32_t restart) {
    if (!restart) {
        SE3::EmitterManagerInit(SI3::PosToWorldChunkCoords, 4, 3);
    }

    SE3::Log_Init();
    SE3::Log_Write("=> SI3 Init");
    SE3::Log_Write("=> WoW Version %s (%s) %s", "4.3.4", "15595", "Apr 10 2012");
    SI3::RegisterCVars();
    SI3::sm_uNextZoneMusicPlayTime = 0;

    if (CmdLineGetBool(static_cast<CMDOPT>(26))) {
        SE3::Log_Write("Sound Disabled by command line. Init Silence:");
        SE3::InitGameSystemForSilence();
        return;
    }

    SE3EngineInitParams params;
    params.m_sfxVolume = SI3::sm_pCVar_Sound_SFXVolume->m_floatValue;
    params.m_musicVolume = SI3::sm_pCVar_Sound_MusicVolume->m_floatValue;
    params.m_ambienceVolume = SI3::sm_pCVar_Sound_AmbienceVolume->m_floatValue;
    params.m_masterVolume = SI3::sm_pCVar_Sound_MasterVolume->m_floatValue;
    params.m_sfxEnabled = SI3::sm_pCVar_Sound_EnableSFX->m_intValue;
    params.m_musicEnabled = SI3::sm_pCVar_Sound_EnableMusic->m_intValue;
    params.m_ambienceEnabled = SI3::sm_pCVar_Sound_EnableAmbience->m_intValue;
    params.m_masterEnabled = SI3::sm_pCVar_Sound_EnableAllSound->m_intValue;
    params.m_hardware = SI3::sm_pCVar_Sound_EnableHardware->m_intValue;
    params.m_reverb = SI3::sm_pCVar_Sound_EnableReverb->m_intValue;
    params.m_softwareHRTF = SI3::sm_pCVar_Sound_EnableSoftwareHRTF->m_intValue;
    params.unk_0030 = 0;
    params.m_resampleMethod = SI3::sm_pCVar_Sound_OutputQuality->m_intValue;
    params.m_maxSoftwareChannels = 512;
    params.m_numChannels = SI3::sm_pCVar_Sound_NumChannels->m_intValue;
    params.m_mixMode = static_cast<SE3MIXMODE2>(SI3::sm_pCVar_Sound_EnableMixMode2->m_intValue);
    params.m_dspBufferSize = SI3::sm_pCVar_Sound_DSPBufferSize->m_intValue;
    params.m_outputDriverIndex = SI3::sm_pCVar_Sound_OutputDriverIndex->m_intValue;
    params.m_outputDriverName = SI3::sm_pCVar_Sound_OutputDriverName->m_stringValue.m_str;
    params.unk_0064 = NULL;
    params.m_fpGetDayProgression = _GetDayProgressionCallback;
    params.m_millisecondsPerDay = 86400000;
    params.m_systemDefaultName = "";
    params.m_forceStereo = 0;

    if (SI3::sm_pCVar_Sound_OutputSampleRate) {
        int32_t sampleRate = SI3::sm_pCVar_Sound_OutputSampleRate->m_intValue;
        params.m_sampleRate = sampleRate == 22050 || sampleRate == 44100 || sampleRate == 48000 ? sampleRate : 44100;
    }

    SE3::Log_Write(" - SE3 Init...");

    if (SE3::Init(&params)) {
        SE3::Log_Write(" - ################################################################################################");
        SE3::Log_Write(" - #### Failed to init with user settings. Attempting to init using the most compatible settings...");
        SE3::Log_Write(" - ################################################################################################");
        SE3::CleanUpFMODGameSystem();

        params.m_hardware = 0;
        params.m_softwareHRTF = 0;
        params.m_reverb = 0;
        params.m_resampleMethod = 1;
        params.m_maxSoftwareChannels = 256;
        params.m_numChannels = 32;
        params.m_dspBufferSize = 0;
        params.m_forceStereo = 1;
        params.m_sampleRate = 44100;

        if (SE3::Init(&params)) {
            SE3::Log_Write(" - SE3 Init Failed.");
            SE3::CleanUpFMODGameSystem();
            SE3::InitGameSystemForSilence();
            return;
        }
    }

    SE3::Log_Write(" - SE3 Init Success.");
    EventRegister(EVENT_ID_POLL, SI3::HeartBeat);
    EventRegister(EVENT_ID_FOCUS, SI3::OnFocusChanged);

    if (SI3::sm_pCVar_Sound_EnableDSPEffects && SI3::sm_pCVar_Sound_EnableDSPEffects->m_intValue) {
        SI3::DSP_Init();
    }

    SE3::SetDeviceChangeCallback(_RegisterSoundDeviceChange);
    SE3::Log_Write("=> SI3 Init Complete.");
    SE3::Log_Write(" ");
}

// OFFSET: [0x00401000/0x1407D7E80]
void SI3::SetVolume(uint32_t handle, float volume) {
}
