#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SLock.h"

// STATIC: %0x00C05B34
int32_t SE3::sm_nMaxCacheSizeInBytes = 0x1000000;

// STATIC: %0x00C05B38
uint32_t SE3::sm_uMaxCacheableSize = 0x100000;

// STATIC: %0x00C05B3C
uint32_t SE3WaveDataNode::sm_uNextUniqueID = 1;

// STATIC: %0x00C05B40
TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > SE3::sm_SoundCache_Loading;

// STATIC: %0x00C05B4C
TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > SE3::sm_SoundCache;

// STATIC: %0x00C05B58
TSExplicitList<SE3WaveDataNode, offsetof(SE3WaveDataNode, m_unreferencedLink)> SE3::sm_SoundCacheUnreferenced;

// STATIC: %0x00C05B64
TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > SE3::sm_Streams_Loading;

// STATIC: %0x00C05B70
TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > SE3::sm_Streams;

// STATIC: %0x00C05B7C
TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > SE3::sm_WaveDataNode_DeleteList;

// STATIC: %0x00D0E1AC
int32_t SE3::sm_SoundCache_Stats_Count;

// STATIC: %0x00D0E1B0
int32_t SE3::sm_SoundCache_Stats_TotalSizeInBytes;

// STATIC: %0x00D0E1B4
void (*SE3::sm_fpCacheEventCallback)(SE3CACHEEVENT, uint32_t);

// STATIC: %0x00D0E1B8
FMOD_RESULT (*SE3::sm_fpWaveDataDoneLoadingCallback)(FMOD_SOUND*, FMOD_RESULT);

// STATIC: %0x00D0E1BC
SCritSect SE3::sm_SoundCache_Loading_CritSec;

// STATIC: %0x00D0E1E4
SCritSect SE3::sm_StreamedSound_Loading_CritSec;

// STATIC: %0x00D0E20C
SCritSect SE3::sm_SE3ShuttingDown_CritSec;

// PROVEN: 9329de35 IDENTICAL/NONE
// OFFSET: 0x007153C0/0x1403E80A0
void SE3::SetMaxCacheSizeInBytes(int32_t size) {
    SE3::sm_nMaxCacheSizeInBytes = size;
}

// PROVEN: a622bc94 IDENTICAL/NONE
// OFFSET: 0x007153D0/0x1403E80B0
void SE3::SetMaxCacheableSizeInBytes(uint32_t size) {
    SE3::sm_uMaxCacheableSize = size;
}

// OFFSET: %0x0067C0B0
void SE3::RegisterCacheEventCallback(void (*callback)(SE3CACHEEVENT, uint32_t)) {
    SE3::sm_fpCacheEventCallback = callback;
}

// OFFSET: %0x0067C0C0
void SE3::RegisterWaveDataDoneLoadingCallback(FMOD_RESULT (*callback)(FMOD_SOUND*, FMOD_RESULT)) {
    SE3::sm_fpWaveDataDoneLoadingCallback = callback;
}

// OFFSET: %0x0067C0D0
static FMOD_RESULT F_CALLBACK DoneLoadingCallback(FMOD_SOUND* sound, FMOD_RESULT result) {
    SE3::sm_SE3ShuttingDown_CritSec.Enter();
    FMOD_RESULT r = SE3::sm_fpWaveDataDoneLoadingCallback(sound, result);
    SE3::sm_SE3ShuttingDown_CritSec.Leave();

    return r;
}

// OFFSET: %0x0067C110
static FMOD_RESULT F_CALLBACK SECloseCallback(void* handle, void* userdata) {
    return SFileCloseFile(static_cast<SFileImpl*>(handle)) ? FMOD_OK : FMOD_ERR_INVALID_HANDLE;
}

// OFFSET: %0x0067C130
static FMOD_RESULT F_CALLBACK SEReadCallback(void* handle, void* buffer, unsigned int sizebytes, unsigned int* bytesread, void* userdata) {
    uint32_t read = 0;

    SFile::SetReadFatal(false);
    int32_t success = SFileReadFile(static_cast<SFileImpl*>(handle), buffer, sizebytes, &read, NULL, 0);
    SFile::SetReadFatal(true);

    *bytesread = read;

    if (success) {
        return FMOD_OK;
    }

    return SErrGetLastError() == 38 ? FMOD_ERR_FILE_EOF : FMOD_ERR_INTERNAL;
}

