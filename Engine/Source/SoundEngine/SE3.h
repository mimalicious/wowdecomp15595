#ifndef ENGINE_SOURCE_SOUNDENGINE_SE3_H
#define ENGINE_SOURCE_SOUNDENGINE_SE3_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/SoundEngine/fmodex/src/fmod.hpp"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Services/Panama.h"
#include "Engine/Source/Tempest/C3Vector.h"

#define SE3_ERRCHECK(result) SE3::ErrCheck(__LINE__, __FILE__, (result))
#define SE3_LOG_CHAT_INIT_FAILED() SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################"); SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -######## ERROR INITIALIZING. VOICE CHAT DISABLED."); SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################")

class CAsyncObject;
class SCritSect;
class SFileImpl;
class SoundEntriesAdvancedRec;
class SoundEmittersRec;
class SoundEntriesRec;

enum SE3BUS {
    SE3BUS_ALL = -2,
    SE3BUS_NONE = -1,
    SE3BUS_SFX = 0,
    SE3BUS_MUSIC = 1,
    SE3BUS_AMBIENCE = 2,
    SE3BUS_DIALOG = 3,
    SE3BUS_MOVIE = 4,
    SE3BUS_5 = 5,
    SE3BUS_6 = 6,
    SE3BUS_7 = 7,
    SE3BUS_8 = 8,
    SE3BUS_9 = 9,
    SE3BUS_10 = 10,
    SE3BUS_11 = 11,
    SE3BUS_12 = 12,
    SE3BUS_13 = 13,
    SE3BUS_14 = 14,
    SE3BUS_15 = 15,
    SE3BUS_16 = 16,
    SE3BUS_17 = 17,
    SE3BUS_18 = 18,
    SE3BUS_19 = 19,
    SE3BUS_20 = 20,
    SE3BUS_MASTER = 21,
    SE3BUS_VOICECHAT = 22,
    SE3BUS_LAST = 23,
};

enum SE3RESULT {
    SE3_OK = 0,
    SE3_ERR_INVALID_PARAM = 2,
    SE3_ERR_MEMORY = 3,
    SE3_ERR_FMOD = 4,
    SE3_ERR_NOT_FOUND = 5,
    SE3_ERR_KIT_NAME_NOT_FOUND = 6,
    SE3_ERR_INVALID_KIT = 7,
    SE3_ERR_KIT_EMPTY = 8,
    SE3_ERR_SELECT_FILE = 9,
    SE3_ERR_FILE = 11,
    SE3_ERR_BUS_DISABLED = 12,
    SE3_ERR_BUS_FULL = 13,
    SE3_ERR_SILENT = 15,
    SE3_ERR_CHAT_NOT_INITIALIZED = 17,
    SE3_ERR_CONDITION = 18,
    SE3_ERR_OUT_OF_RANGE = 19,
    SE3_ERR_ALREADY_PLAYING = 20,
    SE3_ERR_NOT_INITIALIZED = 21,
    SE3_ERR_NOT_LOCAL = 23,
    SE3_ERR_NO_FILE = 24,
    SE3_ERR_NO_OUTPUT_DEVICES = 25,
    SE3_ERR_STILL_PLAYING = 28,
};

enum SE3USAGE {
    SE3USAGE_LOOPING = 0,
    SE3USAGE_PERIODIC = 1,
    SE3USAGE_ONESHOT = 2,
};

enum SE3FADESTATE {
    SE3FADE_NONE = 0,
    SE3FADE_IN = 1,
    SE3FADE_OUT = 2,
};

enum SE3CACHEEVENT {
    SE3CACHEEVENT_LOADING_FAILED = 0,
    SE3CACHEEVENT_DELETED = 1,
    SE3CACHEEVENT_LOADED = 2,
};

enum SE3WAVEDATASTATE {
    SE3WAVEDATA_LOADING = 1,
    SE3WAVEDATA_LOADED = 2,
    SE3WAVEDATA_FAILED = 3,
};

enum SE3SOUNDQUALITY {
    SE3SOUNDQUALITY_LOW = 0,
    SE3SOUNDQUALITY_MEDIUM = 1,
    SE3SOUNDQUALITY_HIGH = 2,
};

enum SE3MIXMODE2 {
    SE3MIXMODE2_PCM16 = 0,
    SE3MIXMODE2_PCMFLOAT = 1,
    SE3MIXMODE2_PCM24 = 2,
    SE3MIXMODE2_PCM32 = 3,
};

typedef FMOD_DSP_TYPE SE3_DSP_TYPE;
typedef FMOD_REVERB_PROPERTIES SE3_REVERB_PROPERTIES;

struct SE3DriverInfo {
    /* 0000 */ char m_name[256];
};

struct SE3SoundBus {
    /* 0000 */ SE3BUS m_parent;
    /* 0004 */ float m_volume;
    /* 0008 */ int32_t m_muted;
    /* 000C */ int32_t m_enabled;
    /* 0010 */ int32_t m_channelCount;
    /* 0014 */ int32_t m_maxChannels;

