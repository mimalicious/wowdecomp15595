#include <cstdint>
#include <cstring>
#include <malloc.h>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Services/TextBlock.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Tempest/C3Spline_CatmullRom.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/Rec/LoadingScreenTaxiSplinesRec.h"
#include "WoW/Source/DB/Rec/TaxiPathNodeRec.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Ui/WorldMap.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/Rec/LoadingScreensRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/Event/CSizeEvent.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Scrn/Types.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: #0x009E2DFC
static const BackgroundImage s_textureInfo[2] = {
    { "Interface\\Glues\\LoadingBar\\Loading-BarFill", true, 0.5f, 0.075f, 0.525f, 0.025f },
    { "Interface\\Glues\\LoadingBar\\Loading-BarBorder", false, 0.5f, 0.075f, 0.6f, 0.05f },
};

// STATIC: #0x009E2E2C
static const BackgroundImage s_miniTextureInfo[2] = {
    { "Interface\\Glues\\LoadingBar\\Loading-BarFill", true, 0.5f, 0.975f, 0.525f, 0.025f },
    { "Interface\\Glues\\LoadingBar\\Loading-BarBorder", false, 0.5f, 0.975f, 0.6f, 0.05f },
};

// STATIC: #0x00AB63B4
static float s_normalAspect = 4.0f / 3.0f;

// STATIC: #0x00AB63B8
static float s_wideAspect = 1.6f;

// STATIC: 0x00C97628
static int32_t s_simpleMapID = -1;

// STATIC: 0x00C9762C
static uint16_t s_indices[4] = { 0, 1, 2, 3 };

// STATIC: #0x00AB63C8
static const char* s_fontNames[9] = {
    "Fonts\\FRIZQT__.TTF",
    "Fonts\\2002.TTF",
    "Fonts\\FRIZQT__.TTF",
    "Fonts\\FRIZQT__.TTF",
    "Fonts\\ZYKai_T.TTF",
    "Fonts\\blei00d.TTF",
    "Fonts\\FRIZQT__.TTF",
    "Fonts\\FRIZQT__.TTF",
    "Fonts\\NIM_____.TTF",
};

// STATIC: #0x00AB63FC
static CImVector s_pathColor = CImVector(0xFF, 0xFF, 0xFF, 0xFF);

// STATIC: #0x00AB6400
static C2Vector s_texCoord[4] = {
    C2Vector(0.0f, 1.0f),
    C2Vector(1.0f, 1.0f),
    C2Vector(0.0f, 0.0f),
    C2Vector(1.0f, 0.0f),
};

// STATIC: #0x00AB6420
static C3Vector s_pathOffsets[4] = {
    C3Vector(-0.01f, -0.015f, 0.0f),
    C3Vector(0.01f, -0.015f, 0.0f),
    C3Vector(-0.01f, 0.015f, 0.0f),
    C3Vector(0.01f, 0.015f, 0.0f),
};

// STATIC: #0x00AB6450
static C2Vector s_pathTexCoords[4] = {
    C2Vector(0.0f, 1.0f),
    C2Vector(0.5f, 1.0f),
    C2Vector(0.0f, 0.5f),
    C2Vector(0.5f, 0.5f),
};

// STATIC: 0x00C976D4
static CSizeEvent s_sizeEvent;

// STATIC: 0x00D3654C
static CGxShader* s_pixelShader;

// STATIC: 0x00D36550
static CGxShader* s_vertexShader;

// STATIC: 0x00D36554
static uint32_t s_lastUpdateTime;

// STATIC: 0x00D36558
static uint32_t s_nextPingTime;

// STATIC: #0x00B2FE74
static CTexture* s_dynamicElementsTexture;

// STATIC: #0x00B2FE78
static CTexture* s_dynamicBackgroundTextures[12];

// STATIC: #0x00B2FEA8
static CTexture* s_simpleBackgroundTexture;

// STATIC: #0x00B2FEAC
static float s_progress;

// STATIC: #0x00B2FEB0
static float s_worldProgress;

// STATIC: #0x00B2FEB4
static float s_asyncProgress;

// STATIC: #0x00B2FEB8
static float s_xmlProgress;

// STATIC: #0x00B2FEBC
static bool s_loadingWorld;

// STATIC: #0x00B2FEC0
static HLAYER s_miniLoadingLayer;

// STATIC: #0x00B2FEC4
static CTexture* unk_B2FEC4[2];

// STATIC: #0x00B2FECC
static CTexture* s_textures[2];

// STATIC: 0x00D365C4
static HLAYER s_loadingScreenLayer;

// STATIC: #0x00B2FED8
static bool s_usingWideScreen;

// STATIC: 0x00D365D4
static int32_t s_backgroundLocal;

// STATIC: #0x00B2FED9
static bool s_raceDataTip;

// STATIC: #0x00B2FEDC
static float s_spellProgress;

// STATIC: #0x00B2FEE0
static HTEXTFONT s_textFont;

// STATIC: #0x00B2FEE4
static CGxString* s_tipStr;

// STATIC: 0x00D365E8
static const char* s_gameTip;

// STATIC: #0x00B2FEEC
static CGxStringBatch* s_batch;

// STATIC: 0x00D365F0
static int32_t s_sizeEventPosted;

// STATIC: 0x00D365F8
static C3Vector s_tileVertices[12][4];

// STATIC: 0x00D36838
static C2Vector s_tileTexCoords[12][4];

// STATIC: #0x00B302C0
static TSGrowableArray<DYNAMICELEMENTVERT> s_dynamicElements;

