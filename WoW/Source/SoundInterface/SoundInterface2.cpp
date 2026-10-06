#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <new>
#include "Engine/Source/Base/CmdLine.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Frame/CSimpleMovieFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "Engine/Source/Tempest/CRange.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ResistancesRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Game/GameClient/ComSatClient.h"
#include "WoW/Source/Game/GameTime.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"
#include "WoW/Source/SoundInterface/SoundInterface2Internal.h"
#include "WoW/Source/SoundInterface/SoundInterface2ZoneSounds.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WorldClient/AreaList.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: #0x00B24180
static const char* s_raceFileNames[] = { NULL, "Human", "Orc", "Dwarf", "NightElf", "Scourge", "Tauren", "Gnome", "Troll", "Goblin", "BloodElf", "Draenei" };

const SE3BUS SI2::s_channelGroupBus[18] = {
    SE3BUS_SFX,
    SE3BUS_MUSIC,
    SE3BUS_AMBIENCE,
    SE3BUS_4,
    SE3BUS_5,
    SE3BUS_6,
    SE3BUS_8,
    SE3BUS_9,
    SE3BUS_10,
    SE3BUS_11,
    SE3BUS_12,
    SE3BUS_13,
    SE3BUS_14,
    SE3BUS_15,
    SE3BUS_16,
    SE3BUS_17,
    SE3BUS_18,
    SE3BUS_19,
};

// STATIC: #0x00AC384C
static float s_subchunkRound = 0.5f;

TSGrowableArray<SI2AttachedSound> SI2::s_attachedSounds;

// STATIC: #0x00B4A3A0
static const char* s_savedVoiceSession;

// STATIC: #0x00AC3860
static int32_t s_savedVoiceSessionType = 4;

// STATIC: #0x00B4A3E4
static float s_chaosRestartTimer;

// STATIC: #0x00B4A3E8
static float s_chaosTimer;

// STATIC: #0x00B4A3EC
static int32_t s_chaosElapsedMs;

// STATIC: #0x00B4AA94
int32_t SI2::sm_bPlayerInside;

// STATIC: #0x00B4AA98
int32_t SI2::sm_bCameraIsUnderwater;

// STATIC: #0x00B4AA9C
int32_t SI2::sm_bUnderwaterChanged;

// STATIC: #0x00B4AAA0
uint32_t SI2::sm_nextZoneMusicTime;

// STATIC: #0x00B4AAA4
int32_t SI2::sm_bLuaMusicPlaying;

// OFFSET: #0x004C5990
void SE2SoundKitProperties::ResetToDefaults() {
    this->m_volume = -1.0f;
    this->m_volumeScale = 1.0f;
    this->m_fileIndex = -1;
    this->m_priority = -1;
    this->m_fadeInTime = 0.0f;
    this->m_fadeOutTime = 0.0f;
    this->m_group = 0;
    this->m_selectMode = 2;
    this->m_maxDistance = -1.0f;
    this->m_loopMode = 0;
    this->m_muteFadeSpeed = 0.5f;
    this->m_suppressorMode = 0;
    this->m_decodeBufferSize = 0;
    this->m_maxAudibleBehavior = 1;
    this->m_coneInsideAngle = 360.0f;
    this->m_reverb = 1;
    this->m_coneOutsideAngle = 360.0f;
    this->m_streaming = false;
    this->m_noRandomStart = 0;
    this->m_blockMusicFiles = 0;
    this->m_software = 0;
    this->m_coneOutsideVolume = 1.0f;
    this->m_coneOrientation = NULL;
    this->m_useDspChain = 0;
    this->m_forceLoop = 0;
    this->m_checkStreaming = SFile::IsStreamingMode() != 0;
}

// OFFSET: #0x004C5A20
float SI2::GetChannelGroupVolume(int32_t group) {
    return SE3::sm_aSoundBus[SI2::s_channelGroupBus[group]].m_volume;
}

// OFFSET: #0x004C5A40
void SI2::SetChannelGroupVolume(int32_t group, float volume) {
    SE3::SoundBusSetVolume(SI2::s_channelGroupBus[group], volume);
}

// OFFSET: #0x004C5A60
int32_t SI2::GetNumInputDrivers(int32_t chatSystem) {
    return SE3::GetNumInputDrivers(chatSystem ? SE3::sm_pChatSystem : SE3::sm_pGameSystem);
}

// OFFSET: #0x004C5A70
int32_t SI2::GetNumInputDrivers_Cached(int32_t chatSystem) {
    return SE3::GetNumInputDrivers(chatSystem ? SE3::sm_pChatSystem : SE3::sm_pGameSystem);
}

// OFFSET: #0x004C5A80
bool SI2::GetInputDriverName(int32_t index, char* name, int32_t nameLength, int32_t chatSystem) {
    return SE3::GetInputDriverName(index, name, nameLength, chatSystem ? SE3::sm_pChatSystem : SE3::sm_pGameSystem) != 0;
}

// OFFSET: #0x004C5A90
bool SI2::GetInputDriverName_Cached(int32_t index, char* name, int32_t nameLength, int32_t chatSystem) {
    return SE3::GetInputDriverName_Cached(index, name, nameLength, chatSystem ? SE3::sm_pChatSystem : SE3::sm_pGameSystem) != 0;
}

// OFFSET: #0x004C5AA0
int32_t SI2::GetNumOutputDrivers(int32_t chatSystem) {
    return SE3::GetNumOutputDrivers_Cached(chatSystem ? SE3::sm_pChatSystem : SE3::sm_pGameSystem);
}