    SE3SoundBus();
};

struct SE3EngineInitParams {
    /* 0000 */ float m_sfxVolume;
    /* 0004 */ float m_musicVolume;
    /* 0008 */ float m_ambienceVolume;
    /* 000C */ float m_masterVolume;
    /* 0010 */ float m_voiceChatVolume;
    /* 0014 */ int32_t m_sfxEnabled;
    /* 0018 */ int32_t m_musicEnabled;
    /* 001C */ int32_t m_ambienceEnabled;
    /* 0020 */ int32_t m_masterEnabled;
    /* 0024 */ int32_t m_hardware;
    /* 0028 */ int32_t m_reverb;
    /* 002C */ int32_t m_softwareHRTF;
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ int32_t m_maxSoftwareChannels;
    /* 0038 */ int32_t m_numChannels;
    /* 003C */ int32_t m_millisecondsPerDay;
    /* 0040 */ int32_t m_outputDriverIndex;
    /* 0044 */ int32_t m_resampleMethod;
    /* 0048 */ SE3MIXMODE2 m_mixMode;
    /* 004C */ int32_t m_sampleRate;
    /* 0050 */ int32_t m_dspBufferSize;
    /* 0054 */ int32_t m_forceStereo;
    /* 0058 */ const char* m_outputDriverName;
    /* 005C */ const char* m_systemDefaultName;
    /* 0060 */ void (*m_fpFilenameTranslateCallback)(const char**);
    /* 0064 */ void* unk_0064;
    /* 0068 */ int32_t (*m_fpGetDayProgression)();

    SE3EngineInitParams();
};

class SE3SoundKitObject;

class SE3SoundKitProperties {
    public:
    /* 0000 */ SE3BUS m_bus;
    /* 0004 */ float unk_0004;
    /* 0008 */ float m_volume;
    /* 000C */ int32_t m_nIndexOverride;
    /* 0010 */ int32_t m_fadeInTime;
    /* 0014 */ int32_t m_fadeOutTime;
    /* 0018 */ int32_t m_selectMode;
    /* 001C */ int32_t m_usage;
    /* 0020 */ int32_t m_priority;
    /* 0024 */ int32_t m_uniqueness;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ float unk_002C;
    /* 0030 */ int32_t m_maxAudibleBehavior;
    /* 0034 */ float m_muteFadeSpeed;
    /* 0038 */ void (*m_fpCustomDSPChainCallback)(void*, char*);
    /* 003C */ char m_sCustomDSPChainName[128];
    /* 00BC */ SE3SoundKitObject* m_pSKOToReuse;
    /* 00C0 */ const char* m_sFilename;
    /* 00C4 */ C3Vector m_coneOrientation;
    /* 00D0 */ void* m_conditionArg;
    /* 00D4 */ int32_t (*m_fpCondition)(void*);
    /* 00D8 */ uint32_t m_bit0 : 1;
    /* 00D8 */ uint32_t m_reverb : 1;
    /* 00D8 */ uint32_t m_noRandomStart : 1;
    /* 00D8 */ uint32_t m_bit3 : 1;
    /* 00D8 */ uint32_t m_software : 1;
    /* 00D8 */ uint32_t m_stopPrevious : 1;
    /* 00D8 */ uint32_t m_attachCustomDSPChain : 1;
    /* 00D8 */ uint32_t m_noStreaming : 1;
    /* 00D8 */ uint32_t m_checkRange : 1;
    /* 00D8 */ uint32_t m_combinable : 1;
    /* 00D8 */ uint32_t m_bit10 : 1;
    /* 00D8 */ uint32_t m_highPriorityDownload : 1;
    /* 00D8 */ uint32_t m_retriggered : 1;
    /* 00D8 */ uint32_t m_unused : 19;

    SE3SoundKitProperties();
    void ResetToDefaults();
};

class SE3MemorySoundProperties {
    public:
    /* 0000 */ uint32_t* m_handle;
    /* 0004 */ const char* m_data;
    /* 0008 */ uint32_t m_length;
    /* 000C */ SE3BUS m_bus;
    /* 0010 */ float m_volume;
    /* 0014 */ uint32_t m_position;
    /* 0018 */ void (*m_fpPlayFinishedCallback)(class SE3MemorySoundObject*, void*);
    /* 001C */ void* m_playFinishedParam;
    /* 0020 */ int32_t m_attachMonitorDSP;

    void ResetToDefaults();
};

struct SE3SoundChannel {
    /* 0000 */ TSLink<SE3SoundChannel> m_link;
    /* 0008 */ FMOD::Channel* m_pFMODChannel;
    /* 000C */ float m_baseFrequency;
    /* 0010 */ SE3FADESTATE m_fadeState;
    /* 0014 */ float m_fadeVolume;
    /* 0018 */ uint32_t m_waveDataID;

    SE3SoundChannel();
};