// STATIC: 0x00D369C8
static CLoadingScreenShipInfo<TaxiPathNodeRec> s_shipInfo;

// STATIC: 0x00D369E8
static char s_raceDataTipText[260];

// OFFSET: 0x0040AB80/0x14000D460
static void BuildTiles() {
    for (int32_t row = 0; row < 3; row++) {
        for (int32_t col = 0; col < 4; col++) {
            int32_t left = col * 256;
            int32_t top = row * 256;
            int32_t right = left + 256;
            int32_t bottom = top + 256;

            if (right > 1002) {
                right = 1002;
            }

            if (bottom > 668) {
                bottom = 668;
            }

            int32_t tile = row * 4 + col;
            s_tileVertices[tile][0] = C3Vector(left * (1.0f / 1002.0f), 1.0f - bottom * (1.0f / 668.0f), 0.0f);
            s_tileVertices[tile][1] = C3Vector(right * (1.0f / 1002.0f), 1.0f - bottom * (1.0f / 668.0f), 0.0f);
            s_tileVertices[tile][2] = C3Vector(left * (1.0f / 1002.0f), 1.0f - top * (1.0f / 668.0f), 0.0f);
            s_tileVertices[tile][3] = C3Vector(right * (1.0f / 1002.0f), 1.0f - top * (1.0f / 668.0f), 0.0f);

            float u = 1.0f;
            float v = 1.0f;

            if (col == 3) {
                u = 234.0f / 256.0f;
            }

            if (row == 2) {
                v = 156.0f / 256.0f;
            }

            s_tileTexCoords[tile][0] = C2Vector(0.0f, v);
            s_tileTexCoords[tile][1] = C2Vector(u, v);
            s_tileTexCoords[tile][2] = C2Vector(0.0f, 0.0f);
            s_tileTexCoords[tile][3] = C2Vector(u, 0.0f);
        }
    }
}

// OFFSET: 0x0040AE40/0x14000D790
void LoadingScreenInitialize() {
    s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "UI");
    s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "UI");
    BuildTiles();
}

// OFFSET: 0x0040DD70
void LoadingScreenInitializeInGame() {
    s_shipInfo.Initialize(&g_taxiPathNodeDB, g_taxiPathDB.m_maxID + 1);
}

// PROVEN: b1ef4bb2 IDENTICAL/NONE
// OFFSET: 0x00409520/0x14000BC20
static int32_t SizeEvent(const void* data, void* param) {
    memcpy(&s_sizeEvent, data, sizeof(s_sizeEvent));
    s_sizeEventPosted = 1;
    return 0;
}

// PROVEN: e96252b2 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
static int32_t SwallowEvent(const void* data, void* param) {
    return 0;
}

// OFFSET: 0x00409440
static void UpdateProgressValue() {
    float xmlProgress = 0.0f;
    float worldScale = 1.0f;

    if (s_xmlProgress != 0.0f) {
        xmlProgress = s_xmlProgress;

        if (s_loadingWorld) {
            worldScale = 0.5f;
            xmlProgress = 0.5f * s_xmlProgress;
        }
    }

    s_progress = xmlProgress + s_asyncProgress * 0.0f + worldScale * s_worldProgress;

    if (s_progress < 0.0f) {
        s_progress = 0.0f;
    } else if (s_progress >= 1.0f) {
        s_progress = 1.0f;
    }

    if (SFile::IsTrial() && s_raceDataTip) {
        if (StreamingIsRaceDataReady(StreamingGetRace())) {
            s_progress = s_progress * 0.3f + 0.7f;
        } else {
            s_progress = s_spellProgress * 0.7f;
        }
    }
}

// OFFSET: 0x00409570
static void InitializeProgressBarEvents() {
    EventRegisterEx(EVENT_ID_CHAR, SwallowEvent, NULL, 8.0f);
    EventRegisterEx(EVENT_ID_IME, SwallowEvent, NULL, 8.0f);
    EventRegisterEx(EVENT_ID_KEYDOWN, SwallowEvent, NULL, 8.0f);
    EventRegisterEx(EVENT_ID_KEYDOWN_REPEATING, SwallowEvent, NULL, 8.0f);
    EventRegisterEx(EVENT_ID_MOUSEDOWN, SwallowEvent, NULL, 8.0f);
    EventRegisterEx(EVENT_ID_MOUSEMOVE, SwallowEvent, NULL, 8.0f);
    EventRegisterEx(EVENT_ID_SIZE, SizeEvent, NULL, 8.0f);
    s_sizeEventPosted = 0;
}

// PROVEN: 268e3a50 IDENTICAL/NONE
// OFFSET: 0x004096C0/0x14000BDE0
void LoadingScreenSetTip(const char* tip) {
    s_gameTip = tip;
}

// OFFSET: 0x0040AD90/0x14000D6C0
void LoadingScreenSetScreenTipForRaceData() {
    SStrCopy(s_raceDataTipText, FrameScript_GetText("TRIAL_LOADING_MESSAGE", -1, GENDER_NOT_APPLICABLE), sizeof(s_raceDataTipText));
    s_gameTip = s_raceDataTipText;
}

// PROVEN: 01c1912d IDENTICAL/NONE
// OFFSET: 0x00409740/0x14000BE50
bool LoadingScreenDrawing() {
    return s_loadingScreenLayer != NULL;
}

