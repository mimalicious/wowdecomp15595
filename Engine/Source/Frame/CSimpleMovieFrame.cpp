#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Frame/CSimpleMovieFrame.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/DivxDecoderLoader.h"
#include "Engine/Source/Services/Panama.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/KeyChainRec.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

#define MOVIE_CAPTIONS_COMPRESSED 0x5A50424D
#define MOVIE_CAPTIONS_ENCRYPTED 0x4650424D

// STATIC: 0x00CA4614
int32_t CSimpleMovieFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00DD3BF0
int32_t CSimpleMovieFrame::s_objectType;

// STATIC: 0x00CA3490
const char* CSimpleMovieFrame::s_objectTypeName = "MovieFrame";

// STATIC: 0x00DD3BF4
CSimpleMovieFrame* CSimpleMovieFrame::s_currentMovieFrame;

// STATIC: 0x00DD3BF8
int32_t CSimpleMovieFrame::s_decoderCount;

// STATIC: 0x00DD3C38
CDataAllocator CSimpleMovieFrame::s_allocator(sizeof(CSimpleMovieFrame), 5);

// STATIC: 0x00DD3BFC
static int32_t s_sfxWasMuted;

// STATIC: 0x00DD3C00
static int32_t s_musicWasMuted;

// STATIC: 0x00DD3C04
static int32_t s_ambienceWasMuted;

// STATIC: 0x00DD3C08
static int32_t s_voiceChatWasMuted;

static uint32_t s_textureCount[7] = { 6, 2, 3, 4, 6, 2, 6 };

static uint32_t s_frameWidth[7] = { 800, 1024, 800, 1024, 800, 1024, 1280 };

static uint32_t s_frameHeight[7] = { 384, 512, 512, 576, 384, 512, 768 };