// PROVEN: 9c458ad0 IDENTICAL/NONE
// OFFSET: 0x00715730/0x1403E8480
static FMOD_RESULT F_CALLBACK SESeekCallback(void* handle, unsigned int pos, void* userdata) {
    SFile::SetFilePointer(static_cast<SFileImpl*>(handle), pos, NULL, 0);
    return FMOD_OK;
}

// OFFSET: %0x0067C1C0
void SE3::DecrementWaveDataNodeRefCount(uint32_t waveDataID) {
    if (!waveDataID) {
        return;
    }

    for (SE3WaveDataNode* node = SE3::sm_SoundCache.Head(); node; node = SE3::sm_SoundCache.Next(node)) {
        if (node->m_uUniqueID == waveDataID) {
            if (node->m_nRefCount-- == 1) {
                SE3::sm_SoundCacheUnreferenced.LinkToTail(node);
            }

            break;
        }
    }

    for (SE3WaveDataNode* node = SE3::sm_Streams.Head(); node; node = SE3::sm_Streams.Next(node)) {
        if (node->m_uUniqueID == waveDataID) {
            node->m_nRefCount--;
            return;
        }
    }
}

// OFFSET: %0x0067C240
static int32_t OpenLoadingCacheFile(uint32_t key, FMOD_RESULT* result, unsigned int* filesize, void** handle, void** userdata) {
    SE3WaveDataNode* found = NULL;

    SE3::sm_SoundCache_Loading_CritSec.Enter();

    SE3WaveDataNode* node;

    for (node = SE3::sm_SoundCache_Loading.Head(); node; node = SE3::sm_SoundCache_Loading.Next(node)) {
        if (node->m_key == key) {
            found = node;
            *filesize = SFileGetFileSize(node->m_file, NULL);
            *handle = node->m_file;
            *userdata = NULL;
            node->m_file = reinterpret_cast<SFileImpl*>(-1);
            break;
        }
    }

    if (!node) {
        *result = FMOD_ERR_INVALID_HANDLE;
    }

    SE3::sm_SoundCache_Loading_CritSec.Leave();

    return found != NULL;
}

// OFFSET: %0x0067C2D0
static int32_t OpenLoadingStreamFile(uint32_t key, FMOD_RESULT* result, unsigned int* filesize, void** handle, void** userdata) {
    SE3WaveDataNode* found = NULL;
    *result = FMOD_OK;

    SE3::sm_StreamedSound_Loading_CritSec.Enter();

    SE3WaveDataNode* node;

    for (node = SE3::sm_Streams_Loading.Head(); node; node = SE3::sm_Streams_Loading.Next(node)) {
        if (node->m_key == key && node->m_file != reinterpret_cast<SFileImpl*>(-1)) {
            found = node;
            *filesize = SFileGetFileSize(node->m_file, NULL);
            *handle = node->m_file;
            *userdata = NULL;
            node->m_file = reinterpret_cast<SFileImpl*>(-1);
            break;
        }
    }

    if (!node) {
        *result = FMOD_ERR_INVALID_HANDLE;
    }

    SE3::sm_StreamedSound_Loading_CritSec.Leave();

    return found != NULL;
}

// OFFSET: %0x0067C370
int32_t SE3::FindAndReferenceWaveDataNodeInCache(uint32_t key, uint32_t* waveDataID) {
    SE3WaveDataNode* node;

    for (node = SE3::sm_SoundCache.Head(); node; node = SE3::sm_SoundCache.Next(node)) {
        if (node->m_key == key) {
            break;
        }
    }

    if (!node) {
        return 0;
    }

    if (waveDataID) {
        *waveDataID = node->m_uUniqueID;
    }

    if (++node->m_nRefCount == 1) {
        SE3::sm_SoundCacheUnreferenced.UnlinkNode(node);
    }

    return 1;
}