// OFFSET: 0x00409750
void LoadingScreenSetTransparent(bool transparent) {
    if (!s_loadingScreenLayer) {
        return;
    }

    uint32_t flags = ScrnLayerGetFlags(s_loadingScreenLayer);

    if (transparent) {
        ScrnLayerSetFlags(s_loadingScreenLayer, flags | 0x1);
    } else {
        ScrnLayerSetFlags(s_loadingScreenLayer, flags & ~0x1u);
    }
}

// OFFSET: #0x00408520
static void ProgressBarSendKeepAlive(uint32_t time) {
    if (static_cast<int32_t>(time - s_nextPingTime) < 0) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_KEEP_ALIVE));
    msg.Finalize();
    ClientServices::Send2(&msg);
    s_nextPingTime = time + 30000;
}

// OFFSET: 0x0040BAC0
static bool PaintSimpleBackground() {
    if (!s_simpleBackgroundTexture) {
        return false;
    }

    static C3Vector vertices[4] = {
        C3Vector(0.0f, 0.0f, 0.0f),
        C3Vector(1.0f, 0.0f, 0.0f),
        C3Vector(0.0f, 1.0f, 0.0f),
        C3Vector(1.0f, 1.0f, 0.0f),
    };

    CImVector color;
    *reinterpret_cast<uint32_t*>(&color) = 0xFFFFFFFF;

    GxPrimLockVertexPtrs(4, vertices, sizeof(C3Vector), NULL, 0, &color, 0, NULL, 0, s_texCoord, sizeof(C2Vector), NULL, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTexBlocking(s_simpleBackgroundTexture));
    GxDrawLockedElements(GxPrim_TriangleStrip, 4, s_indices);
    GxPrimUnlockVertexPtrs();
    GxuFontRenderBatch(s_batch);

    return true;
}

// OFFSET: 0x0040B300
static bool PaintBackgroundImage() {
    if (!s_dynamicElements.Count()) {
        return false;
    }

    CImVector color;
    *reinterpret_cast<uint32_t*>(&color) = 0xFFFFFFFF;

    for (uint32_t i = 0; i < 12; i++) {
        GxPrimLockVertexPtrs(4, s_tileVertices[i], sizeof(C3Vector), NULL, 0, &color, 0, NULL, 0, s_tileTexCoords[i], sizeof(C2Vector), NULL, 0);
        g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTexBlocking(s_dynamicBackgroundTextures[i]));
        GxDrawLockedElements(GxPrim_TriangleStrip, 4, s_indices);
        GxPrimUnlockVertexPtrs();
    }

    PaintDynamicLoadingBar();
    return true;
}

// OFFSET: 0x0040B580
static void PaintLoadingBar(const BackgroundImage* images, int32_t count, CTexture** textures) {
    for (int32_t i = 0; i < count; i++) {
        if (!textures[i]) {
            continue;
        }

        GxRsSet(GxRs_BlendingMode, 2);
        GxUpdateAlphaRef(GxRs_AlphaRef);

        const BackgroundImage& image = images[i];
        C3Vector vertices[4];
        vertices[0].x = image.m_x - image.m_width * 0.5f;
        vertices[2].x = vertices[0].x;
        vertices[0].y = image.m_y - image.m_height * 0.5f;
        vertices[1].y = vertices[0].y;
        vertices[1].x = image.m_width * 0.5f + image.m_x;
        vertices[3].x = vertices[1].x;
        vertices[2].y = image.m_height * 0.5f + image.m_y;
        vertices[3].y = vertices[2].y;

        if (image.m_fill) {
            vertices[1].x = vertices[0].x + image.m_width * s_progress;
            vertices[3].x = vertices[1].x;
        }

        vertices[0].z = 0.0f;
        vertices[1].z = 0.0f;
        vertices[2].z = 0.0f;
        vertices[3].z = 0.0f;

        CImVector color(0xFF, 0xFF, 0xFF, 0xFF);

        GxPrimLockVertexPtrs(4, vertices, sizeof(C3Vector), NULL, 0, &color, 0, NULL, 0, s_texCoord, sizeof(C2Vector), NULL, 0);
        g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTexBlocking(textures[i]));
        GxDrawLockedElements(GxPrim_TriangleStrip, 4, s_indices);
        GxPrimUnlockVertexPtrs();
    }
}

// OFFSET: 0x0040A3B0/0x14000CB90
bool LoadingScreenGetBackgroundTexturePath(int32_t mapID, char* path, uint32_t size, bool* isWide) {
    LoadingScreensRec* screenRec = g_loadingScreensDB.GetRecord(mapID);

    if (!screenRec) {
        return false;
    }

    if (size) {
        SStrCopy(path, screenRec->m_fileName, size);
    }

    if (!screenRec->m_hasWideScreen || CoordinateGetAspectRatio() <= s_normalAspect + 0.001f) {
        return SFile::FileExists(screenRec->m_fileName) != 0;
    }

    const char* extension = strrchr(screenRec->m_fileName, '.');

    if (!extension) {
        return SFile::FileExists(screenRec->m_fileName) != 0;
    }

    path[extension - screenRec->m_fileName] = 0;
    SStrPack(path, "Wide", size);
    SStrPack(path, extension, size);

    if (!SFile::FileExists(path)) {
        return SFile::FileExists(screenRec->m_fileName) != 0;
    }

    if (isWide) {
        *isWide = true;
    }

    return true;
}

