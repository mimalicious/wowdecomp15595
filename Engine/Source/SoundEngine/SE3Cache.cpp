#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SLock.h"

// STATIC: 0x00D00490/0x140BAFE18
int32_t SE3::sm_nMaxCacheSizeInBytes = 0x1000000;

// STATIC: 0x00D00494/0x140BAFE1C
uint32_t SE3::sm_uMaxCacheableSize = 0x100000;

// STATIC: 0x00D00498
uint32_t SE3WaveDataNode::sm_uNextUniqueID = 1;

// STATIC: 0x00D0049C
TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > SE3::sm_SoundCache_Loading;

// STATIC: 0x00D004A8
TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > SE3::sm_SoundCache;

// STATIC: 0x00D004B4
TSExplicitList<SE3WaveDataNode, offsetof(SE3WaveDataNode, m_unreferencedLink)> SE3::sm_SoundCacheUnreferenced;

// STATIC: 0x00D004C0
TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > SE3::sm_Streams_Loading;

// STATIC: 0x00D004CC
TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > SE3::sm_Streams;

// STATIC: 0x00D004D8
TSList<SE3WaveDataNode, TSGetLink<SE3WaveDataNode> > SE3::sm_WaveDataNode_DeleteList;

// STATIC: 0x00E4E854
int32_t SE3::sm_SoundCache_Stats_Count;

// STATIC: 0x00E4E858
int32_t SE3::sm_SoundCache_Stats_TotalSizeInBytes;

// STATIC: 0x00E4E85C
void (*SE3::sm_fpCacheEventCallback)(SE3CACHEEVENT, uint32_t);

// STATIC: 0x00E4E860/0x140D37B20
FMOD_RESULT (*SE3::sm_fpWaveDataDoneLoadingCallback)(FMOD_SOUND*, FMOD_RESULT);

// STATIC: 0x00E4E864
SCritSect SE3::sm_SoundCache_Loading_CritSec;

// STATIC: 0x00E4E88C
SCritSect SE3::sm_StreamedSound_Loading_CritSec;

// STATIC: 0x00E4E8B4/0x140D37B78
SCritSect SE3::sm_SE3ShuttingDown_CritSec;

// STATIC: 0x00D004E4
static int32_t s_mp3Bitrates[15] = { 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 };

// STATIC: 0x00E4E8DC
static SCritSect s_streamCritSect;

// PROVEN: 9329de35 IDENTICAL/IDENTICAL
// OFFSET: 0x007153C0/0x1403E80A0
void SE3::SetMaxCacheSizeInBytes(int32_t size) {
    SE3::sm_nMaxCacheSizeInBytes = size;
}

// PROVEN: a622bc94 IDENTICAL/IDENTICAL
// OFFSET: 0x007153D0/0x1403E80B0
void SE3::SetMaxCacheableSizeInBytes(uint32_t size) {
    SE3::sm_uMaxCacheableSize = size;
}

// PROVEN: fba01528 IDENTICAL/NONE
// OFFSET: 0x007153E0
void SE3::RegisterCacheEventCallback(void (*callback)(SE3CACHEEVENT, uint32_t)) {
    SE3::sm_fpCacheEventCallback = callback;
}

// PROVEN: 3b32313f IDENTICAL/NONE
// OFFSET: 0x007153F0
void SE3::RegisterWaveDataDoneLoadingCallback(FMOD_RESULT (*callback)(FMOD_SOUND*, FMOD_RESULT)) {
    SE3::sm_fpWaveDataDoneLoadingCallback = callback;
}

// PROVEN: 7ed7f0e6 IDENTICAL/IDENTICAL
// OFFSET: 0x00715400/0x1403E80E0
static FMOD_RESULT F_CALLBACK DoneLoadingCallback(FMOD_SOUND* sound, FMOD_RESULT result) {
    SE3::sm_SE3ShuttingDown_CritSec.Enter();
    FMOD_RESULT r = SE3::sm_fpWaveDataDoneLoadingCallback(sound, result);
    SE3::sm_SE3ShuttingDown_CritSec.Leave();

    return r;
}

// OFFSET: 0x00715440
static int32_t CheckStreamHeader(SE3StreamInfo* info, uint8_t* frame, int32_t size) {
    uint32_t fileSize = SFileGetFileSize(info->m_file, NULL);
    uint32_t sync = (8 * frame[0]) | ((frame[1] >> 5) & 0x7FF);

    if (sync != 0x550) {
        info->m_encrypted = 0;
        return sync == 0x7FF;
    }

    if (!SFile::ReadAtPosition(info->m_file, info->m_iv, fileSize - 32, 32, NULL)) {
        return 0;
    }

    SFile::SetFilePointer(info->m_file, 0, NULL, 0);
    info->m_encrypted = 1;
    info->m_cypher.SetKey(info->m_key, info->m_iv);
    info->m_cypher.ProcessBuffer(frame + 8, size - 8);
    frame[1] |= 0xE0;
    frame[0] = 0xFF;
    return 1;
}

