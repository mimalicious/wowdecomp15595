#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Images/ImageWriter.h"
#include "Engine/Source/Images/tga.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Scrn/Layer.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SRgn.h"
#include "Engine/Source/Tempest/C44Matrix.h"

// STATIC: 0x00BCEA84
static const RECTF s_defaultRect = { 0.0f, 0.0f, 1.0f, 1.0f };

// STATIC: 0x00D07518/0x140BB8120
static char s_screenshotFormat[16] = "jpeg";

// STATIC: 0x00D07528/0x140BB8130
static uint32_t s_screenshotQuality = 60;

// STATIC: 0x00D0752C/0x140BB8138
static TSExplicitList<CILayer, offsetof(CILayer, m_zorderlink)> s_zorderlist;

// STATIC: 0x00EBB0F8/0x140DABE60
static float s_elapsedSec;

// STATIC: 0x00EBB0FC/0x140DABE68
static void (*s_screenshotCallback)(int32_t success);

// STATIC: 0x00EBB100
static int32_t s_noPresent;

// STATIC: 0x00EBB104/0x140DABE70
static uint8_t* s_screenshotData;

// STATIC: 0x00EBB108/0x140DABE78
static uint32_t s_screenshotDataSize;

// STATIC: 0x00EBB10C/0x140DABE80
static const char* s_screenshotDirectory;

// STATIC: 0x00EBB110/0x140DABE88
static const char* s_screenshotFileName;

// STATIC: 0x00EBB114/0x140DABE90
static HSRGN s_layerRgn;

// OFFSET: 0x007FF270/0x140512E50
static void* LAYER_MemAlloc(size_t bytes) {
    return SMemAlloc(bytes, __FILE__, __LINE__, 0);
}

// OFFSET: 0x007FF290/0x140512E70
static void LAYER_MemFree(void* ptr) {
    SMemFree(ptr, __FILE__, __LINE__, 0);
}

// OFFSET: 0x007FF2B0/0x140512E90
static int32_t OnIdle(const void* data, void* param) {
    s_elapsedSec += static_cast<const EVENT_DATA_IDLE*>(data)->m_elapsedSec;

    return 1;
}

// OFFSET: 0x007FF5E0/0x140513230
void ScrnSetScreenshotFormat(const char* format) {
    SStrCopy(s_screenshotFormat, format, sizeof(s_screenshotFormat));
}

// OFFSET: 0x007FF620/0x140513260
void ScrnSetScreenshotQuality(int32_t quality) {
    if (static_cast<uint32_t>(quality - 1) > 9) {
        quality = quality - 1 < 0 ? 1 : 10;
    }

    s_screenshotQuality = static_cast<uint32_t>(quality * 11 + 90) / 2;
}

// OFFSET: 0x007FF2E0/0x140512EB0
void ScrnLayerSetRect(HLAYER layer, const RECTF* rect) {
    reinterpret_cast<CILayer*>(layer)->m_rect = *rect;
}

// OFFSET: 0x007FF310/0x140512ED0
uint32_t ScrnLayerGetFlags(HLAYER layer) {
    return reinterpret_cast<CILayer*>(layer)->m_flags;
}

// OFFSET: 0x007FF320/0x14090EB20
void ScrnLayerSetFlags(HLAYER layer, uint32_t flags) {
    reinterpret_cast<CILayer*>(layer)->m_flags = flags;
}

// OFFSET: 0x007FF330/0x140512EE0
void ScrnScreenshot(void (*callback)(int32_t success), uint8_t* data, uint32_t size, const char* directory, const char* fileName) {
    s_screenshotCallback = callback;
    s_screenshotData = data;
    s_screenshotDataSize = size;
    s_screenshotDirectory = directory;
    s_screenshotFileName = fileName;
}

