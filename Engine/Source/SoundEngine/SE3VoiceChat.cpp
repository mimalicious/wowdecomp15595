#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Os/Processor.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SLock.h"

// STATIC: %0x00D0DE38
static HASHKEY_NONE s_chatLookupKey;

// STATIC: %0x00D0DE40
static FMOD_CREATESOUNDEXINFO s_captureExInfo;

// STATIC: %0x00D0DEB0
static char s_chatInputDeviceName[256];

// STATIC: %0x00D0DFB0
static char s_chatOutputDeviceName[256];

// STATIC: %0x00D0E0B0
int32_t SE3::sm_bCaptureInitialized;

// STATIC: %0x00D0E0B4
static SE3CaptureCallbacks* s_primaryCaptureCallbacks;

// STATIC: %0x00D0E0B8
static SE3CaptureCallbacks* s_alternateCaptureCallbacks;

// STATIC: %0x00D0E0BC
static SE3CaptureCallbacks* s_captureCallbacks;

// STATIC: %0x00D0E0C0
static uint32_t s_captureLength;

// STATIC: %0x00D0E0C4
static uint32_t s_captureChannels;

// STATIC: %0x00D0E0C8
static uint32_t s_captureLastPosition;

// STATIC: %0x00D0E0CC
static int32_t s_capturing;

// STATIC: %0x00D0E0D0
static FMOD::DSP* s_monitorDSP;

// STATIC: %0x00D0E0D4
static void (*s_userMonitorDSPCallback)(float*, uint32_t);

// STATIC: %0x00D0E0DC
TSHashTable<SE3ChatSound, HASHKEY_NONE> SE3::sm_ChatSoundObjectTable;

// STATIC: %0x00D0E108
static SCritSect s_heartBeatCritSect;

// STATIC: %0x00D0E130
char SE3::sm_sSystemDefaultName[64];

// OFFSET: %0x00679110
void SE3::ErrCheck(uint32_t line, const char* file, FMOD_RESULT result) {
    if (result != FMOD_OK && result != FMOD_ERR_CHANNEL_STOLEN && result != FMOD_ERR_INVALID_HANDLE && result != FMOD_ERR_OUTPUT_DRIVERCALL) {
        SE3::Log_Write(line, file, result, "");
    }
}

// OFFSET: %0x00679140
FMOD_RESULT F_CALLBACK SE3ChatSound::DSPReadCallback(FMOD_DSP_STATE* dspState, float* inBuffer, float* outBuffer, unsigned int length, int inChannels, int outChannels) {
    void* userdata;
    FMOD_RESULT result = reinterpret_cast<FMOD::DSP*>(dspState->instance)->getUserData(&userdata);

    if (result != FMOD_OK) {
        return result;
    }

    SE3ChatSound* sound = static_cast<SE3ChatSound*>(userdata);

    if (!sound || !sound->m_pCallbacks) {
        return FMOD_OK;
    }

    if (sound->GetScaledVolume() < 0.0099999998) {
        sound->m_pCallbacks->DiscardSamples(sound->m_position, length);
    } else if (sound->m_pCallbacks->GetSamples(sound->m_position, length, outBuffer)) {
        sound->m_position += length;
        return FMOD_OK;
    }

    memset(outBuffer, 0, length * outChannels * sizeof(float));
    sound->m_position += length;

    return FMOD_OK;
}

// OFFSET: %0x006791D0
FMOD_RESULT F_CALLBACK SE3ChatSound::DSPSeekCallback(FMOD_DSP_STATE* dspState, unsigned int position) {
    void* userdata;
    FMOD_RESULT result = reinterpret_cast<FMOD::DSP*>(dspState->instance)->getUserData(&userdata);

    if (result != FMOD_OK) {
        return result;
    }

    SE3ChatSound* sound = static_cast<SE3ChatSound*>(userdata);
    int32_t count = position - sound->m_position;

    if (count > 0) {
        sound->m_pCallbacks->DiscardSamples(sound->m_position, count);
        sound->m_position = position;
    }

    return FMOD_OK;
}

