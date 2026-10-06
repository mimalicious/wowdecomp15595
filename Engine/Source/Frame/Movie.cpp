#include "Engine/Source/Frame/Movie.h"
#include <cstdio>
#include <cstring>
#include <malloc.h>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/Ui/GameUI.h"

#define MOVIE_CODEC_PIVX 0x58564950
#define MOVIE_CHUNK_KEYFRAME 0x00000010
#define MOVIE_CHUNK_MISSING 0x10000000
#define MOVIE_CHUNK_SKIPPED 0x20000000
#define MOVIE_CHUNK_PREFETCHING 0x40000000
#define MOVIE_CHUNK_LOCAL 0x80000000

// STATIC: %0x00CD1318
MoviePreload MovieStreaming::s_preloads[64][3];

// STATIC: %0x00CD12F8
float MovieStreaming::s_progressBase;

// STATIC: %0x00CD12FC
float MovieStreaming::s_progressScale;

// STATIC: %0x00CD2518
static uint32_t s_bandwidthTime;

// STATIC: %0x00CD251C
static float s_bandwidth;

// OFFSET: %0x00521C90
MoviePreload::MoviePreload() {
    this->m_object = NULL;
    this->m_local = 0;
    this->m_total = 0;
}

// OFFSET: %0x00521CB0
static void PreloadCallback(void* param, CAsyncObject* object) {
    MoviePreload* preload = static_cast<MoviePreload*>(param);
    preload->m_total = 0;
    preload->m_local = 0;
    SFile::FileGetIsLocalAmount(object->m_file, preload->m_local, preload->m_total);
    AsyncFileReadDestroyObject(object);
    preload->m_object = NULL;
}

// OFFSET: %0x00521D00
static void StartPreload(MoviePreload* preload, const char* path, uint8_t priority) {
    SFileImpl* file;
    SFile::EnableHash(false);

    if (!SFileOpenFile(path, &file)) {
        return;
    }

    SFile::EnableHash(true);

    if (SFile::FileIsLocal(file, 6)) {
        SFileCloseFile(file);
        return;
    }

    preload->m_object = AsyncFileReadAllocObject();

    if (!preload->m_object) {
        return;
    }

    preload->m_object->m_file = file;
    preload->m_object->m_size = SFileGetFileSize(file, NULL);
    preload->m_object->m_buffer = NULL;
    preload->m_object->m_userArg = preload;
    preload->m_object->m_userPostloadCallback = reinterpret_cast<void*>(&PreloadCallback);
    preload->m_object->m_userPostUnloadCallback = reinterpret_cast<void*>(&PreloadCallback);
    preload->m_object->m_priority = priority;
    preload->m_total = 0;
    preload->m_local = 0;
    SFile::FileGetIsLocalAmount(file, preload->m_local, preload->m_total);
    AsyncFileReadObject(preload->m_object, 1);
}

// OFFSET: %0x00521DD0
void MovieStreaming::CancelPreloadingMovie(int32_t movieID) {
    MoviePreload* preload = MovieStreaming::s_preloads[movieID];

    for (uint32_t i = 0; i < 3; i++) {
        if (preload[i].m_object) {
            AsyncFileReadCancel(preload[i].m_object, NULL);
            preload[i].m_object = NULL;
        }
    }
}

// OFFSET: %0x00521E10
static void WaitProgressCallback(float progress, int32_t param) {
    LoadingScreenAsyncCallback(MovieStreaming::s_progressScale * progress + MovieStreaming::s_progressBase, NULL);
}

// OFFSET: %0x00521E40
void MovieStreaming::WaitForDownloadingMovies(int32_t movieID) {
    MoviePreload* preload = MovieStreaming::s_preloads[movieID];
    uint32_t sizes[3];
    uint32_t totalSize = 0;
    int32_t loading = 0;

    for (int32_t i = 0; i < 3; i++) {
        if (preload[i].m_object) {
            sizes[i] = SFileGetFileSize(preload[i].m_object->m_file, NULL);
        } else {
            sizes[i] = 0;
        }

        totalSize += sizes[i];
    }

    MovieStreaming::s_progressBase = 0.0f;

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

// OFFSET: %0x00521F60
void CMovieStreamer::Reset() {
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
    this->m_audioFile = NULL;
    this->m_audioBuffer = NULL;
    this->m_audioReadOffset = 0;
    this->m_audioBufferSize = 0;
    this->m_audioChunkCount = 0;
}

// OFFSET: %0x00522050
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
        SMemFree(this->m_frameBuffers, __FILE__, __LINE__, 0);
        this->m_frameBuffers = NULL;
    }

    if (this->m_audioBuffer) {
        SMemFree(this->m_audioBuffer, __FILE__, __LINE__, 0);
        this->m_audioBuffer = NULL;
    }

    if (this->m_aviFile) {
        SFileCloseFile(this->m_aviFile);
        this->m_aviFile = NULL;
    }

    if (this->m_audioFile) {
        SFileCloseFile(this->m_audioFile);
        this->m_audioFile = NULL;
    }
}