// OFFSET: #0x004C5AB0
int32_t SI2::GetNumOutputDrivers_Cached(int32_t chatSystem) {
    return SE3::GetNumOutputDrivers_Cached(chatSystem ? SE3::sm_pChatSystem : SE3::sm_pGameSystem);
}

// OFFSET: #0x004C5AC0
bool SI2::GetOutputDriverName(int32_t index, char* name, int32_t nameLength, int32_t chatSystem) {
    if (index < 0) {
        index = 0;
    }

    return SE3::GetOutputDriverName(index, name, nameLength, chatSystem ? SE3::sm_pChatSystem : SE3::sm_pGameSystem) != 0;
}

// OFFSET: #0x004C5AF0
bool SI2::GetOutputDriverName_Cached(int32_t index, char* name, int32_t nameLength, int32_t chatSystem) {
    if (index < 0) {
        index = 0;
    }

    return SE3::GetOutputDriverName_Cached(index, name, nameLength, chatSystem ? SE3::sm_pChatSystem : SE3::sm_pGameSystem) != 0;
}

// OFFSET: #0x004C5B20
int32_t SI2::SetListenerAttributes(const C3Vector* position, const C3Vector* forward, const C3Vector* up, const C3Vector* velocity) {
    SE3::SetListenerAttributes(*position, *forward, *up, velocity);
    return 0;
}

// OFFSET: #0x004C5B40
int32_t SI2::GetListenerPosition(C3Vector* position) {
    if (!position) {
        return 1;
    }

    SE3::GetListenerPosition(position);
    return 0;
}

// OFFSET: #0x004C5B80
void SI2::StopAllSounds() {
    SE3::StopAllSounds(SE3BUS_ALL, 0.0f);
}

// OFFSET: #0x004C5B90
int32_t SI2::Set3DPosition(SE2SoundKitObject* object, const C3Vector* position) {
    SE3::Set3DPosition(object->m_handle, *position);
    return 0;
}

// PROVEN: 56358c3c IDENTICAL/NONE
// OFFSET: 0x00401000
void SI2::SetVolume(uint32_t handle, float volume) {
}

// OFFSET: #0x004C5BE0
bool SI2::IsPlaying(SE2SoundKitObject* object) {
    return SE3::IsPlaying(object->m_handle) != 0;
}

// OFFSET: #0x004C5BF0
uint32_t SI2::IsSoundKitLooping(int32_t soundKitId) {
    return SE3::IsLooping_ByDef(soundKitId);
}

// OFFSET: #0x004C5C10
int32_t SI2::GetSoundKitUsage(int32_t soundKitId) {
    return SE3::GetSoundKitUsage(soundKitId);
}

// OFFSET: #0x004C5C40
bool SI2::IsLooping(SE2SoundKitObject* object) {
    return SE3::IsLoopingOrPeriodic(object->m_handle) != 0;
}

// OFFSET: #0x004C5C60
int32_t SI2::GetNumSoundsInSoundKit(int32_t soundKitId) {
    return SE3::GetNumSoundsInSoundKit(soundKitId);
}

// OFFSET: #0x004C5C80
int32_t SI2::AttachToObject(SE2SoundKitObject* object, WOWGUID guid) {
    if (!object->m_handle) {
        return 12;
    }

    for (uint32_t i = 0; i < SI2::s_attachedSounds.Count(); i++) {
        if (SI2::s_attachedSounds[i].m_handle == object->m_handle) {
            SI2::s_attachedSounds[i].m_guid = guid.m_guid;
            return 0;
        }
    }

    SI2AttachedSound* attached = SI2::s_attachedSounds.New();
    attached->m_handle = object->m_handle;
    attached->m_guid = guid.m_guid;
    return 0;
}

// OFFSET: #0x004C5CC0
void SI2::StopAllSounds(float fadeOutTime) {
    SE3::StopAllSounds(SE3BUS_SFX, fadeOutTime);
}

// OFFSET: #0x004C5CE0
int32_t SI2ZoneSoundPair::GetCurrentType() {
    return this->m_sounds[this->m_current].m_type;
}

// OFFSET: #0x004C5D00
bool SI2ZoneSoundPair::IsPlaying() {
    return SE3::IsPlaying(this->m_sounds[this->m_current].m_object.m_handle) != 0;
}

// OFFSET: #0x004C5D20
void SI2ZoneSoundPair::SetFinishedCallback(void (*callback)(SE3SoundKitObject*)) {
    SE3::SetPlayFinishedCallback(this->m_sounds[this->m_current].m_object.m_handle, callback);
}

// OFFSET: #0x004C5DC0
int32_t SI2::OnFocusChanged(const void* data, void* param) {
    static CVar* s_enableSoundWhenGameIsInBG = CVar::Lookup("Sound_EnableSoundWhenGameIsInBG");

    CVar* enableVoiceChat = CVar::Lookup("EnableVoiceChat");
    const int32_t* focus = static_cast<const int32_t*>(data);

    if (!focus) {
        return 1;
    }

    if (*focus) {
        SE3::SetFocusScalar(1.0f);

        if (CGGameUI::m_initialized && enableVoiceChat && enableVoiceChat->m_intValue && s_savedVoiceSession) {
            CGChat::SetActiveVoiceSession(s_savedVoiceSession, s_savedVoiceSessionType, -1);
            s_savedVoiceSession = NULL;
            s_savedVoiceSessionType = 4;
        }
    } else if (s_enableSoundWhenGameIsInBG->m_intValue) {
        SE3::SetFocusScalar(1.0f);
    } else {
        SE3::SetFocusScalar(0.0f);

        if (CGGameUI::m_initialized && !s_savedVoiceSession) {
            s_savedVoiceSession = CGChat::GetChannelStringForDisplay(CGChat::GetActiveVoiceSession(), &s_savedVoiceSessionType);
            CGChat::SetActiveVoiceSession(NULL, 4, -1);
        }
    }

    return 1;
}

