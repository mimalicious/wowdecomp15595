#include "Engine/Source/Frame/Movie.h"
#include <cstdio>
#include <cstring>
#include <malloc.h>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/DivxDecoderLoader.h"
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/Ui/GameUI.h"

#define MOVIE_CODEC_PMVX 0x58564D50
#define MOVIE_CHUNK_KEYFRAME 0x00000010
#define MOVIE_CHUNK_MISSING 0x10000000
#define MOVIE_CHUNK_SKIPPED 0x20000000
#define MOVIE_CHUNK_PREFETCHING 0x40000000
#define MOVIE_CHUNK_LOCAL 0x80000000

// STATIC: 0x00CA331C
MoviePreloadMap MovieStreaming::s_preloads;

float MovieStreaming::s_progressBase;

float MovieStreaming::s_progressScale;

// STATIC: 0x00DD39C0
static uint32_t s_bandwidthTime;

// STATIC: 0x00DD39C4
static float s_bandwidth;

// INLINED
MoviePreload::MoviePreload() {
    this->m_object = NULL;
    this->m_local = 0;
    this->m_total = 0;
}

// INLINED
MoviePreloadMap::MoviePreloadMap() {
    this->m_bucketCount = 8;
    this->m_count = 0;
    this->m_buckets = static_cast<Node**>(Blizzard::Memory::s_allocHook(sizeof(Node*) * 8));

    for (uint32_t i = 0; i < 8; i++) {
        this->m_buckets[i] = NULL;
    }

    this->m_maxLoadFactor = 1.0f;
}

// INLINED
MoviePreloadMap::~MoviePreloadMap() {
    for (uint32_t i = 0; i < this->m_bucketCount; i++) {
        for (Node* node = this->m_buckets[i]; node; ) {
            Node* next = node->m_next;
            Blizzard::Memory::s_freeHook(node, sizeof(Node));
            node = next;
        }
    }

    Blizzard::Memory::s_freeHook(this->m_buckets, sizeof(Node*) * this->m_bucketCount);
}

// OFFSET: 0x005132F0/0x14015D6C0
MoviePreload* MoviePreloadMap::operator[](const int32_t& key) {
    Node** bucket = &this->m_buckets[key & (this->m_bucketCount - 1)];

    for (Node* node = *bucket; node; node = node->m_next) {
        if (node->m_value.m_key == key) {
            return node->m_value.m_preloads;
        }
    }

    Node* node = static_cast<Node*>(Blizzard::Memory::s_allocHook(sizeof(Node)));

    if (node) {
        Node::Value value;
        value.m_key = key;
        node->m_value = value;
        node->m_next = *bucket;
    }

    *bucket = node;

    if (static_cast<double>(++this->m_count) / static_cast<double>(this->m_bucketCount) > this->m_maxLoadFactor) {
        this->Rehash(2 * this->m_bucketCount);
    }

    return node->m_value.m_preloads;
}

// OFFSET: 0x00829160
void MoviePreloadMap::Rehash(uint32_t bucketCount) {
    uint32_t count = 8;

    while (count < bucketCount) {
        count *= 2;
    }

    Node** buckets = static_cast<Node**>(Blizzard::Memory::s_allocHook(sizeof(Node*) * count));

    if (count) {
        memset(buckets, 0, sizeof(Node*) * count);
    }

    for (uint32_t i = 0; i < this->m_bucketCount; i++) {
        for (Node* node = this->m_buckets[i]; node; ) {
            Node* next = node->m_next;
            uint32_t index = static_cast<uint32_t>(node->m_value.m_key) & (count - 1);
            node->m_next = buckets[index];
            buckets[index] = node;
            node = next;
        }
    }

    Blizzard::Memory::s_freeHook(this->m_buckets, sizeof(Node*) * this->m_bucketCount);
    this->m_bucketCount = count;
    this->m_buckets = buckets;
}

// OFFSET: 0x00511920
static void PreloadFailedCallback(CAsyncObject* object) {
    MoviePreload* preload = static_cast<MoviePreload*>(object->m_userArg);
    AsyncFileReadDestroyObject(object);
    preload->m_object = NULL;
}

// OFFSET: 0x00511950
static void PreloadCallback(CAsyncObject* object) {
    MoviePreload* preload = static_cast<MoviePreload*>(object->m_userArg);
    AsyncFileReadDestroyObject(object);
    preload->m_object = NULL;
    preload = static_cast<MoviePreload*>(object->m_userArg);
    preload->m_local = preload->m_total;
}

// OFFSET: 0x00511990
static void StartPreload(MoviePreload* preload, const char* path, uint8_t priority) {
    AsyncFileReadCreateObject(path, priority, 1, 1, &preload->m_object, NULL, preload, PreloadCallback, PreloadFailedCallback, 0, 0, 0);

    if (preload->m_object) {
        preload->m_total = 0;
        preload->m_local = 0;
        SFile::FileGetIsLocalAmount(preload->m_object->m_file, preload->m_local, preload->m_total);
    }
}

// OFFSET: 0x005135F0/0x14015DC30
void MovieStreaming::CancelPreloadingMovie(int32_t movieID) {
    MoviePreload* preload = MovieStreaming::s_preloads[movieID];

    for (uint32_t i = 0; i < 3; i++) {
        if (preload[i].m_object) {
            AsyncFileReadCancel(preload[i].m_object, NULL);
            preload[i].m_object = NULL;
        }
    }
}