// PROVEN: 5dc6ffd5 IDENTICAL/IDENTICAL
// OFFSET: 0x00511A50/0x14015B8D0
void CMovieStreamer::GetVideoInfo(MovieInfo& info) {
    info = this->m_info;
}

// OFFSET: %0x00522150
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

// OFFSET: %0x005221C0
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

// OFFSET: %0x00522240
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

// OFFSET: %0x00522320
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

// OFFSET: %0x005223F0
void CMovieStreamer::ReadVideoFrame(int32_t index, uint8_t* buffer) {
    SFile::SetFilePointer(this->m_aviFile, this->m_index[index].m_offset + this->m_moviOffset + 4, NULL, 0);
    SFileReadFile(this->m_aviFile, buffer + 4, this->m_index[index].m_size, NULL, NULL, 0);

    if (this->m_videoCodec == MOVIE_CODEC_PIVX) {
        this->m_cypher.ProcessBuffer(buffer, this->m_index[index].m_size + 4);
    }
}

// OFFSET: %0x00522480
void CMovieStreamer::ReadAudioFrame(int32_t index, uint8_t* buffer) {
    SFile::SetFilePointer(this->m_aviFile, this->m_index[index].m_offset + this->m_moviOffset + 4, NULL, 0);
    SFileReadFile(this->m_aviFile, buffer, this->m_index[index].m_size, NULL, NULL, 0);
    this->m_index[index].m_flags &= ~MOVIE_CHUNK_PREFETCHING;
    this->m_index[index].m_flags |= MOVIE_CHUNK_LOCAL;

    if (this->m_videoCodec == MOVIE_CODEC_PIVX) {
        this->m_cypher.ProcessBuffer(buffer, this->m_index[index].m_size + 4);
    }
}

// OFFSET: %0x00522530
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

// OFFSET: %0x005225B0
void CMovieStreamer::PrefetchChunk(int32_t index, uint8_t wait, int32_t queue) {
    AVIINDEXENTRY& entry = this->m_index[index];

    if (!(entry.m_flags & (MOVIE_CHUNK_LOCAL | MOVIE_CHUNK_PREFETCHING))) {
        SFile::MakeFileRangeLocal(this->m_aviFile, queue, wait != 0, entry.m_offset + this->m_moviOffset + 4, entry.m_size);
        this->m_index[index].m_flags |= MOVIE_CHUNK_PREFETCHING;
    }
}

// OFFSET: %0x00522620
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
                this->PrefetchChunk(index, wait, this->m_interleavedAudio != 0);
                remaining--;
            }
        }
    }

    this->m_videoPrefetchIndex = index < this->m_indexCount - 1 ? index : this->m_indexCount - 1;
}

// OFFSET: %0x005226E0
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
            this->PrefetchChunk(index, wait, 0);
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