// OFFSET: #0x004C5F00
static C3Vector RandomListenerOffset() {
    C3Vector listener;
    listener.x = 0.0f;
    listener.y = 0.0f;
    listener.z = 0.0f;

    float scale = CRandom::sfloat_(g_rndSeed);
    C3Vector direction = CRandom::C3Vector_(g_rndSeed);
    float x = direction.x * scale * 50.0f;
    float y = direction.y * scale * 50.0f;
    float z = direction.z * scale * 50.0f;

    SE3::GetListenerPosition(&listener);

    return C3Vector(x + listener.x, y + listener.y, z + listener.z);
}

// OFFSET: #0x004C6230
int32_t SI2::ShutDown(int32_t restart) {
    SE3::Log_Write(" ");
    SE3::Log_Write("=> Shutting Down SI3");
    EventUnregister(EVENT_ID_POLL, SI2::HeartBeat);
    EventUnregister(EVENT_ID_FOCUS, SI2::OnFocusChanged);

    if (!restart) {
        SI2::FreeWorldStateZoneSoundOverrides();
        SI2::FreeZoneSoundOverrides();
        SI2::FreeFootstepSounds();
        SI2::FreeEmoteSounds();
    }

    SI2::DSP_ShutDown();
    SE3::ShutDown();
    SI2::s_attachedSounds.Clear();

    if (!restart) {
        SE3::EmitterManagerShutDown();
    }

    SE3::Log_Write("=> Shutting Down SI3 COMPLETE.");
    SE3::Log_Write(" ");
    SLogFlush(SE3::sm_log);
    SLogClose(SE3::sm_log);

    return 0;
}

// OFFSET: #0x004C6330
int32_t SI2::ResetSoundKitVariants(const char* name) {
    int32_t soundKitId = SI2::LookupSoundKitId(name);

    if (!soundKitId) {
        return 7;
    }

    return SE3::ResetSoundKitOrder(soundKitId);
}

// OFFSET: #0x004C6390
int32_t SI2::StopSound(SE2SoundKitObject* object, int32_t immediate, float fadeOutTime, int32_t stopAdvanced) {
    int32_t fadeOutMs = -1;

    if (immediate) {
        fadeOutMs = 0;
    } else if (fadeOutTime >= 0.0f) {
        fadeOutMs = static_cast<int32_t>(fadeOutTime * 1000.0f);
    }

    SE3::StopSound(object->m_handle, fadeOutMs, __FILE__, __LINE__, "SI2::StopSound");
    SE3::VoiceChat_StopSound(object->m_handle);
    return 0;
}

// OFFSET: #0x004C6410
int32_t SI2ZoneSoundPair::GetPlayingSoundKitId() {
    if (SE3::IsPlaying(this->m_sounds[this->m_current].m_object.m_handle)) {
        return this->m_sounds[this->m_current].m_soundKitId;
    }

    return 0;
}

// OFFSET: #0x004C6450
void SI2ZoneSoundPair::Stop(int32_t immediate) {
    SI2::StopSound(&this->m_sounds[this->m_current].m_object, immediate, -1.0f, 1);
    this->m_sounds[this->m_current].m_type = 13;
    this->m_sounds[this->m_current].m_soundKitId = 0;

    SI2::StopSound(&this->m_sounds[this->m_next].m_object, immediate, -1.0f, 1);
    this->m_sounds[this->m_next].m_type = 13;
    this->m_sounds[this->m_next].m_soundKitId = 0;
}

// OFFSET: %0x0063E670
void SI2::ValidateOutputDriver() {
    char name[256];

    int32_t count = SE3::GetNumOutputDrivers_Cached(SE3::sm_pGameSystem);
    int32_t i;

    for (i = 0; i < count; i++) {
        SE3::GetOutputDriverName_Cached(i, name, sizeof(name), SE3::sm_pGameSystem);

        if (!strcmp(SE3::GetCurrentGameOutputDeviceName(), name)) {
            break;
        }
    }

    static CVar* s_outputDriverIndex = CVar::Lookup("Sound_OutputDriverIndex");

    if (i < count) {
        if (!s_outputDriverIndex || s_outputDriverIndex->m_intValue) {
            return;
        }

        SE3::GetOutputDriverName_Cached(1, name, sizeof(name), SE3::sm_pGameSystem);

        if (!strcmp(SE3::GetCurrentGameOutputDeviceName(), name)) {
            return;
        }
    }

    if (s_outputDriverIndex) {
        s_outputDriverIndex->Set("0", true, false, false, true);
    }

    int32_t restart = count != 0;
    SI2::ShutDown(restart);
    SI2::Init(restart);
}

// OFFSET: #0x004C6700
void SI2::RegisterDeviceChangeHandler() {
    EventRegister(EVENT_ID_POLL, SI2::HandleSoundDeviceChange);
}