// OFFSET: 0x005119F0
static void WaitProgressCallback(float progress, int32_t param) {
    LoadingScreenAsyncCallback(MovieStreaming::s_progressScale * progress + MovieStreaming::s_progressBase, NULL);
}

// OFFSET: 0x00513910
void MovieStreaming::WaitForDownloadingMovies(int32_t movieID) {
    MoviePreload* preload = MovieStreaming::s_preloads[movieID];
    uint32_t sizes[3];
    uint32_t totalSize = 0;
    int32_t loading = 0;

    for (int32_t i = 0; i < 3; i++) {
        AsyncFileReadEnterQueueLock();

        if (preload[i].m_object && preload[i].m_object->m_file) {
            sizes[i] = SFileGetFileSize(preload[i].m_object->m_file, NULL);
        } else {
            sizes[i] = 0;
        }

        AsyncFileReadLeaveQueueLock();
        totalSize += sizes[i];
    }

    MovieStreaming::s_progressBase = 0.0f;

    if (totalSize <= 0) {
        return;
    }

    for (int32_t i = 0; i < 3; i++) {
        if (preload[i].m_object) {
            MovieStreaming::s_progressScale = static_cast<float>(static_cast<double>(sizes[i]) / static_cast<double>(totalSize));

            if (!loading) {
                LoadingScreenEnable(CGGameUI::m_iCurrentMapID, true);
                AsyncFileReadIngameProgressCallback(NULL, WaitProgressCallback);
                loading = 1;
            }

            AsyncFileReadWait(preload[i].m_object);
            MovieStreaming::s_progressBase = MovieStreaming::s_progressScale + MovieStreaming::s_progressBase;
        }
    }

    if (loading) {
        LoadingScreenAsyncCallback(1.0f, NULL);
        LoadingScreenDisable();
        AsyncFileReadIngameProgressCallback(NULL, NULL);
    }
}

// PROVEN: a5b90972 IDENTICAL/DIFFERENT
// OFFSET: 0x00513AF0/0x14015E2A0
void CMovieStreamer::Reset() {
    if (this->m_aviFile) {
        this->Cleanup();
    }

    memset(this->m_key, 0, sizeof(this->m_key));
    this->m_videoIV = NULL;
    this->m_audioIV = NULL;
    this->m_videoCodec = 0;
    this->m_moviOffset = 0;
    this->m_index = NULL;
    this->m_indexCount = 0;
    this->m_videoPrefetchIndex = 0;
    this->m_audioPrefetchIndex = 0;
    this->m_videoReadIndex = 0;
    this->m_aviPath[0] = '\0';
    this->m_movieName[0] = '\0';
    this->m_videoIndex = -1;
    this->m_audioIndex = -1;
    memset(&this->m_info, 0, sizeof(this->m_info));
    this->m_aviFile = NULL;
    this->m_frameBuffers = NULL;
    this->m_videoDataSize = 0;
    this->m_frameBufferCount = 0;
    this->m_currentBuffer = 0;
    this->m_videoFrameCount = 0;
    this->m_maxFrameSize = 0;
    this->m_interleavedAudio = 0;
    this->m_audioBuffer = NULL;
    this->m_audioReadOffset = 0;
    this->m_audioBufferSize = 0;
    this->m_audioChunkCount = 0;
}

// PROVEN: 559c3b92 IDENTICAL/DIFFERENT
// OFFSET: 0x00512BA0/0x14015CD60
void CMovieStreamer::Cleanup() {
    if (this->m_index) {
        SMemFree(this->m_index, __FILE__, __LINE__, 0);
        this->m_index = NULL;
    }

    if (this->m_audioIV) {
        SMemFree(this->m_audioIV, __FILE__, __LINE__, 0);
        this->m_audioIV = NULL;
    }

    if (this->m_videoIV) {
        SMemFree(this->m_videoIV, __FILE__, __LINE__, 0);
        this->m_videoIV = NULL;
    }

    if (this->m_frameBuffers) {
        DivxDecoderFree(this->m_frameBuffers);
        this->m_frameBuffers = NULL;
    }

    if (this->m_audioBuffer) {
        SMemFree(this->m_audioBuffer, __FILE__, __LINE__, 0);
        this->m_audioBuffer = NULL;
    }

    this->CleanupAsyncObjects();

    if (this->m_aviFile) {
        SFileCloseFile(this->m_aviFile);
        this->m_aviFile = NULL;
    }
}

// OFFSET: 0x00511A30
void CMovieStreamer::AsyncObjectCancelCallback(CAsyncObject* object) {
    object->m_file = NULL;
    AsyncFileReadDestroyObject(object);
}

// OFFSET: 0x005129B0/0x14015CB00
void CMovieStreamer::FramePreloadCallback(CAsyncObject* object) {
    TSGrowableArray<CAsyncObject*>* objects = static_cast<TSGrowableArray<CAsyncObject*>*>(object->m_userArg);
    (*objects)[object->m_userFlags] = NULL;
    AsyncFileReadDestroyObject(object);
}

// OFFSET: 0x00512B20/0x14015CCA0
void CMovieStreamer::CleanupAsyncObjects() {
    for (uint32_t i = 0; i < this->m_asyncObjects.Count(); i++) {
        CAsyncObject* object = this->m_asyncObjects[i];

        if (object && !object->m_cancelled && !AsyncFileReadCancel(object, AsyncObjectCancelCallback)) {
            while (this->m_asyncObjects[i]->m_isCurrent) {
                OsSleep(1);
            }
        }
    }

    this->m_asyncObjects.SetCount(0);
}