// OFFSET: 0x007154F0
static int32_t FindMp3Bitrate(const uint8_t* data, uint32_t size) {
    for (uint32_t i = 0; i < size - 3; i++) {
        uint32_t header = (((((data[i] << 8) | data[i + 1]) << 8) | data[i + 2]) << 8) | data[i + 3];

        if (data[i] == 0xFF && (data[i + 1] & 0xE0) == 0xE0 && (header & 0xC00) != 0xC00) {
            int32_t bitrate = s_mp3Bitrates[(header >> 12) & 0xF];

            if (bitrate > 0) {
                return bitrate;
            }
        }
    }

    return 0;
}

// OFFSET: 0x00715580/0x1403E8240
static int32_t PrepareStream(SE3StreamInfo* info) {
    uint8_t buffer[0x4000];

    if (!SFile::Read(info->m_file, buffer, sizeof(buffer), NULL)) {
        return 0;
    }

    SFile::SetFilePointer(info->m_file, 0, NULL, 0);

    if (info->m_encrypted) {
        int32_t offset = 0;

        if (buffer[0] == 'I' && buffer[1] == 'D' && buffer[2] == '3') {
            offset = ((((((buffer[6] << 7) | buffer[7]) << 7) | buffer[8]) << 7) | buffer[9]) + 10;

            if (offset < 0 || offset >= 0x4000) {
                return 0;
            }
        }

        if (!CheckStreamHeader(info, &buffer[offset], 0x4000 - offset)) {
            return 0;
        }

        info->m_headerPosition = offset;
        info->m_headerState = 1;
    }

    int32_t bitrate = FindMp3Bitrate(buffer, sizeof(buffer));
    int32_t duration = static_cast<int32_t>(8 * SFileGetFileSize(info->m_file, NULL)) / 1024 / bitrate;
    int32_t bandwidth = static_cast<int32_t>(AsyncFileReadGetStreamingBandwidth() * 1000.0f);

    if (bandwidth <= 0) {
        bandwidth = 750;
    }

    int32_t blocks;

    if (bandwidth > 2 * bitrate) {
        blocks = 2;
    } else if (bandwidth >= bitrate) {
        blocks = 3;
    } else {
        blocks = (((bitrate - bandwidth) * duration << 10) / 8 + 0x3FFF) / 0x4000 + 3;

        if (blocks <= 0) {
            info->m_nextBlock = blocks;
            return 1;
        }
    }

    int32_t status = 0;

    if (!SFile::MakeFileLocal(info->m_file, true, blocks << 14, &status, 0, blocks << 14) || status) {
        return 0;
    }

    info->m_nextBlock = blocks;
    return 1;
}

// OFFSET: 0x007162C0/0x1403E9370
static void StreamReadCallback(CAsyncObject* object) {
    SE3AsyncData* data = static_cast<SE3AsyncData*>(object->m_userArg);
    SE3StreamInfo* info = data->m_pStreamInfo;
    s_streamCritSect.Enter();
    info->m_pendingReads--;
    info->m_asyncData.DeleteNode(data);

    if (!info->m_pendingReads && info->m_closeRequested) {
        SFileCloseFile(info->m_file);
        delete info;
    }

    AsyncFileReadDestroyObject(object);
    s_streamCritSect.Leave();
}

// OFFSET: 0x00716330/0x1403E9410
static int32_t CancelStreamReads(SE3StreamInfo* info) {
    int32_t pending = 0;
    s_streamCritSect.Enter();

    for (SE3AsyncData* data = info->m_asyncData.Head(); data;) {
        SE3AsyncData* next = info->m_asyncData.Next(data);

        if (AsyncFileReadCancel(data->m_pAsyncObj, StreamReadCallback)) {
            info->m_pendingReads--;
            info->m_asyncData.DeleteNode(data);
        } else {
            info->m_closeRequested = 1;
            pending = 1;
        }

        data = next;
    }

    s_streamCritSect.Leave();
    return pending == 0;
}

// OFFSET: 0x00715C30/0x1403E8AE0
SE3StreamInfo::~SE3StreamInfo() {
}

