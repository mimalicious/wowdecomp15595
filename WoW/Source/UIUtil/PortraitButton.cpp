#include "PortraitButton.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "Engine/Source/Services/Camera.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/BLPFile/blp.h"
#include "Engine/Source/Images/tga.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoExtraRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include <cstring>
#include "Engine/Source/Base/Coordinate.h"

// STATIC: 0x00D3BFD0
static CTexture* s_portraitMaskTexture;

// STATIC: 0x00D3BFD4
static CGxTex* s_portraitMaskGxTex;

// STATIC: 0x00D3BFD8
static CGxFrameBuffer* s_portraitFrameBuffer;

// STATIC: 0x00D3BFDC
static CTexture* s_portraitDepthTexture;

// STATIC: 0x00D3BFE0
static int32_t s_rtAlphaSupported;

// STATIC: 0x00D3BFE4
static PortraitBase* s_playerPortraitCache;

// STATIC: 0x00D3BFE8
static PortraitBase* s_unitPortraitCache;

// STATIC: 0x00D3BFEC
static TSHashTable<PLAYERPORTRAIT, CHashKeyGUID> s_playerPortraits;

// STATIC: 0x00D3C018
static TSHashTable<UNITPORTRAIT, HASHKEY_NONE> s_unitPortraits;

// STATIC: 0x00D3C044
static TSHashTable<ITEMPORTRAIT, HASHKEY_STR> s_itemPortraits;

// STATIC: 0x00D3C070
static TSGrowableArray<CTexture*> s_freePortraitTextures;

// STATIC: 0x00C9A6F0
static TSList<PORTRAITREQUEST, TSGetLink<PORTRAITREQUEST>> s_pendingPortraits;

// STATIC: 0x00C9A6FC
static TSList<PORTRAITREQUEST, TSGetLink<PORTRAITREQUEST>> s_freePortraits;

// STATIC: 0x00C9CAC0
const char* s_sexNames[3] = { "Male", "Female", "NOSEX" };

// STATIC: 0x00BAA3B0
static const uint16_t s_portraitMaskIndices[] = { 0, 1, 2, 3 };

static void PortraitDepthTextureCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
}

// OFFSET: 0x00481B20
static void PortraitCacheTouch(PortraitBase** head, PortraitBase* entry) {
    if (entry == *head) {
        return;
    }

    if (entry->m_prev) {
        entry->m_prev->m_next = entry->m_next;
    }

    if (entry->m_next) {
        entry->m_next->m_prev = entry->m_prev;
    }

    entry->m_prev = NULL;
    entry->m_next = *head;

    if (*head) {
        (*head)->m_prev = entry;
    }

    *head = entry;
}

// OFFSET: 0x00481EB0
static int32_t IsPortraitDrawable(CGUnit_C* unit) {
    if (unit->m_clientFlags & 0x400000) {
        return 0;
    }

    if (unit->m_characterComponent && (!unit->m_characterComponent->RenderPrep(0) || (unit->m_characterComponent->m_flags & 4))) {
        return 0;
    }

    if (!unit->m_worldModel) {
        return 0;
    }

    return unit->m_worldModel->IsDrawableRecursive();
}

// OFFSET: 0x00481F00/0x1400A0C30
static void PortraitLightingCallback(CM2Model* model, CM2Lighting* lighting, void* arg) {
    C3Vector ambient(0.45f, 0.45f, 0.45f);
    lighting->AddAmbient(ambient);
    C3Vector direction(-1.0f, 0.0f, -1.0f);
    C3Vector diffuse(1.0f, 1.0f, 1.0f);
    lighting->AddDiffuse(diffuse, direction);
}

// OFFSET: 0x004820E0/0x1400A0ED0
int32_t RTAlphaSupported() {
    if (g_theGxDevicePtr->DeviceApi() == GxApi_D3d11) {
        return 1;
    }

    if (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl && !g_theGxDevicePtr->Caps().m_oglRtAlpha) {
        return 0;
    }

    int32_t supported = 0;
    GxSceneClear(0x1F, CImVector());

    CiRect rect;
    rect.minY = 0;
    rect.minX = 0;
    rect.maxY = 128;
    rect.maxX = 128;

    TSGrowableArray<CImVector> pixels;
    g_theGxDevicePtr->DeviceReadPixels(rect, pixels);

    for (uint32_t i = 0; i < pixels.Count(); i++) {
        if (pixels[i].a != 0xFF) {
            supported = 1;
            break;
        }
    }

    return supported;
}