// OFFSET: 0x00512C70
uint16_t CMovieStreamer::GetFreeAsyncObjectSlot() {
    uint16_t slot = 0;

    for (; slot < this->m_asyncObjects.Count(); slot++) {
        if (!this->m_asyncObjects[slot]) {
            return slot;
        }
    }

    this->m_asyncObjects.New();

    return static_cast<uint16_t>(this->m_asyncObjects.Count() - 1);
}

// PROVEN: 5dc6ffd5 IDENTICAL/IDENTICAL
// OFFSET: 0x00511A50/0x14015B8D0
void CMovieStreamer::GetVideoInfo(MovieInfo& info) {
    info = this->m_info;
}

// OFFSET: 0x00511A70
void CMovieStreamer::InitStreamIndices() {
    for (int32_t i = 0; i < this->m_indexCount - 1; i++) {
        if (this->m_videoIndex < 0) {
            if (this->m_index[i].m_chunkID[2] == 'd') {
                this->m_videoIndex = i;
                continue;
            }
        } else if (this->m_audioIndex >= 0) {
            return;
        }

        if (this->m_audioIndex < 0 && this->m_index[i].m_chunkID[2] == 'w') {
            this->m_audioIndex = i;
        }
    }
}

// OFFSET: 0x00511AE0
int32_t CMovieStreamer::NextVideoIndex() {
    int32_t last = this->m_indexCount - 1;

    if (this->m_videoIndex >= last) {
        return this->m_videoIndex - 1;
    }

    int32_t index = this->m_videoIndex + 1;

    while (this->m_index[index].m_chunkID[2] != 'd' && index < last) {
        index++;
    }

    return index;
}

// PROVEN: ef43e50f IDENTICAL/NONE
// OFFSET: 0x00511B20
uint8_t* CMovieStreamer::CurrentVideoFrameBuffer() {
    return this->m_frameBuffers + this->m_currentBuffer * (this->m_maxFrameSize + 4);
}

// PROVEN: d2fc8143 IDENTICAL/IDENTICAL
// OFFSET: 0x00511B40/0x14015B9B0
uint32_t CMovieStreamer::CurrentVideoFrameSize() {
    return this->m_index[this->m_videoIndex].m_size;
}

// OFFSET: 0x00511B60
int32_t CMovieStreamer::AdvanceVideoFrame() {
    int32_t count = this->m_indexCount;
    int32_t index = this->m_videoIndex + 1;

    while (index < count && this->m_index[index].m_chunkID[2] != 'd') {
        index++;
    }

    if (index >= count) {
        this->m_videoIndex = -1;
        return this->m_videoIndex;
    }

    this->m_currentBuffer = (this->m_currentBuffer + 1) % this->m_frameBufferCount;
    this->m_videoIndex = index;

    int32_t next = index + 1;
    int32_t last = count - 1;

    while (next < this->m_indexCount - 1) {
        if (!(this->m_index[next].m_flags & (MOVIE_CHUNK_MISSING | MOVIE_CHUNK_SKIPPED)) && this->m_index[next].m_chunkID[2] == 'd') {
            break;
        }

        next++;
    }

    int32_t prefetch = this->m_videoPrefetchIndex > next ? this->m_videoPrefetchIndex : next;
    this->m_videoPrefetchIndex = prefetch < last ? prefetch : last;
    return this->m_videoIndex;
}

// OFFSET: 0x00511C40
int32_t CMovieStreamer::AdvanceAudioFrame() {
    int32_t count = this->m_indexCount;
    int32_t index = this->m_audioIndex + 1;

    while (index < count && this->m_index[index].m_chunkID[2] != 'w') {
        index++;
    }

    if (index >= count) {
        this->m_audioIndex = -1;
        return this->m_audioIndex;
    }

    this->m_audioReadOffset += this->m_index[this->m_audioIndex].m_size;
    this->m_audioIndex = index;

    int32_t next = index + 1;

    while (next < count && this->m_index[next].m_chunkID[2] != 'w') {
        next++;
    }

    if (next >= count) {
        return this->m_audioIndex;
    }

    int32_t prefetch = this->m_audioPrefetchIndex > next ? this->m_audioPrefetchIndex : next;
    this->m_audioPrefetchIndex = prefetch < count - 1 ? prefetch : count - 1;
    return this->m_audioIndex;
}

// PROVEN: a71c07a1 IDENTICAL/NONE
// OFFSET: 0x00511D10
void CMovieStreamer::ReadVideoFrame(int32_t index, uint8_t* buffer) {
    SFile::ReadAtPosition(this->m_aviFile, buffer + 4, this->m_index[index].m_offset + this->m_moviOffset + 4, this->m_index[index].m_size, NULL);

    if (this->m_videoCodec == MOVIE_CODEC_PMVX) {
        this->m_cypher.ProcessBuffer(buffer, this->m_index[index].m_size + 4);
    }
}

// PROVEN: 87926015 IDENTICAL/NONE
// OFFSET: 0x00511D80
void CMovieStreamer::ReadAudioFrame(int32_t index, uint8_t* buffer) {
    SFile::ReadAtPosition(this->m_aviFile, buffer, this->m_index[index].m_offset + this->m_moviOffset + 4, this->m_index[index].m_size, NULL);
    this->m_index[index].m_flags &= ~MOVIE_CHUNK_PREFETCHING;
    this->m_index[index].m_flags |= MOVIE_CHUNK_LOCAL;

    if (this->m_videoCodec == MOVIE_CODEC_PMVX) {
        this->m_audioCypher.ProcessBuffer(buffer, this->m_index[index].m_size);
    }
}