static MOVIETEXTURERECT s_textureRects[7][6] = {
    {
        { 0.0f, 0.64f, 0.11f, -0.33f },
        { 0.64f, 0.96f, 0.11f, -0.33f },
        { 0.96f, 1.0f, 0.11f, -0.33f },
        { 0.0f, 0.64f, 0.33f, 0.11f },
        { 0.64f, 0.96f, 0.33f, 0.11f },
        { 0.96f, 1.0f, 0.33f, 0.11f },
    },
    {
        { 0.0f, 0.5f, 0.333f, -0.333f },
        { 0.5f, 1.0f, 0.333f, -0.333f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
    },
    {
        { 0.0f, 0.64f, 0.42f, -0.42f },
        { 0.64f, 0.96f, 0.42f, -0.42f },
        { 0.96f, 1.0f, 0.42f, -0.42f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
    },
    {
        { 0.0f, 0.5f, 0.3266f, -0.42f },
        { 0.5f, 1.0f, 0.3266f, -0.42f },
        { 0.0f, 0.5f, 0.42f, 0.3266f },
        { 0.5f, 1.0f, 0.42f, 0.3266f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
    },
    {
        { 0.0f, 0.64f, 0.11f, -0.33f },
        { 0.64f, 0.96f, 0.11f, -0.33f },
        { 0.96f, 1.0f, 0.11f, -0.33f },
        { 0.0f, 0.64f, 0.33f, 0.11f },
        { 0.64f, 0.96f, 0.33f, 0.11f },
        { 0.96f, 1.0f, 0.33f, 0.11f },
    },
    {
        { 0.0f, 0.5f, 0.333f, -0.333f },
        { 0.5f, 1.0f, 0.333f, -0.333f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
    },
    {
        { 0.0f, 0.4f, 0.16f, -0.37328f },
        { 0.4f, 0.8f, 0.16f, -0.37328f },
        { 0.8f, 1.0f, 0.16f, -0.37328f },
        { 0.0f, 0.4f, 0.42664f, 0.16f },
        { 0.4f, 0.8f, 0.42664f, 0.16f },
        { 0.8f, 1.0f, 0.42664f, 0.16f },
    },
};

static MOVIETEXTUREINFO s_textureInfo[7][6] = {
    {
        { 512, 256, 0, 3200 },
        { 256, 256, 2048, 3200 },
        { 32, 256, 3072, 3200 },
        { 512, 128, 819200, 3200 },
        { 256, 128, 821248, 3200 },
        { 32, 128, 822272, 3200 },
    },
    {
        { 512, 512, 0, 4096 },
        { 512, 512, 2048, 4096 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 512, 512, 0, 3200 },
        { 256, 512, 2048, 3200 },
        { 32, 512, 3072, 3200 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 512, 512, 0, 4096 },
        { 512, 512, 2048, 4096 },
        { 512, 64, 2097152, 4096 },
        { 512, 64, 2099200, 4096 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 512, 256, 0, 3200 },
        { 256, 256, 2048, 3200 },
        { 32, 256, 3072, 3200 },
        { 512, 128, 819200, 3200 },
        { 256, 128, 821248, 3200 },
        { 32, 128, 822272, 3200 },
    },
    {
        { 512, 512, 0, 4096 },
        { 512, 512, 2048, 4096 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 512, 512, 0, 5120 },
        { 512, 512, 2048, 5120 },
        { 256, 512, 4096, 5120 },
        { 512, 256, 2621440, 5120 },
        { 512, 256, 2623488, 5120 },
        { 256, 256, 2625536, 5120 },
    },
};

// STATIC: 0x00BE2D8C
static MOVIEUPDATERECT s_updateRects[7][6] = {
    {
        { 0, 6, 512, 256 },
        { 0, 6, 256, 256 },
        { 0, 6, 32, 256 },
        { 0, 0, 512, 122 },
        { 0, 0, 256, 122 },
        { 0, 0, 32, 122 },
    },
    {
        { 0, 24, 512, 512 },
        { 0, 0, 512, 488 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 0, 32, 512, 480 },
        { 0, 32, 256, 480 },
        { 0, 32, 32, 480 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 0, 0, 512, 512 },
        { 0, 0, 512, 512 },
        { 0, 0, 512, 64 },
        { 0, 0, 512, 64 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 0, 21, 512, 256 },
        { 0, 21, 256, 256 },
        { 0, 21, 32, 256 },
        { 0, 0, 512, 107 },
        { 0, 0, 256, 107 },
        { 0, 0, 32, 107 },
    },
    {
        { 0, 38, 512, 512 },
        { 0, 0, 512, 474 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 0, 0, 512, 512 },
        { 0, 0, 512, 512 },
        { 0, 0, 256, 512 },
        { 0, 0, 512, 256 },
        { 0, 0, 512, 256 },
        { 0, 0, 256, 256 },
    },
};

static uint32_t s_frameOffset[7] = { 0x4B00, 0x18000, 0x19000, 0, 0x10680, 0x26000, 0 };

static const uint8_t s_defaultKey[32] = { 0xD7, 0xC8, 0xD9, 0xA0, 0xC8, 0xC5, 0xCC, 0xCC, 0xCF, 0xA0, 0xD4, 0xC8, 0xC5, 0xD2, 0xC5, 0xAC, 0xA0, 0xC9, 0xA7, 0xCD, 0xA0, 0xC1, 0xA0, 0xD4, 0xC5, 0xD3, 0xD4, 0xA0, 0xCB, 0xC5, 0xD9, 0xA1 };

static uint16_t s_indices[4] = { 0, 1, 2, 3 };

// OFFSET: 0x00523B40/0x1401720B0
static void MovieSoundFinished(SE3MemorySoundObject* object, void* param) {
    if (param) {
        bool playing = false;
        object->m_pFMODChannel->isPlaying(&playing);

        if (!playing) {
            static_cast<CSimpleMovieFrame*>(param)->m_finished = 1;
        }
    }
}

// PROVEN: b83e0506 IDENTICAL/NONE
// OFFSET: 0x00523B80/0x1401720F0
static void MovieSoundKitFinished(SE3SoundKitObject* sko) {
    CSimpleMovieFrame* movieFrame = static_cast<CSimpleMovieFrame*>(sko->m_playFinishedParam);

    if (movieFrame && !SE3::IsPlaying(movieFrame->m_sound)) {
        movieFrame->m_finished = 1;
    }
}

// OFFSET: 0x00523BB0/0x140172130
static void MovieTextureUpdate(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    if (command == GxTex_Latch) {
        MOVIETEXTUREDATA* data = static_cast<MOVIETEXTUREDATA*>(userArg);
        texels = data->m_buffer + data->m_info->m_offset;
        texelStrideInBytes = data->m_info->m_stride;
    }
}

// INLINED
void CSimpleMovieFrame::operator delete(void* ptr) {
    CSimpleMovieFrame::s_allocator.PutData(ptr, NULL, 0);
}

// PROVEN: 1ea1c914 IDENTICAL/NONE
// OFFSET: 0x00523BE0/0x140172160
FrameScript_Object::ScriptIx* CSimpleMovieFrame::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
    FrameScript_Object::ScriptIx* script = CSimpleFrame::GetScriptByName(name, data);

    if (script) {
        return script;
    }

    if (!SStrCmp(name, "onmoviefinished", STORM_MAX_STR)) {
        return &this->m_onMovieFinished;
    }

    if (!SStrCmp(name, "onmovieshowsubtitle", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,text) %s end";
        return &this->m_onMovieShowSubtitle;
    }

    if (!SStrCmp(name, "onmoviehidesubtitle", STORM_MAX_STR)) {
        return &this->m_onMovieHideSubtitle;
    }

    return NULL;
}

// PROVEN: 93efb220 IDENTICAL/NONE
// OFFSET: 0x00523CD0/0x140172210
void CSimpleMovieFrame::GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) {
    FrameScript_Object::GetScriptTime(L, this->m_onMovieFinished, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onMovieShowSubtitle, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onMovieHideSubtitle, time, count);
    CSimpleFrame::GetScriptTime(L, time, count, recurse);
}

// OFFSET: 0x00523D40/0x1401722B0
void CSimpleMovieFrame::CloseVideo() {
    if (this->m_decoder) {
        g_unInitializeDivxDecoder(this->m_decoder);
        CSimpleMovieFrame::s_decoderCount--;
        this->m_decoder = 0;
    }

    if (this->m_frameBuffer) {
        SMemFree(this->m_frameBuffer, __FILE__, __LINE__, 0);
        this->m_frameBuffer = NULL;
    }

    for (uint32_t i = 0; i < 6; i++) {
        if (this->m_textures[i]) {
            GxTexDestroy(this->m_textures[i]);
            this->m_textures[i] = NULL;
        }
    }
}

// OFFSET: 0x005243E0/0x140172A60
int32_t CSimpleMovieFrame::OpenAudio() {
    CSimpleMovieFrame::s_currentMovieFrame = this;

    s_sfxWasMuted = SE3::SoundBusIsMuted(SE3BUS_SFX);
    s_musicWasMuted = SE3::SoundBusIsMuted(SE3BUS_MUSIC);
    s_ambienceWasMuted = SE3::SoundBusIsMuted(SE3BUS_AMBIENCE);
    s_voiceChatWasMuted = SE3::SoundBusIsMuted(SE3BUS_VOICECHAT);
    SE3::SoundBusMute(SE3BUS_SFX, 1);
    SE3::SoundBusMute(SE3BUS_MUSIC, 1);
    SE3::SoundBusMute(SE3BUS_AMBIENCE, 1);
    SE3::SoundBusMute(SE3BUS_VOICECHAT, 1);

    if (this->m_streamer.m_interleavedAudio) {
        SE3MemorySoundProperties properties;
        properties.ResetToDefaults();
        properties.m_bus = SE3BUS_MOVIE;
        properties.m_fpPlayFinishedCallback = MovieSoundFinished;
        properties.m_playFinishedParam = this;
        properties.m_data = reinterpret_cast<const char*>(this->m_streamer.GetAudioBuffer());
        properties.m_length = this->m_streamer.GetAudioBufferSize();

        float volume = this->m_volume * 0.0039215689f;

        if (volume < 0.0f) {
            volume = 0.0f;
        } else if (volume >= 1.0f) {
            volume = 1.0f;
        }

        properties.m_volume = volume;
        properties.m_position = 0;
        properties.m_handle = &this->m_sound;
        SE3::PlaySoundFromMemory(&properties, 0);
        return 1;
    }

    char path[260];
    SStrCopy(path, this->m_movieName, sizeof(path));
    SStrPack(path, ".mp3", sizeof(path));

    SE3SoundKitProperties properties;
    float volume = this->m_volume * 0.0039215689f;
    properties.m_reverb = 0;
    properties.m_bit0 = 1;
    properties.m_bit10 = 1;
    properties.m_sFilename = path;
    properties.m_bus = SE3BUS_MOVIE;

    if (volume < 0.0f) {
        volume = 0.0f;
    } else if (volume >= 1.0f) {
        volume = 1.0f;
    }

    properties.unk_0004 = volume;
    SE3::PlaySoundKit(-128, NULL, &properties, &this->m_sound, __FILE__, __LINE__, "CSimpleMovieFrame::OpenAudio", 1, this->m_key);
    SE3::SetPlayFinishedCallback(this->m_sound, MovieSoundKitFinished, this);
    return 1;
}

// PROVEN: b29eb7d5 IDENTICAL/NONE
// OFFSET: 0x00523DB0/0x140172330
void CSimpleMovieFrame::AudioDeviceChanged_ShutDown() {
    if (CSimpleMovieFrame::s_currentMovieFrame) {
        CSimpleMovieFrame::s_currentMovieFrame->m_audioRestart = 1;
    }
}

// PROVEN: 98f4c054 IDENTICAL/NONE
// OFFSET: 0x00523DD0/0x140172350
void CSimpleMovieFrame::AudioDeviceChanged_Restart() {
    if (CSimpleMovieFrame::s_currentMovieFrame && CSimpleMovieFrame::s_currentMovieFrame->m_audioRestart) {
        CSimpleMovieFrame::s_currentMovieFrame->m_audioRestart = 0;
    }
}

// OFFSET: 0x00523DF0
void CSimpleMovieFrame::CloseAudio() {
    CSimpleMovieFrame::s_currentMovieFrame = NULL;

    if (this->m_streamer.m_interleavedAudio) {
        SE3::SetMemorySoundFinishedCallback(this->m_sound, NULL);
    } else {
        SE3::SetPlayFinishedCallback(this->m_sound, NULL, NULL);
    }

    SE3::StopMemorySound(this->m_sound);
}

// OFFSET: 0x00523E50/0x140172380
bool CSimpleMovieFrame::DecodeFrame(bool render) {
    bool skipped;

    if (!this->m_streamer.StreamFrame(render, skipped)) {
        return false;
    }

    DivxDecodeFrame frame = {};
    frame.m_output = this->m_frameBuffer + s_frameOffset[this->m_layout];
    frame.m_input = this->m_streamer.CurrentVideoFrameBuffer() + 4;
    frame.m_inputSize = this->m_streamer.CurrentVideoFrameSize();
    frame.unk_0014 = skipped;
    frame.m_render = render && !skipped;

    if (g_divxDecode(this->m_decoder, &frame, 0)) {
        return false;
    }

    if (render && !skipped) {
        for (uint32_t i = 0; i < s_textureCount[this->m_layout]; i++) {
            const MOVIEUPDATERECT& rect = s_updateRects[this->m_layout][i];
            GxTexUpdate(this->m_textures[i], rect.m_minX, rect.m_minY, rect.m_maxX, rect.m_maxY, 1);
        }
    }

    this->m_streamer.AdvanceVideoFrame();
    return true;
}

// PROVEN: 555b6dd5 IDENTICAL/NONE
// OFFSET: 0x00523F90
bool CSimpleMovieFrame::SubtitlesVisible() {
    return this->m_subtitlesEnabled || !SE3::IsPlaying(this->m_sound);
}

// PROVEN: 58471d3c IDENTICAL/NONE
// OFFSET: 0x00524070/0x140172610
void CSimpleMovieFrame::RunOnMovieFinishedScript() {
    if (this->HasScript(this->m_onMovieFinished)) {
        this->RunScript(this->m_onMovieFinished, 0, NULL);
    }
}

// PROVEN: 711ac025 IDENTICAL/NONE
// OFFSET: 0x005240A0
void CSimpleMovieFrame::RunOnMovieShowSubtitleScript(const char* text) {
    if (this->HasScript(this->m_onMovieShowSubtitle)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, text);
        this->RunScript(this->m_onMovieShowSubtitle, 1, NULL);
    }
}

// PROVEN: 82ad950b IDENTICAL/NONE
// OFFSET: 0x005240F0
void CSimpleMovieFrame::RunOnMovieHideSubtitleScript() {
    if (this->HasScript(this->m_onMovieHideSubtitle)) {
        this->RunScript(this->m_onMovieHideSubtitle, 0, NULL);
    }
}

// OFFSET: 0x00524120/0x140172700
int32_t CSimpleMovieFrame::OpenVideo() {
    int32_t result = 0;

    this->m_startTime = OsGetAsyncTimeMs();
    this->m_elapsedMs = 0;
    this->m_decoder = ++CSimpleMovieFrame::s_decoderCount;

    if (!g_initializeDivxDecoder(this->m_decoder, this->m_info.m_width, this->m_info.m_height) && !g_setOutputFormat(this->m_decoder, 1, this->m_info.m_width, this->m_info.m_height)) {
        int32_t layoutFound = 1;

        if (this->m_info.m_width == 1280) {
            this->m_layout = 6;
        } else if (this->m_info.m_width == 1024) {
            if (this->m_info.m_height == 576) {
                this->m_layout = 3;
            } else if (this->m_info.m_height == 436) {
                this->m_layout = 5;
            } else {
                this->m_layout = 1;
            }
        } else if (this->m_info.m_width == 800) {
            if (this->m_info.m_height == 448) {
                this->m_layout = 2;
            } else if (this->m_info.m_height == 342 || this->m_info.m_height == 340) {
                this->m_layout = 4;
            } else {
                this->m_layout = 0;
            }
        } else {
            layoutFound = 0;
        }

        if (layoutFound) {
            this->m_frameBuffer = static_cast<uint8_t*>(SMemAlloc(4 * s_frameWidth[this->m_layout] * s_frameHeight[this->m_layout], __FILE__, __LINE__, 0));

            if (this->m_frameBuffer) {
                memset(this->m_frameBuffer, 0, 4 * s_frameWidth[this->m_layout] * s_frameHeight[this->m_layout]);

                int32_t created = 1;
                CGxTexFlags flags(GxTex_Linear, 0, 0, 0, 0, 1);

                for (uint32_t i = 0; i < s_textureCount[this->m_layout]; i++) {
                    this->m_textureData[i].m_info = &s_textureInfo[this->m_layout][i];
                    this->m_textureData[i].m_buffer = this->m_frameBuffer;
                    created &= GxTexCreate(s_textureInfo[this->m_layout][i].m_width, s_textureInfo[this->m_layout][i].m_height, GxTex_Argb8888, flags, &this->m_textureData[i], MovieTextureUpdate, this->m_textures[i]);
                }

                if (created) {
                    for (uint32_t i = 0; i < s_textureCount[this->m_layout]; i++) {
                        GxTexUpdate(this->m_textures[i], 0, 0, 0, 0, 1);
                    }

                    result = 1;
                }
            }
        }
    }

    this->m_targetFrame = 0;
    this->m_currentFrame = -1;
    this->m_droppedFrames = 0;
    this->m_frameOffset = 0;

    if (!result) {
        this->CloseVideo();
    }

    return result;
}

// OFFSET: 0x00524620
CSimpleMovieFrame::CSimpleMovieFrame(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_sound = 0;
    this->m_isPlaying = 0;
    this->m_finished = 0;
    this->m_subtitlesEnabled = 0;
    this->m_waitingForKey = 0;
    this->m_audioRestart = 0;
    this->m_volume = 100;
    this->m_decoder = 0;
    this->m_frameBuffer = NULL;
    memset(&this->m_info, 0, sizeof(this->m_info));

    for (uint32_t i = 0; i < 6; i++) {
        this->m_textures[i] = NULL;
    }
}

// PROVEN: bbd5c1ae IDENTICAL/NONE
// OFFSET: 0x00524710
const char* CSimpleMovieFrame::GetObjectTypeName() {
    return CSimpleMovieFrame::s_objectTypeName;
}

// PROVEN: 630c5040 IDENTICAL/NONE
// OFFSET: 0x00524720/0x140172F10
int32_t CSimpleMovieFrame::GetScriptMetaTable() {
    return CSimpleMovieFrame::s_metatable;
}

// PROVEN: 7184a3e2 IDENTICAL/NONE
// OFFSET: 0x00524740/0x140172F30
bool CSimpleMovieFrame::IsA(int32_t type) {
    return type == CSimpleMovieFrame::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: 56b800de IDENTICAL/NONE
// OFFSET: 0x00524780
bool CSimpleMovieFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleMovieFrame::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// OFFSET: 0x005247C0/0x140172FD0
int32_t CSimpleMovieFrame::OpenCaptions() {
    char path[260];
    SStrCopy(path, this->m_movieName, sizeof(path));
    SStrPack(path, ".sbt", sizeof(path));

    void* data;
    uint32_t size;

    if (SFile::LoadFile(path, &data, &size, 1)) {
        const char* text = static_cast<const char*>(data);
        uint32_t magic = *static_cast<uint32_t*>(data);
        char* decoded = NULL;

        if (magic == MOVIE_CAPTIONS_COMPRESSED || magic == MOVIE_CAPTIONS_ENCRYPTED) {
            uint8_t iv[32];
            memcpy(iv, static_cast<uint8_t*>(data) + 4, sizeof(iv));
            uint32_t decodedSize = *reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(data) + 36);
            decoded = static_cast<char*>(SMemAlloc(decodedSize + 1, __FILE__, __LINE__, 0));
            size -= 40;

            Crypt::PanamaCypher cypher;
            cypher.SetKey(this->m_key, iv);
            cypher.ProcessBuffer(static_cast<uint8_t*>(data) + 40, size);

            if (magic == MOVIE_CAPTIONS_COMPRESSED) {
                zlib_uncompress(reinterpret_cast<uint8_t*>(decoded), &decodedSize, static_cast<uint8_t*>(data) + 40, size);
            } else {
                memcpy(decoded, static_cast<uint8_t*>(data) + 40, size);
            }

            decoded[decodedSize] = '\0';
            text = decoded;
        }

        if (*reinterpret_cast<const uint8_t*>(text) == 0xEF) {
            text += 3;
        }

        char line[1024];

        do {
            SStrTokenize(&text, line, sizeof(line), "\r\n", NULL);

            if (!line[0]) {
                break;
            }

            if (line[0] == '0') {
                MOVIECAPTION* caption = this->m_captions.New();

                uint32_t time = 60 * SStrToInt(&line[0]);
                time = 60 * (time + SStrToInt(&line[3]));
                time = 1000 * (time + SStrToInt(&line[6]));
                caption->m_startTime = time + 41 * SStrToInt(&line[9]);

                time = 60 * SStrToInt(&line[14]);
                time = 60 * (time + SStrToInt(&line[17]));
                time = 1000 * (time + SStrToInt(&line[20]));
                caption->m_endTime = time + 41 * SStrToInt(&line[23]);

                SStrTokenize(&text, line, sizeof(line), "\r\n", NULL);
                caption->m_text = SStrDupA(line, __FILE__, __LINE__);
            }
        } while (line[0] && *text);

        SFile::Unload(data);

        if (decoded) {
            SMemFree(decoded, __FILE__, __LINE__, 0);
        }
    }

    this->m_currentCaption = -1;
    return 1;
}

// PROVEN: ac91373a IDENTICAL/NONE
// OFFSET: 0x00524B10/0x140173370
void CSimpleMovieFrame::CloseCaptions() {
    if (this->m_currentCaption >= 0) {
        this->RunOnMovieHideSubtitleScript();
        this->m_currentCaption = -1;
    }

    for (uint32_t i = 0; i < this->m_captions.Count(); i++) {
        SMemFree(this->m_captions.m_data[i].m_text, __FILE__, __LINE__, 0);
    }

    this->m_captions.Clear();
}

// PROVEN: 602601e4 IDENTICAL/NONE
// OFFSET: 0x00524BD0/0x140173460
void CSimpleMovieFrame::Cleanup() {
    SE3::SoundBusMute(SE3BUS_SFX, s_sfxWasMuted);
    SE3::SoundBusMute(SE3BUS_MUSIC, s_musicWasMuted);
    SE3::SoundBusMute(SE3BUS_AMBIENCE, s_ambienceWasMuted);
    SE3::SoundBusMute(SE3BUS_VOICECHAT, s_voiceChatWasMuted);

    if (!this->m_isPlaying) {
        return;
    }

    this->CloseVideo();
    this->CloseAudio();
    this->CloseCaptions();
    UnloadDivxDecoder();
    this->m_finished = 0;
    this->m_isPlaying = 0;
    this->m_streamer.Cleanup();
}

// PROVEN: cc54c1ba IDENTICAL/NONE
// OFFSET: 0x00524C60/0x140173520
void CSimpleMovieFrame::StopMovie() {
    if (this->m_isPlaying) {
        this->Cleanup();
        this->RunOnMovieFinishedScript();
    }
}

// OFFSET: 0x00524CA0
bool CSimpleMovieFrame::UpdateTiming() {
    int32_t playing = SE3::IsPlaying(this->m_sound);

    if (playing) {
        this->m_elapsedMs = SE3::GetPositionInMS(this->m_sound);
        this->m_startTime = OsGetAsyncTimeMs() - this->m_elapsedMs;
    } else {
        this->m_elapsedMs = OsGetAsyncTimeMs() - this->m_startTime;
    }

    int32_t frame = static_cast<int32_t>(static_cast<double>(this->m_elapsedMs) * this->m_info.m_fps * 0.001 + 0.5);

    if (playing) {
        uint32_t position = SE3::GetPositionInMS(this->m_sound);
        int32_t audioFrame = static_cast<int32_t>(static_cast<double>(position) * this->m_info.m_fps * 0.001 + 0.5);
        int32_t drift = audioFrame - this->m_frameOffset - frame;
        int32_t absDrift = abs(drift);

        if (absDrift > 2 || (absDrift > 1 && this->m_frameOffset + frame == this->m_currentFrame)) {
            if (drift > 0) {
                this->m_frameOffset++;
            } else {
                this->m_frameOffset--;
            }
        }
    }

    int32_t targetFrame = frame + this->m_frameOffset;

    if (targetFrame <= this->m_currentFrame) {
        targetFrame = this->m_currentFrame;
    }

    this->m_targetFrame = targetFrame;

    if (targetFrame >= this->m_info.m_frameCount) {
        this->m_finished = 1;
    }

    if (this->m_finished) {
        this->StopMovie();
        return false;
    }

    if (targetFrame == this->m_currentFrame || (targetFrame == 3481 && this->m_movieID == 23)) {
        return true;
    }

    if (targetFrame != this->m_currentFrame + 1) {
        this->m_droppedFrames++;
    }

    while (this->m_currentFrame < this->m_targetFrame - 1) {
        this->DecodeFrame(false);
        this->m_currentFrame++;
    }

    if (!this->DecodeFrame(true)) {
        this->m_finished = 1;
    }

    this->m_currentFrame = this->m_targetFrame;

    if (this->m_currentCaption >= 0 && this->m_captions.m_data[this->m_currentCaption].m_endTime <= this->m_elapsedMs) {
        this->RunOnMovieHideSubtitleScript();
        this->m_currentCaption = -1;
    }

    if (this->m_currentCaption < 0 && this->SubtitlesVisible()) {
        for (uint32_t i = 0; i < this->m_captions.Count(); i++) {
            MOVIECAPTION& caption = this->m_captions.m_data[i];

            if (caption.m_startTime <= this->m_elapsedMs && caption.m_endTime > this->m_elapsedMs) {
                this->RunOnMovieShowSubtitleScript(this->m_captions.m_data[i].m_text);
                this->m_currentCaption = i;
                break;
            }
        }
    }

    return true;
}

// OFFSET: 0x00524F20
void CSimpleMovieFrame::Render() {
    float aspect = CoordinateGetAspectCompensation();

    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);
    GxXformSetViewport(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);

    CImVector clearColor(0, 0, 0, 0xFF);
    GxSceneClear(31, clearColor);

    C44Matrix view;
    C44Matrix projection;
    GxuXformCreateOrtho(0.0f, 1.0f, -0.5f, 0.5f, 0.0f, 500.0f, projection);
    GxXformSetView(view);
    GxXformSetProjection(projection);

    // STATIC: 0x00DD3C6C
    static C3Vector s_normal(0.0f, 0.0f, 1.0f);

    static C2Vector s_texCoords[4] = { C2Vector(0.0f, 0.0f), C2Vector(1.0f, 0.0f), C2Vector(0.0f, 1.0f), C2Vector(1.0f, 1.0f) };

    g_theGxDevicePtr->RsPush();

    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_BlendingMode, GxBlend_Opaque);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    DefaultShaders::SetFogEnabled(0);
    DefaultShaders::SetDefaultShaders(GxVBF_PNT, 1);

    for (uint32_t i = 0; i < s_textureCount[this->m_layout]; i++) {
        const MOVIETEXTURERECT& rect = s_textureRects[this->m_layout][i];

        C3Vector positions[4] = {
            C3Vector(rect.m_left, rect.m_bottom * aspect, 0.0f),
            C3Vector(rect.m_right, rect.m_bottom * aspect, 0.0f),
            C3Vector(rect.m_left, rect.m_top * aspect, 0.0f),
            C3Vector(rect.m_right, rect.m_top * aspect, 0.0f)
        };

        GxPrimLockVertexPtrs(4, positions, sizeof(C3Vector), &s_normal, 0, NULL, 0, NULL, 0, s_texCoords, sizeof(C2Vector), NULL, 0);
        g_theGxDevicePtr->RsSet(GxRs_Texture0, this->m_textures[i]);
        GxDrawLockedElements(GxPrim_TriangleStrip, 4, s_indices);
        GxPrimUnlockVertexPtrs();
    }

    g_theGxDevicePtr->RsPop();
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

