#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include "WoW/Source/SoundInterface/SoundInterface2VoiceChat.h"
#include "Engine/Source/Base/CmdLine.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"

// STATIC: #0x00B2D9E4
static SI2LoopbackCapture s_loopbackCapture;

// STATIC: #0x00DCEAA4
static int32_t s_microphoneSignalLevel;

// STATIC: #0x00DCEAA8
static uint8_t* s_loopbackBuffer;

// STATIC: #0x00DCEAAC
static uint8_t* s_loopbackWrite;

// STATIC: #0x00DCEAB0
static uint32_t s_loopbackSize;

// STATIC: %0x00D0B8B4
static uint32_t s_loopbackSound;

// OFFSET: #0x00986390
void SI2::VoiceChat_ShutDownCapture() {
    SE3::ShutDownCapture();
}

// OFFSET: #0x009863A0
void SI2::VoiceChat_CleanUpLoopbackSound() {
    SE3::VoiceChat_StopSound(s_loopbackSound);

    if (s_loopbackBuffer) {
        SMemFree(s_loopbackBuffer, __FILE__, __LINE__, 0);
        s_loopbackBuffer = NULL;
        s_loopbackSize = 0;
    }
}

// OFFSET: #0x009863F0
void SI2::VoiceChat_StopPlayingLoopbackSound() {
    SE3::StopMemorySound(s_loopbackSound);
}

// OFFSET: #0x00986410
bool SI2::VoiceChat_IsPlayingLoopbackSound() {
    return SE3::IsPlaying(s_loopbackSound) != 0;
}

// OFFSET: #0x00986420
void SI2::VoiceChat_SetCaptureDevice(int32_t index) {
    static CVar* s_inputDriverIndex = CVar::Lookup("Sound_VoiceChatInputDriverIndex");

    if (s_inputDriverIndex) {
        char value[32];
        SStrPrintf(value, sizeof(value), "%d", index);
        s_inputDriverIndex->Set(value, true, false, false, true);
    }
}

// OFFSET: #0x00986490
void SI2::VoiceChat_SetOutputDevice(int32_t index) {
    static CVar* s_outputDriverIndex = CVar::Lookup("Sound_VoiceChatOutputDriverIndex");

    if (s_outputDriverIndex) {
        char value[32];
        SStrPrintf(value, sizeof(value), "%d", index);
        s_outputDriverIndex->Set(value, true, false, false, true);
    }
}

// OFFSET: #0x00986500
void SI2::VoiceChat_DestroyChatSystem() {
    SE3::StopMemorySound(s_loopbackSound);
    SE3::DestroyChatSystem();
}

// PROVEN: a99efdd2 IDENTICAL/NONE
// OFFSET: 0x00630110/0x1402C4BA0
int32_t SI2::VoiceChat_IsVoiceChatInitialized() {
    return SE3::IsChatSoundInitialized();
}

