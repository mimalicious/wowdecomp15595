#include "Engine/Source/ComSat/ComSatSoundIOSoundEngine.h"
#include "Engine/Source/ComSat/ComSatAllocator.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Game/GameClient/ComSatClient.h"
#include <malloc.h>
#include <new>

// PROVEN: ed1c5c8c DIFFERENT/IDENTICAL
// OFFSET: 0x006301E0/0x1402C52E0
ComSat::Sound::IOCallback::~IOCallback() {
}

// OFFSET: 0x00A3A9F0
ComSat::Sound::IO::~IO() {
}

// PROVEN: 8a7536ad IDENTICAL/IDENTICAL
// OFFSET: 0x00A3B3D0/0x1407E8100
ComSat::Sound::IO* ComSat::Sound::IO::Create(const ComSat::Initialization& init, ComSat::Sound::IOCallback& callback, ComSat::Allocator& allocator) {
    return new (allocator.Alloc(sizeof(ComSat::Sound::IOSoundEngine))) ComSat::Sound::IOSoundEngine(init, callback, allocator);
}

// INLINED
ComSat::Sound::CaptureChannel::CaptureChannel(ComSat::Allocator& allocator) : m_allocator(&allocator) {
}

// OFFSET: 0x00A3B290/0x1407E7F40
ComSat::Sound::CaptureChannel::~CaptureChannel() {
}

// PROVEN: ae2154d0 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AEB0/0x1407E7950
void ComSat::Sound::CaptureChannel::ProcessData(int16_t* data, uint32_t samples) {
    SI3::VoiceChat_AmplifyMicrophoneInput(data, samples);
    this->m_buffer.Write(data, samples);
}

// INLINED
ComSat::Sound::OutputChannel::OutputChannel(ComSat::Allocator& allocator) : m_allocator(&allocator) {
}

// OFFSET: 0x00A3B2B0/0x1407E7F70
ComSat::Sound::OutputChannel::~OutputChannel() {
}

// OFFSET: 0x00A3AF00/0x1407E79D0
bool ComSat::Sound::OutputChannel::GetSamples(uint32_t position, uint32_t count, float* buffer) {
    return this->m_buffer.Read(buffer, count) != 0;
}

// PROVEN: cf0eb99b IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AEE0/0x1407E79A0
void ComSat::Sound::OutputChannel::Delete() {
    ComSat::Delete(*this->m_allocator, this);
}

// OFFSET: 0x00A3B2D0/0x1407E8030
ComSat::Sound::IOSoundEngine::IOSoundEngine(const ComSat::Initialization& init, ComSat::Sound::IOCallback& callback, ComSat::Allocator& allocator) : m_allocator(&allocator), m_callback(&callback), m_captureChannel(allocator) {
    this->m_outputDevice = 0;
    this->m_captureDevice = 0;
    this->m_channels.SetCount(init.m_maxChannels);
    for (uint32_t i = 0; i < this->m_channels.Count(); i++) {
        this->m_channels[i] = NULL;
    }
}

// OFFSET: 0x00A3B380/0x1407E79F0
ComSat::Sound::IOSoundEngine::~IOSoundEngine() {
    this->ShutDownCapture();
}

// PROVEN: d85ef02b IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool ComSat::Sound::IOSoundEngine::Init() {
    return true;
}

// PROVEN: 5996f1c6 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void ComSat::Sound::IOSoundEngine::Work(uint32_t time) {
}

// PROVEN: 27bc5b3f IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ComSat::Sound::IOSoundEngine::Shutdown() {
}

// PROVEN: 93b819e0 IDENTICAL/DIFFERENT
// OFFSET: 0x00A332C0/0x1407DD5F0
bool ComSat::Sound::IOSoundEngine::DestroyLocalPlayer(uint8_t player) {
    return true;
}

// PROVEN: 465b08f6 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA40/0x1407E7420
int32_t ComSat::Sound::IOSoundEngine::GetInputBlockSize(uint8_t player) {
    return this->m_samplesPerFrame * this->m_bitsPerSample / 8;
}