// OFFSET: 0x00511E10
int32_t CMovieStreamer::IsChunkLocal(int32_t index) {
    AVIINDEXENTRY& entry = this->m_index[index];

    if (entry.m_flags & MOVIE_CHUNK_LOCAL) {
        return 1;
    }

    if (SFile::FileRangeIsLocal(this->m_aviFile, entry.m_offset + this->m_moviOffset + 4, entry.m_size) == 1) {
        this->m_index[index].m_flags &= ~MOVIE_CHUNK_PREFETCHING;
        this->m_index[index].m_flags |= MOVIE_CHUNK_LOCAL;
        return 1;
    }

    return 0;
}

// OFFSET: 0x00512D30
void CMovieStreamer::PrefetchFrame(int32_t index, bool wait, uint8_t priority) {
    AVIINDEXENTRY& entry = this->m_index[index];

    if (entry.m_flags & (MOVIE_CHUNK_LOCAL | MOVIE_CHUNK_PREFETCHING)) {
        return;
    }

    CAsyncObject* object;

    if (wait) {
        AsyncFileReadCreateObject(this->m_aviFile, priority, 1, &object, NULL, NULL, NULL, NULL, 0, entry.m_offset + this->m_moviOffset + 4, entry.m_size);

        if (object) {
            AsyncFileReadWait(object);
        }
    } else {
        uint16_t slot = this->GetFreeAsyncObjectSlot();
        AsyncFileReadCreateObject(this->m_aviFile, priority, 0, &object, NULL, &this->m_asyncObjects, FramePreloadCallback, FramePreloadCallback, slot, this->m_index[index].m_offset + this->m_moviOffset + 4, this->m_index[index].m_size);

        if (object) {
            this->m_asyncObjects[slot] = object;
        }
    }

    this->m_index[index].m_flags |= MOVIE_CHUNK_PREFETCHING;
}

// OFFSET: 0x00512E30
void CMovieStreamer::PrefetchVideoFrames(int32_t count, uint8_t wait) {
    int32_t end = this->m_videoPrefetchIndex;
    int32_t pending = 0;

    for (int32_t i = this->m_videoReadIndex + 1; i < end; i++) {
        if (!(this->m_index[i].m_flags & (MOVIE_CHUNK_MISSING | MOVIE_CHUNK_SKIPPED)) && this->m_index[i].m_chunkID[2] == 'd') {
            pending++;
        }
    }

    if (pending >= count) {
        return;
    }

    int32_t remaining = count - pending;
    int32_t index = end;

    for (; index < this->m_indexCount; index++) {
        if (!(this->m_index[index].m_flags & (MOVIE_CHUNK_MISSING | MOVIE_CHUNK_SKIPPED))) {
            if (remaining <= 0) {
                break;
            }

            if (this->m_index[index].m_chunkID[2] == 'd') {
                this->PrefetchFrame(index, wait != 0, 2);
                remaining--;
            }
        }
    }

    this->m_videoPrefetchIndex = index < this->m_indexCount - 1 ? index : this->m_indexCount - 1;
}

// OFFSET: 0x00512EF0
void CMovieStreamer::PrefetchAudioFrames(int32_t count, uint8_t wait) {
    int32_t index = this->m_audioPrefetchIndex;

    if (static_cast<float>(index) > this->m_info.m_fps * 6.0f + static_cast<float>(this->m_videoPrefetchIndex)) {
        return;
    }

    for (; index < this->m_indexCount; index++) {
        if (index >= 2 * count + this->m_audioIndex) {
            break;
        }

        if (this->m_index[index].m_chunkID[2] == 'w') {
            this->PrefetchFrame(index, wait != 0, 1);
        }
    }

    if (index >= this->m_indexCount) {
        return;
    }

    while (index < this->m_indexCount && this->m_index[index].m_chunkID[2] != 'w') {
        index++;
    }

    if (index < this->m_indexCount) {
        this->m_audioPrefetchIndex = index;
    }
}