// OFFSET: 0x0040C750
static bool LoadSimpleBackgroundTexture() {
    s_backgroundLocal = 1;
    bool result = false;
    char path[260];

    if (LoadingScreenGetBackgroundTexturePath(s_simpleMapID, path, sizeof(path), &s_usingWideScreen)) {
        if (SFile::IsStreamingMode() && SFile::FileIsLocal(path) != 1) {
            s_backgroundLocal = 0;
            s_simpleMapID = 0;
            AsyncFileReadCreateObject(path, 5, 0, 0, NULL, NULL, NULL, AsyncFileReadDestroyObject, AsyncFileReadDestroyObject, 0, 0, 0);
            SStrCopy(path, "Interface\\Glues\\loading", sizeof(path));
        }
    } else {
        s_backgroundLocal = 0;
        s_simpleMapID = 0;
        SStrCopy(path, "Interface\\Glues\\loading", sizeof(path));
    }

    CStatus status;
    CGxTexFlags flags;
    s_simpleBackgroundTexture = TextureCreate(path, flags, &status, 1, NULL, 2);

    if (s_simpleBackgroundTexture && s_backgroundLocal) {
        result = true;
    }

    return result;
}

// OFFSET: 0x0040BEE0
int32_t IsStillLoading() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetTransportGUID()) {
        return 1;
    }

    CGGameObject_C* transport = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(player->GetTransportGUID()), TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

    if (transport) {
        uintptr_t object = transport->m_typeData->GetMapObjectTransportID();

        if (!object || !World::ObjectIsDrawable(object)) {
            return 1;
        }
    }

    LoadingScreenDisable();
    return 0;
}

// OFFSET: 0x0040CB30
static void LoadingScreenPaint(void* param, const RECTF* rect, const RECTF* visible, float elapsedSec) {
    if (!GxHasContext() || !GxIsWindowVisible() || (s_progress > 0.99f && !IsStillLoading())) {
        return;
    }

    if (!s_simpleBackgroundTexture && s_simpleMapID != -1 && !LoadSimpleBackgroundTexture()) {
        s_simpleMapID = -1;
    }

    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);

    GxXformSetViewport(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);

    CImVector black;
    *reinterpret_cast<uint32_t*>(&black) = 0xFF000000;
    GxSceneClear(1, black);

    float aspect = CoordinateGetAspectCompensation();
    float ratio = s_usingWideScreen ? s_wideAspect / s_normalAspect : 1.0f;
    float scale = aspect / ratio;

    float left = 0.0f;
    float width = 1.0f;
    float bottom = 0.0f;
    float height = 1.0f;

    if (scale > 1.0f) {
        width = 1.0f / scale;
        left = (1.0f - width) * 0.5f;
    } else if (scale < 1.0f) {
        height = scale;
        bottom = (1.0f - scale) * 0.5f;
    }

    GxXformSetViewport(left, width + left, bottom, height + bottom, 0.0f, 1.0f);

    C44Matrix view;
    C44Matrix projection;
    GxuXformCreateOrtho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 500.0f, projection);
    g_theGxDevicePtr->XformSetView(view);
    g_theGxDevicePtr->XformSetProjection(projection);

    g_theGxDevicePtr->RsPush();

    C44Matrix transform;

    if (s_vertexShader && s_vertexShader->m_permutes[0].Valid() && s_pixelShader && s_pixelShader->m_permutes[0].Valid()) {
        GxRsSet(GxRs_VertexShader, s_vertexShader, 0);
        GxRsSet(GxRs_PixelShader, s_pixelShader, 0);
        GxXformViewProjNativeTranspose(transform);
        GxShaderConstantsSet(GxSh_Vertex, 0, &transform.a0, 4);
    } else {
        GxRsSet(GxRs_Lighting, 0);
    }

    GxRsSet(GxRs_Fog, 0);

    if (!PaintBackgroundImage()) {
        PaintSimpleBackground();

        if (s_vertexShader && s_vertexShader->m_permutes[0].Valid() && GxShaderValid(s_pixelShader, 0)) {
            GxShaderConstantsSet(GxSh_Vertex, 0, &transform.a0, 4);
        }

        PaintLoadingBar(s_textureInfo, 2, s_textures);
    }

    g_theGxDevicePtr->RsPop();
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

// OFFSET: 0x0040D280
static void UpdateProgressBar(int32_t force) {
    uint32_t time = OsGetAsyncTimeMs();
    EventInputProcess(EventGetCurrentContext());

    if (!force && time - s_lastUpdateTime < 250) {
        return;
    }

    s_lastUpdateTime = time;
    SE3::HeartBeat(NULL, NULL);
    ProgressBarSendKeepAlive(time);
    UpdateProgressValue();
    LoadingScreenPaint(NULL, NULL, NULL, 0.0f);
    GxScenePresent(NULL);
}

// OFFSET: 0x0040D2F0
static void InitializeProgressBar(bool loadingWorld) {
    if (s_loadingScreenLayer) {
        ScrnLayerSetFlags(s_loadingScreenLayer, ScrnLayerGetFlags(s_loadingScreenLayer) & ~0x1u);
    }

    s_xmlProgress = 0.0f;
    s_asyncProgress = 0.0f;
    s_worldProgress = 0.0f;
    s_loadingWorld = loadingWorld;

    if (!s_loadingScreenLayer) {
        CStatus status;

        for (uint32_t i = 0; i < sizeof(s_textures) / sizeof(s_textures[0]); i++) {
            if (!s_textures[i]) {
                CGxTexFlags flags;
                s_textures[i] = TextureCreate(s_textureInfo[i].m_path, flags, &status, 0, NULL, 2);
            }
        }

        RECTF rect;
        rect.bottom = 0.0f;
        rect.left = 0.0f;
        rect.top = 1.0f;
        rect.right = 1.0f;
        ScrnLayerCreate(&rect, 9.0f, 6, NULL, LoadingScreenPaint, &s_loadingScreenLayer);
        InitializeProgressBarEvents();
    }

    s_nextPingTime = OsGetAsyncTimeMs() + 30000;

    uint32_t time = OsGetAsyncTimeMs();
    EventInputProcess(EventGetCurrentContext());

    if (time - s_lastUpdateTime >= 250) {
        s_lastUpdateTime = time;
        SE3::HeartBeat(NULL, NULL);
        ProgressBarSendKeepAlive(time);
        UpdateProgressValue();
        LoadingScreenPaint(NULL, NULL, NULL, 0.0f);
        GxScenePresent(NULL);
    }
}