// OFFSET: %0x0067C410
int32_t SE3::FindAndReferenceWaveDataNodeLoadingInCache(uint32_t key, uint32_t* waveDataID) {
    SE3WaveDataNode* node;

    for (node = SE3::sm_SoundCache_Loading.Head(); node; node = SE3::sm_SoundCache_Loading.Next(node)) {
        if (node->m_key == key) {
            break;
        }
    }

    if (!node) {
        return 0;
    }

    if (waveDataID) {
        *waveDataID = node->m_uUniqueID;
    }

    if (++node->m_nRefCount == 1) {
        SE3::sm_SoundCacheUnreferenced.UnlinkNode(node);
    }

    return 1;
}

// OFFSET: %0x0067C4B0
static FMOD_RESULT F_CALLBACK SEOpenCallback(const char* name, int unicode, unsigned int* filesize, void** handle, void** userdata) {
    FMOD_RESULT result = FMOD_OK;
    uint32_t key = atoi(name);

    if (!OpenLoadingCacheFile(key, &result, filesize, handle, userdata)) {
        OpenLoadingStreamFile(key, &result, filesize, handle, userdata);
    }

    return result;
}

// OFFSET: %0x0067C510
int32_t SE3::FindWaveDataNode(uint32_t waveDataID, SE3WaveDataNode** node) {
    for (SE3WaveDataNode* cached = SE3::sm_SoundCache.Head(); cached; cached = SE3::sm_SoundCache.Next(cached)) {
        if (cached->m_uUniqueID == waveDataID) {
            *node = cached;
            return 1;
        }
    }

    for (SE3WaveDataNode* stream = SE3::sm_Streams.Head(); stream; stream = SE3::sm_Streams.Next(stream)) {
        if (stream->m_uUniqueID == waveDataID) {
            *node = stream;
            return 1;
        }
    }

    return 0;
}

// OFFSET: %0x0067C580
void SE3::WaveData_PutOnDeleteList(SE3WaveDataNode* node, int32_t referenced) {
    if (node->m_nRefCount && SE3::sm_fpCacheEventCallback) {
        SE3::sm_fpCacheEventCallback(SE3CACHEEVENT_DELETED, node->m_uUniqueID);
    }

    SE3::sm_WaveDataNode_DeleteList.LinkToTail(node);

    if (referenced && !node->m_isStream) {
        SE3::sm_SoundCache_Stats_Count--;
        SE3::sm_SoundCache_Stats_TotalSizeInBytes -= node->m_size;
    }
}

// OFFSET: %0x0067C5E0
SE3WaveDataNode::SE3WaveDataNode() {
    this->m_file = NULL;
    this->m_pFMODSound = NULL;
    this->m_name[0] = '\0';
    this->m_key = 0;
    this->m_state = static_cast<SE3WAVEDATASTATE>(0);
    this->m_nRefCount = 0;
    this->m_size = 0;
    this->m_uUniqueID = ++SE3WaveDataNode::sm_uNextUniqueID;
    this->m_isStream = 0;
    this->m_deleteTimer = rand() % 512 + 500;
}