// OFFSET: 0x00511E90/0x14015BD80
int32_t CMovieStreamer::ParseAVIHeader() {
    if (!this->m_aviFile) {
        return 0;
    }

    char* header = NULL;
    uint32_t headerSize = 0;
    char chunk[12];

    if (!SFile::Read(this->m_aviFile, chunk, 12, NULL) || SStrCmpI(&chunk[0], "RIFF", 4) || SStrCmpI(&chunk[8], "AVI ", 4)) {
        return 0;
    }

    while (SFile::Read(this->m_aviFile, chunk, 8, NULL)) {
        uint32_t size = *reinterpret_cast<uint32_t*>(&chunk[4]);

        if (!SStrCmpI(&chunk[0], "LIST", 4)) {
            if (!SFile::Read(this->m_aviFile, chunk, 4, NULL)) {
                break;
            }

            size -= 4;

            if (!SStrCmpI(&chunk[0], "hdrl", 4)) {
                headerSize = size;
                header = static_cast<char*>(alloca(size));

                if (!SFile::Read(this->m_aviFile, header, size, NULL)) {
                    break;
                }

                continue;
            }

            if (!SStrCmpI(&chunk[0], "movi", 4)) {
                this->m_moviOffset = SFile::SetFilePointer(this->m_aviFile, 0, NULL, 1);
            }

            SFile::SetFilePointer(this->m_aviFile, size, NULL, 1);
        } else if (!SStrCmpI(&chunk[0], "idx1", 4)) {
            this->m_indexCount = size >> 4;
            this->m_index = static_cast<AVIINDEXENTRY*>(SMemAlloc(size, __FILE__, __LINE__, 0));

            if (!SFile::Read(this->m_aviFile, this->m_index, size, NULL)) {
                break;
            }
        } else if (!SStrCmpI(&chunk[0], "pviv", 4)) {
            this->m_videoIV = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));

            if (!SFile::Read(this->m_aviFile, this->m_videoIV, size, NULL)) {
                break;
            }
        } else if (!SStrCmpI(&chunk[0], "paiv", 4)) {
            this->m_audioIV = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));

            if (!SFile::Read(this->m_aviFile, this->m_audioIV, size, NULL)) {
                break;
            }
        } else {
            SFile::SetFilePointer(this->m_aviFile, size, NULL, 1);
        }
    }

    int32_t streamIndex = 0;
    int32_t videoStream = -1;
    int32_t audioStream = -1;

    for (uint32_t offset = 0; offset < headerSize;) {
        if (!SStrCmpI(&header[offset], "LIST", 4)) {
            offset += 12;
            continue;
        }

        uint32_t chunkSize = *reinterpret_cast<uint32_t*>(&header[offset + 4]);

        if (!SStrCmpI(&header[offset], "strh", 4)) {
            offset += 8;

            if (!SStrCmpI(&header[offset], "vids", 4)) {
                videoStream = streamIndex;
                this->m_info.m_fps = static_cast<float>(static_cast<double>(*reinterpret_cast<uint32_t*>(&header[offset + 24])) / static_cast<double>(*reinterpret_cast<uint32_t*>(&header[offset + 20])));
                this->m_info.m_frameCount = *reinterpret_cast<int32_t*>(&header[offset + 32]);
            }

            if (!SStrCmpI(&header[offset], "auds", 4)) {
                audioStream = streamIndex;
            }

            streamIndex++;
        } else {
            int32_t isFormat = !SStrCmpI(&header[offset], "strf", 4);
            offset += 8;

            if (isFormat && videoStream >= 0 && audioStream < 0) {
                this->m_info.m_width = *reinterpret_cast<int32_t*>(&header[offset + 4]);
                this->m_info.m_height = *reinterpret_cast<int32_t*>(&header[offset + 8]);
                this->m_videoCodec = *reinterpret_cast<uint32_t*>(&header[offset + 16]);
            }
        }

        offset += chunkSize;
    }

    return 1;
}

// OFFSET: 0x00512210
int32_t CMovieStreamer::ParseAVIIndex() {
    if (!this->m_index || !this->m_indexCount) {
        return 0;
    }

    this->m_videoFrameCount = 0;
    this->m_videoDataSize = 0;
    this->m_maxFrameSize = 0;
    this->m_audioChunkCount = 0;
    this->m_audioBufferSize = 0;

    for (int32_t i = 0; i < this->m_indexCount; i++) {
        uint8_t type = this->m_index[i].m_chunkID[2];
        uint32_t size = this->m_index[i].m_size;

        if (type == 'd') {
            this->m_videoFrameCount++;
            this->m_videoDataSize += size;

            if (size > this->m_maxFrameSize) {
                this->m_maxFrameSize = size;
            }
        } else if (type == 'w') {
            this->m_audioChunkCount++;
            this->m_audioBufferSize += size;
        }
    }

    int32_t videoSize = this->m_videoDataSize;
    int32_t audioSize = this->m_audioBufferSize;
    float duration = static_cast<float>(static_cast<double>(this->m_videoFrameCount) / this->m_info.m_fps);
    this->m_info.m_duration = duration;
    this->m_info.m_videoRate = static_cast<float>(videoSize / 1000) * (1.0f / duration);
    this->m_info.m_audioRate = static_cast<float>(audioSize / 1000) * (1.0f / duration);
    this->m_info.m_totalRate = static_cast<float>((audioSize + videoSize) / 1000) * (1.0f / duration);
    return 1;
}

// PROVEN: 8b57b994 IDENTICAL/NONE
// OFFSET: 0x00512360/0x14015C2B0
uint8_t* CMovieStreamer::GetAudioBuffer() {
    return this->m_audioBuffer;
}

// PROVEN: 9fd12659 IDENTICAL/NONE
// OFFSET: 0x00512370/0x14015C2C0
uint32_t CMovieStreamer::GetAudioBufferSize() {
    return this->m_audioBufferSize;
}

// PROVEN: 5da70f59 IDENTICAL/NONE
// OFFSET: 0x00512380
int32_t CMovieStreamer::ParseAVIFile() {
    SFile::EnableHash(false);

    if (!SFile::OpenEx(NULL, this->m_aviPath, 1, &this->m_aviFile)) {
        SFile::EnableHash(true);
        return 0;
    }

    SFile::EnableHash(true);

    if (!this->ParseAVIHeader()) {
        return 0;
    }

    return this->ParseAVIIndex() != 0;
}