// OFFSET: 0x007163C0/0x1403E94D0
static FMOD_RESULT F_CALLBACK SECloseCallback(void* handle, void* userdata) {
    SE3StreamInfo* info = static_cast<SE3StreamInfo*>(userdata);

    if (info) {
        if (!CancelStreamReads(info)) {
            return FMOD_OK;
        }

        delete info;
    }

    return SFileCloseFile(static_cast<SFileImpl*>(handle)) ? FMOD_OK : FMOD_ERR_INVALID_HANDLE;
}

// OFFSET: 0x00716410/0x1403E9540
static FMOD_RESULT F_CALLBACK SEReadCallback(void* handle, void* buffer, unsigned int sizebytes, unsigned int* bytesread, void* userdata) {
    SFileImpl* file = static_cast<SFileImpl*>(handle);
    SE3StreamInfo* info = static_cast<SE3StreamInfo*>(userdata);
    uint32_t fileSize = SFileGetFileSize(file, NULL);

    if (info) {
        uint32_t offset = info->m_nextBlock << 14;

        if (offset < fileSize) {
            s_streamCritSect.Enter();
            SE3AsyncData* data = info->m_asyncData.NewNode(2, 0, 0);
            data->m_pStreamInfo = info;
            data->m_pAsyncObj = NULL;
            info->m_pendingReads++;
            uint32_t size = fileSize - offset;

            if (size > 0x4000) {
                size = 0x4000;
            }

            AsyncFileReadCreateObject(file, 0, 0, &data->m_pAsyncObj, NULL, data, StreamReadCallback, StreamReadCallback, 0, offset, size);

            if (!data->m_pAsyncObj) {
                info->m_pendingReads--;
                info->m_asyncData.DeleteNode(data);
            }

            info->m_nextBlock++;
            s_streamCritSect.Leave();
        }
    }

    uint32_t position = SFile::GetFilePointer(file);
    uint32_t read = 0;

    SFile::SetReadFatal(false);
    int32_t success = SFile::Read(file, buffer, sizebytes, &read);
    SFile::SetReadFatal(true);

    *bytesread = read;

    if (!success) {
        return SErrGetLastError() == 38 ? FMOD_ERR_FILE_EOF : FMOD_ERR_INTERNAL;
    }

    if (!info || !info->m_encrypted) {
        return FMOD_OK;
    }

    uint8_t* data = static_cast<uint8_t*>(buffer);

    if (!position) {
        info->m_cypher.Reset();
        info->m_cypher.SetKey(info->m_key, info->m_iv);
    }

    uint32_t header = info->m_headerPosition;
    uint32_t end = position + read;

    switch (info->m_headerState) {
        case 1:
            if (header >= end) {
                return FMOD_OK;
            }

            data[header - position] = 0xFF;
            header++;

            if (header >= end) {
                info->m_headerState = 3;
                return FMOD_OK;
            }

            data[header - position] |= 0xE0;
            info->m_headerState = 2;
            break;

        case 3:
            data[header - position + 1] |= 0xE0;
            info->m_headerState = 2;
            break;
    }

    uint32_t start = info->m_headerPosition + 8;

    if (position > start) {
        start = position;
    }

    uint32_t limit = SFileGetFileSize(file, NULL) - 32;

    if (end < limit) {
        limit = end;
    }

    if (start < limit) {
        info->m_cypher.ProcessBuffer(data + start - position, limit - start);
    }

    return FMOD_OK;
}

// PROVEN: 9c458ad0 IDENTICAL/IDENTICAL
// OFFSET: 0x00715730/0x1403E8480
static FMOD_RESULT F_CALLBACK SESeekCallback(void* handle, unsigned int pos, void* userdata) {
    SFile::SetFilePointer(static_cast<SFileImpl*>(handle), pos, NULL, 0);
    return FMOD_OK;
}

// OFFSET: 0x00715810
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

// OFFSET: 0x00715890/0x1403E85E0
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
            node->m_pStreamInfo = NULL;
            break;
        }
    }

    if (!node) {
        *result = FMOD_ERR_INVALID_HANDLE;
    }

    SE3::sm_SoundCache_Loading_CritSec.Leave();

    return found != NULL;
}

// OFFSET: 0x00715920/0x1403E86C0
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
            node->m_file = reinterpret_cast<SFileImpl*>(-1);
            node->m_pStreamInfo = NULL;
            break;
        }
    }

    if (!node) {
        *result = FMOD_ERR_INVALID_HANDLE;
    }

    SE3::sm_StreamedSound_Loading_CritSec.Leave();

    return found != NULL;
}

// OFFSET: 0x007159C0
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

// OFFSET: 0x00715A60
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

