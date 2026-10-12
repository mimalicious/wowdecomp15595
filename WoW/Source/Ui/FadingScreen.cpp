#include "WoW/Source/Ui/GameUI.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CMath.h"

static const uint16_t s_fadeIndices[4] = { 0, 1, 2, 3 };

static CTexture* s_fadeTexture;

static void* s_fadeParam;

// STATIC: 0x00F771B4
static void (*s_fadeCallback)(void* param);

static uint8_t s_fadeFinished;

static uint32_t s_fadeStartTime;

static float s_fadeDuration;

static int32_t s_fadeDirection;

static HLAYER s_fadeLayer;

// STATIC: 0x00F771CC/0x140E6D3E0
static uint8_t s_fadeActive;

// STATIC: 0x00F771CD
static uint8_t s_fadeStarted;

// OFFSET: 0x00842700/0x1407FDD80
static int32_t SwallowEvent(const void* data, void* param) {
    return 0;
}

// OFFSET: 0x00A0A260/0x1407AB2E0
static void RegisterHandlers() {
    EventRegisterEx(EVENT_ID_CHAR, SwallowEvent, NULL, 9.0f);
    EventRegisterEx(EVENT_ID_KEYDOWN, SwallowEvent, NULL, 9.0f);
    EventRegisterEx(EVENT_ID_KEYUP, SwallowEvent, NULL, 9.0f);
    EventRegisterEx(EVENT_ID_KEYDOWN_REPEATING, SwallowEvent, NULL, 9.0f);
    EventRegisterEx(EVENT_ID_MOUSEDOWN, SwallowEvent, NULL, 9.0f);
    EventRegisterEx(EVENT_ID_MOUSEUP, SwallowEvent, NULL, 9.0f);
    EventRegisterEx(EVENT_ID_MOUSEMOVE, SwallowEvent, NULL, 9.0f);
}

// OFFSET: 0x00A0A320/0x1407AB3A0
static void FadingScreenUnregisterEvents() {
    EventUnregister(EVENT_ID_CHAR, SwallowEvent);
    EventUnregister(EVENT_ID_KEYDOWN, SwallowEvent);
    EventUnregister(EVENT_ID_KEYDOWN_REPEATING, SwallowEvent);
    EventUnregister(EVENT_ID_KEYUP, SwallowEvent);
    EventUnregister(EVENT_ID_MOUSEDOWN, SwallowEvent);
    EventUnregister(EVENT_ID_MOUSEUP, SwallowEvent);
    EventUnregister(EVENT_ID_MOUSEMOVE, SwallowEvent);
}

// OFFSET: 0x00A0A380/0x1407AB420
void DisableFadingScreen(float duration, void (*callback)(void*), void* param) {
    if (!s_fadeActive) {
        return;
    }

    s_fadeCallback = callback;
    s_fadeStarted = 0;
    s_fadeDirection = 1;
    s_fadeDuration = duration;
    s_fadeFinished = 0;
    s_fadeParam = param;

    if (duration != 0.0f) {
        return;
    }

    HandleClose(reinterpret_cast<HOBJECT>(s_fadeLayer));
    TextureRelease(s_fadeTexture);
    FadingScreenUnregisterEvents();
    s_fadeActive = 0;
    s_fadeStarted = 0;
    s_fadeDirection = 0;

    if (s_fadeCallback) {
        s_fadeCallback(s_fadeParam);
    }
}

// OFFSET: 0x00A0A420/0x1407AB4C0
static void FadingScreenPaint(void* param, const RECTF* rect, const RECTF* visible, float elapsedSec) {
    static C3Vector s_positions[4] = { C3Vector(0.0f, 0.0f, 0.0f), C3Vector(1.0f, 0.0f, 0.0f), C3Vector(0.0f, 1.0f, 0.0f), C3Vector(1.0f, 1.0f, 0.0f) };
    static C2Vector s_texCoords[4] = { C2Vector(0.0f, 1.0f), C2Vector(1.0f, 1.0f), C2Vector(0.0f, 0.0f), C2Vector(1.0f, 0.0f) };
    static C3Vector s_normal = C3Vector(0.0f, 0.0f, 1.0f);
    uint8_t finished = 0;
    CImVector color(0xFF, 0xFF, 0xFF, 0xFF);

    if (s_fadeDirection) {
        float elapsed;

        if (s_fadeStarted) {
            elapsed = static_cast<float>(FrameTime::GetCurTimeMs() - s_fadeStartTime) * 0.001f;
        } else {
            elapsed = 0.0f;
            s_fadeStartTime = FrameTime::GetCurTimeMs();
        }

        if (s_fadeDuration > elapsed) {
            uint8_t alpha = CMath::fint_(elapsed / s_fadeDuration * 255.0f);
            color.a = s_fadeDirection == 2 ? alpha : 0xFF - alpha;
        } else {
            finished = 1;
            color.a = s_fadeDirection == 2 ? 0xFF : 0x00;
        }
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxPrimLockVertexPtrs(4, s_positions, sizeof(C3Vector), &s_normal, 0, &color, 0, NULL, 0, s_texCoords, sizeof(C2Vector), NULL, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTexBlocking(s_fadeTexture));
    GxDrawLockedElements(GxPrim_TriangleStrip, 4, s_fadeIndices);
    GxPrimUnlockVertexPtrs();
    g_theGxDevicePtr->RsPop();
    s_fadeStarted = 1;

    if (s_fadeFinished) {
        if (s_fadeDirection == 1) {
            HandleClose(reinterpret_cast<HOBJECT>(s_fadeLayer));
            TextureRelease(s_fadeTexture);
            FadingScreenUnregisterEvents();
            s_fadeActive = 0;
            s_fadeStarted = 0;
        }

        s_fadeDirection = 0;
        s_fadeFinished = 0;

        if (s_fadeCallback) {
            s_fadeCallback(s_fadeParam);
        }
    } else if (finished) {
        s_fadeFinished = 1;
    }
}

// OFFSET: 0x00A0A780/0x1407AB850
void EnableFadingScreen(float duration, void (*callback)(void*), void* param) {
    if (s_fadeActive) {
        HandleClose(reinterpret_cast<HOBJECT>(s_fadeLayer));
        TextureRelease(s_fadeTexture);
        FadingScreenUnregisterEvents();
        s_fadeActive = 0;
        s_fadeStarted = 0;
    }

    s_fadeTexture = TextureCreate(8, 8, GxTex_Argb8888, GxTex_Argb8888, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 1), reinterpret_cast<void*>(static_cast<uintptr_t>(0xFF000000)), GxuUpdateSingleColorTexture, NULL, 0);
    RECTF rect = { 0.0f, 0.0f, 1.0f, 1.0f };
    ScrnLayerCreate(&rect, 10.0f, duration != 0.0f ? 7 : 6, NULL, FadingScreenPaint, &s_fadeLayer);
    RegisterHandlers();
    s_fadeDuration = duration;
    s_fadeStarted = 0;
    s_fadeFinished = 0;
    s_fadeActive = 1;
    s_fadeDirection = 2;
    s_fadeCallback = callback;
    s_fadeParam = param;
}