// OFFSET: %0x00679220
void SE3::CaptureWork() {
    if (!s_capturing) {
        return;
    }

    s_heartBeatCritSect.Enter();

    uint32_t position = 0;
    FMOD_RESULT result = SE3::sm_pChatSystem->getRecordPosition(&position);
    SE3_ERRCHECK(result);

    if (position != s_captureLastPosition) {
        int32_t samples = position - s_captureLastPosition;

        if (samples < 0) {
            samples += s_captureLength;
        }

        if (s_captureCallbacks) {
            void* ptr1;
            void* ptr2;
            unsigned int len1;
            unsigned int len2;

            SE3::sm_pCaptureSound->lock(s_captureLastPosition * s_captureChannels * sizeof(int16_t), samples * s_captureChannels * sizeof(int16_t), &ptr1, &ptr2, &len1, &len2);

            if (ptr1 && len1) {
                s_captureCallbacks->ProcessData(static_cast<int16_t*>(ptr1), len1 / sizeof(int16_t));
            } else {
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, "*** SE3::CaptureWork : Lock failed! (%d, %d) %s", ptr1, len1, "??");
            }

            if (ptr2 && len2) {
                s_captureCallbacks->ProcessData(static_cast<int16_t*>(ptr2), len2 / sizeof(int16_t));
            }

            SE3::sm_pCaptureSound->unlock(ptr1, ptr2, len1, len2);
        }
    }

    s_captureLastPosition = position;

    s_heartBeatCritSect.Leave();
}

// PROVEN: 422455fa IDENTICAL
// OFFSET: 0x007126B0
void SE3::SetUserMonitorDSPCallback(void (*callback)(float*, uint32_t)) {
    s_userMonitorDSPCallback = callback;
}

// PROVEN: 7d0c9594 IDENTICAL
// OFFSET: 0x007126C0
FMOD_RESULT SE3::AttachMonitorDSP(FMOD::Channel* channel) {
    return channel->addDSP(s_monitorDSP);
}

// PROVEN: f375c512 IDENTICAL
// OFFSET: 0x007126E0
static FMOD_RESULT F_CALLBACK MonitorDSPCallback(FMOD_DSP_STATE* dspState, float* inBuffer, float* outBuffer, unsigned int length, int inChannels, int outChannels) {
    memcpy(outBuffer, inBuffer, length * sizeof(float));

    if (s_userMonitorDSPCallback) {
        s_userMonitorDSPCallback(inBuffer, length);
    }

    return FMOD_OK;
}

// OFFSET: %0x006793E0
static FMOD::DSP* CreateMonitorDSP() {
    FMOD_DSP_DESCRIPTION description;
    memset(&description, 0, sizeof(description));

    SStrCopy(description.name, "Monitor DSP", sizeof(description.name));
    description.channels = 0;
    description.read = MonitorDSPCallback;
    description.userdata = reinterpret_cast<void*>(0x12345);

    SE3::sm_pChatSystem->createDSP(&description, &s_monitorDSP);

    return s_monitorDSP;
}

// OFFSET: %0x00679470
static int32_t DriverNameMatches(FMOD::System* system, int32_t index, const char* name, int32_t output) {
    char driverName[512];
    driverName[0] = '\0';

    if (output) {
        SE3::GetOutputDriverName(index, driverName, sizeof(driverName), system);
    } else {
        SE3::GetInputDriverName(index, driverName, sizeof(driverName), system);
    }

    return _stricmp(driverName, name) == 0;
}

// OFFSET: %0x006794D0
static int32_t FindDriverIndex(FMOD::System* system, const char* name, int32_t index, int32_t output) {
    int32_t numDrivers = output ? SE3::GetNumOutputDrivers_Cached(system) : SE3::GetNumInputDrivers(system);

    if (index >= 0 && index < numDrivers && DriverNameMatches(system, index, name, output)) {
        return index;
    }

    for (int32_t i = 0; i < numDrivers; i++) {
        if (i == index) {
            continue;
        }

        char driverName[512];
        driverName[0] = '\0';

        if (output) {
            SE3::GetOutputDriverName(i, driverName, sizeof(driverName), system);
        } else {
            SE3::GetInputDriverName(i, driverName, sizeof(driverName), system);
        }

        if (!_stricmp(driverName, name)) {
            return i;
        }
    }

    return 0;
}