// OFFSET: #0x004C6A40
int32_t SI2::PlaySoundKit(int32_t soundKitId, const C3Vector* position, SE2SoundKitObject* object, SE2SoundKitProperties* properties, uint32_t advancedId, uint32_t* soundId, int32_t checkDistance, int32_t keepPlaying) {
    if (soundKitId <= 0) {
        return 5;
    }

    static CVar* s_enableAllSound = CVar::Lookup("Sound_EnableAllSound");

    if (!s_enableAllSound || !s_enableAllSound->m_intValue || CGGameUI::m_inMovie) {
        return 17;
    }

    SE2SoundKitProperties defaults;
    defaults.ResetToDefaults();

    if (!properties) {
        properties = &defaults;
    }

    if (properties->m_group >= 18) {
        return 8;
    }

    if (CGChat::IsPaused() && !position) {
        return 9;
    }

    if (object && !keepPlaying) {
        SI2::StopSound(object, 1, -1.0f, 1);
    }

    SE3SoundKitProperties kitProperties;
    kitProperties.m_bus = SI2::s_channelGroupBus[properties->m_group];
    kitProperties.m_nIndexOverride = properties->m_fileIndex;
    kitProperties.m_fadeInTime = static_cast<int32_t>(properties->m_fadeInTime * 1000.0f);
    kitProperties.m_fadeOutTime = static_cast<int32_t>(properties->m_fadeOutTime * 1000.0f);
    kitProperties.m_selectMode = properties->m_selectMode;
    kitProperties.m_usage = object ? properties->m_loopMode : SE3USAGE_ONESHOT;
    kitProperties.m_uniqueness = properties->m_suppressorMode;
    kitProperties.unk_002C = properties->m_maxDistance;
    kitProperties.m_maxAudibleBehavior = properties->m_maxAudibleBehavior;
    kitProperties.m_muteFadeSpeed = properties->m_muteFadeSpeed;
    kitProperties.m_reverb = properties->m_reverb != 0;
    kitProperties.m_noRandomStart = properties->m_noRandomStart != 0 || properties->m_group == 1 || properties->m_group == 5;
    kitProperties.m_software = properties->m_software != 0;
    kitProperties.m_checkRange = checkDistance != 0;

    if (properties->m_priority >= 0) {
        kitProperties.m_priority = properties->m_priority;
    }

    if (properties->m_volume >= 0.0f && properties->m_volume <= 1.0f) {
        kitProperties.m_volume = properties->m_volume;
    } else {
        kitProperties.m_volume = properties->m_volumeScale;
    }

    if (properties->m_coneOrientation) {
        kitProperties.m_coneOrientation = *properties->m_coneOrientation;
    }

    if (properties->m_forceLoop) {
        kitProperties.m_usage = SE3USAGE_PERIODIC;
        kitProperties.m_maxAudibleBehavior = 0;
        kitProperties.m_software = 1;
    }

    if (properties->m_useDspChain) {
        kitProperties.m_attachCustomDSPChain = 1;
        kitProperties.m_fpCustomDSPChainCallback = SI2::DSP_AttachCustomDspChain;
        SStrCopy(kitProperties.m_sCustomDSPChainName, properties->m_dspChainName, sizeof(kitProperties.m_sCustomDSPChainName));
    }

    uint32_t handle = 0;
    uint32_t* target = object ? &object->m_handle : &handle;
    int32_t result = SE3::PlaySoundKit(soundKitId, position, &kitProperties, target, __FILE__, __LINE__, "SI2::PlaySoundKit");

    if (soundId) {
        *soundId = *target;
    }

    return result;
}

// OFFSET: #0x004C7140
int32_t SI2ZoneSoundPair::Transition(int32_t soundKitId, int32_t type, SE2SoundKitProperties* properties, int32_t immediate, float fadeOutTime) {
    if (type >= 11) {
        SI2::StopSound(&this->m_sounds[this->m_current].m_object, 0, -1.0f, 1);
        this->m_sounds[this->m_current].m_type = type;
        this->m_sounds[this->m_current].m_soundKitId = 0;
        return 18;
    }

    if (immediate) {
        SI2::StopSound(&this->m_sounds[this->m_current].m_object, 1, -1.0f, 1);
        SI2::StopSound(&this->m_sounds[this->m_next].m_object, 1, -1.0f, 1);

        int32_t result = SI2::PlaySoundKit(soundKitId, NULL, &this->m_sounds[this->m_current].m_object, properties, 0, NULL, 1, 0);

        if (!result) {
            this->m_sounds[this->m_current].m_soundKitId = soundKitId;
            this->m_sounds[this->m_current].m_type = type;
            return 0;
        }

        return result;
    }

    if (soundKitId == this->m_sounds[this->m_current].m_soundKitId && SE3::IsPlaying(this->m_sounds[this->m_current].m_object.m_handle)) {
        return 15;
    }

    if (soundKitId == this->m_sounds[this->m_next].m_soundKitId && SE3::IsPlaying(this->m_sounds[this->m_next].m_object.m_handle)) {
        SI2::StopSound(&this->m_sounds[this->m_current].m_object, 0, fadeOutTime, 1);
        SE3::FadeInNow(this->m_sounds[this->m_next].m_object.m_handle);

        uint32_t current = this->m_current;
        this->m_next = current;
        this->m_current = 1 - current;
        return 21;
    }

    SI2::StopSound(&this->m_sounds[this->m_next].m_object, 0, -1.0f, 1);
    SI2::StopSound(&this->m_sounds[this->m_current].m_object, 0, fadeOutTime, 1);

    int32_t result = SI2::PlaySoundKit(soundKitId, NULL, &this->m_sounds[this->m_next].m_object, properties, 0, NULL, 1, 0);

    if (!result) {
        this->m_sounds[this->m_next].m_soundKitId = soundKitId;
        this->m_sounds[this->m_next].m_type = type;

        uint32_t current = this->m_current;
        this->m_next = current;
        this->m_current = 1 - current;
        return 0;
    }

    return result;
}