// OFFSET: #0x0040AB20
void LoadingScreenSpellUpdateProgress(float progress) {
    if (progress <= s_spellProgress) {
        return;
    }

    s_spellProgress = progress;
    UpdateProgressBar(1.0f == progress);
}

// OFFSET: 0x0040D490
void LoadingScreenEnable(int32_t mapID, bool loadingWorld) {
    CGGameUI::SetAspect();

    if (s_gameTip) {
        s_textFont = TextBlockGenerateFont(s_fontNames[s_locale], 1, NDCToDDCHeight(0.018f));
        CGxFont* font = TextBlockGetFontPtr(s_textFont);

        float width = DDCToNDCWidth(NDCToDDCWidth(515.0f / (CoordinateGetAspectCompensation() * 1024.0f)));
        float compensation = CoordinateGetAspectCompensation();
        float offset = compensation >= 1.0f ? 0.0f : (1.0f - compensation) * 0.5f;

        C3Vector position;
        position.x = 0.5f - width * 0.5f;
        position.y = offset + 0.1f;
        position.z = 1.0f;

        char* tip = static_cast<char*>(alloca(SStrLen(s_gameTip) + 1));
        memcpy(tip, s_gameTip, SStrLen(s_gameTip) + 1);

        for (char* end = &tip[SStrLen(tip) - 1]; *end == '\r' || *end == '\n'; end--) {
            *end = '\0';
        }

        CImVector color;
        *reinterpret_cast<uint32_t*>(&color) = 0xD7C8C8C8;
        GxuFontCreateString(font, tip, 0.018f, position, width, 1.0f, 0.005f, s_tipStr, GxVJ_Bottom, GxHJ_Left, 0, color, 0.0f, 1.0f, 1.0f, 0.0f);

        C2Vector shadowOffset;
        shadowOffset.x = 0.001f;
        shadowOffset.y = -0.001f;

        CImVector shadowColor(0, 0, 0, 0xFF);
        GxuFontAddShadow(s_tipStr, shadowColor, shadowOffset);

        s_batch = GxuFontCreateBatch(false, false, false);
        GxuFontAddToBatch(s_batch, s_tipStr);
    }

    s_simpleMapID = mapID;
    InitializeProgressBar(loadingWorld);
}

// OFFSET: 0x0040D940
void LoadingScreenAsyncCallback(float progress, void* param) {
    if (progress <= s_asyncProgress) {
        return;
    }

    s_asyncProgress = progress;
    UpdateProgressBar(1.0f == progress);
}

// OFFSET: 0x0040D990
void LoadingScreenWorldCallback(float progress, void* param) {
    if (progress <= s_worldProgress) {
        return;
    }

    s_worldProgress = progress;
    UpdateProgressBar(1.0f == progress);
}

// OFFSET: #0x0040AF90
void LoadingScreenXMLCallback(float progress, void* param) {
    if (progress <= s_xmlProgress) {
        return;
    }

    s_xmlProgress = progress;
    UpdateProgressBar(1.0f == progress);
}

// OFFSET: 0x00409640
static void LoadingScreenDisableEvents() {
    EventUnregister(EVENT_ID_CHAR, SwallowEvent);
    EventUnregister(EVENT_ID_IME, SwallowEvent);
    EventUnregister(EVENT_ID_KEYDOWN, SwallowEvent);
    EventUnregister(EVENT_ID_KEYDOWN_REPEATING, SwallowEvent);
    EventUnregister(EVENT_ID_MOUSEDOWN, SwallowEvent);
    EventUnregister(EVENT_ID_MOUSEMOVE, SwallowEvent);
    EventUnregister(EVENT_ID_SIZE, SizeEvent);

    if (s_sizeEventPosted) {
        EventQueuePost(EventGetCurrentContext(), EVENT_ID_SIZE, &s_sizeEvent, sizeof(s_sizeEvent));
    }
}

// OFFSET: 0x0040BA40
static void ClearDynamicData() {
    if (s_dynamicElementsTexture) {
        TextureRelease(s_dynamicElementsTexture);
    }

    s_dynamicElementsTexture = NULL;

    for (uint32_t i = 0; i < sizeof(s_dynamicBackgroundTextures) / sizeof(s_dynamicBackgroundTextures[0]); i++) {
        if (s_dynamicBackgroundTextures[i]) {
            TextureRelease(s_dynamicBackgroundTextures[i]);
        }

        s_dynamicBackgroundTextures[i] = NULL;
    }

    s_dynamicElements.Clear();
}