// OFFSET: 0x004821B0
void UpdatePendingPortraits() {
    if (!g_theGxDevicePtr || !g_theGxDevicePtr->CapsIsWindowVisible(-1) || !g_theGxDevicePtr || !g_theGxDevicePtr->CapsHasContext(-1)) {
        return;
    }

    bool updated = false;
    PORTRAITREQUEST* next;

    for (PORTRAITREQUEST* request = s_pendingPortraits.Head(); request; request = next) {
        next = s_pendingPortraits.Next(request);
        WOWGUID guid = request->m_guid;
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit) {
            s_freePortraits.LinkToTail(request);
            continue;
        }

        if ((unit->m_clientFlags & 0x400000) && !unit->InitializeComponent()) {
            continue;
        }

        if (!IsPortraitDrawable(unit)) {
            continue;
        }

        uint32_t flags = request->m_flags;
        s_freePortraits.LinkToTail(request);

        if (flags & 2) {
            Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_PORTRAIT_UPDATE);
        }

        if (flags & 1) {
            Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_MODEL_CHANGED);
        }

        updated = true;
    }

    if (updated) {
        Script_SendUniqueSignal(SCRIPT_EVENT_UNIT_PORTRAIT_UPDATE);
    }
}

// OFFSET: 0x004822F0
static int32_t AddPendingPortraitFlags(CGUnit_C* unit, uint32_t flags) {
    for (PORTRAITREQUEST* request = s_pendingPortraits.Head(); request; request = s_pendingPortraits.Next(request)) {
        if (request->m_guid == unit->m_obj->m_guid) {
            request->m_flags |= flags;
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00482340
static void PurgePortraitCache(PortraitBase*& head, uint32_t maxEntries, int32_t force) {
    uint32_t index = 1;
    PortraitBase* next;

    for (PortraitBase* entry = head; entry; entry = next) {
        next = entry->m_next;

        if (index > maxEntries && (TextureGetRefCnt(entry->m_data.m_texture) == 1 || force)) {
            CGxTexFlags flags;
            TextureGetGxTexFlags(entry->m_data.m_texture, &flags);

            if (flags.m_renderTarget) {
                s_freePortraitTextures.New(entry->m_data.m_texture);
            } else {
                TextureRelease(entry->m_data.m_texture);
            }

            entry->m_data.m_texture = NULL;
            entry->m_dirty = 1;

            if (entry->m_next) {
                entry->m_next->m_prev = entry->m_prev;
            }

            if (entry->m_prev) {
                entry->m_prev->m_next = entry->m_next;
            }

            if (head == entry) {
                head = entry->m_next;
            }

            entry->m_prev = NULL;
            entry->m_next = NULL;
        }

        index++;
    }
}

// OFFSET: 0x00482520
void __fastcall UpdateAllPortraits() {
    s_rtAlphaSupported = RTAlphaSupported();

    for (PLAYERPORTRAIT* portrait = s_playerPortraits.Head(); portrait; portrait = s_playerPortraits.Next(portrait)) {
        portrait->m_cache.m_dirty = 1;
    }

    for (UNITPORTRAIT* portrait = s_unitPortraits.Head(); portrait; portrait = s_unitPortraits.Next(portrait)) {
        portrait->m_cache.m_dirty = 1;
    }

    Script_SendUniqueSignalToAllUnits(SCRIPT_EVENT_UNIT_PORTRAIT_UPDATE);
    Script_SendUniqueSignal(SCRIPT_EVENT_UNIT_PORTRAIT_UPDATE);
}

// OFFSET: 0x004825B0/0x1400A13A0
void UpdateModelForUI(const WOWGUID& guid, uint32_t flags) {
    if (flags & 2) {
        PLAYERPORTRAIT* portrait = s_playerPortraits.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

        if (portrait) {
            portrait->m_cache.m_dirty = 1;
        }
    }

    if (flags & 2) {
        Script_SendUniqueSignal(guid, SCRIPT_EVENT_UNIT_PORTRAIT_UPDATE);
    }

    if (flags & 1) {
        Script_SendUniqueSignal(guid, SCRIPT_EVENT_UNIT_MODEL_CHANGED);
    }
}

// OFFSET: 0x00482620
static int32_t QueuePortraitUpdate(CGUnit_C* unit, uint32_t flags) {
    if (g_theGxDevicePtr && g_theGxDevicePtr->CapsIsWindowVisible(-1) && g_theGxDevicePtr && g_theGxDevicePtr->CapsHasContext(-1) && IsPortraitDrawable(unit)) {
        return 0;
    }

    PORTRAITREQUEST* request = s_freePortraits.Head();

    if (!request) {
        void* memory = SMemAlloc(sizeof(PORTRAITREQUEST), __FILE__, __LINE__, 0);
        request = memory ? new (memory) PORTRAITREQUEST() : NULL;
    }

    s_pendingPortraits.LinkToTail(request);
    request->m_guid = unit->m_obj->m_guid;
    request->m_flags = flags;
    return 1;
}

// OFFSET: 0x004826D0/0x1400A1530
void SetPortraitTexture(CSimpleTexture* texture, uint32_t race, uint32_t sex, WOWGUID guid) {
    if (guid) {
        PLAYERPORTRAIT* portrait = s_playerPortraits.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

        if (portrait && portrait->m_cache.m_data.m_texture && !portrait->m_cache.m_dirty) {
            texture->SetTexture(portrait->m_cache.m_data.m_texture);
            return;
        }
    }

    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(race);
    char fileName[64];
    SStrPrintf(fileName, sizeof(fileName), "Interface\\CharacterFrame\\TemporaryPortrait-%s-%s", s_sexNames[sex], raceRec->m_clientFileString);
    texture->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
}

// OFFSET: 0x00482780/0x1400A1660
void SetPortraitTexture(CSimpleTexture* texture, int32_t displayId, const WOWGUID& petGuid) {
    HASHKEY_NONE key;
    UNITPORTRAIT* portrait = s_unitPortraits.Ptr(displayId, key);

    if (portrait && !portrait->m_cache.m_dirty) {
        texture->SetTexture(portrait->m_cache.m_data.m_texture);
        return;
    }

    if (petGuid) {
        CGPartyMemberState* state = CGGameUI::GetPartyMemberStateFromPet(petGuid);
        uint32_t seatId = state->m_vehicleSeatId;

        if (seatId) {
            VehicleSeatRec* seat = g_vehicleSeatDB.GetRecord(seatId);

            if (seat->m_uiSkin == 0) {
                texture->SetTexture("Interface\\CharacterFrame\\TemporaryPortrait-Vehicle-Organic", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
                return;
            }

            if (seat->m_uiSkin == 1) {
                texture->SetTexture("Interface\\CharacterFrame\\TemporaryPortrait-Vehicle-Mechanical", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
                return;
            }
        }
    }

    texture->SetTexture("Interface\\CharacterFrame\\TemporaryPortrait-Pet", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
}

// OFFSET: 0x00482870/0x1400A17B0
int32_t ReadyModelForUI(CGUnit_C* unit, uint32_t flags) {
    if (!unit || AddPendingPortraitFlags(unit, flags)) {
        return 0;
    }

    return QueuePortraitUpdate(unit, flags) == 0;
}

// OFFSET: 0x00482D70/0x1400A1E10
static TSFixedArray<uint8_t>* GetAlphaMask(uint32_t size) {
    // STATIC: 0x00D3C080
    static TSFixedArray<uint8_t> s_alphaMasks[2];
    TSFixedArray<uint8_t>* mask = &s_alphaMasks[size == 128];

    if (mask->m_count) {
        return mask;
    }

    mask->SetCount(size * size);
    int32_t loaded = 0;
    CTgaFile tga;
    const char* tgaName = NULL;

    if (size == 128) {
        tgaName = "Interface\\CharacterFrame\\TempPortraitAlphaMask.tga";
    } else if (size == 64) {
        tgaName = "Interface\\CharacterFrame\\TempPortraitAlphaMaskSmall.tga";
    }

    if (tga.Open(tgaName, 1) && tga.LoadImageData(0)) {
        memcpy(mask->m_data, tga.Image(), mask->m_count);
    } else {
        CBLPFile blp;
        const char* blpName = NULL;

        if (size == 128) {
            blpName = "Interface\\CharacterFrame\\TempPortraitAlphaMask.blp";
        } else if (size == 64) {
            blpName = "Interface\\CharacterFrame\\TempPortraitAlphaMaskSmall.blp";
        }

        if (blp.Open(blpName, 1)) {
            uint8_t* pixels;
            uint32_t stride;

            if (blp.Lock(PIXEL_ARGB8888, 0, pixels, stride)) {
                for (uint32_t i = 0; i < mask->m_count; i++) {
                    mask->m_data[i] = pixels[4 * i];
                }

                loaded = 1;
            }

            blp.Unlock(0);
            blp.Close();
        }

        blp.Close();

        if (!loaded) {
            memset(mask->m_data, mask->m_count, 0xFF);
        }
    }

    tga.Close();
    return mask;
}

// OFFSET: 0x00482F50
void PortraitShutdown() {
    s_portraitMaskGxTex = NULL;

    if (s_portraitMaskTexture) {
        TextureRelease(s_portraitMaskTexture);
        s_portraitMaskTexture = NULL;
    }

    if (s_portraitDepthTexture) {
        TextureRelease(s_portraitDepthTexture);
        s_portraitDepthTexture = NULL;
    }

    if (s_portraitFrameBuffer) {
        g_theGxDevicePtr->FrameBufferDelete(s_portraitFrameBuffer);
    }

    PurgePortraitCache(s_playerPortraitCache, 0, 1);
    PurgePortraitCache(s_unitPortraitCache, 0, 1);

    for (uint32_t i = 0; i < s_freePortraitTextures.m_count; i++) {
        TextureRelease(s_freePortraitTextures.m_data[i]);
    }

    s_freePortraitTextures.Clear();
    s_playerPortraits.Clear();
    s_unitPortraits.Clear();
    s_itemPortraits.Clear();
    s_pendingPortraits.Clear();
    s_freePortraits.Clear();
}

// PROVEN: bfc87778 IDENTICAL/IDENTICAL
// OFFSET: 0x004F8B90/0x1400A0C00
static void TextureUpdate(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    if (command != GxTex_Latch || !userArg) {
        return;
    }

    texelStrideInBytes = 4 * width;
    texels = static_cast<TSGrowableArray<CImVector>*>(userArg)->m_data;
}

// OFFSET: 0x00483130/0x1400A2540
bool RenderPortraitTexture(CSimpleTexture* texture, CM2Model* model, CM2Scene* scene, PortraitData* data) {
    if (!model || !scene || !data || !texture) {
        return false;
    }

    CCamera* createdCamera = NULL;
    CCamera* camera;

    if (model->HasCamera(0)) {
        camera = model->GetCameraById(0);
    } else {
        createdCamera = CameraCreate();

        if (!createdCamera) {
            return false;
        }

        C3Vector target(0.0f, 0.0f, 0.0f);
        M2SequenceInfo info;
        info.Clear();
        model->GetSequenceInfo(0, 0, info);
        C3Vector position;
        target = info.m_center;
        position.y = info.m_center.y;
        position.z = info.m_center.z;
        position.x = info.m_radius * 1.7f + info.m_center.x;
        CameraSetFloat(createdCamera, 4, 1.57f);
        CameraSetCoord(createdCamera, 8, target);
        CameraSetCoord(createdCamera, 7, position);
        CameraSetFloat(createdCamera, 3, 0.027777778f);
        camera = createdCamera;
    }

    CCharacterComponent::RemoveVisuals(model);
    model->SetBoneSequence(0xFFFFFFFF, 0, 0, 0, 0.0f, 0, 1, 0, 0, 0);

    float viewportMinX;
    float viewportMaxX;
    float viewportMinY;
    float viewportMaxY;
    float viewportMinZ;
    float viewportMaxZ;
    GxXformViewport(viewportMinX, viewportMaxX, viewportMinY, viewportMaxY, viewportMinZ, viewportMaxZ);
    GxXformSetViewport(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);

    C44Matrix savedProjection;
    C44Matrix savedView;
    GxXformProjection(savedProjection);
    GxXformView(savedView);
    int32_t fogEnabled = GxMasterEnable(GxMasterEnable_Fog);
    GxMasterEnableSet(GxMasterEnable_Fog, 0);

    // STATIC: 0x00D3C0F0
    static C44Matrix s_identity;

    C3Vector cameraPosition(0.0f, 0.0f, 0.0f);
    C3Vector cameraTarget(0.0f, 0.0f, 0.0f);
    model->WaitUntilLoadedRecursive();
    model->SetAnimating(1);
    C3Vector origin(0.0f, 0.0f, 0.0f);
    scene->Animate(origin);
    CameraGetCoord(camera, 7, &cameraPosition);
    CameraGetCoord(camera, 8, &cameraTarget);

    CRect window;
    window.minY = 0.0f;
    window.minX = 0.0f;
    window.maxY = 0.0f;
    window.maxX = 0.0f;
    GxCapsWindowSizeInScreenCoords(window);
    float width = window.maxX - window.minX;
    float height = window.maxY - window.minY;
    float ddcHeight = NDCToDDCHeight(1.0f);
    float top = ddcHeight - NDCToDDCHeight(1.0f) / height * 128.0f;
    ddcHeight = NDCToDDCHeight(1.0f);
    float right = NDCToDDCWidth(1.0f) / width * 128.0f;

    CRect viewport;
    viewport.minY = top;
    viewport.minX = 0.0f;
    viewport.maxY = ddcHeight;
    viewport.maxX = right;

    CRect projection;
    projection.minY = 0.0f;
    projection.minX = 0.0f;
    projection.maxY = 1.0f;
    projection.maxX = right / (ddcHeight - top);
    CameraSetupWorldProjection(camera, projection, 0);

    if (!data->m_texture) {
        if (s_freePortraitTextures.Count()) {
            uint32_t count = s_freePortraitTextures.Count() - 1;
            data->m_texture = s_freePortraitTextures.m_data[count];
            s_freePortraitTextures.SetCount(count);
        }

        if (!data->m_texture) {
            CGxTexFlags flags;
            flags.m_filter = GxTex_Linear;

            if (s_rtAlphaSupported) {
                flags.m_renderTarget = 1;
            }

            data->m_texture = TextureCreate(128, 128, GxTex_Argb8888, GxTex_Argb8888, flags, s_rtAlphaSupported ? NULL : &data->m_texels, TextureUpdate, "Portrait1", 0);
        }
    }

    texture->SetTexture(data->m_texture);
    CGxTex* gxTex = TextureGetGxTex(data->m_texture);
    CGxFrameBuffer* savedFrameBuffer = NULL;

    if (!gxTex) {
        if (s_rtAlphaSupported) {
            s_rtAlphaSupported = 0;
            return RenderPortraitTexture(texture, model, scene, data);
        }

        return false;
    }

    uint32_t renderTarget = gxTex->m_flags.m_renderTarget;

    if (renderTarget) {
        savedFrameBuffer = g_theGxDevicePtr->FrameBufferGet();
        GxFrameBufferAttach(s_portraitFrameBuffer, GxBuffers_Color, gxTex, 0);

        if (GxFrameBufferFinalize(s_portraitFrameBuffer)) {
            GxFrameBufferSet(s_portraitFrameBuffer);
        }
    } else {
        DDCToNDC(viewport.minX, viewport.minY, &viewport.minX, &viewport.minY);
        DDCToNDC(viewport.maxX, viewport.maxY, &viewport.maxX, &viewport.maxY);
        GxXformSetViewport(viewport.minX, viewport.maxX, viewport.minY, viewport.maxY, 0.0f, 1.0f);
    }

    GxSceneClear(0x1F, CImVector());
    model->WaitUntilDrawableRecursive();
    model->SetAnimating(1);

    if (model->m_attachParent) {
        model->m_fFlags |= 0x40;
    } else {
        model->m_fFlags |= 0x8;
    }

    if (model->m_attachParent) {
        model->m_fFlags |= 0x10000;
    } else {
        model->m_fFlags |= 0x8000;
    }

    model->m_lightingCallback = PortraitLightingCallback;
    model->m_lightingArg = NULL;
    scene->Animate(cameraPosition);
    scene->BeginDraw();
    scene->Draw(M2PASS_0);
    scene->Draw(M2PASS_1);
    scene->Draw(M2PASS_2);
    scene->EndDraw();

    if (renderTarget) {
        C44Matrix view;
        GxXformSetView(view);
        C44Matrix ortho;
        GxuXformCreateOrtho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 500.0f, ortho);
        GxXformSetProjection(ortho);
        g_theGxDevicePtr->RsPush();
        GxRsSet(GxRs_Texture0, s_portraitMaskGxTex);
        GxRsSet(GxRs_ColorOp0, 0);
        GxRsSet(GxRs_AlphaOp0, 0);
        GxRsSet(GxRs_ColorWrite, 8);
        GxRsSet(GxRs_BlendingMode, 0);
        GxRsSet(GxRs_Lighting, 0);
        GxRsSet(GxRs_Culling, 0);
        GxRsSet(GxRs_DepthTest, 0);
        GxRsSet(GxRs_AlphaRef, 0);
        GxRsSet(GxRs_Multisample, 0);
        g_theGxDevicePtr->XformPushAndSet(GxXform_World, view);
        DefaultShaders::SetFogEnabled(0);

        // STATIC: 0x00D3C0BC
        static C3Vector s_positions[] = { C3Vector(-1.0f, 1.0f, 0.0f), C3Vector(-1.0f, -1.0f, 0.0f), C3Vector(1.0f, 1.0f, 0.0f), C3Vector(1.0f, -1.0f, 0.0f) };
        // STATIC: 0x00D3C09C
        static C2Vector s_texCoords[] = { C2Vector(0.0f, 0.0f), C2Vector(0.0f, 1.0f), C2Vector(1.0f, 0.0f), C2Vector(1.0f, 1.0f) };

        GxPrimLockVertexPtrs(4, s_positions, sizeof(C3Vector), NULL, 0, NULL, 0, NULL, 0, s_texCoords, sizeof(C2Vector), NULL, 0);
        DefaultShaders::SetDefaultShaders(static_cast<EGxVertexBufferFormat>(22), 0xFFFFFFFF);
        GxDrawLockedElements(GxPrim_TriangleStrip, 4, s_portraitMaskIndices);
        GxPrimUnlockVertexPtrs();
        GxXformPop(GxXform_World);
        g_theGxDevicePtr->RsPop();
        GxFrameBufferSet(savedFrameBuffer);

        if (!s_portraitDepthTexture) {
            CImVector black(0, 0, 0, 0xFF);
            GxSceneClear(0x10, black);
        }
    } else {
        CiRect rect;
        rect.minY = 0;
        rect.minX = 0;
        rect.maxY = 128;
        rect.maxX = 128;
        GxReadPixels(rect, data->m_texels);
        TSFixedArray<uint8_t>* mask = GetAlphaMask(128);

        for (uint32_t i = 0; i < data->m_texels.Count(); i++) {
            data->m_texels.m_data[i].a = mask->m_data[i];
        }

        GxTexUpdate(gxTex, rect, 1);
        GxSceneClear(0x1F, CImVector());
    }

    GxXformSetProjection(savedProjection);
    GxXformSetView(savedView);
    GxMasterEnableSet(GxMasterEnable_Fog, fogEnabled);
    GxXformSetViewport(viewportMinX, viewportMaxX, viewportMinY, viewportMaxY, viewportMinZ, viewportMaxZ);

    if (createdCamera) {
        CameraRelease(createdCamera);
    }

    return true;
}

// OFFSET: 0x00484170/0x1400A3E60
void PortraitInitialize() {
    CStatus status;

    s_portraitMaskTexture = TextureCreate("Interface\\CharacterFrame\\TempPortraitAlphaMask.blp", CGxTexFlags(), &status, 0, NULL, 2);
    s_portraitMaskGxTex = TextureGetGxTexBlocking(s_portraitMaskTexture);
    s_rtAlphaSupported = RTAlphaSupported();
    CGxTex* depthTex = g_theGxDevicePtr->BackBufferDepthTex();

    if (s_rtAlphaSupported && (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl || g_theGxDevicePtr->DeviceApi() == GxApi_GLL || g_theGxDevicePtr->DeviceApi() == GxApi_D3d11 || g_theGxDevicePtr->DeviceFormat().m_multisampleCount > 1)) {
        CGxTexFlags flags;
        flags.m_filter = GxTex_Linear;
        flags.m_renderTarget = 1;
        s_portraitDepthTexture = TextureCreate(128, 128, GxTex_D24X8, GxTex_D24X8, flags, NULL, PortraitDepthTextureCallback, "PortraitDepth", 0);
        depthTex = TextureGetGxTex(s_portraitDepthTexture);
    }

    s_portraitFrameBuffer = GxFrameBufferCreate(depthTex, NULL);
    s_playerPortraitCache = NULL;
    s_unitPortraitCache = NULL;
}

// OFFSET: 0x00485300/0x1400A5390
void SetPortraitTexture(CSimpleTexture* texture, const char* fileName) {
    if (!fileName || !*fileName) {
        return;
    }

    ITEMPORTRAIT* portrait = s_itemPortraits.Ptr(fileName);

    if (portrait) {
        texture->SetTexture(portrait->m_data.m_texture);
        return;
    }

    portrait = s_itemPortraits.New(fileName, 0, 0);
    CBLPFile blp;

    if (blp.OpenAddBlp(fileName, 1)) {
        TSGrowableArray<CImVector>& texels = portrait->m_data.m_texels;

        if (!texels.Count()) {
            texels.SetCount(4096);
        }

        uint8_t* pixels;
        uint32_t stride;

        if (blp.Lock(PIXEL_ARGB8888, 0, pixels, stride)) {
            for (uint32_t i = 0; i < texels.Count(); i++) {
                texels.m_data[i] = *reinterpret_cast<CImVector*>(&pixels[4 * i]);
            }
        }

        blp.Unlock(0);
        blp.Close();
        TSFixedArray<uint8_t>* mask = GetAlphaMask(64);

        for (uint32_t i = 0; i < texels.Count(); i++) {
            texels.m_data[i].a = mask->m_data[i];
        }

        CGxTexFlags flags;
        CTexture* portraitTexture = TextureCreate(64, 64, GxTex_Argb8888, GxTex_Argb8888, flags, &texels, TextureUpdate, "Portrait2", 0);
        texture->SetTexture(portraitTexture);
        portrait->m_data.m_texture = portraitTexture;
    }

    blp.Close();
}

// OFFSET: 0x00485510
void SetPortraitTexture(CSimpleTexture* texture, CGUnit_C* unit) {
    if (!texture || !unit || AddPendingPortraitFlags(unit, 2)) {
        return;
    }

    if (unit->GetPortraitTextureName() && *unit->GetPortraitTextureName()) {
        char fileName[260];
        SStrPrintf(fileName, sizeof(fileName), "interface\\icons\\%s", unit->GetPortraitTextureName());
        SetPortraitTexture(texture, fileName);
        return;
    }

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        WOWGUID guid = unit->m_obj->m_guid;
        PLAYERPORTRAIT* portrait = s_playerPortraits.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

        if (portrait && !portrait->m_cache.m_dirty) {
            texture->SetTexture(portrait->m_cache.m_data.m_texture);
            PortraitCacheTouch(&s_playerPortraitCache, &portrait->m_cache);
            return;
        }
    } else {
        HASHKEY_NONE key;
        UNITPORTRAIT* portrait = s_unitPortraits.Ptr(unit->GetLocalDisplayID(), key);

        if (portrait && !portrait->m_cache.m_dirty) {
            texture->SetTexture(portrait->m_cache.m_data.m_texture);
            PortraitCacheTouch(&s_unitPortraitCache, &portrait->m_cache);
            return;
        }
    }

    if (QueuePortraitUpdate(unit, 2)) {
        return;
    }

    CM2Scene* scene = M2CreateScene();

    if (!scene) {
        return;
    }

    CM2Model* model = unit->DuplicateModel(scene);

    if (!model) {
        scene->Release();
        return;
    }

    model->SetWorldTransform(unit->GetPosition(), 0.0f, 1.0f);
    int32_t faction = -1;

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        faction = static_cast<CGPlayer_C*>(unit)->GetPVPFactionIndex();
    }

    CGObject_C::ReplaceGuildTextures(model, unit->GetGuildGUID().m_guid, faction);
    PortraitBase* cache;

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        WOWGUID guid = unit->m_obj->m_guid;
        PLAYERPORTRAIT* portrait = s_playerPortraits.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

        if (!portrait) {
            portrait = s_playerPortraits.New(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid), 0, 0);
        }

        cache = &portrait->m_cache;
        PortraitCacheTouch(&s_playerPortraitCache, cache);
    } else {
        uint32_t displayId = unit->GetLocalDisplayID();
        HASHKEY_NONE key;
        UNITPORTRAIT* portrait = s_unitPortraits.Ptr(displayId, key);

        if (!portrait) {
            portrait = s_unitPortraits.New(displayId, key, 0, 0);
        }

        cache = &portrait->m_cache;
        PortraitCacheTouch(&s_unitPortraitCache, cache);
    }

    if (!RenderPortraitTexture(texture, model, scene, &cache->m_data)) {
        model->Release();
        scene->Release();
        return;
    }

    model->Release();
    scene->Release();

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        PurgePortraitCache(s_playerPortraitCache, 8, 0);
    } else {
        PurgePortraitCache(s_unitPortraitCache, 8, 0);
    }

    if (cache) {
        cache->m_dirty = 0;
    }
}