// OFFSET: 0x005123D0
bool CMovieStreamer::MeasureBandwidth(float& bandwidth) {
    float streaming = AsyncFileReadGetStreamingBandwidth();

    if (streaming > 0.0f) {
        bandwidth = streaming * 128.0f;
        s_bandwidth = bandwidth;
        return true;
    }

    if (s_bandwidth > 0.0f) {
        bandwidth = s_bandwidth;
        return true;
    }

    int32_t first = 0;

    for (; this->IsChunkLocal(first); first++) {
        if (first >= this->m_indexCount - 1) {
            break;
        }
    }

    int32_t last = this->m_indexCount - 1;

    if (first == last) {
        bandwidth = 0.0f;
        s_bandwidth = 0.0f;
        s_bandwidthTime = OsGetAsyncTimeMs();
        return false;
    }

    int32_t span = 3 * static_cast<int32_t>(this->m_info.m_fps) * ((this->m_interleavedAudio != 0) + 1);

    if (first + span < last) {
        last = first + span;
    }

    uint32_t start = this->m_moviOffset + this->m_index[first].m_offset;
    uint32_t end = this->m_moviOffset + this->m_index[last].m_offset + this->m_index[last].m_size;

    int64_t localBefore = 0;
    int64_t localAfter = 0;
    int64_t total = 0;
    SFile::FileGetIsLocalAmount(this->m_aviFile, localBefore, total);

    uint32_t startTime = OsGetAsyncTimeMs();
    CAsyncObject* object;
    AsyncFileReadCreateObject(this->m_aviFile, 2, 1, &object, NULL, NULL, AsyncFileReadDestroyObject, AsyncFileReadDestroyObject, 0, start, end - start);

    if (object) {
        AsyncFileReadWait(object);
    }

    uint32_t endTime = OsGetAsyncTimeMs();
    SFile::FileGetIsLocalAmount(this->m_aviFile, localAfter, total);

    if (localAfter == localBefore || endTime == startTime) {
        bandwidth = 0.0f;
        return true;
    }

    bandwidth = static_cast<float>(static_cast<double>(localAfter - localBefore) / static_cast<double>(endTime - startTime));
    s_bandwidth = bandwidth;
    s_bandwidthTime = OsGetAsyncTimeMs();
    return true;
}

// OFFSET: 0x00512670/0x14015C670
static int32_t FindMovieVariationIndex(int32_t movieID) {
    int32_t high = g_movieVariationDB.m_numRecords;
    int32_t low = 0;

    if (high <= 0) {
        return -1;
    }

    do {
        int32_t middle = (high + low) / 2;

        if (g_movieVariationDB.GetRecordByIndex(middle)->m_movieID >= movieID) {
            high = middle;
        } else {
            low = middle + 1;
        }
    } while (low < high);

    if (low < g_movieVariationDB.m_numRecords && g_movieVariationDB.GetRecordByIndex(low)->m_movieID == movieID) {
        return low;
    }

    return -1;
}

// OFFSET: 0x005126D0/0x14015C6F0
int32_t MovieStreaming::FindBestMovieResolutionFileDataID(int32_t movieID, bool localOnly) {
    if (!g_movieDB.GetRecord(movieID)) {
        return -1;
    }

    int32_t index = FindMovieVariationIndex(movieID);

    if (index == -1) {
        return -1;
    }

    int32_t best = -1;
    int32_t bestResolution = 0;
    const CRect& window = g_theGxDevicePtr->DeviceDefWindow();
    int32_t width = static_cast<int32_t>(window.maxX - window.minX);

    for (; index < g_movieVariationDB.m_numRecords; index++) {
        MovieVariationRec* variation = g_movieVariationDB.GetRecordByIndex(index);

        if (variation->m_movieID != movieID) {
            return best;
        }

        MovieFileDataRec* fileData = g_movieFileDataDB.GetRecord(variation->m_fileDataID);
        FileDataRec* file = g_fileDataDB.GetRecord(variation->m_fileDataID);

        char path[260];
        SStrCopy(path, file->m_filepath, sizeof(path));
        SStrPack(path, file->m_filename, sizeof(path));

        if (localOnly) {
            SFileImpl* handle;
            SFile::EnableHash(false);
            int32_t opened = SFile::OpenEx(NULL, path, 1, &handle);
            SFile::EnableHash(true);

            if (!opened) {
                continue;
            }

            int32_t local = SFile::FileIsLocal(handle, 0, 0);
            SFileCloseFile(handle);

            if (local != 1) {
                continue;
            }
        }

        int32_t fileResolution = fileData->m_resolution;

        if (fileResolution == width) {
            return variation->m_fileDataID;
        }

        if (best < 0) {
            best = variation->m_fileDataID;
            bestResolution = fileResolution;
        } else if (bestResolution >= width) {
            if (fileResolution < bestResolution) {
                best = variation->m_fileDataID;
                bestResolution = fileResolution;
            }
        } else if (fileResolution > bestResolution && fileResolution < width) {
            best = variation->m_fileDataID;
            bestResolution = fileResolution;
        }
    }

    return best;
}

// PROVEN: 25875b5b IDENTICAL/DIFFERENT
// OFFSET: 0x00513410/0x14015D850
void MovieStreaming::PreloadMovie(int32_t movieID) {
    FileDataRec* file = g_fileDataDB.GetRecord(MovieStreaming::FindBestMovieResolutionFileDataID(movieID, false));

    if (!file) {
        return;
    }

    char path[260];
    SStrCopy(path, file->m_filepath, sizeof(path));
    SStrPack(path, file->m_filename, sizeof(path));

    if (!MovieStreaming::s_preloads[movieID][0].m_object) {
        StartPreload(&MovieStreaming::s_preloads[movieID][0], path, 5);
    }

    MovieRec* movie = g_movieDB.GetRecord(movieID);
    SStrCopy(path, movie->m_filename, sizeof(path));
    size_t length = strlen(path);

    if (!MovieStreaming::s_preloads[movieID][1].m_object) {
        strcpy(&path[length], ".mp3");
        StartPreload(&MovieStreaming::s_preloads[movieID][1], path, 5);
    }

    if (!MovieStreaming::s_preloads[movieID][2].m_object) {
        strcpy(&path[length], ".sbt");
        StartPreload(&MovieStreaming::s_preloads[movieID][2], path, 5);
    }
}