// OFFSET: 0x0040BBE0/0x14000EA90
void LoadingScreenDisable() {
    if (s_loadingScreenLayer) {
        HandleClose(reinterpret_cast<HOBJECT>(s_loadingScreenLayer));
    }

    s_loadingScreenLayer = NULL;

    for (uint32_t i = 0; i < sizeof(s_textures) / sizeof(s_textures[0]); i++) {
        if (s_textures[i]) {
            TextureRelease(s_textures[i]);
        }

        s_textures[i] = NULL;
    }

    if (s_simpleBackgroundTexture) {
        TextureRelease(s_simpleBackgroundTexture);
    }

    s_simpleBackgroundTexture = NULL;

    LoadingScreenDisableEvents();
    ClearDynamicData();

    s_gameTip = NULL;

    if (s_batch) {
        GxuFontDestroyBatch(s_batch);
    }

    s_batch = NULL;

    if (s_tipStr) {
        GxuFontDestroyString(s_tipStr);
    }

    s_tipStr = NULL;

    if (s_textFont) {
        HandleClose(reinterpret_cast<HOBJECT>(s_textFont));
    }

    s_textFont = NULL;

    SI3::StopAllMusic();
    SI3::StopZoneAmbience();

    if (SFile::IsStreamingMode()) {
        CGSpellBook::MakeKnownSpellModelsLocal();
    }
}

// OFFSET: 0x0040BD20
void LoadingScreenShutdown() {
    LoadingScreenDisable();

    if (s_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(s_vertexShader);
        s_vertexShader = NULL;
    }

    if (s_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(s_pixelShader);
        s_pixelShader = NULL;
    }
}

// OFFSET: 0x004097A0
void LoadingScreenMiniDisable() {
    if (s_miniLoadingLayer) {
        HandleClose(reinterpret_cast<HOBJECT>(s_miniLoadingLayer));
    }

    s_miniLoadingLayer = NULL;

    for (uint32_t i = 0; i < sizeof(unk_B2FEC4) / sizeof(unk_B2FEC4[0]); i++) {
        if (unk_B2FEC4[i]) {
            TextureRelease(unk_B2FEC4[i]);
        }

        unk_B2FEC4[i] = NULL;
    }
}

// OFFSET: 0x0040D120
void LoadingScreenMiniUpdate(float progress) {
    LoadingScreenMiniEnable();
    s_progress = progress;
}

