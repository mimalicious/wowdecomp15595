#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include "WoW/Source/SoundInterface/SI3VoiceChat.h"
#include "Engine/Source/Base/CmdLine.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/SoundInterface/SI3.h"

// STATIC: 0x00CC8E68
static SI3LoopbackCapture s_loopbackCapture;

// STATIC: 0x00E28730/0x140D102C0
static uint32_t s_loopbackSound;

// STATIC: 0x00E28734/0x140D102C4
static int32_t s_microphoneSignalLevel;

// STATIC: 0x00E28738/0x140D102C8
static uint8_t* s_loopbackBuffer;

// STATIC: 0x00E2873C/0x140D102D0
static uint8_t* s_loopbackWrite;

// STATIC: 0x00E28740/0x140D102D8
static uint32_t s_loopbackSize;

// OFFSET: 0x00630090/0x1402C4B10
void SI3::VoiceChat_CleanUpLoopbackSound() {
    SE3::StopMemorySound(s_loopbackSound);

    if (s_loopbackBuffer) {
        SMemFree(s_loopbackBuffer, __FILE__, __LINE__, 0);
        s_loopbackBuffer = NULL;
        s_loopbackSize = 0;
    }
}

// OFFSET: 0x006300D0/0x1402C4B60
void SI3::VoiceChat_StopPlayingLoopbackSound() {
    SE3::StopMemorySound(s_loopbackSound);
}

// OFFSET: 0x006300E0/0x1402C4B70
bool SI3::VoiceChat_IsPlayingLoopbackSound() {
    return SE3::IsPlaying(s_loopbackSound) != 0;
}

// OFFSET: 0x006300F0/0x1402C4B80
void SI3::VoiceChat_DestroyChatSystem() {
    SE3::StopMemorySound(s_loopbackSound);
    SE3::DestroyChatSystem();
}

// OFFSET: 0x00630110/0x1402C4BA0
int32_t SI3::VoiceChat_IsVoiceChatInitialized() {
    return SE3::IsChatSoundInitialized();
}

// OFFSET: 0x00630120/0x1402C4BB0
int32_t SI3::VoiceChat_CreateChatSound(uint32_t& handle, SE3StreamCallbacks* callbacks, int32_t chatSystem) {
    if (!SE3::IsChatSoundInitialized()) {
        return 16;
    }

    return SE3::CreateChatSound(callbacks, &handle, chatSystem ? SE3::sm_pChatSystem : SE3::sm_pGameSystem);
}

// OFFSET: 0x00630160/0x1402C4C20
void SI3::VoiceChat_SetFrequencyScale(uint32_t handle, float scale) {
    SE3::VoiceChat_SetFrequencyScale(handle, scale);
}

// OFFSET: 0x00630180/0x1402C4C30
int32_t SI3::VoiceChat_Play(uint32_t& handle) {
    if (!SE3::IsChatSoundInitialized()) {
        return 15;
    }

    SE3::VoiceChat_Play(handle);
    return 0;
}

// OFFSET: 0x006301B0/0x1402C4C60
void SI3::VoiceChat_StopSound(uint32_t handle) {
    SE3::VoiceChat_StopSound(handle);
}

// OFFSET: 0x006301C0/0x1402C4C70
int32_t SI3::VoiceChat_GetNumOutputDrivers() {
    if (!SE3::sm_pGameSystem) {
        return 0;
    }

    return SE3::GetNumOutputDrivers_Cached(SE3::sm_pGameSystem);
}

// INLINED
SI3LoopbackCapture::~SI3LoopbackCapture() {
}