// INLINED
static void LogUsingDriver(int32_t index, const char* use, int32_t output, FMOD::System* system) {
    char driverName[512];
    driverName[0] = '\0';

    if (output) {
        SE3::GetOutputDriverName(index, driverName, sizeof(driverName), system);
    } else {
        SE3::GetInputDriverName(index, driverName, sizeof(driverName), system);
    }

    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - Using [%d] %s for %s", index, driverName, use);
}

// OFFSET: %0x00679590
bool SE3::IsCapturing() {
    bool recording = false;

    if (!SE3::sm_pChatSystem) {
        return false;
    }

    SE3::sm_pChatSystem->isRecording(&recording);

    return recording;
}

// OFFSET: %0x006795C0
void SE3::StartCapture() {
    if (!SE3::sm_pChatSystem) {
        return;
    }

    s_capturing = 1;

    if (!SE3::IsChatSoundInitialized()) {
        return;
    }

    bool recording = false;

    if (SE3::sm_pChatSystem) {
        SE3::sm_pChatSystem->isRecording(&recording);

        if (recording) {
            return;
        }
    }

    FMOD_RESULT result = SE3::sm_pChatSystem->recordStart(SE3::sm_pCaptureSound, true);
    SE3_ERRCHECK(result);
}

// PROVEN: 65a94e21 IDENTICAL
// OFFSET: 0x007128F0
void SE3::SetPrimaryCaptureCallback(SE3CaptureCallbacks* callbacks) {
    s_primaryCaptureCallbacks = callbacks;
}

// PROVEN: 2e04463d IDENTICAL
// OFFSET: 0x00712900
void SE3::SetAlternateCaptureCallback(SE3CaptureCallbacks* callbacks) {
    s_alternateCaptureCallbacks = callbacks;
}

// PROVEN: 3c77d607 IDENTICAL
// OFFSET: 0x00712910
void SE3::ActivateAlternateCaptureCallback() {
    s_heartBeatCritSect.Enter();
    s_captureCallbacks = s_alternateCaptureCallbacks;
    s_heartBeatCritSect.Leave();
}

// PROVEN: 3cd00168 IDENTICAL
// OFFSET: 0x00712930
bool SE3::AlternateCaptureCallbackIsActive() {
    return s_captureCallbacks == s_alternateCaptureCallbacks;
}

// PROVEN: 49a86223 IDENTICAL
// OFFSET: 0x00712950
void SE3::ActivatePrimaryCaptureCallback() {
    s_heartBeatCritSect.Enter();
    s_captureCallbacks = s_primaryCaptureCallbacks;
    s_heartBeatCritSect.Leave();
}

// PROVEN: 2593c197 IDENTICAL
// OFFSET: 0x00401000
void SE3StreamCallbacks::Delete() {
}

// OFFSET: %0x006796C0
void SE3StreamCallbacks::DiscardSamples(uint32_t position, uint32_t count) {
    float buffer[1024];

    while (count) {
        uint32_t samples = count;

        if (samples >= 1024) {
            samples = 1024;
        }

        this->GetSamples(position, samples, buffer);
        position += samples;
        count -= samples;
    }
}

// OFFSET: %0x00679720
void SE3::StopCapture() {
    if (!SE3::sm_pChatSystem || !SE3::IsChatSoundInitialized()) {
        return;
    }

    bool recording = false;

    if (SE3::sm_pChatSystem) {
        SE3::sm_pChatSystem->isRecording(&recording);

        if (recording) {
            FMOD_RESULT result = SE3::sm_pChatSystem->recordStop();
            SE3_ERRCHECK(result);
        }
    }
}

// OFFSET: %0x00679790
void SE3::ShutDownCapture() {
    s_capturing = 0;

    bool recording = false;

    if (SE3::sm_pChatSystem) {
        SE3::sm_pChatSystem->isRecording(&recording);

        if (recording) {
            SE3::StopCapture();
        }
    }

    SE3::HeartBeat(NULL, NULL);
}