// OFFSET: 0x0040CA70
bool LoadDynamicBackground() {
    CStatus status;

    if (!s_dynamicElementsTexture) {
        CGxTexFlags flags;
        s_dynamicElementsTexture = TextureCreate("Interface\\Glues\\LoadingScreens\\DynamicElements", flags, &status, 1, NULL, 2);

        if (!s_dynamicElementsTexture) {
            return false;
        }
    }

    for (uint32_t i = 0; i < 12; i++) {
        char fileName[260];
        SStrPrintf(fileName, sizeof(fileName), "Interface\\WorldMap\\World\\World%d", i + 1);

        CGxTexFlags flags;
        s_dynamicBackgroundTextures[i] = TextureCreate(fileName, flags, &status, 1, NULL, 2);

        if (!s_dynamicBackgroundTextures[i]) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x0040C260
static void LoadingScreenMiniPaint(void* param, const RECTF* rect, const RECTF* visible, float elapsedSec) {
    if (!GxHasContext() || !GxIsWindowVisible()) {
        return;
    }

    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);

    GxXformSetViewport(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);

    float aspect = CoordinateGetAspectCompensation();
    float ratio = s_usingWideScreen ? s_wideAspect / s_normalAspect : 1.0f;
    float scale = aspect / ratio;

    float left = 0.0f;
    float width = 1.0f;
    float bottom = 0.0f;
    float height = 1.0f;

    if (scale > 1.0f) {
        width = 1.0f / scale;
        left = (1.0f - width) * 0.5f;
    } else if (scale < 1.0f) {
        height = scale;
        bottom = (1.0f - scale) * 0.5f;
    }

    GxXformSetViewport(left, width + left, bottom, height + bottom, 0.0f, 1.0f);

    C44Matrix view;
    C44Matrix projection;
    GxuXformCreateOrtho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 500.0f, projection);
    g_theGxDevicePtr->XformSetView(view);
    g_theGxDevicePtr->XformSetProjection(projection);

    g_theGxDevicePtr->RsPush();

    C44Matrix transform;

    if (s_vertexShader && s_vertexShader->m_permutes[0].Valid() && s_pixelShader && s_pixelShader->m_permutes[0].Valid()) {
        GxRsSet(GxRs_VertexShader, s_vertexShader, 0);
        GxRsSet(GxRs_PixelShader, s_pixelShader, 0);
        GxXformViewProjNativeTranspose(transform);
        GxShaderConstantsSet(GxSh_Vertex, 0, &transform.a0, 4);
    } else {
        GxRsSet(GxRs_Lighting, 0);
    }

    GxRsSet(GxRs_Fog, 0);
    PaintLoadingBar(s_miniTextureInfo, 2, unk_B2FEC4);
    g_theGxDevicePtr->RsPop();
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

// OFFSET: 0x0040D040
void LoadingScreenMiniEnable() {
    if (s_miniLoadingLayer) {
        return;
    }

    CStatus status;

    for (uint32_t i = 0; i < sizeof(unk_B2FEC4) / sizeof(unk_B2FEC4[0]); i++) {
        if (!unk_B2FEC4[i]) {
            CGxTexFlags flags;
            unk_B2FEC4[i] = TextureCreate(s_miniTextureInfo[i].m_path, flags, &status, 0, NULL, 2);
        }
    }

    RECTF rect;
    rect.bottom = 0.0f;
    rect.left = 0.0f;
    rect.top = 1.0f;
    rect.right = 1.0f;
    ScrnLayerCreate(&rect, 9.0f, 3, NULL, LoadingScreenMiniPaint, &s_miniLoadingLayer);
    s_progress = 0.5f;
}

// OFFSET: 0x0040A560
static void BuildEndpointVertices(const C3Vector& start, const C3Vector& end) {
    s_dynamicElements.m_data[0].m_position = C3Vector(start.x - 0.02f, start.y - 0.0265f, 0.0f);
    s_dynamicElements.m_data[0].m_texCoord.x = 0.5f;
    s_dynamicElements.m_data[0].m_texCoord.y = 0.5f;
    s_dynamicElements.m_data[0].m_color = s_pathColor;

    s_dynamicElements.m_data[1].m_position = C3Vector(start.x + 0.02f, start.y - 0.0265f, 0.0f);
    s_dynamicElements.m_data[1].m_texCoord.x = 1.0f;
    s_dynamicElements.m_data[1].m_texCoord.y = 0.5f;
    s_dynamicElements.m_data[1].m_color = s_pathColor;

    s_dynamicElements.m_data[2].m_position = C3Vector(start.x - 0.02f, start.y + 0.0265f, 0.0f);
    s_dynamicElements.m_data[2].m_texCoord.x = 0.5f;
    s_dynamicElements.m_data[2].m_texCoord.y = 0.0f;
    s_dynamicElements.m_data[2].m_color = s_pathColor;

    s_dynamicElements.m_data[3].m_position = C3Vector(start.x + 0.02f, start.y + 0.0265f, 0.0f);
    s_dynamicElements.m_data[3].m_texCoord.x = 1.0f;
    s_dynamicElements.m_data[3].m_texCoord.y = 0.0f;
    s_dynamicElements.m_data[3].m_color = s_pathColor;

    s_dynamicElements.m_data[4].m_position = C3Vector(end.x - 0.02f, end.y - 0.0265f, 0.0f);
    s_dynamicElements.m_data[4].m_texCoord.x = 0.0f;
    s_dynamicElements.m_data[4].m_texCoord.y = 0.5f;
    s_dynamicElements.m_data[4].m_color = s_pathColor;

    s_dynamicElements.m_data[5].m_position = C3Vector(end.x + 0.02f, end.y - 0.0265f, 0.0f);
    s_dynamicElements.m_data[5].m_texCoord.x = 0.5f;
    s_dynamicElements.m_data[5].m_texCoord.y = 0.5f;
    s_dynamicElements.m_data[5].m_color = s_pathColor;

    s_dynamicElements.m_data[6].m_position = C3Vector(end.x - 0.02f, end.y + 0.0265f, 0.0f);
    s_dynamicElements.m_data[6].m_texCoord.x = 0.0f;
    s_dynamicElements.m_data[6].m_texCoord.y = 0.0f;
    s_dynamicElements.m_data[6].m_color = s_pathColor;

    s_dynamicElements.m_data[7].m_position = C3Vector(end.x + 0.02f, end.y + 0.0265f, 0.0f);
    s_dynamicElements.m_data[7].m_texCoord.x = 0.5f;
    s_dynamicElements.m_data[7].m_texCoord.y = 0.0f;
    s_dynamicElements.m_data[7].m_color = s_pathColor;
}

// OFFSET: 0x0040A910
static void BuildPathVertices(C3Spline* spline, int32_t segments) {
    float t = 0.0f;
    float step = 1.0f / segments;
    DYNAMICELEMENTVERT* vertex = &s_dynamicElements.m_data[8];

    for (int32_t i = 0; i < segments + 1; i++) {
        t = t < 0.0f ? 0.0f : (1.0f <= t ? 1.0f : t);

        C3Vector position(t, t, t);
        spline->Pos(t, position, C3Spline::EVAL_ARCLENGTH);

        for (uint32_t j = 0; j < 4; j++) {
            vertex[j].m_position = s_pathOffsets[j] + position;
            vertex[j].m_texCoord = s_pathTexCoords[j];
        }

        vertex += 4;
        t += step;
    }
}

// OFFSET: #0x00408BB0
static LoadingScreenTaxiSplinesRec* FindTaxiSpline(int32_t pathID, int32_t legIndex) {
    for (int32_t i = 0; i < g_loadingScreenTaxiSplinesDB.m_numRecords; i++) {
        LoadingScreenTaxiSplinesRec* rec = g_loadingScreenTaxiSplinesDB.GetRecordByIndex(i);

        if (rec->m_pathID == pathID && rec->m_legIndex == legIndex) {
            return rec;
        }
    }

    return NULL;
}

// OFFSET: 0x0040B0F0
void PaintDynamicLoadingBar() {
    if (!s_dynamicElements.Count()) {
        return;
    }

    int32_t segments = (static_cast<int32_t>(s_dynamicElements.Count()) - 8) / 4;
    int32_t lit = 0;

    while (lit < segments) {
        DYNAMICELEMENTVERT* vertex = &s_dynamicElements.m_data[8 + lit * 4];
        vertex[0].m_color = s_pathColor;
        vertex[1].m_color = s_pathColor;
        vertex[2].m_color = s_pathColor;
        vertex[3].m_color = s_pathColor;

        if (static_cast<float>(lit) / static_cast<float>(segments - 1) > s_progress) {
            break;
        }

        lit++;
    }

    uint32_t indexCount = 6 * lit + 12;
    uint32_t vertexCount = 4 * lit + 8;
    uint16_t* indices = static_cast<uint16_t*>(alloca(indexCount * sizeof(uint16_t)));
    uint16_t* index = indices;

    for (int32_t i = 0; i < lit; i++) {
        uint16_t base = static_cast<uint16_t>(8 + i * 4);
        index[0] = base;
        index[1] = base + 1;
        index[2] = base + 2;
        index[3] = base + 1;
        index[4] = base + 3;
        index[5] = base + 2;
        index += 6;
    }

    index[0] = 0;
    index[1] = 1;
    index[2] = 2;
    index[3] = 1;
    index[4] = 3;
    index[5] = 2;
    index[6] = 4;
    index[7] = 5;
    index[8] = 6;
    index[9] = 5;
    index[10] = 7;
    index[11] = 6;

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTexBlocking(s_dynamicElementsTexture));
    GxRsSet(GxRs_BlendingMode, 2);
    GxUpdateAlphaRef(GxRs_AlphaRef);

    DYNAMICELEMENTVERT* vertices = s_dynamicElements.m_data;
    GxPrimLockVertexPtrs(vertexCount, &vertices->m_position, sizeof(DYNAMICELEMENTVERT), NULL, 0, &vertices->m_color, sizeof(DYNAMICELEMENTVERT), NULL, 0, &vertices->m_texCoord, sizeof(DYNAMICELEMENTVERT), NULL, 0);
    GxDrawLockedElements(GxPrim_Triangles, indexCount, indices);
    GxPrimUnlockVertexPtrs();
}