// OFFSET: %0x005227A0
int32_t CMovieStreamer::ParseAVIHeader() {
    if (!this->m_aviFile) {
        return 0;
    }

    char* header = NULL;
    uint32_t headerSize = 0;
    char chunk[12];

    if (!SFileReadFile(this->m_aviFile, chunk, 12, NULL, NULL, 0) || SStrCmpI(&chunk[0], "RIFF", 4) || SStrCmpI(&chunk[8], "AVI ", 4)) {
        return 0;
    }

    while (SFileReadFile(this->m_aviFile, chunk, 8, NULL, NULL, 0)) {
        uint32_t size = *reinterpret_cast<uint32_t*>(&chunk[4]);

        if (!SStrCmpI(&chunk[0], "LIST", 4)) {
            if (!SFileReadFile(this->m_aviFile, chunk, 4, NULL, NULL, 0)) {
                break;
            }

            size -= 4;

            if (!SStrCmpI(&chunk[0], "hdrl", 4)) {
                headerSize = size;
                header = static_cast<char*>(alloca(size));

                if (!SFileReadFile(this->m_aviFile, header, size, NULL, NULL, 0)) {
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

            if (!SFileReadFile(this->m_aviFile, this->m_index, size, NULL, NULL, 0)) {
                break;
            }
        } else if (!SStrCmpI(&chunk[0], "pviv", 4)) {
            this->m_videoIV = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));

            if (!SFileReadFile(this->m_aviFile, this->m_videoIV, size, NULL, NULL, 0)) {
                break;
            }
        } else if (!SStrCmpI(&chunk[0], "paiv", 4)) {
            this->m_audioIV = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));

            if (!SFileReadFile(this->m_aviFile, this->m_audioIV, size, NULL, NULL, 0)) {
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

// OFFSET: %0x00522B30
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

// OFFSET: %0x00522C80
int32_t CMovieStreamer::ReadStandaloneAudio() {
    char path[256];
    SStrCopy(path, this->m_movieName, sizeof(path));
    SStrPack(path, ".mp3", sizeof(path));

    if (!SFile::OpenEx(NULL, path, 1, &this->m_audioFile)) {
        return 0;
    }

    uint32_t sizeHigh = 0;
    this->m_audioBufferSize = SFileGetFileSize(this->m_audioFile, &sizeHigh);
    this->m_audioBuffer = static_cast<uint8_t*>(SMemAlloc(this->m_audioBufferSize, __FILE__, __LINE__, 0));

    if (!SFileReadFile(this->m_audioFile, this->m_audioBuffer, this->m_audioBufferSize, NULL, NULL, 0)) {
        return 0;
    }

    uint32_t header = (this->m_audioBuffer[0] << 3) | (this->m_audioBuffer[1] >> 5);

    if (header == 0x550) {
        Crypt::PanamaCypher cypher;
        cypher.SetKey(this->m_key, this->m_audioBuffer + this->m_audioBufferSize - 32);
        cypher.ProcessBuffer(this->m_audioBuffer + 8, this->m_audioBufferSize - 40);
        this->m_audioBuffer[0] = 0xFF;
        this->m_audioBuffer[1] |= 0xE0;
    } else if (header != 0x7FF) {
        return 0;
    }

    return 1;
}

// OFFSET: %0x00522E10
uint8_t* CMovieStreamer::GetAudioBuffer() {
    return this->m_audioBuffer;
}

// OFFSET: %0x00522E20
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

// OFFSET: %0x00522E80
bool CMovieStreamer::MeasureBandwidth(float& bandwidth) {
    if (s_bandwidth > 0.0f) {
        bandwidth = s_bandwidth;
        return true;
    }

    int32_t first = 0;

    while (this->IsChunkLocal(first) && first < this->m_indexCount - 1) {
        first++;
    }

    int32_t last = this->m_indexCount - 1;

    if (first == last) {
        bandwidth = 0.0f;
        s_bandwidth = 0.0f;
        s_bandwidthTime = OsGetAsyncTimeMs();
        return false;
    }

    int32_t span = first + 3 * static_cast<int32_t>(this->m_info.m_fps) * ((this->m_interleavedAudio != 0) + 1);

    if (span < last) {
        last = span;
    }

    uint32_t start = this->m_index[first].m_offset + this->m_moviOffset;
    uint32_t end = this->m_index[last].m_size + this->m_index[last].m_offset + this->m_moviOffset;

    int64_t localBefore = 0;
    int64_t localAfter = 0;
    int64_t total = 0;
    SFile::FileGetIsLocalAmount(this->m_aviFile, localBefore, total);

    uint32_t startTime = OsGetAsyncTimeMs();
    uint32_t half = (end - start) >> 1;
    SFile::MakeFileRangeLocal(this->m_aviFile, 0, false, start, half);
    SFile::MakeFileRangeLocal(this->m_aviFile, 0, true, (static_cast<int64_t>(end) + start) / 2, half);

    uint32_t endTime = OsGetAsyncTimeMs();
    SFile::FileGetIsLocalAmount(this->m_aviFile, localAfter, total);

    if (localAfter == localBefore || endTime == startTime) {
        bandwidth = 0.0f;
        return true;
    }

    bandwidth = static_cast<float>(localAfter - localBefore) / static_cast<float>(endTime - startTime);
    s_bandwidth = bandwidth;
    s_bandwidthTime = OsGetAsyncTimeMs();
    return true;
}

// OFFSET: %0x00523050
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

// OFFSET: %0x005230B0
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
    int32_t width = 0;
    int32_t height = 0;
    CVar* resolution = CVar::Lookup("gxResolution");

    if (resolution && resolution->m_stringValue.m_str) {
        sscanf(resolution->m_stringValue.m_str, "%dx%d", &width, &height);
    } else {
        width = 800;
    }

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

            int32_t local = SFile::FileIsLocal(handle, 6);
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

// OFFSET: %0x005232F0
void MovieStreaming::PreloadMovie(int32_t movieID) {
    FileDataRec* file = g_fileDataDB.GetRecord(MovieStreaming::FindBestMovieResolutionFileDataID(movieID, false));

    if (!file) {
        return;
    }

    char path[260];
    SStrCopy(path, file->m_filepath, sizeof(path));
    SStrPack(path, file->m_filename, sizeof(path));

    MoviePreload* preload = MovieStreaming::s_preloads[movieID];

    if (!preload[0].m_object) {
        StartPreload(&preload[0], path, 132);
    }

    MovieRec* movie = g_movieDB.GetRecord(movieID);
    SStrCopy(path, movie->m_filename, sizeof(path));
    size_t length = strlen(path);

    if (!preload[1].m_object) {
        strcpy(&path[length], ".mp3");
        StartPreload(&preload[1], path, 132);
    }

    if (!preload[2].m_object) {
        strcpy(&path[length], ".sbt");
        StartPreload(&preload[2], path, 132);
    }
}

// OFFSET: %0x005234B0
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

// OFFSET: %0x005236E0
bool MovieStreaming::GetMovieDownloadProgress(int32_t movieID, int64_t& local, int64_t& total) {
    total = 0;
    bool preloading = false;
    MoviePreload* preload = MovieStreaming::s_preloads[movieID];
    local = 0;

    for (uint32_t i = 0; i < 3; i++) {
        if (preload[i].m_object) {
            SFile::FileGetIsLocalAmount(preload[i].m_object->m_file, local, total);
            preloading = true;
        } else {
            local += preload[i].m_local;
            total += preload[i].m_total;
        }
    }

    if (!total) {
        GetMovieLocalAmount(local, total, movieID);
    }

    return preloading;
}

// OFFSET: %0x00523770
bool MovieStreaming::IsMovieLocal(int32_t movieID) {
    return g_fileDataDB.GetRecord(MovieStreaming::FindBestMovieResolutionFileDataID(movieID, true)) != NULL;
}

// OFFSET: %0x005237C0
CMovieStreamer::CMovieStreamer() {
    this->Reset();
}

// OFFSET: %0x005237F0
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

// OFFSET: %0x00523890
void CMovieStreamer::StreamAudio() {
    for (int32_t index = this->m_audioIndex; index >= 0; index = this->AdvanceAudioFrame()) {
        if (!this->IsChunkLocal(index)) {
            break;
        }

        this->ReadAudioFrame(index, this->m_audioBuffer + this->m_audioReadOffset);
    }
}

// OFFSET: %0x005238E0
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
        this->PrefetchAudioFrames(66, 0);

        if (this->m_audioIndex >= 0 && this->m_audioIndex < next) {
            return false;
        }
    }

    if (nextMissing || nextSkipped) {
        if (this->m_videoCodec == MOVIE_CODEC_PIVX) {
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

    this->PrefetchVideoFrames(2, 0);
    skipped = currentMissing || currentSkipped;
    return true;
}

// OFFSET: %0x00523B00
int32_t CMovieStreamer::PrepareForStreaming() {
    if (!this->ParseAVIFile()) {
        return 0;
    }

    this->m_frameBufferCount = 2;
    this->m_frameBuffers = static_cast<uint8_t*>(SMemAlloc(2 * this->m_maxFrameSize + 8, __FILE__, __LINE__, 0));

    if (this->m_videoCodec == MOVIE_CODEC_PIVX) {
        this->m_cypher.SetKey(this->m_key, this->m_videoIV);
    }

    this->InitStreamIndices();
    this->PrefetchVideoFrames(3 * static_cast<int32_t>(this->m_info.m_fps), 1);
    this->ReadVideoFrame(this->m_videoIndex, this->m_frameBuffers + this->m_currentBuffer * (this->m_maxFrameSize + 4));

    if (this->m_audioChunkCount && this->m_audioBufferSize) {
        this->m_interleavedAudio = 1;
        this->m_audioBuffer = static_cast<uint8_t*>(SMemAlloc(this->m_audioBufferSize, __FILE__, __LINE__, 0));
        this->PrefetchAudioFrames(3 * static_cast<int32_t>(this->m_info.m_fps), 1);
        this->StreamAudio();
        return 1;
    }

    if (this->ReadStandaloneAudio()) {
        return 1;
    }

    if (this->m_frameBuffers) {
        SMemFree(this->m_frameBuffers, __FILE__, __LINE__, 0);
    }

    if (this->m_audioBuffer) {
        SMemFree(this->m_audioBuffer, __FILE__, __LINE__, 0);
    }

    this->m_audioBuffer = NULL;
    this->m_frameBuffers = NULL;
    return 0;
}

// OFFSET: %0x00523C50
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