// OFFSET: #0x00986520
void SI2::VoiceChat_AmplifyMicrophoneInput(int16_t* data, uint32_t samples) {
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

// OFFSET: #0x009865C0
static void ComputeAverageLevel(float* data, uint32_t count) {
    if (!SE3::IsChatSoundInitialized()) {
        return;
    }

    float sum = 0.0f;

    for (uint32_t i = 0; i < count; i++) {
        sum += fabs(data[i]);
    }

    s_microphoneSignalLevel = static_cast<int32_t>(sqrt(sum / count) * 100.0f);
}

// PROVEN: 19e34e6e IDENTICAL/NONE
// OFFSET: 0x00630360/0x1402C4EF0
int32_t SI2::VoiceChat_GetCurrentMicrophoneSignalLevel() {
    return SE3::IsChatSoundInitialized() != 0 ? s_microphoneSignalLevel : 0;
}

// OFFSET: #0x00986680
int32_t SI2::VoiceChat_CreateChatSound(uint32_t& handle, SE3StreamCallbacks* callbacks, int32_t chatSystem) {
    if (!SE3::IsChatSoundInitialized()) {
        return 16;
    }

    return SE3::CreateChatSound(callbacks, &handle, chatSystem ? SE3::sm_pChatSystem : SE3::sm_pGameSystem);
}

// OFFSET: %0x0063EB60
void SI2::VoiceChat_SetFrequencyScale(uint32_t handle, float scale) {
    SE3::VoiceChat_SetFrequencyScale(handle, scale);
}

// OFFSET: %0x0063EBB0
void SI2::VoiceChat_StopSound(uint32_t handle) {
    SE3::VoiceChat_StopSound(handle);
}

// OFFSET: %0x0063EBC0
int32_t SI2::VoiceChat_GetNumOutputDrivers() {
    if (!SE3::sm_pGameSystem) {
        return 0;
    }

    return SE3::GetNumOutputDrivers_Cached(SE3::sm_pGameSystem);
}

// OFFSET: #0x009866B0
int32_t SI2::VoiceChat_Play(uint32_t& handle) {
    if (!SE3::IsChatSoundInitialized()) {
        return 17;
    }

    SE3::VoiceChat_Play(handle);
    return 0;
}

// PROVEN: 9aa575c3 IDENTICAL/NONE
// OFFSET: 0x00630370/0x1402C4F10
static void MonitorCallback(float* data, uint32_t count) {
    if (SE3::IsChatSoundInitialized()) {
        ComputeAverageLevel(data, count);
    }
}

// OFFSET: #0x00986700
void SI2::VoiceChat_InitCapture(float sampleRate) {
    if (CmdLineGetBool(static_cast<CMDOPT>(26))) {
        return;
    }

    int32_t outputDriverIndex = 0;
    static CVar* s_outputDriverIndex = CVar::Lookup("Sound_VoiceChatOutputDriverIndex");

    if (s_outputDriverIndex) {
        outputDriverIndex = s_outputDriverIndex->m_intValue;
    }

    const char* outputDriverName = "";
    static CVar* s_outputDriverName = CVar::Lookup("Sound_VoiceChatOutputDriverName");

    if (s_outputDriverName) {
        outputDriverName = s_outputDriverName->m_stringValue.m_str;
    }

    int32_t inputDriverIndex = 0;
    static CVar* s_inputDriverIndex = CVar::Lookup("Sound_VoiceChatInputDriverIndex");

    if (s_inputDriverIndex) {
        inputDriverIndex = s_inputDriverIndex->m_intValue;
    }

    const char* inputDriverName = "";
    static CVar* s_inputDriverName = CVar::Lookup("Sound_VoiceChatInputDriverName");

    if (s_inputDriverName) {
        inputDriverName = s_inputDriverName->m_stringValue.m_str;
    }

    static CVar* s_enableMixMode2 = CVar::Lookup("Sound_EnableMixMode2");
    static CVar* s_outputQuality = CVar::Lookup("Sound_OutputQuality");
    static CVar* s_enableMicrophone = CVar::Lookup("EnableMicrophone");

    SE3MIXMODE2 mixMode = s_enableMixMode2 ? static_cast<SE3MIXMODE2>(s_enableMixMode2->m_intValue) : SE3MIXMODE2_PCM16;
    SE3SOUNDQUALITY quality = s_outputQuality ? static_cast<SE3SOUNDQUALITY>(s_outputQuality->m_intValue) : SE3SOUNDQUALITY_MEDIUM;
    int32_t hasInputDevices = s_enableMicrophone && s_enableMicrophone->m_intValue;

    SE3::InitCapture(outputDriverIndex, outputDriverName, inputDriverIndex, inputDriverName, sampleRate, hasInputDevices, quality, mixMode);
    s_enableMicrophone->Set(hasInputDevices ? "1" : "0", true, false, false, true);

    char value[32];

    if (s_outputDriverIndex) {
        SStrPrintf(value, sizeof(value), "%d", outputDriverIndex);
        s_outputDriverIndex->Set(value, true, false, false, true);
    }

    if (s_inputDriverName) {
        SStrPrintf(value, sizeof(value), "%d", inputDriverIndex);
        s_inputDriverIndex->Set(value, true, false, false, true);
    }

    SE3::SetUserMonitorDSPCallback(MonitorCallback);
}

// PROVEN: 49ff57ae IDENTICAL/NONE
// OFFSET: 0x00630550/0x1402C51C0
void SI2::VoiceChat_SetPrimaryCaptureCallback(SE3CaptureCallbacks* callbacks) {
    if (SE3::IsChatSoundInitialized()) {
        SE3::SetPrimaryCaptureCallback(callbacks);
    }
}

// OFFSET: %0x0063EF70
void SI2::VoiceChat_StartCapture() {
    if (SE3::IsChatSoundInitialized()) {
        SE3::StartCapture();
    }
}

// OFFSET: %0x0063EF80
void SI2::VoiceChat_StopCapture() {
    if (SE3::IsChatSoundInitialized()) {
        SE3::StopCapture();
    }
}

// OFFSET: #0x009868E0
static void ComputeMicrophoneLevel(int16_t* data, uint32_t samples) {
    if (!SE3::IsChatSoundInitialized()) {
        return;
    }

    uint32_t sum = 0;

    for (uint32_t i = 0; i < samples; i++) {
        sum += abs(data[i]);
    }

    s_microphoneSignalLevel = static_cast<int32_t>(sqrt(static_cast<float>(sum / samples) * 0.000030517578f) * 100.0f);
}

// OFFSET: #0x00986960
SI2LoopbackCapture::~SI2LoopbackCapture() {
}

// OFFSET: %0x0063F110
bool SI2::VoiceChat_IsRecordingLoopbackSound() {
    if (!SE3::IsChatSoundInitialized()) {
        return false;
    }

    return SE3::IsCapturing() && SE3::AlternateCaptureCallbackIsActive();
}

// PROVEN: 5003db86 IDENTICAL/NONE
// OFFSET: 0x00630700/0x1402C5400
void SI2::VoiceChat_ActivatePrimaryCaptureCallback() {
    if (SE3::IsChatSoundInitialized()) {
        SE3::ActivatePrimaryCaptureCallback();
    }
}

// PROVEN: cfa2b2de IDENTICAL/NONE
// OFFSET: 0x00630710/0x1402C5420
void SI2::VoiceChat_StopRecordingLoopbackSound() {
    if (SE3::IsChatSoundInitialized()) {
        SI2::VoiceChat_ActivatePrimaryCaptureCallback();
    }
}

// OFFSET: #0x009869F0
void SI2LoopbackCapture::ProcessData(int16_t* data, uint32_t samples) {
    if (!s_loopbackBuffer) {
        return;
    }

    SI2::VoiceChat_AmplifyMicrophoneInput(data, samples);
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
        SI2::VoiceChat_ActivatePrimaryCaptureCallback();
    }
}