// OFFSET: 0x00715B00/0x1403E8920
static FMOD_RESULT F_CALLBACK SEOpenCallback(const char* name, int unicode, unsigned int* filesize, void** handle, void** userdata) {
    if (*userdata) {
        PrepareStream(static_cast<SE3StreamInfo*>(*userdata));
    }

    FMOD_RESULT result = FMOD_OK;
    uint32_t key = atoi(name);

    if (!OpenLoadingCacheFile(key, &result, filesize, handle, userdata)) {
        OpenLoadingStreamFile(key, &result, filesize, handle, userdata);
    }

    return result;
}

// OFFSET: 0x00715B60
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

// OFFSET: 0x00715BD0/0x1403E8A20
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

// OFFSET: 0x00715C80/0x1403E8B50
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

// OFFSET: 0x00715D00/0x1403E8BE0
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

// OFFSET: 0x00716650/0x1403E9820
int32_t SE3::CreateWaveData(uint32_t key, FMOD::SoundGroup* soundGroup, uint32_t mode, int32_t stream, uint32_t size, const char* filename, SFileImpl* file, uint32_t* waveDataID, SE3StreamInfo* streamInfo) {
    FMOD_CREATESOUNDEXINFO exinfo;
    memset(&exinfo, 0, sizeof(exinfo));
    exinfo.cbsize = sizeof(exinfo);
    exinfo.nonblockcallback = DoneLoadingCallback;
    exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_WAV;
    exinfo.useropen = SEOpenCallback;
    exinfo.userclose = SECloseCallback;
    exinfo.userread = SEReadCallback;
    exinfo.userseek = SESeekCallback;
    exinfo.userdata = streamInfo;
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
    node->m_pStreamInfo = streamInfo;
    node->m_nRefCount = 1;
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

// OFFSET: 0x007168E0/0x1403E9C40
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

// PROVEN: ab383207 IDENTICAL/IDENTICAL
// OFFSET: 0x00716C30/0x1403EA110
FMOD_RESULT SE3::SetDspParam(void* dsp, int32_t index, float value) {
    return static_cast<FMOD::DSP*>(dsp)->setParameter(index, value);
}

// PROVEN: 8ea572e5 IDENTICAL/IDENTICAL
// OFFSET: 0x00716C50/0x1403EA120
FMOD_RESULT SE3::BypassDsp(void* dsp, bool bypass) {
    return static_cast<FMOD::DSP*>(dsp)->setBypass(bypass);
}

// PROVEN: ec08a0ca IDENTICAL/IDENTICAL
// OFFSET: 0x00716C70/0x1403EA130
FMOD_RESULT SE3::EnableDSP(void* dsp, bool active) {
    return static_cast<FMOD::DSP*>(dsp)->setActive(active);
}

// PROVEN: eada81e0 IDENTICAL/IDENTICAL
// OFFSET: 0x00716C90/0x1403EA140
FMOD_RESULT SE3::DspDestroy(void* dsp) {
    static_cast<FMOD::DSP*>(dsp)->remove();
    static_cast<FMOD::DSP*>(dsp)->release();
    return FMOD_OK;
}

// PROVEN: eec84d60 IDENTICAL/IDENTICAL
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

// PROVEN: e0d5fdfc IDENTICAL/IDENTICAL
// OFFSET: 0x00716D20/0x1403EA1E0
void* SE3::CreateChannelGroup(const char* name) {
    FMOD::ChannelGroup* channelGroup = NULL;
    SE3::sm_pGameSystem->createChannelGroup(name, &channelGroup);
    return channelGroup;
}

// PROVEN: 238dcca3 IDENTICAL/IDENTICAL
// OFFSET: 0x00716D50/0x1403EA210
FMOD_RESULT SE3::DestroyChannelGroup(void* channelGroup) {
    return static_cast<FMOD::ChannelGroup*>(channelGroup)->release();
}

// PROVEN: 40960923 IDENTICAL/IDENTICAL
// OFFSET: 0x00716D60/0x1403EA220
FMOD_RESULT SE3::AddChannelToChannelGroup(void* channel, void* channelGroup) {
    return static_cast<FMOD::Channel*>(channel)->setChannelGroup(static_cast<FMOD::ChannelGroup*>(channelGroup));
}

// PROVEN: 2823cd42 IDENTICAL/UNPROVEN
// OFFSET: 0x00716D80/0x1403EA230
FMOD_RESULT SE3::SetFMODChannelGroupVolume(void* channelGroup, float volume) {
    return static_cast<FMOD::ChannelGroup*>(channelGroup)->setVolume(volume);
}