// PROVEN: 697947b9 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA40/0x1407E7420
int32_t ComSat::Sound::IOSoundEngine::GetPlaybackBlockSize(uint8_t player) {
    return this->m_samplesPerFrame * this->m_bitsPerSample / 8;
}

// PROVEN: e593f0a3 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA60/0x1407E7440
int32_t ComSat::Sound::IOSoundEngine::EnumerateOutputDevices(ComSat::DeviceInfo* devices, int32_t maxDevices) {
    int32_t count = SI3::VoiceChat_GetNumOutputDrivers();
    if (maxDevices < count) {
        count = maxDevices;
    }
    for (int32_t i = 0; i < count; i++) {
        SE3::GetOutputDriverName(i, devices[i].m_name, sizeof(devices[i].m_name), SE3::sm_pChatSystem ? SE3::sm_pChatSystem : SE3::sm_pGameSystem);
    }
    return count;
}

// PROVEN: 1c7dc768 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AAC0/0x1407E74C0
int32_t ComSat::Sound::IOSoundEngine::EnumerateCaptureDevices(ComSat::DeviceInfo* devices, int32_t maxDevices) {
    int32_t count = SE3::GetNumInputDrivers(SE3::sm_pChatSystem);
    if (maxDevices < count) {
        count = maxDevices;
    }
    for (int32_t i = 0; i < count; i++) {
        SE3::GetInputDriverName(i, devices[i].m_name, sizeof(devices[i].m_name), SE3::sm_pChatSystem);
    }
    return count;
}

// PROVEN: a4101061 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AB20/0x1407E7530
int32_t ComSat::Sound::IOSoundEngine::LocalPlayerGetCurrentOutputDeviceIndex(uint8_t player) {
    return this->m_outputDevice;
}

// PROVEN: 9d957f5f IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AB30/0x1407E7540
int32_t ComSat::Sound::IOSoundEngine::LocalPlayerGetCurrentCaptureDeviceIndex(uint8_t player) {
    return this->m_captureDevice;
}

// PROVEN: 61c8d275 IDENTICAL/DIFFERENT
// OFFSET: 0x00A3AE30/0x1407E78C0
void ComSat::Sound::IOSoundEngine::ShutDownCapture() {
    for (uint32_t i = 0; i < this->m_channels.Count(); i++) {
        if (this->m_channels[i]) {
            SI3::VoiceChat_StopSound(this->m_channels[i]->m_handle);
            this->m_channels[i] = NULL;
        }
    }
    SI3::VoiceChat_ShutDownCapture();
    SI3::VoiceChat_CleanUpLoopbackSound();
}

// PROVEN: ece9809f IDENTICAL/UNPROVEN
// OFFSET: 0x00A3AE80/0x1407E7930
void ComSat::Sound::IOSoundEngine::SetPlayerPlaybackVolume(uint8_t player, int32_t channel, float volume) {
    SI3::SetVolume(this->m_channels[channel]->m_handle, volume);
}

// OFFSET: 0x00A3B0F0/0x1407E7CC0
bool ComSat::Sound::IOSoundEngine::PlayDataForPlayer(uint8_t player, int32_t channel, uint8_t* data, uint32_t size, double rate) {
    double pitch = 0.9;
    if (rate >= 0.9) {
        pitch = rate;
        if (rate >= 1.1) {
            pitch = 1.1;
        }
    }
    SE3::SetFrequencyScale(this->m_channels[channel]->m_handle, static_cast<float>(pitch * this->m_sampleRate));
    uint32_t count = size >> 1;
    float* samples = static_cast<float*>(alloca(sizeof(float) * count));
    const int16_t* pcm = reinterpret_cast<const int16_t*>(data);
    for (uint32_t i = 0; i < count; i++) {
        samples[i] = pcm[i] * (1.0f / 32768.0f);
    }
    this->m_channels[channel]->m_buffer.Write(samples, count);
    return true;
}

// PROVEN: 142452c9 IDENTICAL/DIFFERENT
// OFFSET: 0x00A3B220/0x1407E7E90
int32_t ComSat::Sound::IOSoundEngine::GetRemainingPlaybackBytes(uint8_t player, int32_t channel) {
    return 2 * this->m_channels[channel]->m_buffer.Count();
}