// OFFSET: [0x005253A0/0x140173F00, 0x00524730]
CSimpleMovieFrame::~CSimpleMovieFrame() {
    this->Cleanup();
}

// OFFSET: 0x00525840/0x140174470
void CSimpleMovieFrame::StartMovieDelayed(bool success) {
    MovieRec* movie = g_movieDB.GetRecord(this->m_movieID);
    this->m_waitingForKey = 0;

    const uint8_t* key = s_defaultKey;

    if (success) {
        WOWGUID guid = WOWGUID::Make(0);
        KeyChainRec* keyChain = g_keyChainDB.GetRecord(movie->m_keyID, guid, NULL, NULL, false);

        if (keyChain) {
            key = keyChain->m_key;
        }
    }

    memcpy(this->m_key, key, sizeof(this->m_key));
    this->m_streamer.Initialize(this->m_movieName, this->m_moviePath, this->m_key);

    if (!this->m_streamer.PrepareForStreaming()) {
        UnloadDivxDecoder();
        this->RunOnMovieFinishedScript();
        return;
    }

    this->m_streamer.GetVideoInfo(this->m_info);

    if (this->OpenVideo()) {
        this->OpenAudio();
        this->OpenCaptions();
        this->m_isPlaying = 1;
        return;
    }

    UnloadDivxDecoder();
    this->m_streamer.Cleanup();
    this->RunOnMovieFinishedScript();
}