// OFFSET: 0x007FF360/0x140512F10
static int32_t CaptureScreen() {
    uint32_t width;
    uint32_t height;
    const CImVector* pixels;

    g_theGxDevicePtr->DeviceReadScreenShot(width, height, pixels);

    if (pixels && ImageWriterInitialize(s_screenshotDataSize, LAYER_MemAlloc, LAYER_MemFree, 0)) {
        OsCreateDirectory("Screenshots", 0);

        char directory[260];

        if (s_screenshotDirectory) {
            SStrPrintf(directory, sizeof(directory), "%s\\%s", "Screenshots", s_screenshotDirectory);
            OsCreateDirectory(directory, 0);
        } else {
            SStrPrintf(directory, sizeof(directory), "%s", "Screenshots");
        }

        char name[260];

        if (s_screenshotFileName) {
            SStrPrintf(name, sizeof(name), "%s", s_screenshotFileName);
        } else {
            char timeStamp[260];
            OsGetTimeStamp(timeStamp, sizeof(timeStamp));
            SStrPrintf(name, sizeof(name), "WoWScrnShot_%s", timeStamp);
        }

        if (_stricmp(s_screenshotFormat, "tga") && _stricmp(s_screenshotFormat, "png") && s_screenshotQuality < 100) {
            ImageWriterEmbedData(pixels, width, height, 4, s_screenshotData, s_screenshotDataSize);
        }

        char fileName[260];

        if (!_stricmp(s_screenshotFormat, "tga")) {
            SStrPrintf(fileName, sizeof(fileName), "%s\\%s.tga", directory, name);

            CTgaFile tga;
            tga.SetImage(pixels, width, height, 32, 0, 1, 0);
            tga.RemoveAlphaChannels();
            tga.Compress();
            tga.Write(fileName);
            tga.Close();
        } else {
            SStrPrintf(fileName, sizeof(fileName), "%s\\%s.jpg", directory, name);
            ImageWriterWriteJpeg(fileName, pixels, width, height, 4, s_screenshotQuality);
        }

        ImageWriterShutdown();
    }

    return pixels != NULL;
}

// OFFSET: 0x007FF650/0x140513290
static int32_t OnPaint(const void* data, void* param) {
    if (GxHasContext() && GxIsWindowVisible()) {
        SRgnClear(s_layerRgn);

        RECTF screen = { 0.0f, 0.0f, 1.0f, 1.0f };
        SRgnCombineRectf(s_layerRgn, &screen, NULL, 2);

        for (CILayer* layer = s_zorderlist.Tail(); layer; layer = s_zorderlist.Prev(layer)) {
            SRgnGetBoundingRectf(s_layerRgn, &layer->m_visible);

            layer->m_visible.left = layer->m_rect.left >= layer->m_visible.left ? layer->m_rect.left : layer->m_visible.left;
            layer->m_visible.bottom = layer->m_rect.bottom >= layer->m_visible.bottom ? layer->m_rect.bottom : layer->m_visible.bottom;
            layer->m_visible.right = layer->m_rect.right <= layer->m_visible.right ? layer->m_rect.right : layer->m_visible.right;
            layer->m_visible.top = layer->m_rect.top <= layer->m_visible.top ? layer->m_rect.top : layer->m_visible.top;

            if (!(layer->m_flags & 1)) {
                SRgnCombineRectf(s_layerRgn, &layer->m_rect, NULL, 4);
            }
        }

        float minX;
        float maxX;
        float minY;
        float maxY;
        float minZ;
        float maxZ;
        GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);

        CILayer* next;

        for (CILayer* layer = s_zorderlist.Head(); !s_zorderlist.IsEnd(layer); layer = next) {
            next = s_zorderlist.RawNext(layer);

            if (layer->m_visible.left < layer->m_visible.right && layer->m_visible.bottom < layer->m_visible.top) {
                if (layer->m_flags & 4) {
                    GxXformSetViewport(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
                } else {
                    GxXformSetViewport(layer->m_visible.left, layer->m_visible.right, layer->m_visible.bottom, layer->m_visible.top, 0.0f, 1.0f);
                }

                if (layer->m_flags & 2) {
                    C44Matrix view;
                    C44Matrix proj;
                    GxuXformCreateOrtho(layer->m_visible.left, layer->m_visible.right, layer->m_visible.bottom, layer->m_visible.top, 0.0f, 500.0f, proj);
                    GxuAdjustProjForHalfPixel(proj);
                    GxXformSetView(view);
                    GxXformSetProjection(proj);
                }

                layer->m_paintFunc(layer->m_param, &layer->m_rect, &layer->m_visible, s_elapsedSec);
            }
        }

        GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
        GxuFontUpdate();

        C3Vector viewPos(0.0f, 0.0f, 0.0f);
        GxuFlushDrawList(GxuCat_2, viewPos);

        if (!s_noPresent) {
            if (s_screenshotCallback) {
                g_theGxDevicePtr->DeviceTakeScreenShot();
                GxScenePresent(NULL);

                void (*callback)(int32_t success) = s_screenshotCallback;
                s_screenshotCallback = NULL;
                callback(CaptureScreen());

                g_theGxDevicePtr->DeviceClearScreenShot();
            } else {
                GxScenePresent(NULL);
            }
        }

        s_elapsedSec = 0.0f;

        if (!GxIsWindowVisible() && OsSleepInBackground()) {
            uint32_t count = OsGetBackgroundSleepMs() / 25;

            for (uint32_t i = 0; i < count; i++) {
                OsSleep(25);
                SE3::HeartBeat(NULL, NULL);
            }

            uint32_t remainder = OsGetBackgroundSleepMs() - count * 25;

            if (remainder) {
                OsSleep(remainder);
            }
        }
    } else {
        C3Vector viewPos(0.0f, 0.0f, 0.0f);
        GxuFlushDrawList(GxuCat_2, viewPos);
        GxScenePresent(NULL);
    }

    return 1;
}