// OFFSET: %0x006799D0
void SE3::DestroyChatSystem() {
    if (!SE3::sm_pChatSystem) {
        return;
    }

    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " ");
    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, "=> Shutting Down Voice Chat");

    s_capturing = 0;

    bool recording = false;

    if (SE3::sm_pChatSystem) {
        SE3::sm_pChatSystem->isRecording(&recording);

        if (recording) {
            SE3::StopCapture();
        }
    }

    SE3::HeartBeat(NULL, NULL);
    SE3::sm_ChatSoundObjectTable.InternalClear(0);

    if (SE3::sm_pCaptureSound) {
        SE3::sm_pCaptureSound->release();
        SE3::sm_pCaptureSound = NULL;
    }

    if (s_monitorDSP) {
        s_monitorDSP->release();
        s_monitorDSP = NULL;
    }

    SE3::sm_pChatSystem->close();
    SE3::sm_pChatSystem->release();
    SE3::sm_pChatSystem = NULL;

    s_userMonitorDSPCallback = NULL;
    SE3::sm_bCaptureInitialized = 0;

    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, "=> Done Shutting Down Voice Chat");
}

// OFFSET: %0x00679AD0
void SE3::InitCapture(int32_t& outputDriverIndex, const char* outputDriverName, int32_t& inputDriverIndex, const char* inputDriverName, float sampleRate, int32_t& hasInputDevices, SE3SOUNDQUALITY quality, SE3MIXMODE2 mixMode) {
    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " ");
    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, "=> Setting up Voice Chat:");

    if (!(OsGetProcessorFeatures() & 4)) {
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -# ERROR! SSE support is required for voice chat!");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -# Voice Chat DISABLED.");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################");
        SE3_LOG_CHAT_INIT_FAILED();
        goto nosound;
    }

    {
        FMOD_RESULT result;

        if (!SE3::sm_pChatSystem && (result = FMOD::System_Create(&SE3::sm_pChatSystem)) != FMOD_OK) {
            SE3_ERRCHECK(result);
            SE3_LOG_CHAT_INIT_FAILED();
            SE3::sm_pChatSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
            goto failed;
        }

        unsigned int version;
        result = SE3::sm_pChatSystem->getVersion(&version);

        if (result != FMOD_OK) {
            SE3_ERRCHECK(result);
            SE3_LOG_CHAT_INIT_FAILED();
            SE3::sm_pChatSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
            goto failed;
        }

        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - FMOD version %08x detected", version);

        if (version != FMOD_VERSION) {
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -######## ERROR! FMOD version mismatch! This program requires version %08x", FMOD_VERSION);
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################");
            SE3_LOG_CHAT_INIT_FAILED();
            goto nosound;
        }

        int32_t numOutputDrivers = SE3::GetNumOutputDrivers_Cached(SE3::sm_pChatSystem);
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - %d Output drivers detected", numOutputDrivers);
        SE3::sm_aCachedChatSoundOutputDriverInfo.SetCount(numOutputDrivers);

        if (!numOutputDrivers) {
            SE3_LOG_CHAT_INIT_FAILED();
            goto nosound;
        }

        for (int32_t i = 0; i < numOutputDrivers; i++) {
            char driverName[256];
            SE3::GetOutputDriverName(i, driverName, sizeof(driverName), SE3::sm_pChatSystem);
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " --- [%d] %s", i, driverName);
            SStrCopy(SE3::sm_aCachedChatSoundOutputDriverInfo[i].m_name, driverName, sizeof(SE3DriverInfo));
        }

        int32_t numInputDrivers = SE3::GetNumInputDrivers(SE3::sm_pChatSystem);
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - %d Input drivers detected", numInputDrivers);
        SE3::sm_aCachedChatSoundInputDriverInfo.SetCount(numInputDrivers);

        for (int32_t i = 0; i < numInputDrivers; i++) {
            char driverName[256];
            SE3::GetInputDriverName(i, driverName, sizeof(driverName), SE3::sm_pChatSystem);
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " --- [%d] %s", i, driverName);
            SStrCopy(SE3::sm_aCachedChatSoundInputDriverInfo[i].m_name, driverName, sizeof(SE3DriverInfo));
        }

        if (!numInputDrivers) {
            hasInputDevices = 0;
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -#############################################################################################");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -# NOTICE:   No input devices detected! ");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -#           You will be able to hear others chat, but you will not be able to talk to anyone!");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -#############################################################################################");
        }

        result = SE3::sm_pChatSystem->setHardwareChannels(0, 0, 0, 0);

        if (result != FMOD_OK) {
            SE3_ERRCHECK(result);
            SE3_LOG_CHAT_INIT_FAILED();
            goto nosound;
        }

        result = SE3::sm_pChatSystem->setSpeakerMode(FMOD_SPEAKERMODE_MAX);

        if (result != FMOD_OK) {
            SE3_ERRCHECK(result);
            SE3_LOG_CHAT_INIT_FAILED();
            goto nosound;
        }

        outputDriverIndex = FindDriverIndex(SE3::sm_pChatSystem, outputDriverName, outputDriverIndex, 1);
        inputDriverIndex = FindDriverIndex(SE3::sm_pChatSystem, inputDriverName, inputDriverIndex, 0);

        int32_t outputDriver = outputDriverIndex ? outputDriverIndex - 1 : 0;

        SE3::sm_pChatSystem->getDriverName(outputDriver, s_chatOutputDeviceName, sizeof(s_chatOutputDeviceName));
        result = SE3::sm_pChatSystem->setDriver(outputDriver);

        if (result != FMOD_OK) {
            SE3_ERRCHECK(result);
            SE3_LOG_CHAT_INIT_FAILED();
            goto nosound;
        }

        int32_t inputDriver = inputDriverIndex;

        if (inputDriver) {
            inputDriver--;
        }

        SE3::sm_pChatSystem->getRecordDriverName(inputDriver, s_chatInputDeviceName, sizeof(s_chatInputDeviceName));
        result = SE3::sm_pChatSystem->setRecordDriver(inputDriver);
        SE3_ERRCHECK(result);

        int32_t outputRate;
        FMOD_DSP_RESAMPLER resampler = FMOD_DSP_RESAMPLER_NOINTERP;

        if (quality == SE3SOUNDQUALITY_LOW) {
            outputRate = 22050;
        } else if (quality == SE3SOUNDQUALITY_MEDIUM) {
            outputRate = 44100;
        } else {
            outputRate = 48000;

            if (quality == SE3SOUNDQUALITY_HIGH) {
                resampler = FMOD_DSP_RESAMPLER_LINEAR;
            }
        }

        if (mixMode) {
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -# NOTICE:   Using Mix Mode 2.");

            if (mixMode == SE3MIXMODE2_PCM24) {
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -# 2 PCM24");
                SE3_ERRCHECK(SE3::sm_pGameSystem->setSoftwareFormat(outputRate, FMOD_SOUND_FORMAT_PCM24, 0, 0, resampler));
            } else if (mixMode == SE3MIXMODE2_PCM32) {
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -# 3 PCM32");
                SE3_ERRCHECK(SE3::sm_pGameSystem->setSoftwareFormat(outputRate, FMOD_SOUND_FORMAT_PCM32, 0, 0, resampler));
            } else {
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -# 1 PCMFLOAT");
                SE3_ERRCHECK(SE3::sm_pGameSystem->setSoftwareFormat(outputRate, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, resampler));
            }

            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################");
        }

        result = SE3::sm_pChatSystem->init(8, FMOD_INIT_3D_RIGHTHANDED, NULL);

        if (result != FMOD_OK) {
            SE3_ERRCHECK(result);
            SE3_LOG_CHAT_INIT_FAILED();
            goto nosound;
        }

        LogUsingDriver(outputDriverIndex, "Voice Chat Output", 1, SE3::sm_pChatSystem);

        if (numInputDrivers) {
            LogUsingDriver(inputDriverIndex, "Voice Chat Input", 0, SE3::sm_pChatSystem);
        }

        memset(&s_captureExInfo, 0, sizeof(s_captureExInfo));
        s_captureExInfo.defaultfrequency = static_cast<int32_t>(sampleRate);
        s_captureExInfo.cbsize = sizeof(s_captureExInfo);
        s_captureExInfo.format = FMOD_SOUND_FORMAT_PCM16;
        s_captureExInfo.numchannels = 1;
        s_captureExInfo.length = static_cast<int32_t>(sampleRate) * sizeof(int16_t) * 2;

        result = SE3::sm_pChatSystem->createSound(NULL, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &s_captureExInfo, &SE3::sm_pCaptureSound);

        if (result != FMOD_OK) {
            SE3_ERRCHECK(result);
            SE3_LOG_CHAT_INIT_FAILED();
            goto nosound;
        }

        unsigned int length;
        SE3::sm_pCaptureSound->getLength(&length, FMOD_TIMEUNIT_PCM);

        s_captureChannels = s_captureExInfo.numchannels;
        s_captureLength = length;
        s_capturing = 1;
        s_primaryCaptureCallbacks = NULL;
        s_alternateCaptureCallbacks = NULL;
        s_captureCallbacks = NULL;

        CreateMonitorDSP();

        result = SE3::sm_pChatSystem->set3DSettings(1.0f, 1.0f, 4.0f);
        SE3_ERRCHECK(result);

        if (result != FMOD_OK) {
            SE3_LOG_CHAT_INIT_FAILED();
            goto nosound;
        }

        FMOD::Debug_SetLevel(FMOD_DEBUG_LEVEL_NONE);

        SE3::sm_bCaptureInitialized = 1;

        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, "=> Voice Chat is ready.");
        return;
    }