// OFFSET: 0x0040BF70
static bool InitializeSpline(uint32_t path, int32_t leg) {
    LoadingScreenTaxiSplinesRec* splineRec = FindTaxiSpline(path, leg);

    if (!splineRec) {
        return false;
    }

    int32_t legCount = path < s_shipInfo.m_paths.Count() ? s_shipInfo.m_paths.m_data[path].m_legs.Count() : 0;

    if (!legCount) {
        return false;
    }

    int32_t startNodeID;
    int32_t endNodeID = 0;
    int32_t nextStartNodeID = 0;

    if (!s_shipInfo.GetLeg(path, leg, &startNodeID, &endNodeID)) {
        return false;
    }

    if (!s_shipInfo.GetLeg(path, (leg + 1) % legCount, &nextStartNodeID, &startNodeID)) {
        return false;
    }

    TaxiPathNodeRec* startNode = g_taxiPathNodeDB.GetRecord(endNodeID);
    TaxiPathNodeRec* endNode = g_taxiPathNodeDB.GetRecord(nextStartNodeID);

    if (!startNode || !endNode) {
        return false;
    }

    C3Vector start;
    start.x = startNode->m_loc[0];
    start.y = startNode->m_loc[1];
    start.z = 0.0f;

    C3Vector end;
    end.x = endNode->m_loc[0];
    end.y = endNode->m_loc[1];
    end.z = 0.0f;

    if (!CGWorldMap::GetContinentPosition(start.x, start.y, startNode->m_continentID, &start.x, &start.y) || !CGWorldMap::GetContinentPosition(end.x, end.y, endNode->m_continentID, &end.x, &end.y)) {
        return false;
    }

    TSStackArray<C3Vector> points(static_cast<C3Vector*>(alloca(sizeof(C3Vector) * 12)), 12, 1);
    points.m_data[1] = start;
    points.m_data[0] = points.m_data[1];

    uint32_t count = 0;

    while (count < 8 && (splineRec->m_locx[count] > 0.0f || splineRec->m_locy[count] > 0.0f)) {
        points.m_data[count + 2].x = splineRec->m_locx[count];
        points.m_data[count + 2].y = splineRec->m_locy[count];
        points.m_data[count + 2].z = 0.0f;
        count++;
    }

    points.m_data[count + 3] = end;
    points.m_data[count + 2] = points.m_data[count + 3];

    C3Spline_CatmullRom spline;
    spline.SetPoints(points.m_data, count + 4);

    int32_t segments = FloatToInt64(spline.m_length * 66.666672f);

    if (segments < 1) {
        segments = 1;
    }

    s_dynamicElements.SetCount(4 * segments + 12);
    BuildEndpointVertices(start, end);
    BuildPathVertices(&spline, segments);

    return true;
}

// OFFSET: 0x0040D7A0
void LoadingScreenEnableShip(uint32_t transportEntry, int32_t transportMap, int32_t mapID) {
    s_simpleMapID = mapID;
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        CGGameObject_C* transport = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(player->GetTransportGUID()), TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

        if (transport && transport->m_obj->m_entryID == transportEntry) {
            uint32_t path = transport->GetPropertyValue(CGameObjectDef::GetPropNum(transport->m_gameObject->m_typeID, 35));
            uint32_t leg = s_shipInfo.FindLeg(&g_taxiPathNodeDB, path, transportMap);

            if (!InitializeSpline(path, leg) || !LoadDynamicBackground()) {
                ClearDynamicData();
                InitializeProgressBar(true);
                return;
            }

            s_simpleMapID = -1;
        }
    }

    InitializeProgressBar(true);
}

// OFFSET: 0x0040D880
void LoadingScreenEnableShipLeg(int32_t entryId, uint32_t leg) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        CGGameObject_C* transport = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(player->GetTransportGUID()), TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

        if (transport && transport->m_obj->m_entryID == entryId) {
            uint32_t path = transport->GetPropertyValue(CGameObjectDef::GetPropNum(transport->m_gameObject->m_typeID, 35));

            if (!InitializeSpline(path, leg) || !LoadDynamicBackground()) {
                ClearDynamicData();
                InitializeProgressBar(true);
                return;
            }

            s_simpleMapID = -1;
        }
    }

    InitializeProgressBar(true);
}