// OFFSET: #0x004C7410
SE2SoundKitObject::SE2SoundKitObject() {
    this->m_handle = 0;
    this->unk_000C = 0;
    this->unk_0010 = 0;
}

// OFFSET: #0x004C7440
SE2SoundKitObject::~SE2SoundKitObject() {
}

// OFFSET: #0x004C74A0
int32_t SI2::PlaySoundKit(const char* name, const C3Vector* position, SE2SoundKitObject* object, SE2SoundKitProperties* properties) {
    int32_t soundKitId = SI2::LookupSoundKitId(name);

    if (!soundKitId) {
        return 7;
    }

    return SI2::PlaySoundKit(soundKitId, position, object, properties, 0, NULL, 1, 0);
}

// OFFSET: #0x004C74F0
int32_t SI2::ChaosModeUpdate(const void* data, void* param) {
    static CVar* s_chaosMode = CVar::Lookup("Sound_ChaosMode");

    int32_t mode = 0;

    if (s_chaosMode) {
        mode = s_chaosMode->m_intValue;
    }

    static uint32_t s_maxSoundKitId = g_soundEntriesDB.m_maxID;

    C3Vector position;
    position.x = 0.0f;
    position.y = 0.0f;
    position.z = 0.0f;

    uint32_t soundKitId = static_cast<uint32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * s_maxSoundKitId) >> 32);

    static SE2SoundKitObject s_object2D;
    static SE2SoundKitObject s_object3D;
    static SE2SoundKitObject s_loopObjects[80];
    static uint32_t s_lastTime = OsGetAsyncTimeMs();

    uint32_t now = OsGetAsyncTimeMs();
    s_chaosElapsedMs += now - s_lastTime;
    float elapsed = static_cast<int32_t>(now - s_lastTime) * 0.001f;

    SE2SoundKitProperties properties;
    properties.ResetToDefaults();
    properties.m_volumeScale = 0.3f;
    properties.m_loopMode = 2;
    properties.m_streaming = false;
    properties.m_blockMusicFiles = 1;

    SE2SoundKitProperties loopProperties;
    loopProperties.ResetToDefaults();
    loopProperties.m_volumeScale = 0.3f;
    loopProperties.m_loopMode = 1;
    loopProperties.m_streaming = false;
    loopProperties.m_blockMusicFiles = 1;

    switch (mode) {
        case 2:
            if (s_chaosElapsedMs < 100) {
                break;
            }

            properties.m_streaming = true;
            position = RandomListenerOffset();
            SI2::PlaySoundKit(static_cast<int32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * s_maxSoundKitId) >> 32), &position, NULL, &properties, 0, NULL, 1, 0);

        case 1:
            if (s_chaosElapsedMs >= 100) {
                properties.m_streaming = false;
                position = RandomListenerOffset();
                SI2::PlaySoundKit(static_cast<int32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * s_maxSoundKitId) >> 32), &position, NULL, &properties, 0, NULL, 1, 0);
                s_chaosElapsedMs = 0;
            }

            break;

        case 13:
            if (s_chaosElapsedMs < 100) {
                break;
            }

            properties.m_streaming = true;
            position = RandomListenerOffset();
            SI2::PlaySoundKit(static_cast<int32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * s_maxSoundKitId) >> 32), &position, NULL, &properties, 0, NULL, 1, 0);
            properties.m_streaming = false;
            position = RandomListenerOffset();
            SI2::PlaySoundKit(static_cast<int32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * s_maxSoundKitId) >> 32), &position, NULL, &properties, 0, NULL, 1, 0);

        case 12:
            if (s_chaosElapsedMs < 100) {
                break;
            }

            properties.m_streaming = true;
            SI2::PlaySoundKit(static_cast<int32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * s_maxSoundKitId) >> 32), NULL, NULL, &properties, 0, NULL, 1, 0);

        case 11:
            if (s_chaosElapsedMs >= 100) {
                properties.m_streaming = false;
                SI2::PlaySoundKit(static_cast<int32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * s_maxSoundKitId) >> 32), NULL, NULL, &properties, 0, NULL, 1, 0);
                s_chaosElapsedMs = 0;
            }

            break;

        case 4:
            SI2::PlaySoundKit(soundKitId, NULL, &s_object2D, &properties, 0, NULL, 1, 0);
            SI2::StopSound(&s_object2D, 1, -1.0f, 1);
            SI2::PlaySoundKit(soundKitId, NULL, &s_object3D, &properties, 0, NULL, 1, 0);
            SI2::StopSound(&s_object3D, 1, -1.0f, 1);

        case 3:
            SI2::PlaySoundKit(soundKitId, NULL, &s_object2D, &properties, 0, NULL, 1, 0);
            SI2::StopSound(&s_object2D, 1, -1.0f, 1);
            position = RandomListenerOffset();
            SI2::PlaySoundKit(soundKitId, &position, &s_object3D, &properties, 0, NULL, 1, 0);
            SI2::StopSound(&s_object3D, 1, -1.0f, 1);
            break;

        case 5:
            s_chaosTimer += elapsed;

            if (s_chaosTimer <= 1.0f) {
                break;
            }

            for (int32_t i = 80; i; i--) {
                SI2::PlaySoundKit(soundKitId, NULL, NULL, &properties, 0, NULL, 1, 0);
            }

            s_chaosTimer = 0.0f;
            break;

        case 6:
            s_chaosTimer += elapsed;

            if (s_chaosTimer <= 1.0f) {
                break;
            }

            for (int32_t i = 80; i; i--) {
                position = RandomListenerOffset();
                SI2::PlaySoundKit(soundKitId, &position, NULL, &properties, 0, NULL, 1, 0);
            }

            s_chaosTimer = 0.0f;
            break;

        case 7:
            s_chaosTimer += elapsed;

            if (s_chaosTimer <= 1.0f) {
                break;
            }

            for (SE2SoundKitObject* object = s_loopObjects; object < &s_loopObjects[80]; object++) {
                if (!SI2::IsPlaying(object)) {
                    position = RandomListenerOffset();
                    SI2::PlaySoundKit(soundKitId, &position, object, &loopProperties, 0, NULL, 1, 0);
                }
            }

            s_chaosTimer = 0.0f;
            break;

        case 8:
            s_chaosTimer += elapsed;

            if (s_chaosTimer <= 0.05f) {
                break;
            }

            printf("----------playing...\n");
            SI2::PlaySoundFile("Data\\Alert.mp3", 4);
            s_chaosTimer = 0.0f;
            break;

        case 9:
            s_chaosRestartTimer += elapsed;

            if (s_chaosRestartTimer <= 10.0f) {
                break;
            }

            DebugLog("##RESTART SOUND ENGINE!!\n");
            SI2::ShutDown(1);
            SI2::Init(1);
            s_chaosRestartTimer = 0.0f;

            {
                static CVar* s_chaosMode9 = CVar::Lookup("Sound_ChaosMode");
                s_chaosMode9->Set("9", true, false, false, true);
            }

            break;

        case 10:
            s_chaosRestartTimer += elapsed;

            if (s_chaosElapsedMs < 100) {
                break;
            }

            s_chaosElapsedMs = 0;
            position = RandomListenerOffset();
            SI2::PlaySoundKit(static_cast<int32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * s_maxSoundKitId) >> 32), &position, NULL, &properties, 0, NULL, 1, 0);
            position = RandomListenerOffset();
            SI2::PlaySoundKit(static_cast<int32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * s_maxSoundKitId) >> 32), &position, NULL, &properties, 0, NULL, 1, 0);

            if (s_chaosRestartTimer <= 10.0f) {
                break;
            }

            DebugLog("##RESTART SOUND ENGINE!!\n");
            SI2::ShutDown(1);
            SI2::Init(1);
            s_chaosRestartTimer = 0.0f;

            {
                static CVar* s_chaosMode10 = CVar::Lookup("Sound_ChaosMode");
                s_chaosMode10->Set("10", true, false, false, true);
            }

            break;

        case 14: {
            s_chaosRestartTimer += elapsed;

            if (s_chaosElapsedMs < 100) {
                break;
            }

            s_chaosElapsedMs = 0;
            uint32_t sameId = static_cast<uint32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * s_maxSoundKitId) >> 32);

            for (int32_t i = 20; i; i--) {
                position = RandomListenerOffset();
                SI2::PlaySoundKit(sameId, &position, NULL, &properties, 0, NULL, 1, 0);
            }

            position = RandomListenerOffset();
            SI2::PlaySoundKit(static_cast<int32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * s_maxSoundKitId) >> 32), &position, NULL, &properties, 0, NULL, 1, 0);

            if (s_chaosRestartTimer <= 10.0f) {
                break;
            }

            DebugLog("##RESTART SOUND ENGINE!!\n");
            SI2::ShutDown(1);
            SI2::Init(1);
            s_chaosRestartTimer = 0.0f;

            static CVar* s_chaosMode14 = CVar::Lookup("Sound_ChaosMode");
            s_chaosMode14->Set("14", true, false, false, true);
            break;
        }

        case 15: {
            int32_t ids[3] = { 13061, 15969, 12173 };

            position = RandomListenerOffset();
            SI2::PlaySoundKit(ids[(static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * 3) >> 32], &position, &s_object3D, &properties, 0, NULL, 1, 0);
            position = RandomListenerOffset();
            SI2::PlaySoundKit(ids[(static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * 3) >> 32], &position, &s_object3D, &properties, 0, NULL, 1, 0);
            position = RandomListenerOffset();
            SI2::PlaySoundKit(ids[(static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * 3) >> 32], &position, &s_object3D, &properties, 0, NULL, 1, 0);
            position = RandomListenerOffset();
            SI2::PlaySoundKit(ids[(static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * 3) >> 32], &position, &s_object3D, &properties, 0, NULL, 1, 0);
            position = RandomListenerOffset();
            SI2::PlaySoundKit(ids[(static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * 3) >> 32], &position, &s_object3D, &properties, 0, NULL, 1, 0);
            position = RandomListenerOffset();
            SI2::PlaySoundKit(ids[(static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * 3) >> 32], &position, &s_object3D, &properties, 0, NULL, 1, 0);
            position = RandomListenerOffset();
            SI2::PlaySoundKit(ids[(static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * 3) >> 32], &position, &s_object3D, &properties, 0, NULL, 1, 0);
            position = RandomListenerOffset();
            SI2::PlaySoundKit(ids[(static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * 3) >> 32], &position, &s_object3D, &properties, 0, NULL, 1, 0);

            break;
        }

        default:
            break;
    }

    s_lastTime = now;
    return 1;
}