// OFFSET: 0x00630210/0x1402C4C90
void SI3::VoiceChat_AmplifyMicrophoneInput(int16_t* data, uint32_t samples) {
    if (!SE3::IsChatSoundInitialized()) {
        return;
    }

    static CVar* s_outboundChatVolume = CVar::Lookup("OutboundChatVolume");

    if (!s_outboundChatVolume) {
        return;
    }

    float volume = s_outboundChatVolume->m_floatValue;

    if (1.0f == volume) {
        return;
    }

    for (uint32_t i = 0; i < samples; i++) {
        int32_t value = static_cast<int32_t>(data[i] * volume);

        if (value < -32768) {
            value = -32768;
        } else if (value > 32767) {
            value = 32767;
        }

        data[i] = static_cast<int16_t>(value);
    }
}

// OFFSET: 0x006302B0/0x1402C4E40
static void ComputeAverageLevel(float* data, uint32_t count) {
    if (!SE3::IsChatSoundInitialized()) {
        return;
    }

    float sum = 0.0f;

    for (uint32_t i = 0; i < count; i++) {
        sum += fabs(data[i]);
    }

    s_microphoneSignalLevel = static_cast<int32_t>(sqrt(sum / static_cast<double>(count)) * 100.0);
}

// OFFSET: 0x00630360/0x1402C4EF0
int32_t SI3::VoiceChat_GetCurrentMicrophoneSignalLevel() {
    return SE3::IsChatSoundInitialized() != 0 ? s_microphoneSignalLevel : 0;
}

// OFFSET: 0x00630370/0x1402C4F10
static void MonitorCallback(float* data, uint32_t count) {
    if (SE3::IsChatSoundInitialized()) {
        ComputeAverageLevel(data, count);
    }
}

// OFFSET: 0x00630390/0x1402C4F40
void SI3::VoiceChat_InitCapture(float sampleRate) {
    if (CmdLineGetBool(static_cast<CMDOPT>(26))) {
        return;
    }

    int32_t outputDriverIndex = 0;

    if (SI3::sm_pCVar_Sound_VoiceChatOutputDriverIndex) {
        outputDriverIndex = SI3::sm_pCVar_Sound_VoiceChatOutputDriverIndex->m_intValue;
    }

    const char* outputDriverName = "";

    if (SI3::sm_pCVar_Sound_VoiceChatOutputDriverName) {
        outputDriverName = SI3::sm_pCVar_Sound_VoiceChatOutputDriverName->m_stringValue.m_str;
    }

    int32_t inputDriverIndex = 0;

    if (SI3::sm_pCVar_Sound_VoiceChatInputDriverIndex) {
        inputDriverIndex = SI3::sm_pCVar_Sound_VoiceChatInputDriverIndex->m_intValue;
    }

    const char* inputDriverName = "";

    if (SI3::sm_pCVar_Sound_VoiceChatInputDriverName) {
        inputDriverName = SI3::sm_pCVar_Sound_VoiceChatInputDriverName->m_stringValue.m_str;
    }

    CVar* enableMixMode2 = CVar::Lookup("Sound_EnableMixMode2");
    SE3MIXMODE2 mixMode = enableMixMode2 ? static_cast<SE3MIXMODE2>(enableMixMode2->m_intValue) : SE3MIXMODE2_PCM16;

    static CVar* s_outputQuality = CVar::Lookup("Sound_OutputQuality");
    SE3SOUNDQUALITY quality = s_outputQuality ? static_cast<SE3SOUNDQUALITY>(s_outputQuality->m_intValue) : SE3SOUNDQUALITY_MEDIUM;

    static CVar* s_enableMicrophone = CVar::Lookup("EnableMicrophone");
    int32_t hasInputDevices = s_enableMicrophone && s_enableMicrophone->m_intValue;

    SE3::InitCapture(outputDriverIndex, outputDriverName, inputDriverIndex, inputDriverName, sampleRate, hasInputDevices, quality, mixMode);
    s_enableMicrophone->Set(hasInputDevices ? "1" : "0", true, false, false, true);

    char value[32];

    if (SI3::sm_pCVar_Sound_VoiceChatOutputDriverIndex) {
        SStrPrintf(value, sizeof(value), "%d", outputDriverIndex);
        SI3::sm_pCVar_Sound_VoiceChatOutputDriverIndex->Set(value, true, false, false, true);
    }

    if (SI3::sm_pCVar_Sound_VoiceChatInputDriverIndex) {
        SStrPrintf(value, sizeof(value), "%d", inputDriverIndex);
        SI3::sm_pCVar_Sound_VoiceChatInputDriverIndex->Set(value, true, false, false, true);
    }

    SE3::SetUserMonitorDSPCallback(MonitorCallback);
}