// PROVEN: 4d4c54b5 IDENTICAL/NONE
// OFFSET: 0x00525980/0x140174610
void CSimpleMovieFrame::KeyChainCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (param) {
        static_cast<CSimpleMovieFrame*>(param)->StartMovieDelayed(success);
    }
}

// OFFSET: 0x005259A0/0x140174700
int32_t CSimpleMovieFrame::StartMovie(int32_t movieID) {
    if (!LoadDivxDecoder()) {
        return 1;
    }

    this->m_finished = 0;
    this->m_movieID = movieID;

    MovieRec* movie = g_movieDB.GetRecord(movieID);
    FileDataRec* file = g_fileDataDB.GetRecord(MovieStreaming::FindBestMovieResolutionFileDataID(movieID, false));

    if (!file || !movie) {
        UnloadDivxDecoder();
        this->RunOnMovieFinishedScript();
        return 2;
    }

    MovieStreaming::CancelPreloadingMovie(this->m_movieID);
    this->m_volume = movie->m_volume;
    SStrCopy(this->m_moviePath, file->m_filepath, sizeof(this->m_moviePath));
    SStrPack(this->m_moviePath, file->m_filename, sizeof(this->m_moviePath));
    SStrCopy(this->m_movieName, movie->m_filename, sizeof(this->m_movieName));

    const uint8_t* key = s_defaultKey;

    if (movie->m_keyID) {
        WOWGUID guid = WOWGUID::Make(0);
        KeyChainRec* keyChain = g_keyChainDB.GetRecord(movie->m_keyID, guid, CSimpleMovieFrame::KeyChainCallback, this, false);

        if (!keyChain) {
            this->m_waitingForKey = 1;
            this->m_waitTimeout = 1000 * (ClientServices::Connection() ? 6 : 1) + OsGetAsyncTimeMs();
            return 0;
        }

        key = keyChain->m_key;
    }

    memcpy(this->m_key, key, sizeof(this->m_key));
    this->m_streamer.Initialize(this->m_movieName, this->m_moviePath, this->m_key);

    if (!this->m_streamer.PrepareForStreaming()) {
        UnloadDivxDecoder();
        this->RunOnMovieFinishedScript();
        return 3;
    }

    this->m_streamer.GetVideoInfo(this->m_info);

    if (!this->OpenVideo()) {
        UnloadDivxDecoder();
        this->m_streamer.Cleanup();
        this->RunOnMovieFinishedScript();
        return 4;
    }

    this->OpenAudio();
    this->OpenCaptions();
    this->m_isPlaying = 1;
    return 0;
}

// OFFSET: 0x00525CA0
void CSimpleMovieFrame::RenderMovie(void* param) {
    CSimpleMovieFrame* movieFrame = static_cast<CSimpleMovieFrame*>(param);

    if (!movieFrame) {
        return;
    }

    if (movieFrame->m_isPlaying) {
        if (movieFrame->UpdateTiming()) {
            movieFrame->Render();
        }
    } else if (movieFrame->m_waitingForKey && static_cast<int32_t>(OsGetAsyncTimeMs() - movieFrame->m_waitTimeout) >= 0) {
        g_keyChainDB.CancelCallback(movieFrame->m_keyID, CSimpleMovieFrame::KeyChainCallback, movieFrame);
        movieFrame->StartMovieDelayed(false);
    }
}

// PROVEN: b044b2b1 IDENTICAL/NONE
// OFFSET: 0x00525CC0
void CSimpleMovieFrame::OnFrameRender(CRenderBatch* batch, uint32_t layer) {
    CSimpleFrame::OnFrameRender(batch, layer);

    if (layer == 2) {
        batch->QueueCallback(&CSimpleMovieFrame::RenderMovie, this);
    }
}