// OFFSET: %0x0063F750
static void PosToWorldChunkCoords(const C3Vector& position, uint32_t* x, uint32_t* y) {
    int64_t chunkY = static_cast<int64_t>(-(position.y - 17066.666f) * 0.029999999f);
    int64_t chunkX = static_cast<int64_t>(0.029999999 * -(position.x - 17066.666f));
    *x = static_cast<uint32_t>(((chunkY & 0xF) + 16 * ((static_cast<uint32_t>(chunkY) >> 4) & 0x3F)) >> 2);
    *y = static_cast<uint32_t>(((chunkX & 0xF) + 16 * ((static_cast<uint32_t>(chunkX) >> 4) & 0x3F)) >> 2);
}

// PROVEN: 7b31778d IDENTICAL/NONE
// OFFSET: 0x0062F9E0
static int32_t GetDayProgression() {
    return static_cast<int32_t>(g_clientGameTime.GameTimeGetDayProgression() * 86400000.0);
}

// OFFSET: %0x0063E830
int32_t SI2::Init(int32_t restart) {
    if (!restart) {
        SE3::EmitterManagerInit(PosToWorldChunkCoords, 4, 3);
    }

    SE3::Log_Init();
    SE3::Log_Write("=> SI3 Init");

    if (!restart) {
        SI2::InitFootstepSounds();
        SI2::InitEmoteSounds();
        SI2::InitWeaponImpactCombatSounds();
        SI2::InitWeaponSwingCombatSounds();
        SI2::RegisterSoundEngineCVars();
    }

    SI2::RegisterCVars();

    if (CmdLineGetBool(static_cast<CMDOPT>(26))) {
        SE3::Log_Write("Sound Disabled by command line. Init Silence:");
        SE3::InitGameSystemForSilence();
        return 17;
    }

    static CVar* s_sfxVolume = CVar::Lookup("Sound_SFXVolume");
    static CVar* s_musicVolume = CVar::Lookup("Sound_MusicVolume");
    static CVar* s_ambienceVolume = CVar::Lookup("Sound_AmbienceVolume");
    static CVar* s_masterVolume = CVar::Lookup("Sound_MasterVolume");
    static CVar* s_enableSFX = CVar::Lookup("Sound_EnableSFX");
    static CVar* s_enableMusic = CVar::Lookup("Sound_EnableMusic");
    static CVar* s_enableAmbience = CVar::Lookup("Sound_EnableAmbience");
    static CVar* s_enableAllSound = CVar::Lookup("Sound_EnableAllSound");
    static CVar* s_enableHardware = CVar::Lookup("Sound_EnableHardware");
    static CVar* s_enableReverb = CVar::Lookup("Sound_EnableReverb");
    static CVar* s_enableSoftwareHRTF = CVar::Lookup("Sound_EnableSoftwareHRTF");
    static CVar* s_numChannels = CVar::Lookup("Sound_NumChannels");
    static CVar* s_outputDriverIndex = CVar::Lookup("Sound_OutputDriverIndex");
    static CVar* s_outputQuality = CVar::Lookup("Sound_OutputQuality");
    static CVar* s_enableMixMode2 = CVar::Lookup("Sound_EnableMixMode2");
    static CVar* s_dspBufferSize = CVar::Lookup("Sound_DSPBufferSize");
    static CVar* s_outputDriverName = CVar::Lookup("Sound_OutputDriverName");
    static CVar* s_outputSampleRate = CVar::Lookup("Sound_OutputSampleRate");

    SE3EngineInitParams params;
    params.m_sfxVolume = s_sfxVolume->m_floatValue;
    params.m_musicVolume = s_musicVolume->m_floatValue;
    params.m_ambienceVolume = s_ambienceVolume->m_floatValue;
    params.m_masterVolume = s_masterVolume->m_floatValue;
    params.m_sfxEnabled = s_enableSFX->m_intValue;
    params.m_musicEnabled = s_enableMusic->m_intValue;
    params.m_ambienceEnabled = s_enableAmbience->m_intValue;
    params.m_masterEnabled = s_enableAllSound->m_intValue;
    params.m_hardware = s_enableHardware->m_intValue;
    params.m_reverb = s_enableReverb->m_intValue;
    params.m_softwareHRTF = s_enableSoftwareHRTF->m_intValue;
    params.unk_0030 = 0;
    params.m_maxSoftwareChannels = 512;
    params.m_numChannels = s_numChannels->m_intValue;
    params.m_millisecondsPerDay = 86400000;
    params.m_outputDriverIndex = s_outputDriverIndex->m_intValue;
    params.m_resampleMethod = s_outputQuality->m_intValue;
    params.m_mixMode = static_cast<SE3MIXMODE2>(s_enableMixMode2->m_intValue);
    params.m_dspBufferSize = s_dspBufferSize->m_intValue;
    params.m_outputDriverName = s_outputDriverName->m_stringValue.m_str;
    params.m_systemDefaultName = "";
    params.m_forceStereo = 0;
    params.m_fpGetDayProgression = GetDayProgression;

    if (s_outputSampleRate) {
        int32_t sampleRate = s_outputSampleRate->m_intValue;
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
            return 17;
        }
    }

    SE3::Log_Write(" - SE3 Init Success.");
    EventRegister(EVENT_ID_POLL, SI2::HeartBeat);
    EventRegister(EVENT_ID_FOCUS, SI2::OnFocusChanged);

    if (!restart) {
        SI2::InitWorldStateZoneSoundOverrides();
        SI2::InitZoneSoundOverrides();
    }

    if (CmdLineGetBool(static_cast<CMDOPT>(27))) {
        EventRegister(EVENT_ID_POLL, SI2::ChaosModeUpdate);
    }

    static CVar* s_enableDSPEffects = CVar::Lookup("Sound_EnableDSPEffects");

    if (s_enableDSPEffects && s_enableDSPEffects->m_intValue) {
        SI2::DSP_Init();
    }

    SE3::SetDeviceChangeCallback(SI2::RegisterDeviceChangeHandler);
    SE3::Log_Write("=> SI3 Init Complete.");
    SE3::Log_Write(" ");

    return 0;
}