// PROVEN: bc8b7e2b IDENTICAL/DIFFERENT
// OFFSET: 0x00A3B250/0x1407E7ED0
int32_t ComSat::Sound::IOSoundEngine::ReadDataForPlayer(uint8_t player, uint8_t* data, int32_t size) {
    return 2 * this->m_captureChannel.m_buffer.Read(reinterpret_cast<int16_t*>(data), static_cast<uint32_t>(size) >> 1);
}

// PROVEN: ab778835 IDENTICAL/DIFFERENT
// OFFSET: 0x00A3B270/0x1407E7F00
int32_t ComSat::Sound::IOSoundEngine::GetAvailableDataBytes(uint8_t player) {
    return 2 * this->m_captureChannel.m_buffer.Count();
}

// OFFSET: 0x00A3AF20/0x1407E7A60
void ComSat::Sound::IOSoundEngine::InitCapture() {
    SI3::VoiceChat_InitCapture(this->m_sampleRate);
    SI3::VoiceChat_SetPrimaryCaptureCallback(&this->m_captureChannel);
    SI3::VoiceChat_ActivatePrimaryCaptureCallback();
    static CVar* s_enableMicrophone = CVar::Lookup("EnableMicrophone");
    static CVar* s_enableVoiceChat = CVar::Lookup("EnableVoiceChat");
    if (s_enableVoiceChat && s_enableVoiceChat->m_intValue && s_enableMicrophone && s_enableMicrophone->m_intValue && !ComSatClient_IsComsatDisabledByClient()) {
        SI3::VoiceChat_StartCapture();
    }
    if (SI3::VoiceChat_IsVoiceChatInitialized()) {
        for (uint32_t i = 0; i < this->m_channels.Count(); i++) {
            this->m_channels[i] = new (this->m_allocator->Alloc(sizeof(ComSat::Sound::OutputChannel))) ComSat::Sound::OutputChannel(*this->m_allocator);
            ComSat::Sound::OutputChannel* channel = this->m_channels[i];
            SI3::VoiceChat_CreateChatSound(channel->m_handle, channel, 1);
            SI3::VoiceChat_SetFrequencyScale(channel->m_handle, this->m_sampleRate);
            SI3::VoiceChat_Play(channel->m_handle);
        }
    }
}

// PROVEN: 38601996 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3B050/0x1407E7BF0
bool ComSat::Sound::IOSoundEngine::InitLocalPlayer(uint8_t player, int32_t outputDevice, int32_t captureDevice, int32_t sampleRate, int32_t bitsPerSample, int32_t samplesPerFrame) {
    this->m_outputDevice = outputDevice;
    this->m_sampleRate = static_cast<float>(sampleRate);
    this->m_captureDevice = captureDevice;
    this->m_bitsPerSample = bitsPerSample;
    this->m_samplesPerFrame = samplesPerFrame;
    this->InitCapture();
    return true;
}

// PROVEN: a1142393 IDENTICAL/UNPROVEN
// OFFSET: 0x00A3B090/0x1407E7C40
bool ComSat::Sound::IOSoundEngine::LocalPlayerChangeOutputDevice(uint8_t player, int32_t device) {
    this->ShutDownCapture();
    this->m_outputDevice = device;
    SI3::VoiceChat_SetOutputDevice(device);
    SI3::VoiceChat_DestroyChatSystem();
    this->InitCapture();
    return true;
}

// PROVEN: 092e1f1a IDENTICAL/UNPROVEN
// OFFSET: 0x00A3B0C0/0x1407E7C80
bool ComSat::Sound::IOSoundEngine::LocalPlayerChangeCaptureDevice(uint8_t player, int32_t device) {
    this->ShutDownCapture();
    this->m_captureDevice = device;
    SI3::VoiceChat_SetCaptureDevice(device);
    SI3::VoiceChat_DestroyChatSystem();
    this->InitCapture();
    return true;
}

// PROVEN: 0f8734c7 IDENTICAL/IDENTICAL
// OFFSET: 0x00841AE0/0x140211B90
void ComSat::Sound::IOSoundEngine::DebugSetSoundOutputDelay(uint8_t player, int32_t delay) {
}