nosound:
    SE3::sm_pChatSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);

failed:
    if (SE3::sm_pChatSystem && SE3::sm_pChatSystem != SE3::sm_pGameSystem) {
        SE3::DestroyChatSystem();
    }

    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, "=>###########################################################################################");
    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, "=># Voice Chat Init FAILED.");
    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, "=>###########################################################################################");
}

// OFFSET: %0x0067A640
void SE3::VoiceChat_Play(uint32_t handle) {
    SE3ChatSound* sound = SE3::sm_ChatSoundObjectTable.Ptr(handle, s_chatLookupKey);

    if (sound) {
        sound->m_pFMODChannel->setPaused(false);
    }
}

// OFFSET: %0x0067A670
void SE3::VoiceChat_StopSound(uint32_t handle) {
    SE3ChatSound* sound = SE3::sm_ChatSoundObjectTable.Ptr(handle, s_chatLookupKey);

    if (sound) {
        SE3::sm_ChatSoundObjectTable.Delete(sound);
    }
}

// OFFSET: %0x0067A6B0
void SE3::VoiceChat_SetFrequencyScale(uint32_t handle, float scale) {
    SE3ChatSound* sound = SE3::sm_ChatSoundObjectTable.Ptr(handle, s_chatLookupKey);

    if (sound) {
        sound->m_pFMODChannel->setFrequency(scale);
    }
}