// OFFSET: %0x0063F4F0
int32_t SI2::HeartBeat(const void* data, void* param) {
    uint32_t i = 0;

    while (i < SI2::s_attachedSounds.Count()) {
        SI2AttachedSound* attached = &SI2::s_attachedSounds[i];

        if (!SE3::IsPlaying(attached->m_handle)) {
            *attached = SI2::s_attachedSounds[SI2::s_attachedSounds.Count() - 1];
            SI2::s_attachedSounds.SetCount(SI2::s_attachedSounds.Count() - 1);
            continue;
        }

        CGObject_C* object = ClntObjMgrObjectPtr(WOWGUID::Make(attached->m_guid), TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object) {
            SE3::Set3DPosition(attached->m_handle, object->GetPosition());
        }

        i++;
    }

    return 1;
}

// OFFSET: #0x004CFF30
void SI2::PlaySpellFizzleSound(uint32_t spellId, const void* object) {
    if (!object) {
        return;
    }

    SpellRec spell;

    if (!g_spellDB.GetRecord(spellId, &spell)) {
        return;
    }

    for (int32_t i = 0; i < 7; i++) {
        if (!((1 << i) & spell.m_schoolMask)) {
            continue;
        }

        ResistancesRec* resistance = g_resistancesDB.GetRecord(i);

        if (resistance) {
            C3Vector position = static_cast<const CGObject_C*>(object)->GetPosition();
            SI2::PlaySoundKit(resistance->m_fizzleSoundID, &position, NULL, NULL, 0, NULL, 1, 0);
            return;
        }
    }
}