class SE3SoundKitDef : public TSLinkedNode<SE3SoundKitDef> {
    public:
    /* 0008 */ FMOD::SoundGroup* m_pSoundGroup;
    /* 000C */ uint8_t m_weights[10];
    /* 0018 */ int32_t m_nNumFiles : 5;
    /* 0018 */ int32_t m_lastIndex : 5;
    /* 0018 */ uint32_t m_ownsRecords : 1;
    /* 0018 */ uint32_t m_hasTimeOfDay : 1;
    /* 0018 */ uint32_t m_notLocal : 1;
    /* 0018 */ uint32_t m_unused : 19;
    /* 001C */ int32_t m_timeOfDay[4];
    /* 002C */ int32_t m_nPlayingCount;
    /* 0030 */ const SoundEntriesRec* m_pSoundKitRec;
    /* 0034 */ const SoundEntriesAdvancedRec* m_pAdvancedSoundKitRec;
    /* 0038 */ CAsyncObject* m_pAsyncObj;
    /* 003C */ int32_t m_downloaded;
    /* 0040 */ uint32_t m_downloadStartTime;

    SE3SoundKitDef();
    ~SE3SoundKitDef();
    int32_t BeginDownload(const char* filename, int32_t index, uint32_t* handle, SE3SoundKitProperties* properties, const C3Vector* position);
    void BuildPath(char* path, int32_t size, int32_t index);
    int32_t IsLocal(int32_t index);
    int32_t SelectLocalFile(int32_t* index);
    void SelectNextFile(const SE3SoundKitProperties* properties, int32_t* index);
};

struct SE3SOUNDKITOBJECT_LOOKUP : public TSHashObject<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE> {
    /* 0018 */ SE3SoundKitObject* m_pSKO;
};

class SE3SoundKitObject : public TSLinkedNode<SE3SoundKitObject> {
    public:
    /* 0008 */ SE3SoundChannel m_channels[2];
    /* 0040 */ SE3SoundKitDef* m_pSoundKitDef;
    /* 0044 */ uint32_t m_uUID;
    /* 0048 */ SE3FADESTATE m_fadeState;
    /* 004C */ float m_fadeVolume;
    /* 0050 */ float m_randomVolume;
    /* 0054 */ C3Vector m_position;
    /* 0060 */ C3Vector m_positionOffset;
    /* 006C */ int32_t unk_006C;
    /* 0070 */ int32_t unk_0070;
    /* 0074 */ uint32_t m_modeFlags;
    /* 0078 */ void (*m_fpPlayFinishedCallback)(SE3SoundKitObject*);
    /* 007C */ void* m_playFinishedParam;
    /* 0080 */ int32_t m_priority;
    /* 0084 */ float m_frequencyScale;
    /* 0088 */ SE3USAGE m_usage;
    /* 008C */ int32_t m_periodicTimer;
    /* 0090 */ void (*m_fpCustomDSPChainCallback)(void*, char*);
    /* 0094 */ char* m_sCustomDSPChainName;
    /* 0098 */ int32_t unk_0098;
    /* 009C */ int32_t unk_009C;
    /* 00A0 */ SE3SoundKitProperties m_properties;
    /* 017C */ SE3SOUNDKITOBJECT_LOOKUP* m_pMySKOL;
    /* 0180 */ int32_t unk_0180;
    /* 0184 */ uint32_t m_notCacheable : 1;
    /* 0184 */ uint32_t m_reverb : 1;
    /* 0184 */ uint32_t m_bit2 : 1;
    /* 0184 */ uint32_t m_hasPosition : 1;
    /* 0184 */ uint32_t m_unused : 28;
    /* 0188 */ float m_fTimeOfDayScalar;
    /* 018C */ int32_t m_timeOfDayOffset;
    /* 0190 */ int32_t m_virtualTime;
    /* 0194 */ TSLink<SE3SoundKitObject> m_loadingLink;
    /* 019C */ TSLink<SE3SoundKitObject> m_goGoGoLink;
    /* 01A4 */ TSLink<SE3SoundKitObject> m_deleteLink;
    /* 01AC */ TSLink<SE3SoundKitObject> m_fadeLink;
    /* 01B4 */ TSLink<SE3SoundKitObject> m_link5;
    /* 01BC */ TSLink<SE3SoundKitObject> m_downloadLink;
    /* 01C4 */ int32_t unk_01C4;

    SE3SoundKitObject();
    ~SE3SoundKitObject();
    float GetChannelVolume(int32_t index);
    int32_t IsPlaying();
    int32_t IsVirtual();
    void Set3DPosition(const C3Vector& position);
    void UpdateVolume();
};

struct SOUNDKITLOOKUP : public TSHashObject<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI> {
    /* 0018 */ int32_t m_nSoundKitID;
};

struct SE3CombinableSoundKitPosition : public TSLinkedNode<SE3CombinableSoundKitPosition> {
    /* 0008 */ int32_t m_hasPosition;
    /* 000C */ C3Vector m_position;
};