// OFFSET: %0x0067C660
void SE3::ProcessDoneLoadingMessageQueue() {
    TSList<SE3DoneLoadingMessage, TSGetLink<SE3DoneLoadingMessage> > messages;

    SE3::sm_DoneLoading_MessageQueue_CritSect.Enter();
    messages.Combine(&SE3::sm_DoneLoading_MessageQueue, 2, NULL);
    SE3::sm_DoneLoading_MessageQueue_CritSect.Leave();

    SE3DoneLoadingMessage* message = messages.Head();

    while (message) {
        SE3DoneLoadingMessage* next = messages.Next(message);
        FMOD::Sound* sound = message->m_pFMODSound;
        FMOD_RESULT result = message->m_result;
        SE3WaveDataNode* found = NULL;
        int32_t matched = 0;
        int32_t loaded = 0;

        SE3::sm_SoundCache_Loading_CritSec.Enter();

        for (SE3WaveDataNode* node = SE3::sm_SoundCache_Loading.Head(); node; node = SE3::sm_SoundCache_Loading.Next(node)) {
            if (node->m_pFMODSound == sound) {
                if (result != FMOD_OK) {
                    if (SE3::sm_fpCacheEventCallback) {
                        SE3::sm_fpCacheEventCallback(SE3CACHEEVENT_LOADING_FAILED, node->m_uUniqueID);
                    }

                    node->m_state = SE3WAVEDATA_FAILED;
                    matched = 1;
                    found = node;
                    SE3::sm_SoundCache_Loading.UnlinkNode(node);
                    SE3::WaveData_PutOnDeleteList(node, 0);
                } else {
                    node->m_state = SE3WAVEDATA_LOADED;
                    matched = 1;
                    loaded = 1;
                    found = node;
                    SE3::sm_SoundCache_Loading.UnlinkNode(node);
                    SE3::sm_SoundCache.LinkToTail(node);
                    SE3::sm_SoundCache_Stats_Count++;
                    SE3::sm_SoundCache_Stats_TotalSizeInBytes += node->m_size;
                }

                break;
            }
        }

        SE3::sm_SoundCache_Loading_CritSec.Leave();

        if (!matched) {
            SE3::sm_StreamedSound_Loading_CritSec.Enter();

            for (SE3WaveDataNode* node = SE3::sm_Streams_Loading.Head(); node; node = SE3::sm_Streams_Loading.Next(node)) {
                if (node->m_pFMODSound == sound) {
                    if (result != FMOD_OK) {
                        if (SE3::sm_fpCacheEventCallback) {
                            SE3::sm_fpCacheEventCallback(SE3CACHEEVENT_LOADING_FAILED, node->m_uUniqueID);
                        }

                        node->m_state = SE3WAVEDATA_FAILED;
                        found = node;
                        SE3::sm_Streams_Loading.UnlinkNode(node);
                        SE3::WaveData_PutOnDeleteList(node, 0);
                    } else {
                        node->m_state = SE3WAVEDATA_LOADED;
                        loaded = 1;
                        found = node;
                        SE3::sm_Streams_Loading.UnlinkNode(node);
                        SE3::sm_Streams.LinkToTail(node);
                    }

                    break;
                }
            }

            SE3::sm_StreamedSound_Loading_CritSec.Leave();
        }

        if (!loaded || (found && found->m_state == SE3WAVEDATA_FAILED)) {
            if (found) {
                SE3::WaveData_PutOnDeleteList(found, 0);
            } else {
                sound->release();
            }
        } else if (SE3::sm_fpCacheEventCallback) {
            SE3::sm_fpCacheEventCallback(SE3CACHEEVENT_LOADED, found->m_uUniqueID);
        }

        SE3::DoneLoadingMessageQueuePut(message);
        message = next;
    }
}