// OFFSET: 0x007FFC00/0x140513910
void ILayerInitialize() {
    SRgnCreate(&s_layerRgn);
    EventRegister(EVENT_ID_IDLE, OnIdle);
    EventRegister(EVENT_ID_PAINT, OnPaint);
}

// OFFSET: 0x007FFC30/0x140513950
static void ILayerPaint() {
    OnPaint(NULL, NULL);
}

// OFFSET: 0x007FFC40/0x140513960
void ScrnLayerCreate(const RECTF* rect, float zorder, uint32_t flags, void* param, void (*paintFunc)(void* param, const RECTF* rect, const RECTF* visible, float elapsedSec), HLAYER* layer) {
    const RECTF* layerRect = rect ? rect : &s_defaultRect;

    void* mem = SMemAlloc(sizeof(CILayer), "HLAYER", SLOG_OBJECT, 0);
    CILayer* ilayer = mem ? new (mem) CILayer : NULL;

    ilayer->m_rect = *layerRect;
    ilayer->m_zorder = zorder;
    ilayer->m_flags = flags;
    ilayer->m_param = param;
    ilayer->m_paintFunc = paintFunc;

    CILayer* existing;

    for (existing = s_zorderlist.Tail(); existing && zorder < existing->m_zorder; existing = s_zorderlist.Prev(existing)) {
    }

    s_zorderlist.LinkNode(ilayer, 1, existing);

    *layer = reinterpret_cast<HLAYER>(HandleCreate(ilayer, "HLAYER"));
}

// OFFSET: 0x007FFD20/0x140513B00
CILayer::~CILayer() {
}

// OFFSET: 0x007FFBC0/0x1405138C0
void ILayerDestroy() {
    SRgnDelete(s_layerRgn);
    s_layerRgn = NULL;
    EventUnregister(EVENT_ID_IDLE, OnIdle);
    EventUnregister(EVENT_ID_PAINT, OnPaint);
}

// OFFSET: 0x007FFD90/0x140513B90
void ScrnLayerPaint() {
    ILayerPaint();
}