struct SE3COMBINABLESOUNDLIST : public TSHashObject<SE3COMBINABLESOUNDLIST, HASHKEY_NONE> {
    /* 0018 */ int32_t m_nSoundKitID;
    /* 001C */ SE3SoundKitProperties m_properties;
    /* 00F8 */ TSList<SE3CombinableSoundKitPosition, TSGetLink<SE3CombinableSoundKitPosition> > m_positions;

    SE3COMBINABLESOUNDLIST();
    ~SE3COMBINABLESOUNDLIST();
};

struct SE3StreamInfo;

struct SE3AsyncData : public TSLinkedNode<SE3AsyncData> {
    /* 0008 */ CAsyncObject* m_pAsyncObj;
    /* 000C */ SE3StreamInfo* m_pStreamInfo;
};

struct SE3StreamInfo {
    /* 0000 */ uint32_t m_nextBlock;
    /* 0004 */ SFileImpl* m_file;
    /* 0008 */ int32_t m_pendingReads;
    /* 000C */ int32_t m_closeRequested;
    /* 0010 */ TSList<SE3AsyncData, TSGetLink<SE3AsyncData> > m_asyncData;
    /* 001C */ int32_t m_encrypted;
    /* 0020 */ uint32_t m_headerPosition;
    /* 0024 */ int32_t m_headerState;
    /* 0028 */ uint8_t m_key[32];
    /* 0048 */ uint8_t m_iv[32];
    /* 0068 */ Crypt::PanamaCypher m_cypher;

    ~SE3StreamInfo();
};

class SE3WaveDataNode : public TSLinkedNode<SE3WaveDataNode> {
    public:
    /* 0008 */ SFileImpl* m_file;
    /* 000C */ FMOD::Sound* m_pFMODSound;
    /* 0010 */ SE3StreamInfo* m_pStreamInfo;
    /* 0014 */ char m_name[128];
    /* 0094 */ uint32_t m_key;
    /* 0098 */ SE3WAVEDATASTATE m_state;
    /* 009C */ int32_t m_nRefCount;
    /* 00A0 */ uint32_t m_size;
    /* 00A4 */ uint32_t m_uUniqueID;
    /* 00A8 */ int32_t m_deleteTimer;
    /* 00AC */ int32_t m_isStream;
    /* 00B0 */ TSLink<SE3WaveDataNode> m_unreferencedLink;

    static uint32_t sm_uNextUniqueID;

    SE3WaveDataNode();
    ~SE3WaveDataNode();
};

class SE3SoundEmitter : public TSLinkedNode<SE3SoundEmitter> {
    public:
    /* 0008 */ int32_t m_userData;
    /* 000C */ uint32_t m_hSoundEmitter;
    /* 0010 */ uint32_t m_uSoundKitObjectUID;
    /* 0014 */ struct SE3SoundChunk* m_pParentSoundChunk;
    /* 0018 */ int32_t m_nSoundKitID;
    /* 001C */ C3Vector m_position;
    /* 0028 */ TSGrowableArray<C3Vector> m_positions;
    /* 0038 */ SE3SoundKitProperties m_properties;

    SE3SoundEmitter();
    ~SE3SoundEmitter();
    void UpdatePillPosition(C3Vector listener);
};

struct SE3SoundEmitterLookup : public TSHashObject<SE3SoundEmitterLookup, HASHKEY_NONE> {
    /* 0018 */ SE3SoundEmitter* m_pSoundEmitter;
};

struct SE3SoundChunk : public TSHashObject<SE3SoundChunk, HASHKEY_NONE> {
    /* 0018 */ uint32_t m_handle;
    /* 001C */ float m_minX;
    /* 0020 */ float m_maxX;
    /* 0024 */ float m_minY;
    /* 0028 */ float m_maxY;
    /* 002C */ TSList<SE3SoundEmitter, TSGetLink<SE3SoundEmitter> > m_emitters;
    /* 0038 */ TSLink<SE3SoundChunk> m_dirtyLink;

    SE3SoundChunk();
    ~SE3SoundChunk();
};

struct SE3CreateSoundEmitterMessage : public TSLinkedNode<SE3CreateSoundEmitterMessage> {
    /* 0008 */ int32_t m_userData;
    /* 000C */ uint32_t m_hSoundEmitter;
    /* 0010 */ SE3SoundChunk* m_pParentSoundChunk;
    /* 0014 */ int32_t m_nSoundKitID;
    /* 0018 */ C3Vector m_position;
    /* 0024 */ TSGrowableArray<C3Vector> m_positions;
    /* 0034 */ SE3SoundKitProperties m_properties;

    SE3CreateSoundEmitterMessage();
    ~SE3CreateSoundEmitterMessage();
};

struct SE3Set3DPositionMessage : public TSLinkedNode<SE3Set3DPositionMessage> {
    /* 0008 */ uint32_t m_hSoundEmitter;
    /* 000C */ C3Vector m_position;
};