// OFFSET: %0x0067CBB0
int32_t SE3::CreateWaveData(uint32_t key, FMOD::SoundGroup* soundGroup, uint32_t mode, int32_t stream, uint32_t size, const char* filename, SFileImpl* file, uint32_t* waveDataID) {
    FMOD_CREATESOUNDEXINFO exinfo;
    memset(&exinfo, 0, sizeof(exinfo));
    exinfo.cbsize = sizeof(exinfo);
    exinfo.nonblockcallback = DoneLoadingCallback;
    exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_WAV;
    exinfo.useropen = SEOpenCallback;
    exinfo.userclose = SECloseCallback;
    exinfo.userread = SEReadCallback;
    exinfo.userseek = SESeekCallback;
    exinfo.initialsoundgroup = reinterpret_cast<FMOD_SOUNDGROUP*>(soundGroup);

    const char* extension = strrchr(filename, '.');

    if (extension) {
        if (!_stricmp(extension, ".wav")) {
            exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_WAV;
        } else if (!_stricmp(extension, ".ogg")) {
            exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_OGGVORBIS;
        } else if (!_stricmp(extension, ".mp3")) {
            mode |= FMOD_MPEGSEARCH;
            exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_MPEG;
        }
    }

    SE3WaveDataNode* node = new (__FILE__, __LINE__) SE3WaveDataNode();

    if (!node) {
        SFileCloseFile(file);
        return SE3_ERR_MEMORY;
    }

    node->m_file = file;
    node->m_state = SE3WAVEDATA_LOADING;
    SStrCopy(node->m_name, filename, sizeof(node->m_name));
    node->m_isStream = stream;
    node->m_key = key;
    node->m_size = size;

    if (waveDataID) {
        *waveDataID = node->m_uUniqueID;
    }

    char name[32];
    SStrPrintf(name, sizeof(name), "%d", node->m_key);

    FMOD_RESULT result;

    if (node->m_isStream) {
        SE3::sm_StreamedSound_Loading_CritSec.Enter();
        SE3::sm_Streams_Loading.LinkToTail(node);
        result = SE3::sm_pGameSystem->createStream(name, mode, &exinfo, &node->m_pFMODSound);

        if (result != FMOD_OK) {
            SFileCloseFile(file);
            node->m_file = NULL;
            delete node;
        }

        SE3::sm_StreamedSound_Loading_CritSec.Leave();
    } else {
        SE3::sm_SoundCache_Loading_CritSec.Enter();
        SE3::sm_SoundCache_Loading.LinkToTail(node);
        result = SE3::sm_pGameSystem->createSound(name, mode, &exinfo, &node->m_pFMODSound);

        if (result != FMOD_OK) {
            SFileCloseFile(file);
            node->m_file = NULL;
            delete node;
        }

        SE3::sm_SoundCache_Loading_CritSec.Leave();
    }

    return result != FMOD_OK ? SE3_ERR_FMOD : SE3_OK;
}

// OFFSET: %0x0067CE30
void SE3::ProcessWaveDataNodeDeleteList(int32_t elapsed, int32_t force) {
    if (force) {
        SE3::sm_SoundCache_Loading_CritSec.Enter();

        for (SE3WaveDataNode* node = SE3::sm_SoundCache_Loading.Head(); node; ) {
            SE3WaveDataNode* next = SE3::sm_SoundCache_Loading.Next(node);
            SE3::sm_WaveDataNode_DeleteList.LinkToTail(node);
            node = next;
        }

        SE3::sm_SoundCache_Loading_CritSec.Leave();

        SE3::sm_StreamedSound_Loading_CritSec.Enter();

        for (SE3WaveDataNode* node = SE3::sm_Streams_Loading.Head(); node; ) {
            SE3WaveDataNode* next = SE3::sm_Streams_Loading.Next(node);
            SE3::sm_WaveDataNode_DeleteList.LinkToTail(node);
            node = next;
        }

        SE3::sm_StreamedSound_Loading_CritSec.Leave();

        for (SE3WaveDataNode* node = SE3::sm_SoundCache.Head(); node; ) {
            SE3WaveDataNode* next = SE3::sm_SoundCache.Next(node);
            SE3::sm_WaveDataNode_DeleteList.LinkToTail(node);
            node = next;
        }

        for (SE3WaveDataNode* node = SE3::sm_Streams.Head(); node; ) {
            SE3WaveDataNode* next = SE3::sm_Streams.Next(node);
            SE3::sm_WaveDataNode_DeleteList.LinkToTail(node);
            node = next;
        }
    }

    SE3WaveDataNode* node = SE3::sm_WaveDataNode_DeleteList.Head();

    while (node) {
        SE3WaveDataNode* next = SE3::sm_WaveDataNode_DeleteList.Next(node);
        node->m_deleteTimer -= elapsed;

        bool ready = force != 0;

        if (force || (node->m_deleteTimer <= 0 && (ready = true, !node->m_pFMODSound || (node->m_pFMODSound->readyForRelease(&ready), ready)))) {
            if (node->m_pFMODSound) {
                node->m_pFMODSound->release();
                node->m_pFMODSound = reinterpret_cast<FMOD::Sound*>(-1);
                delete node;
            }
        }

        node = next;
    }
}

