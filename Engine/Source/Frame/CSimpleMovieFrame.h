#ifndef ENGINE_SOURCE_FRAME_CSIMPLEMOVIEFRAME_H
#define ENGINE_SOURCE_FRAME_CSIMPLEMOVIEFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/Movie.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Storm/H/stpl.h"

class CGxTex;
class CRenderBatch;
struct lua_State;
struct WOWGUID;

struct MOVIECAPTION {
    /* 0000 */ uint32_t m_startTime;
    /* 0004 */ uint32_t m_endTime;
    /* 0008 */ char* m_text;
};

struct MOVIETEXTUREINFO {
    /* 0000 */ uint32_t m_width;
    /* 0004 */ uint32_t m_height;
    /* 0008 */ uint32_t m_offset;
    /* 000C */ uint32_t m_stride;
};

struct MOVIETEXTUREDATA {
    /* 0000 */ const MOVIETEXTUREINFO* m_info;
    /* 0004 */ uint8_t* m_buffer;
};

struct MOVIETEXTURERECT {
    /* 0000 */ float m_left;
    /* 0004 */ float m_right;
    /* 0008 */ float m_top;
    /* 000C */ float m_bottom;
};

struct MOVIEUPDATERECT {
    /* 0000 */ int32_t m_minX;
    /* 0004 */ int32_t m_minY;
    /* 0008 */ int32_t m_maxX;
    /* 000C */ int32_t m_maxY;
};

class CSimpleMovieFrame : public CSimpleFrame {
    public:
    static CDataAllocator s_allocator;
    static CSimpleMovieFrame* s_currentMovieFrame;
    static int32_t s_decoderCount;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static void AudioDeviceChanged_Restart();
    static void AudioDeviceChanged_ShutDown();
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void KeyChainCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static void RegisterScriptMethods(lua_State* L);
    static void RenderMovie(void* param);

    /* 01F0 */ uint8_t m_key[32];
    /* 0210 */ uint32_t m_sound;
    /* 0214 */ CMovieStreamer m_streamer;
    /* 1624 */ int32_t m_isPlaying;
    /* 1628 */ int32_t m_finished;
    /* 162C */ int32_t m_subtitlesEnabled;
    /* 1630 */ int32_t m_waitingForKey;
    /* 1634 */ uint32_t m_waitTimeout;
    /* 1638 */ int32_t m_audioRestart;
    /* 163C */ char m_moviePath[260];
    /* 1740 */ char m_movieName[260];
    /* 1844 */ int32_t m_volume;
    /* 1848 */ int32_t m_movieID;
    /* 184C */ int32_t m_keyID;
    /* 1850 */ FrameScript_Object::ScriptIx m_onMovieFinished;
    /* 1851 */ FrameScript_Object::ScriptIx m_onMovieShowSubtitle;
    /* 1852 */ FrameScript_Object::ScriptIx m_onMovieHideSubtitle;
    /* 1854 */ int32_t m_decoder;
    /* 1858 */ MovieInfo m_info;
    /* 1878 */ uint32_t m_layout;
    /* 187C */ CGxTex* m_textures[6];
    /* 1894 */ MOVIETEXTUREDATA m_textureData[6];
    /* 18C4 */ uint8_t* m_frameBuffer;
    /* 18C8 */ uint32_t m_startTime;
    /* 18CC */ int32_t m_currentFrame;
    /* 18D0 */ int32_t m_targetFrame;
    /* 18D4 */ int32_t m_droppedFrames;
    /* 18D8 */ int32_t m_frameOffset;
    /* 18DC */ uint32_t m_elapsedMs;
    /* 18E0 */ TSGrowableArray<MOVIECAPTION> m_captions;
    /* 18F0 */ int32_t m_currentCaption;

    virtual ~CSimpleMovieFrame();
    virtual int32_t GetScriptMetaTable();
    virtual FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data);
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse);
    virtual void OnFrameRender(CRenderBatch* batch, uint32_t layer);

    CSimpleMovieFrame(CSimpleFrame* parent);
    void Cleanup();
    void CloseCaptions();
    void CloseVideo();
    bool DecodeFrame(bool render);
    int32_t OpenAudio(uint32_t position);
    int32_t OpenCaptions();
    int32_t OpenVideo();
    void Render();
    void RunOnMovieFinishedScript();
    void RunOnMovieHideSubtitleScript();
    void RunOnMovieShowSubtitleScript(const char* text);
    int32_t StartMovie(int32_t movieID);
    void StartMovieDelayed(bool success);
    void StopMovie();
    bool SubtitlesVisible();
    bool UpdateTiming();
};

#include "Engine/Source/Frame/CSimpleMovieFrame.inl"

#endif