struct SE3DestroySoundEmitterMessage : public TSLinkedNode<SE3DestroySoundEmitterMessage> {
    /* 0008 */ uint32_t m_hSoundEmitter;
};

struct SE3MarkSoundChunkDirtyMessage : public TSLinkedNode<SE3MarkSoundChunkDirtyMessage> {
    /* 0008 */ SE3SoundChunk* m_pSoundChunk;
};

struct SE3DoneLoadingMessage : public TSLinkedNode<SE3DoneLoadingMessage> {
    /* 0008 */ FMOD::Sound* m_pFMODSound;
    /* 000C */ FMOD_RESULT m_result;
};

struct SE3MemorySoundEndedMessage : public TSLinkedNode<SE3MemorySoundEndedMessage> {
    /* 0008 */ uint32_t m_uUID;
};

struct SE3SoundEndedMessage : public TSLinkedNode<SE3SoundEndedMessage> {
    /* 0008 */ FMOD::Channel* m_pFMODChannel;
    /* 000C */ uint32_t m_uSoundKitObjectUID;
};

class SE3MemorySoundObject : public TSHashObject<SE3MemorySoundObject, HASHKEY_NONE> {
    public:
    /* 0018 */ uint32_t m_uUID;
    /* 001C */ FMOD::Channel* m_pFMODChannel;
    /* 0020 */ FMOD::Sound* m_pFMODSound;
    /* 0024 */ void (*m_fpPlayFinishedCallback)(SE3MemorySoundObject*, void*);
    /* 0028 */ void* m_playFinishedParam;

    SE3MemorySoundObject();
};

class SE3CaptureCallbacks {
    public:
    virtual ~SE3CaptureCallbacks();
    virtual void ProcessData(int16_t* data, uint32_t samples) = 0;
};

class SE3StreamCallbacks {
    public:
    virtual ~SE3StreamCallbacks();
    virtual bool GetSamples(uint32_t position, uint32_t count, float* buffer) = 0;
    virtual void DiscardSamples(uint32_t position, uint32_t count);
    virtual void Delete();
};

class SE3ChatSound : public TSHashObject<SE3ChatSound, HASHKEY_NONE> {
    public:
    static FMOD_RESULT F_CALLBACK DSPReadCallback(FMOD_DSP_STATE* dspState, float* inBuffer, float* outBuffer, unsigned int length, int inChannels, int outChannels);
    static FMOD_RESULT F_CALLBACK DSPSeekCallback(FMOD_DSP_STATE* dspState, unsigned int position);

    /* 0018 */ SE3StreamCallbacks* m_pCallbacks;
    /* 001C */ uint32_t m_position;
    /* 0020 */ FMOD::DSP* m_pFMODDSP;
    /* 0024 */ FMOD::Channel* m_pFMODChannel;
    /* 0028 */ float m_volume;
    /* 002C */ SE3BUS m_bus;
    /* 0030 */ uint32_t m_uUID;
    /* 0034 */ float m_frequencyScale;

    SE3ChatSound();
    ~SE3ChatSound();
    float GetScaledVolume();
};

struct SuppressorSound : public TSLinkedNode<SuppressorSound> {
    /* 0008 */ uint32_t m_uUID;
    /* 000C */ float m_scalar[4];
    /* 001C */ float m_distance;
    /* 0020 */ float m_duckTo[4];
    /* 0030 */ float m_innerRadius;
    /* 0034 */ float m_outerRadius;
    /* 0038 */ int32_t m_active;
    /* 003C */ int32_t m_timeToDuck;
    /* 0040 */ int32_t m_timeToUnduck;

    SuppressorSound();
};

class SE3 {
    public:
    static HSLOG sm_log;
    static int32_t sm_bCaptureInitialized;
    static FMOD::System* sm_pGameSystem;
    static FMOD::System* sm_pChatSystem;
    static FMOD::Sound* sm_pCaptureSound;
    static int32_t sm_bShuttingDown;
    static FMOD_OUTPUTTYPE sm_eGameSystemOutputType;
    static float sm_fFocusScalar;
    static uint32_t sm_uNextUniqueID;
    static SE3SoundBus sm_aSoundBus[SE3BUS_LAST];
    static char sm_sSystemDefaultName[64];
    static TSGrowableArray<SE3DriverInfo> sm_aCachedGameSoundOutputDriverInfo;
    static TSGrowableArray<SE3DriverInfo> sm_aCachedChatSoundInputDriverInfo;
    static TSGrowableArray<SE3DriverInfo> sm_aCachedChatSoundOutputDriverInfo;
    static TSHashTable<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE> sm_SoundKitObjectTable;
    static TSList<SE3SoundKitObject, TSGetLink<SE3SoundKitObject> > sm_SoundKitObjects;
    static TSHashTable<SE3MemorySoundObject, HASHKEY_NONE> sm_MemorySoundObjectTable;
    static TSHashTable<SE3ChatSound, HASHKEY_NONE> sm_ChatSoundObjectTable;
    static TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > sm_SoundCache_Loading;
    static TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > sm_SoundCache;
    static TSExplicitList<SE3WaveDataNode, offsetof(SE3WaveDataNode, m_unreferencedLink)> sm_SoundCacheUnreferenced;
    static TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > sm_Streams_Loading;
    static TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > sm_Streams;
    static TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > sm_WaveDataNode_DeleteList;
    static int32_t sm_nMaxCacheSizeInBytes;
    static uint32_t sm_uMaxCacheableSize;
    static int32_t sm_SoundCache_Stats_Count;
    static int32_t sm_SoundCache_Stats_TotalSizeInBytes;
    static void (*sm_fpCacheEventCallback)(SE3CACHEEVENT, uint32_t);
    static FMOD_RESULT (*sm_fpWaveDataDoneLoadingCallback)(FMOD_SOUND*, FMOD_RESULT);
    static SCritSect sm_SoundCache_Loading_CritSec;
    static SCritSect sm_StreamedSound_Loading_CritSec;
    static SCritSect sm_SE3ShuttingDown_CritSec;
    static SCritSect sm_DoneLoading_MessageQueue_CritSect;
    static TSList<SE3DoneLoadingMessage, TSGetLink<SE3DoneLoadingMessage> > sm_DoneLoading_MessageQueue;