// OFFSET: #0x004C6710
void SI2::ForceUpdateZoneSounds() {
    CGObject_C* unit = ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!unit) {
        unit = ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__);

        if (!unit) {
            return;
        }
    }

    CMapEntity* entity = unit->m_worldObject;
    uint32_t areaId = 0;

    if (!CMap::QueryAreaId(entity, areaId)) {
        return;
    }

    const AreaTableRec* area = g_areaTableDB.GetRecord(areaId);

    if (!area) {
        return;
    }

    const AreaTableRec* zone;

    if (area->m_parentAreaID) {
        zone = g_areaTableDB.GetRecord(area->m_parentAreaID);
    } else {
        zone = area;
        area = NULL;
    }

    WMOAreaTableRec* groupRec = NULL;
    WMOAreaTableRec* rootRec = NULL;
    int32_t wmoGroupId = 0;
    int32_t inside = World::QueryObjectInside(entity);
    World::QueryMapObjAreaTable(entity, &groupRec, &rootRec, &wmoGroupId);
    AreaListUpdateSounds(zone, area, rootRec, groupRec, inside);
}


// OFFSET: #0x004C6810
int32_t SI2::GetChunkLocation(int32_t* mapId, int32_t* chunkX, int32_t* chunkY, int32_t* subchunkX, int32_t* subchunkY) {
    *mapId = World::QueryMapId();
    CGObject_C* player = ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__);

    if (!player) {
        return 0;
    }

    C3Vector position = player->GetPosition();
    float x = -(position.x - 17066.666f);
    float y = -(position.y - 17066.666f) * 0.029999999f;
    *subchunkX = CMath::fint_(y - s_subchunkRound);
    x *= 0.029999999f;
    *subchunkY = CMath::fint_(x - s_subchunkRound);
    *chunkX = (*subchunkX >> 4) & 0x3F;
    *chunkY = (*subchunkY >> 4) & 0x3F;
    *subchunkX &= 0xF;
    *subchunkY &= 0xF;
    return 1;
}

// OFFSET: #0x004CDA20
void SI2::DK_GetPlayerFilterName(CGUnit_C* unit, char* name, uint32_t size) {
    uint8_t race = unit->m_unit->m_unitFieldBytes0[0];
    SStrPrintf(name, size, "Death Knight %s %s", s_raceFileNames[race], unit->m_unit->m_unitFieldBytes0[2] ? "Female" : "Male");
}

// OFFSET: #0x004C66B0
int32_t SI2::HandleSoundDeviceChange(const void* data, void* param) {
    SI2::VoiceChat_StopPlayingLoopbackSound();
    EventUnregister(EVENT_ID_POLL, SI2::HandleSoundDeviceChange);
    CSimpleMovieFrame::AudioDeviceChanged_ShutDown();
    SI2::ValidateOutputDriver();

    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_SOUND_DEVICE_UPDATE, NULL);
    }

    CSimpleMovieFrame::AudioDeviceChanged_Restart();
    return 1;
}

// OFFSET: #0x004CB3B0
void SI2::ClearSuppressorSounds() {
    SE3::InitSuppressorSounds();
}
