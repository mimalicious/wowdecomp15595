#ifndef ENGINE_SOURCE_FRAME_MOVIE_H
#define ENGINE_SOURCE_FRAME_MOVIE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/Panama.h"
#include "Engine/Source/Storm/H/stpl.h"

class CAsyncObject;
class SFileImpl;

struct AVIINDEXENTRY {
    /* 0000 */ uint8_t m_chunkID[4];
    /* 0004 */ uint32_t m_flags;
    /* 0008 */ uint32_t m_offset;
    /* 000C */ uint32_t m_size;
};

struct MovieInfo {
    /* 0000 */ float m_fps;
    /* 0004 */ int32_t m_width;
    /* 0008 */ int32_t m_height;
    /* 000C */ int32_t m_frameCount;
    /* 0010 */ float m_videoRate;
    /* 0014 */ float m_audioRate;
    /* 0018 */ float m_totalRate;
    /* 001C */ float m_duration;
};

struct MoviePreload {
    /* 0000 */ CAsyncObject* m_object;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ int64_t m_local;
    /* 0010 */ int64_t m_total;

    MoviePreload();
};

class MoviePreloadMap {
    public:
    struct Node {
        struct Value {
            /* 0000 */ int32_t m_key;
            /* 0008 */ MoviePreload m_preloads[3];
        };

        /* 0000 */ Node* m_next;
        /* 0008 */ Value m_value;
    };

    /* 0000 */ uint32_t m_bucketCount;
    /* 0004 */ Node** m_buckets;
    /* 0008 */ uint32_t m_count;
    /* 000C */ float m_maxLoadFactor;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;

    MoviePreloadMap();
    ~MoviePreloadMap();
    MoviePreload* operator[](const int32_t& key);
    void Rehash(uint32_t bucketCount);
};

class CMovieStreamer {
    public:
    /* 0000 */ uint8_t m_key[32];
    /* 0020 */ Crypt::PanamaCypher m_cypher;
    /* 08D4 */ uint8_t* m_videoIV;
    /* 08D8 */ Crypt::PanamaCypher m_audioCypher;
    /* 118C */ uint8_t* m_audioIV;
    /* 1190 */ uint32_t m_videoCodec;
    /* 1194 */ uint32_t m_moviOffset;
    /* 1198 */ AVIINDEXENTRY* m_index;
    /* 119C */ int32_t m_indexCount;
    /* 11A0 */ int32_t m_videoIndex;
    /* 11A4 */ int32_t m_videoPrefetchIndex;
    /* 11A8 */ int32_t m_audioPrefetchIndex;
    /* 11AC */ int32_t m_videoReadIndex;
    /* 11B0 */ int32_t m_audioIndex;
    /* 11B4 */ char m_movieName[260];
    /* 12B8 */ char m_aviPath[260];
    /* 13BC */ MovieInfo m_info;
    /* 13DC */ SFileImpl* m_aviFile;
    /* 13E0 */ uint8_t* m_frameBuffers;
    /* 13E4 */ uint32_t m_videoDataSize;
    /* 13E8 */ int32_t m_frameBufferCount;
    /* 13EC */ int32_t m_currentBuffer;
    /* 13F0 */ uint32_t m_videoFrameCount;
    /* 13F4 */ uint32_t m_maxFrameSize;
    /* 13F8 */ int32_t m_interleavedAudio;
    /* 13FC */ uint8_t* m_audioBuffer;
    /* 1400 */ uint32_t m_audioReadOffset;
    /* 1404 */ uint32_t m_audioBufferSize;
    /* 1408 */ int32_t m_audioChunkCount;
    /* 140C */ TSGrowableArray<CAsyncObject*> m_asyncObjects;

    static void AsyncObjectCancelCallback(CAsyncObject* object);
    static void FramePreloadCallback(CAsyncObject* object);

    CMovieStreamer();
    ~CMovieStreamer();
    int32_t AdvanceAudioFrame();
    int32_t AdvanceVideoFrame();
    void Cleanup();
    void CleanupAsyncObjects();
    uint8_t* CurrentVideoFrameBuffer();
    uint32_t CurrentVideoFrameSize();
    uint8_t* GetAudioBuffer();
    uint32_t GetAudioBufferSize();
    uint16_t GetFreeAsyncObjectSlot();
    void GetVideoInfo(MovieInfo& info);
    void Initialize(const char* movieName, const char* aviPath, const uint8_t* key);
    void InitStreamIndices();
    int32_t IsChunkLocal(int32_t index);
    bool MeasureBandwidth(float& bandwidth);
    int32_t NextVideoIndex();
    int32_t ParseAVIFile();
    int32_t ParseAVIHeader();
    int32_t ParseAVIIndex();
    void PrefetchAudioFrames(int32_t count, uint8_t wait);
    void PrefetchFrame(int32_t index, bool wait, uint8_t priority);
    void PrefetchVideoFrames(int32_t count, uint8_t wait);
    int32_t PrepareForStreaming();
    void ReadAudioFrame(int32_t index, uint8_t* buffer);
    void ReadVideoFrame(int32_t index, uint8_t* buffer);
    void Reset();
    void StreamAudio();
    bool StreamFrame(bool decode, bool& skipped);
};

namespace ClientStreamingMovies {
    extern uint32_t s_handler;
}

namespace MovieStreaming {
    extern MoviePreloadMap s_preloads;
    extern float s_progressBase;
    extern float s_progressScale;

    void CancelPreloadingMovie(int32_t movieID);
    int32_t FindBestMovieResolutionFileDataID(int32_t movieID, bool localOnly);
    bool GetMovieDownloadProgress(int32_t movieID, int64_t& local, int64_t& total);
    void InitializeGame();
    bool IsMovieLocal(int32_t movieID);
    bool IsMoviePlayable(int32_t movieID);
    void PreloadMovie(int32_t movieID);
    void ShutdownGame();
    void WaitForDownloadingMovies(int32_t movieID);
}

#endif