    static void ActivateAlternateCaptureCallback();
    static void ActivatePrimaryCaptureCallback();
    static FMOD_RESULT AddChannelToChannelGroup(void* channel, void* channelGroup);
    static FMOD_RESULT AddMessageToQueue_DoneLoading(FMOD_SOUND* sound, FMOD_RESULT result);
    static void AddMessageToQueue_MemorySoundEnded(uint32_t uid);
    static void AddMessageToQueue_SoundEnded(FMOD::Channel* channel);
    static bool AlternateCaptureCallbackIsActive();
    static FMOD_RESULT AttachMonitorDSP(FMOD::Channel* channel);
    static FMOD_RESULT BypassDsp(void* dsp, bool bypass);
    static void CacheEventCallback(SE3CACHEEVENT event, uint32_t waveDataID);
    static void CaptureWork();
    static void CleanUpFMODGameSystem();
    static FMOD_RESULT ConnectDspToChannelGroup(void* dsp, void* channelGroup);
    static void* CreateChannelGroup(const char* name);
    static int32_t CreateChatSound(SE3StreamCallbacks* callbacks, uint32_t* handle, FMOD::System* system);
    static FMOD_RESULT CreateDSPByType(SE3_DSP_TYPE type, void** dsp);
    static SE3SoundKitObject* CreateNewSoundKitObject(uint32_t* handle, SE3SoundKitProperties* properties);
    static void CreateSoundEmitter(uint32_t* handle, const SoundEmittersRec* rec, SE3SoundKitProperties* properties);
    static int32_t CreateWaveData(uint32_t key, FMOD::SoundGroup* soundGroup, uint32_t mode, int32_t stream, uint32_t size, const char* filename, SFileImpl* file, uint32_t* waveDataID, SE3StreamInfo* streamInfo);
    static void DecrementWaveDataNodeRefCount(uint32_t waveDataID);
    static FMOD_RESULT DestroyChannelGroup(void* channelGroup);
    static void DestroyChatSystem();
    static void DoneLoadingMessageQueuePut(SE3DoneLoadingMessage* message);
    static FMOD_RESULT DspDestroy(void* dsp);
    static void EmitterManagerBuildSoundChunkList(uint32_t x, uint32_t y);
    static void EmitterManagerDestroyAllEmitters();
    static int32_t EmitterManagerEmitterExists(uint32_t handle);
    static uint32_t EmitterManagerGetSoundChunkHandle(const C3Vector& position);
    static SE3SoundChunk* EmitterManagerGetOrCreateSoundChunk(uint32_t handle);
    static void EmitterManagerHeartbeat(int32_t elapsed);
    static void EmitterManagerInit(void (*callback)(const C3Vector&, uint32_t*, uint32_t*), uint32_t chunkSize, uint32_t chunkRadius);
    static void EmitterManagerProcessEmitterMessages();
    static void StreamingUpdate();
    static void EmitterManagerSendMessage_CreateEmitter(uint32_t* handle, SE3SoundChunk* chunk, int32_t soundKitID, const C3Vector& position, SE3SoundKitProperties* properties, const TSGrowableArray<C3Vector>* positions, int32_t userData);
    static void EmitterManagerSendMessage_MarkSoundChunkDirty(SE3SoundChunk* chunk);
    static void EmitterManagerSendMessage_Set3DPosition(uint32_t handle, const C3Vector& position);
    static void EmitterManagerShutDown();
    static void EmitterMangerSendMessage_DestroyEmitter(uint32_t handle);
    static FMOD_RESULT EnableDSP(void* dsp, bool active);
    static void ErrCheck(uint32_t line, const char* file, FMOD_RESULT result);
    static void FadeInNow(uint32_t handle);
    static int32_t FindAndReferenceWaveDataNodeInCache(uint32_t key, uint32_t* waveDataID);
    static int32_t FindAndReferenceWaveDataNodeLoadingInCache(uint32_t key, uint32_t* waveDataID);
    static int32_t FindWaveDataNode(uint32_t waveDataID, SE3WaveDataNode** node);
    static void FreeSoundKitDefList();
    static int32_t GetChannelGroupNumChannels(void* channelGroup);
    static char* GetCurrentGameOutputDeviceName();
    static int32_t GetFallbackSoundKitID(int32_t soundKitID);
    static int32_t GetInputDriverName(int32_t index, char* name, int32_t nameLength, FMOD::System* system);
    static int32_t GetInputDriverName_Cached(int32_t index, char* name, int32_t nameLength, FMOD::System* system);
    static FMOD_RESULT GetListenerPosition(C3Vector* position);
    static uint32_t GetModeFlags(const C3Vector* position, const SE3SoundKitDef* def, SE3SoundKitProperties* properties);
    static int32_t GetNumInputDrivers(FMOD::System* system);
    static int32_t GetNumOutputDrivers_Cached(FMOD::System* system);
    static int32_t GetNumSoundsInSoundKit(int32_t soundKitID);
    static int32_t GetOutputDriverName(int32_t index, char* name, int32_t nameLength, FMOD::System* system);
    static int32_t GetOutputDriverName_Cached(int32_t index, char* name, int32_t nameLength, FMOD::System* system);
    static C3Vector GetPosition(uint32_t handle);
    static uint32_t GetPositionInMS(uint32_t handle);
    static SE3SoundKitDef* GetSoundKitDef(int32_t soundKitID);
    static int32_t GetSoundKitID(const char* name);
    static int32_t GetSoundKitID(uint32_t handle);
    static int32_t GetSoundKitUsage(int32_t soundKitID);
    static float GetSuppressionScalar(SE3BUS bus, uint32_t uid);
    static int32_t HeartBeat(const void* data, void* param);
    static int32_t Init(SE3EngineInitParams* params);
    static void InitCapture(int32_t& outputDriverIndex, const char* outputDriverName, int32_t& inputDriverIndex, const char* inputDriverName, float sampleRate, int32_t& hasInputDevices, SE3SOUNDQUALITY quality, SE3MIXMODE2 mixMode);
    static void InitGameSystemForSilence();
    static void InitSoundKitDefArray(uint32_t maxID);
    static void InitSoundKitDefList();
    static void InitSuppressorSounds();
    static void InitializeDefaultSystemName();
    static void InsertSoundKitDef(const SoundEntriesRec* rec, const SoundEntriesAdvancedRec* advancedRec, int32_t ownsRecords);
    static int32_t IsBusEqualOrChild(SE3BUS bus, SE3BUS parent);
    static bool IsCapturing();
    static int32_t IsChatSoundInitialized();
    static int32_t IsLoopingOrPeriodic(uint32_t handle);
    static int32_t IsLoopingOrPeriodic_ByDef(int32_t soundKitID);
    static int32_t IsLooping_ByDef(int32_t soundKitID);
    static int32_t IsPlaying(uint32_t handle);
    static int32_t IsStoppable(uint32_t handle);
    static int32_t IsTooCloseToExistingSoundKit(int32_t soundKitID, const C3Vector& position);
    static void KillVirtualSounds(int32_t elapsed);
    static void Log_Init();
    static int32_t PreloadSoundKit(int32_t soundKitID, int32_t wait);
    static void Log_Write(const char* format, ...);
    static void Log_Write(uint32_t line, const char* file, FMOD_RESULT result, const char* format, ...);
    static void ManageOutOfRangeSounds(int32_t elapsed);
    static int32_t PlaySoundFile(const char* filename, uint32_t* handle, SE3BUS bus, SE3USAGE usage);
    static int32_t PlaySoundFromMemory(SE3MemorySoundProperties* properties, int32_t chatSystem);
    static int32_t PlaySoundKit(const char* name, const C3Vector* position, SE3SoundKitProperties* properties, uint32_t* handle, const char* file, int32_t line, const char* function);
    static int32_t PlaySoundKit(int32_t soundKitID, const C3Vector* position, SE3SoundKitProperties* properties, uint32_t* handle, const char* file, int32_t line, const char* function, int32_t streamed = 0, const uint8_t* key = NULL);
    static void ProcessCombinableSounds();
    static void ProcessDoneLoadingMessageQueue();
    static void ProcessDownloadList();
    static void ProcessFadeList(uint32_t elapsed);
    static void ProcessGoGoGoList();
    static void ProcessMemorySoundEndedMessageQueue();
    static void ProcessSoundEndedMessageQueue();
    static void ProcessSoundKitObjectDeleteList();
    static void ProcessWaveDataNodeDeleteList(int32_t elapsed, int32_t force);
    static void RegisterCacheEventCallback(void (*callback)(SE3CACHEEVENT, uint32_t));
    static void RegisterSuppressor(SE3SoundKitObject* sko);
    static void RegisterWaveDataDoneLoadingCallback(FMOD_RESULT (*callback)(FMOD_SOUND*, FMOD_RESULT));
    static void ReleaseUnusedStreams();
    static int32_t ResetSoundKitOrder(int32_t soundKitID);
    static void Set3DPosition(uint32_t handle, const C3Vector& position);
    static void SetAlternateCaptureCallback(SE3CaptureCallbacks* callbacks);
    static void SetAttachCustomDSPChainCallback(SE3SoundKitObject* sko, void (*callback)(void*, char*), const char* name);
    static void SetDeviceChangeCallback(void (*callback)());
    static FMOD_RESULT SetDrivers(SE3EngineInitParams* params, FMOD::System* system);
    static FMOD_RESULT SetDspParam(void* dsp, int32_t index, float value);
    static FMOD_RESULT SetFMODChannelGroupVolume(void* channelGroup, float volume);
    static void SetFocusScalar(float scalar);
    static void SetFrequencyScale(uint32_t handle, float scale);
    static void SetListenerAttributes(const C3Vector& position, const C3Vector& forward, const C3Vector& up, const C3Vector* velocity);
    static void SetMaxCacheSizeInBytes(int32_t size);
    static void SetMaxCacheableSizeInBytes(uint32_t size);
    static int32_t SetMemorySoundFinishedCallback(uint32_t handle, void (*callback)(SE3MemorySoundObject*, void*));
    static void SetNewSoundKitObjectProperties(SE3SoundKitObject* sko, const C3Vector* position, SE3SoundKitProperties* properties, uint32_t modeFlags);
    static void SetPlayFinishedCallback(uint32_t handle, void (*callback)(SE3SoundKitObject*), void* param);
    static void SetPrimaryCaptureCallback(SE3CaptureCallbacks* callbacks);
    static void SetReverb(const SE3_REVERB_PROPERTIES* properties);
    static void SetReverbPropertiesAndSpeakerMix(SE3SoundKitObject* sko, FMOD::Channel* channel);
    static void SetUserMonitorDSPCallback(void (*callback)(float*, uint32_t));
    static int32_t ShouldSoundPlay(SE3SoundKitDef* def, SE3SoundKitProperties* properties, const C3Vector* position);
    static void ShutDown();
    static void ShutDownCapture();
    static void SoundBusDecrementChannelCount(SE3BUS bus);
    static void SoundBusEnable(SE3BUS bus, int32_t enable);
    static float SoundBusGetSuppressedVolumeScalar(SE3BUS bus, uint32_t uid);
    static float SoundBusGetVolume(SE3BUS bus);
    static void SoundBusIncrementChannelCount(SE3BUS bus);
    static void SoundBusInitBuses();
    static int32_t SoundBusIsEnabled(SE3BUS bus);
    static int32_t SoundBusIsFull(SE3BUS bus);
    static int32_t SoundBusIsMuted(SE3BUS bus);
    static void SoundBusMute(SE3BUS bus, int32_t mute);
    static void SoundBusSetParameters(SE3BUS bus, SE3BUS parent, int32_t maxChannels, float volume, int32_t muted, int32_t enabled);
    static void SoundBusSetVolume(SE3BUS bus, float volume);
    static void SoundKitDefLog(const char* format, ...);
    static void SoundKitObjectLog(SE3SoundKitObject* sko, const char* file, int32_t line, const char* format, ...);
    static void SoundKitObjects_HandleWaveDataDelete(uint32_t waveDataID);
    static void SoundKitObjects_Loading_Delete(uint32_t waveDataID);
    static void SoundKitObjects_Loading_Ready(uint32_t waveDataID);
    static void StartCapture();
    static void StopAllSounds(SE3BUS bus, int32_t fadeOutTime);
    static int32_t StopMemorySound(uint32_t handle);
    static void StopOrFadeOut(SE3SoundKitObject* sko, int32_t fadeOutTime, const char* file, int32_t line, const char* function);
    static int32_t StopSound(uint32_t handle, int32_t fadeOutTime, const char* file, int32_t line, const char* function);
    static void StopCapture();
    static int32_t SuppressorIsPresent();
    static void TrimCache();
    static void UnregisterSuppressor(uint32_t uid);
    static void UpdateAdvancedProperties(SE3SoundKitObject* sko, uint32_t elapsed, int32_t retrigger);
    static void UpdateSuppressors(int32_t elapsed);
    static void UpdateVolumes();
    static void VoiceChat_Play(uint32_t handle);
    static void VoiceChat_SetFrequencyScale(uint32_t handle, float scale);
    static void VoiceChat_StopSound(uint32_t handle);
    static void WaveData_PutOnDeleteList(SE3WaveDataNode* node, int32_t referenced);
};

#include "Engine/Source/SoundEngine/SE3.inl"

#endif