// INLINED
SE3ChatSound::SE3ChatSound() {
    this->m_pFMODChannel = NULL;
    this->m_uUID = ++SE3::sm_uNextUniqueID;
    this->m_bus = SE3BUS_VOICECHAT;
    this->m_volume = 1.0f;
    this->m_frequencyScale = 1.0f;
}

// INLINED
SE3ChatSound::~SE3ChatSound() {
    if (this->m_pFMODDSP) {
        this->m_pFMODDSP->setActive(false);
        this->m_pFMODDSP->remove();
        this->m_pFMODDSP->release();
        this->m_pFMODDSP = NULL;
    }

    if (this->m_pCallbacks) {
        this->m_pCallbacks->Delete();
        this->m_pCallbacks = NULL;
    }
}

// OFFSET: %0x0067AD60
int32_t SE3::CreateChatSound(SE3StreamCallbacks* callbacks, uint32_t* handle, FMOD::System* system) {
    if (!SE3::IsChatSoundInitialized()) {
        return SE3_ERR_CHAT_NOT_INITIALIZED;
    }

    if (!handle || !system || !callbacks) {
        return SE3_ERR_INVALID_PARAM;
    }

    SE3ChatSound* sound = SE3::sm_ChatSoundObjectTable.New(SE3::sm_uNextUniqueID + 1, s_chatLookupKey, 0, 0);
    *handle = sound->m_uUID;
    sound->m_pCallbacks = callbacks;

    FMOD_DSP_DESCRIPTION description = { "VoiceChatStreamedSound", 1000, 1, NULL, NULL, NULL, SE3ChatSound::DSPReadCallback, SE3ChatSound::DSPSeekCallback, 0, NULL, NULL, NULL, NULL, 0, 0, sound };

    SE3_ERRCHECK(system->createDSP(&description, &sound->m_pFMODDSP));
    SE3_ERRCHECK(system->playDSP(FMOD_CHANNEL_FREE, sound->m_pFMODDSP, true, &sound->m_pFMODChannel));

    sound->m_frequencyScale = 1.0f;
    sound->m_pFMODChannel->setFrequency(1.0f);

    return SE3_OK;
}