// PROVEN: 1bd576ea IDENTICAL/NONE
// OFFSET: 0x00716BF0
void SE3::SetReverb(const SE3_REVERB_PROPERTIES* properties) {
    if (SE3::sm_pGameSystem) {
        SE3::sm_pGameSystem->setReverbProperties(properties);
    }
}

// PROVEN: 6ce5932a IDENTICAL/NONE
// OFFSET: 0x00716C10
FMOD_RESULT SE3::CreateDSPByType(SE3_DSP_TYPE type, void** dsp) {
    return SE3::sm_pGameSystem->createDSPByType(type, reinterpret_cast<FMOD::DSP**>(dsp));
}

// PROVEN: ab383207 IDENTICAL/NONE
// OFFSET: 0x00716C30/0x1403EA110
FMOD_RESULT SE3::SetDspParam(void* dsp, int32_t index, float value) {
    return static_cast<FMOD::DSP*>(dsp)->setParameter(index, value);
}

// PROVEN: 8ea572e5 IDENTICAL/NONE
// OFFSET: 0x00716C50/0x1403EA120
FMOD_RESULT SE3::BypassDsp(void* dsp, bool bypass) {
    return static_cast<FMOD::DSP*>(dsp)->setBypass(bypass);
}

// PROVEN: ec08a0ca IDENTICAL/NONE
// OFFSET: 0x00716C70/0x1403EA130
FMOD_RESULT SE3::EnableDSP(void* dsp, bool active) {
    return static_cast<FMOD::DSP*>(dsp)->setActive(active);
}

// PROVEN: eada81e0 IDENTICAL/NONE
// OFFSET: 0x00716C90/0x1403EA140
FMOD_RESULT SE3::DspDestroy(void* dsp) {
    static_cast<FMOD::DSP*>(dsp)->remove();
    static_cast<FMOD::DSP*>(dsp)->release();
    return FMOD_OK;
}

// PROVEN: eec84d60 IDENTICAL/NONE
// OFFSET: 0x00716CB0/0x1403EA160
FMOD_RESULT SE3::ConnectDspToChannelGroup(void* dsp, void* channelGroup) {
    FMOD::DSP* head;
    FMOD::DSP* output;

    static_cast<FMOD::ChannelGroup*>(channelGroup)->getDSPHead(&head);
    head->getOutput(0, &output);
    head->disconnectAll(false, true);
    output->addInput(static_cast<FMOD::DSP*>(dsp));
    static_cast<FMOD::DSP*>(dsp)->addInput(head);

    return FMOD_OK;
}

// PROVEN: 206719e8 IDENTICAL/NONE
// OFFSET: 0x00716D00
int32_t SE3::GetChannelGroupNumChannels(void* channelGroup) {
    int numChannels = 0;
    static_cast<FMOD::ChannelGroup*>(channelGroup)->getNumChannels(&numChannels);
    return numChannels;
}

// PROVEN: e0d5fdfc IDENTICAL/NONE
// OFFSET: 0x00716D20
void* SE3::CreateChannelGroup(const char* name) {
    FMOD::ChannelGroup* channelGroup = NULL;
    SE3::sm_pGameSystem->createChannelGroup(name, &channelGroup);
    return channelGroup;
}

// PROVEN: 238dcca3 IDENTICAL/NONE
// OFFSET: 0x00716D50/0x1403EA210
FMOD_RESULT SE3::DestroyChannelGroup(void* channelGroup) {
    return static_cast<FMOD::ChannelGroup*>(channelGroup)->release();
}

// PROVEN: 40960923 IDENTICAL/NONE
// OFFSET: 0x00716D60/0x1403EA220
FMOD_RESULT SE3::AddChannelToChannelGroup(void* channel, void* channelGroup) {
    return static_cast<FMOD::Channel*>(channel)->setChannelGroup(static_cast<FMOD::ChannelGroup*>(channelGroup));
}

// PROVEN: 2823cd42 IDENTICAL/NONE
// OFFSET: 0x00716D80/0x1403EA230
FMOD_RESULT SE3::SetFMODChannelGroupVolume(void* channelGroup, float volume) {
    return static_cast<FMOD::ChannelGroup*>(channelGroup)->setVolume(volume);
}