// OFFSET: 0x00513640/0x14015DCA0
static void GetMovieLocalAmount(int64_t& local, int64_t& total, int32_t movieID) {
    FileDataRec* file = g_fileDataDB.GetRecord(MovieStreaming::FindBestMovieResolutionFileDataID(movieID, false));

    if (!file) {
        return;
    }

    total = 0;
    local = 0;

    char path[260];
    SStrCopy(path, file->m_filepath, sizeof(path));
    SStrPack(path, file->m_filename, sizeof(path));

    MoviePreload* preload = MovieStreaming::s_preloads[movieID];
    SFileImpl* handle;
    SFile::EnableHash(false);
    int32_t opened = SFile::OpenEx(NULL, path, 1, &handle);
    SFile::EnableHash(true);

    if (opened) {
        SFile::FileGetIsLocalAmount(handle, preload[0].m_local, preload[0].m_total);
        SFileCloseFile(handle);
        local += preload[0].m_local;
        total += preload[0].m_total;
    }

    MovieRec* movie = g_movieDB.GetRecord(movieID);
    SStrCopy(path, movie->m_filename, sizeof(path));
    size_t length = strlen(path);

    strcpy(&path[length], ".mp3");
    SFile::FileGetIsLocalAmount(path, preload[1].m_local, preload[1].m_total);
    local += preload[1].m_local;
    total += preload[1].m_total;

    strcpy(&path[length], ".sbt");
    SFile::FileGetIsLocalAmount(path, preload[2].m_local, preload[2].m_total);
    local += preload[2].m_local;
    total += preload[2].m_total;
}

// OFFSET: 0x00513870/0x14015DFA0
bool MovieStreaming::GetMovieDownloadProgress(int32_t movieID, int64_t& local, int64_t& total) {
    total = 0;
    bool preloading = false;
    MoviePreload* preload = MovieStreaming::s_preloads[movieID];
    local = 0;

    for (uint32_t i = 0; i < 3; i++) {
        AsyncFileReadEnterQueueLock();

        if (preload[i].m_object && preload[i].m_object->m_file) {
            SFile::FileGetIsLocalAmount(preload[i].m_object->m_file, local, total);
            preloading = true;
        } else {
            local += preload[i].m_local;
            total += preload[i].m_total;
        }

        AsyncFileReadLeaveQueueLock();
    }

    if (!total) {
        GetMovieLocalAmount(local, total, movieID);
    }

    return preloading;
}

// PROVEN: 719b0323 IDENTICAL/UNPROVEN
// OFFSET: 0x00512910/0x14015CA00
bool MovieStreaming::IsMovieLocal(int32_t movieID) {
    return g_fileDataDB.GetRecord(MovieStreaming::FindBestMovieResolutionFileDataID(movieID, true)) != NULL;
}

// PROVEN: 11b4d893 IDENTICAL/NONE
// OFFSET: 0x00513C70
CMovieStreamer::CMovieStreamer() : m_aviFile(NULL) {
    this->Reset();
}

// PROVEN: 73774957 IDENTICAL/NONE
// OFFSET: 0x00513AB0
CMovieStreamer::~CMovieStreamer() {
    this->Cleanup();
}

// PROVEN: a8f7ce67 IDENTICAL/DIFFERENT
// OFFSET: 0x00513BF0/0x14015E3B0
void CMovieStreamer::Initialize(const char* movieName, const char* aviPath, const uint8_t* key) {
    this->Reset();
    SStrCopy(this->m_movieName, movieName, sizeof(this->m_movieName));
    SStrCopy(this->m_aviPath, aviPath, sizeof(this->m_aviPath));

    if (key) {
        memcpy(this->m_key, key, sizeof(this->m_key));
    }
}

// PROVEN: 864c3daa IDENTICAL/NONE
// OFFSET: 0x00512960
void CMovieStreamer::StreamAudio() {
    for (int32_t index = this->m_audioIndex; index >= 0; index = this->AdvanceAudioFrame()) {
        if (!this->IsChunkLocal(index)) {
            break;
        }

        this->ReadAudioFrame(index, this->m_audioBuffer + this->m_audioReadOffset);
    }
}