// OFFSET: 0x00485830/0x1400A6170
void SetPortraitTextureFromDisplayID(CSimpleTexture* texture, int32_t displayId) {
    if (!texture) {
        return;
    }

    CreatureDisplayInfoRec* display = g_creatureDisplayInfoDB.GetRecord(displayId);

    if (!display) {
        return;
    }

    if (display->m_portraitTextureName && *display->m_portraitTextureName) {
        char fileName[260];
        SStrPrintf(fileName, sizeof(fileName), "interface\\icons\\%s", display->m_portraitTextureName);
        SetPortraitTexture(texture, fileName);
        return;
    }

    HASHKEY_NONE key;
    UNITPORTRAIT* portrait = s_unitPortraits.Ptr(displayId, key);

    if (portrait && !portrait->m_cache.m_dirty) {
        texture->SetTexture(portrait->m_cache.m_data.m_texture);
        PortraitCacheTouch(&s_unitPortraitCache, &portrait->m_cache);
        return;
    }

    CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(display->m_modelID);

    if (!modelData) {
        return;
    }

    CM2Scene* scene = M2CreateScene();

    if (!scene) {
        return;
    }

    CM2Model* model = scene->CreateModel(modelData->m_modelName, 32, 3);

    if (!model) {
        scene->Release();
        return;
    }

    CreatureDisplayInfoExtraRec* extra = g_creatureDisplayInfoExtraDB.GetRecord(display->m_extendedDisplayInfoID);

    if (extra) {
        CCharacterComponent* component = CCharacterComponent::AllocComponent();
        ComponentData data;
        data.m_preferences.m_raceID = extra->m_displayRaceID;
        data.m_preferences.m_sexID = extra->m_displaySexID;
        data.m_preferences.m_classId = 0;
        data.m_preferences.m_hairColorId = extra->m_hairColorID;
        data.m_preferences.m_skinID = extra->m_skinID;
        data.m_preferences.m_faceId = extra->m_faceID;
        data.m_preferences.m_facialHairId = extra->m_facialHairID;
        data.m_preferences.m_hairStyleId = extra->m_hairStyleID;
        data.m_flags |= 0x1;
        SStrPrintf(data.m_npcSkinTexture, sizeof(data.m_npcSkinTexture), "%s%s", "Textures\\BakedNpcTextures\\", extra->m_bakeName);
        data.m_model = model;
        data.m_model->m_refCount++;
        component->Init(&data, NULL);

        if (component) {
            for (uint32_t i = 0; i < 11; i++) {
                if (extra->m_npcitemDisplay[i]) {
                    component->AddItem(static_cast<ITEM_SLOT>(i), extra->m_npcitemDisplay[i], 0);
                }
            }
        }

        component->RenderPrep(0);
        CCharacterComponent::FreeComponent(component);
    } else {
        CCharacterComponent::ReplaceMonsterSkin(model, display, modelData);
    }

    if (!portrait) {
        portrait = s_unitPortraits.New(displayId, key, 0, 0);
    }

    PortraitBase* cache = &portrait->m_cache;
    PortraitCacheTouch(&s_unitPortraitCache, cache);

    if (!RenderPortraitTexture(texture, model, scene, &cache->m_data)) {
        model->Release();
        scene->Release();
        return;
    }

    model->Release();
    scene->Release();
    PurgePortraitCache(s_unitPortraitCache, 8, 0);

    if (cache) {
        cache->m_dirty = 0;
    }
}