// OFFSET: %0x0063F360
static int32_t DetectRecordingFinished(const void* data, void* param) {
    if (!SE3::IsChatSoundInitialized() || !SE3::IsCapturing() || !SE3::AlternateCaptureCallbackIsActive()) {
        EventUnregister(EVENT_ID_POLL, DetectRecordingFinished);
    }

    return 1;
}

// OFFSET: #0x00986AA0
void SI2::VoiceChat_RecordLoopbackSound(int32_t seconds) {
    if (!SE3::IsChatSoundInitialized()) {
        return;
    }

    SI2::VoiceChat_CleanUpLoopbackSound();

    s_loopbackSize = 16000 * seconds + 44;

    uint32_t* header = static_cast<uint32_t*>(SMemAlloc(16000 * seconds + 44, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));
    header[0] = 'FFIR';
    header[1] = 16000 * seconds + 32;
    header[2] = 'EVAW';
    header[3] = ' tmf';
    header[4] = 16;
    header[5] = 0x10001;
    s_loopbackBuffer = reinterpret_cast<uint8_t*>(header);
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

// OFFSET: #0x00986BC0
void SI2::VoiceChat_PlayLoopbackSound() {
    if (!SE3::IsChatSoundInitialized() || !s_loopbackBuffer) {
        return;
    }

    SE3::StopMemorySound(s_loopbackSound);

    SE3MemorySoundProperties properties;
    properties.ResetToDefaults();
    properties.m_bus = SE3BUS_VOICECHAT;
    properties.m_fpPlayFinishedCallback = NULL;
    properties.m_playFinishedParam = NULL;
    properties.m_data = reinterpret_cast<const char*>(s_loopbackBuffer);
    properties.m_length = s_loopbackSize;
    properties.m_volume = 0.0039215689f;
    properties.m_position = 0;
    properties.m_handle = &s_loopbackSound;
    properties.m_attachMonitorDSP = 1;
    SE3::PlaySoundFromMemory(&properties, 1);
}