// OFFSET: 0x00512FB0/0x14015D2A0
bool CMovieStreamer::StreamFrame(bool decode, bool& skipped) {
    int32_t current = this->m_videoIndex;
    int32_t next = this->NextVideoIndex();
    uint32_t currentFlags = this->m_index[current].m_flags;
    bool currentMissing = (currentFlags & MOVIE_CHUNK_MISSING) != 0;
    bool currentSkipped = (currentFlags & MOVIE_CHUNK_SKIPPED) != 0;
    uint32_t nextFlags = this->m_index[next].m_flags;
    bool nextSkipped = (nextFlags & MOVIE_CHUNK_SKIPPED) != 0;
    bool nextMissing = (nextFlags & MOVIE_CHUNK_MISSING) != 0;

    if (this->m_interleavedAudio) {
        this->StreamAudio();
        this->PrefetchAudioFrames(2 * static_cast<int32_t>(this->m_info.m_fps), 0);

        if (this->m_audioIndex >= 0 && this->m_audioIndex < next) {
            return false;
        }
    }

    if (nextMissing || nextSkipped) {
        if (this->m_videoCodec == MOVIE_CODEC_PMVX) {
            this->m_cypher.ProcessBuffer(this->m_frameBuffers + this->m_currentBuffer * (this->m_maxFrameSize + 4), this->m_index[current].m_size + 4);
        }
    } else if (this->IsChunkLocal(next) || (decode && !this->m_interleavedAudio && (this->m_index[next].m_flags & MOVIE_CHUNK_KEYFRAME))) {
        this->ReadVideoFrame(next, this->m_frameBuffers + (this->m_maxFrameSize + 4) * ((this->m_currentBuffer + 1) % this->m_frameBufferCount));
    } else {
        this->m_index[next].m_flags |= MOVIE_CHUNK_MISSING;

        for (int32_t i = next + 1; i < this->m_indexCount - 1; i++) {
            AVIINDEXENTRY& entry = this->m_index[i];

            if (entry.m_chunkID[2] == 'd') {
                if (entry.m_flags & MOVIE_CHUNK_KEYFRAME) {
                    break;
                }

                entry.m_flags |= MOVIE_CHUNK_SKIPPED;
            }
        }
    }

    for (int32_t i = this->m_videoReadIndex + 1; i < this->m_indexCount - 1; i++) {
        if (i >= this->m_videoPrefetchIndex) {
            break;
        }

        if (this->m_index[i].m_chunkID[2] == 'd' && this->IsChunkLocal(i)) {
            this->m_videoReadIndex = i;
        }
    }

    this->PrefetchVideoFrames(static_cast<int32_t>(this->m_info.m_fps), 0);
    skipped = currentMissing || currentSkipped;
    return true;
}

// PROVEN: b8ef11e7 IDENTICAL/NONE
// OFFSET: 0x005131E0
int32_t CMovieStreamer::PrepareForStreaming() {
    if (!this->ParseAVIFile()) {
        return 0;
    }

    this->m_frameBufferCount = 2;
    this->m_frameBuffers = static_cast<uint8_t*>(DivxDecoderAlloc(2 * this->m_maxFrameSize + 8));

    if (this->m_videoCodec == MOVIE_CODEC_PMVX) {
        this->m_cypher.SetKey(this->m_key, this->m_videoIV);
    }

    this->InitStreamIndices();
    this->PrefetchVideoFrames(3 * static_cast<int32_t>(this->m_info.m_fps), 1);
    this->ReadVideoFrame(this->m_videoIndex, this->m_frameBuffers + this->m_currentBuffer * (this->m_maxFrameSize + 4));

    if (this->m_audioChunkCount && this->m_audioBufferSize) {
        this->m_interleavedAudio = 1;

        if (this->m_videoCodec == MOVIE_CODEC_PMVX) {
            this->m_audioCypher.SetKey(this->m_key, this->m_audioIV);
        }

        this->m_audioBuffer = static_cast<uint8_t*>(SMemAlloc(this->m_audioBufferSize, __FILE__, __LINE__, 0));
        this->PrefetchAudioFrames(3 * static_cast<int32_t>(this->m_info.m_fps), 1);
        this->StreamAudio();
    }

    return 1;
}

// OFFSET: 0x00513CC0
bool MovieStreaming::IsMoviePlayable(int32_t movieID) {
    if (g_fileDataDB.GetRecord(MovieStreaming::FindBestMovieResolutionFileDataID(movieID, true))) {
        return true;
    }

    MovieRec* movie = g_movieDB.GetRecord(movieID);
    FileDataRec* file = g_fileDataDB.GetRecord(MovieStreaming::FindBestMovieResolutionFileDataID(movieID, false));

    if (!file) {
        return false;
    }

    char aviPath[260];
    SStrCopy(aviPath, file->m_filepath, sizeof(aviPath));
    SStrPack(aviPath, file->m_filename, sizeof(aviPath));

    char movieName[260];
    SStrCopy(movieName, movie->m_filename, sizeof(movieName));

    CMovieStreamer streamer;
    streamer.Initialize(movieName, aviPath, NULL);

    if (!streamer.ParseAVIFile()) {
        return false;
    }

    MovieInfo info;
    streamer.GetVideoInfo(info);

    float bandwidth;
    bool measured = streamer.MeasureBandwidth(bandwidth);
    streamer.Cleanup();

    if (!measured) {
        return true;
    }

    float needed = (info.m_totalRate * 1.2f - bandwidth) * info.m_duration * 1024.0f;

    if (needed < 0.0f) {
        return true;
    }

    int64_t local;
    int64_t total;
    MovieStreaming::GetMovieDownloadProgress(movieID, local, total);
    return static_cast<double>(local) > needed;
}

// PROVEN: 37004c8c IDENTICAL/NONE
// OFFSET: 0x00513A40
static bool OnStreamingMoviesUpdate(ClientStreamingMoviesUpdate* msg) {
    for (uint32_t i = 0; i < msg->unk_0010.m_count; i++) {
        uint16_t movieID = msg->unk_0010.m_data[i];
        int32_t fileDataID = MovieStreaming::FindBestMovieResolutionFileDataID(movieID, true);

        if (!g_fileDataDB.GetRecord(fileDataID)) {
            MovieStreaming::PreloadMovie(movieID);
        }
    }

    return true;
}

// OFFSET: 0x00513AA0/0x14015E230
void MovieStreaming::InitializeGame() {
    ClientStreamingMoviesUpdate::s_handler = OnStreamingMoviesUpdate;
}

// PROVEN: 8fa8d028 IDENTICAL/NONE
// OFFSET: 0x00511A20
void MovieStreaming::ShutdownGame() {
    ClientStreamingMoviesUpdate::s_handler = NULL;
}