// OFFSET: 0x00630550/0x1402C51C0
void SI3::VoiceChat_SetPrimaryCaptureCallback(SE3CaptureCallbacks* callbacks) {
    if (SE3::IsChatSoundInitialized()) {
        SE3::SetPrimaryCaptureCallback(callbacks);
    }
}

// OFFSET: 0x00630570/0x1402C51E0
void SI3::VoiceChat_StartCapture() {
    if (SE3::IsChatSoundInitialized()) {
        SE3::StartCapture();
    }
}

// OFFSET: 0x00630580/0x1402C5200
void SI3::VoiceChat_StopCapture() {
    if (SE3::IsChatSoundInitialized()) {
        SE3::StopCapture();
    }
}

// OFFSET: 0x00630590/0x1402C5220
static void ComputeMicrophoneLevel(int16_t* data, uint32_t samples) {
    if (!SE3::IsChatSoundInitialized()) {
        return;
    }

    uint32_t sum = 0;

    for (uint32_t i = 0; i < samples; i++) {
        sum += abs(data[i]);
    }

    s_microphoneSignalLevel = static_cast<int32_t>(sqrt(static_cast<double>(sum / samples) * 0.000030517578) * 100.0);
}

// OFFSET: [0x00401000/0x1407D7E80]
static void loopbackSoundEndedCallback(SE3MemorySoundObject* object, void* param) {
}

// OFFSET: 0x00630640/0x1402C5320
void SI3::VoiceChat_PlayLoopbackSound() {
    if (!SE3::IsChatSoundInitialized() || !s_loopbackBuffer) {
        return;
    }

    SE3::StopMemorySound(s_loopbackSound);

    SE3MemorySoundProperties properties;
    properties.ResetToDefaults();
    properties.m_bus = SE3BUS_VOICECHAT;
    properties.m_fpPlayFinishedCallback = loopbackSoundEndedCallback;
    properties.m_playFinishedParam = NULL;
    properties.m_data = reinterpret_cast<const char*>(s_loopbackBuffer);
    properties.m_length = s_loopbackSize;
    properties.m_volume = 0.0039215689f;
    properties.m_position = 0;
    properties.m_handle = &s_loopbackSound;
    properties.m_attachMonitorDSP = 1;
    SE3::PlaySoundFromMemory(&properties, 1);
}

// OFFSET: 0x006306D0/0x1402C53C0
bool SI3::VoiceChat_IsRecordingLoopbackSound() {
    if (!SE3::IsChatSoundInitialized()) {
        return false;
    }

    return SE3::IsCapturing() && SE3::AlternateCaptureCallbackIsActive();
}

// OFFSET: 0x00630700/0x1402C5400
void SI3::VoiceChat_ActivatePrimaryCaptureCallback() {
    if (SE3::IsChatSoundInitialized()) {
        SE3::ActivatePrimaryCaptureCallback();
    }
}

// OFFSET: 0x00630710/0x1402C5420
void SI3::VoiceChat_StopRecordingLoopbackSound() {
    if (SE3::IsChatSoundInitialized()) {
        SI3::VoiceChat_ActivatePrimaryCaptureCallback();
    }
}

// OFFSET: 0x00630730/0x1402C5440
void SI3::VoiceChat_SetCaptureDevice(int32_t index) {
    if (!SI3::sm_pCVar_Sound_VoiceChatInputDriverIndex || !SI3::sm_pCVar_Sound_VoiceChatInputDriverName) {
        return;
    }

    char value[256];
    SStrPrintf(value, sizeof(value), "%d", index);
    SI3::sm_pCVar_Sound_VoiceChatInputDriverIndex->Set(value, true, false, false, true);
    SE3::GetInputDriverName(index < 0 ? 0 : index, value, sizeof(value), SE3::sm_pChatSystem);
    SI3::sm_pCVar_Sound_VoiceChatInputDriverName->Set(value, true, false, false, true);
}

// OFFSET: 0x006307D0/0x1402C54F0
void SI3::VoiceChat_SetOutputDevice(int32_t index) {
    if (!SI3::sm_pCVar_Sound_VoiceChatOutputDriverIndex || !SI3::sm_pCVar_Sound_VoiceChatOutputDriverName) {
        return;
    }

    char value[256];
    SStrPrintf(value, sizeof(value), "%d", index);
    SI3::sm_pCVar_Sound_VoiceChatOutputDriverIndex->Set(value, true, false, false, true);
    SE3::GetOutputDriverName(index < 0 ? 0 : index, value, sizeof(value), SE3::sm_pChatSystem);
    SI3::sm_pCVar_Sound_VoiceChatOutputDriverName->Set(value, true, false, false, true);
}

// OFFSET: 0x00630870/0x1402C55A0
void SI3::VoiceChat_ShutDownCapture() {
    SI3::VoiceChat_StopRecordingLoopbackSound();
    SI3::VoiceChat_StopPlayingLoopbackSound();
    SE3::ShutDownCapture();
}

// OFFSET: 0x006308A0/0x1402C55D0
void SI3LoopbackCapture::ProcessData(int16_t* data, uint32_t samples) {
    if (!s_loopbackBuffer) {
        return;
    }

    SI3::VoiceChat_AmplifyMicrophoneInput(data, samples);
    ComputeMicrophoneLevel(data, samples);

    uint8_t* write = s_loopbackWrite;
    uint32_t remaining = static_cast<uint32_t>(s_loopbackBuffer + s_loopbackSize - write);
    uint32_t bytes = 2 * samples;

    if (bytes >= remaining) {
        bytes = remaining;
    }

    memcpy(write, data, 2 * (bytes >> 1));
    s_loopbackWrite = write + bytes;

    if (!remaining && SE3::IsChatSoundInitialized()) {
        SI3::VoiceChat_StopRecordingLoopbackSound();
    }
}

// OFFSET: 0x00630920/0x1402C5680
static int32_t DetectRecordingFinished(const void* data, void* param) {
    if (!SI3::VoiceChat_IsRecordingLoopbackSound()) {
        EventUnregister(EVENT_ID_POLL, DetectRecordingFinished);
    }

    return 1;
}

// OFFSET: 0x00630950/0x1402C56C0
void SI3::VoiceChat_RecordLoopbackSound(int32_t seconds) {
    if (!SE3::IsChatSoundInitialized()) {
        return;
    }

    SI3::VoiceChat_CleanUpLoopbackSound();

    s_loopbackSize = 16000 * seconds + 44;

    uint32_t* header = static_cast<uint32_t*>(SMemAlloc(16000 * seconds + 44, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));
    header[0] = 'FFIR';
    header[1] = 16000 * seconds + 32;
    header[2] = 'EVAW';
    header[3] = ' tmf';
    header[4] = 16;
    s_loopbackBuffer = reinterpret_cast<uint8_t*>(header);
    header[5] = 0x10001;
    header[6] = 8000;
    header[7] = 16000;
    header[8] = 0x100002;
    header[9] = 'atad';
    header[10] = 16000 * seconds;
    s_loopbackWrite = reinterpret_cast<uint8_t*>(&header[11]);

    SE3::SetAlternateCaptureCallback(&s_loopbackCapture);
    SE3::ActivateAlternateCaptureCallback();
    EventRegister(EVENT_ID_POLL, DetectRecordingFinished);
}
