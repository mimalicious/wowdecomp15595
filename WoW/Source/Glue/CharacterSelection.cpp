#include "CharacterSelection.h"
#include "WoW/Source/Ui/CGTabardCreationFrame.h"
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/PreloadList.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CharStartOutfitRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/Game/GameClient/CDrawBowString.h"
#include "WoW/Source/Glue/CGlueLoading.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Glue/CSimpleModelFFX.h"
#include "WoW/Source/Glue/CharacterCreation.h"
#include "WoW/Source/Glue/ComponentData.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Ui/AddOns.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00BCEB80
const int32_t g_scriptSexIndex[3] = { 2, 3, 1 };

// STATIC: 0x00D07948
int32_t CCharacterSelection::m_selectionIndex = -1;

// STATIC: 0x00D0794C
int32_t CCharacterSelection::m_enterWorldIndex = -1;

// STATIC: 0x00D079EC
static float s_emoteIntervalMax = 30.0f;

// STATIC: 0x00D079F0
static float s_emoteIntervalMin = 10.0f;

// STATIC: 0x00D079F4
static int32_t s_standAnimKitID = 540;

// STATIC: 0x00D079F8
static float s_characterGlow = 0.4f;

// STATIC: 0x00D079FC
static float s_ghostGlow = 0.15f;

// STATIC: 0x00D07A04
static int32_t s_unk_D07A04 = -1;

// STATIC: 0x00EBC078
CSimpleModelFFX* CCharacterSelection::m_modelFrame;

// STATIC: 0x00EBC07C
uint32_t CCharacterSelection::m_emoteTimerEvent;

// STATIC: 0x00EBC080
int32_t CCharacterSelection::m_characterCount;

// STATIC: 0x00EBC084
float CCharacterSelection::m_charFacing;

// STATIC: 0x00EBC088
int32_t CCharacterSelection::m_characterListRcvd;

// STATIC: 0x00EBC08C
PreloadList* CCharacterSelection::m_startRacePreloadList;

// STATIC: 0x00EBC090
TSGrowableArray<CharacterSelectionDisplay> CCharacterSelection::m_characterList;

uint32_t CCharacterSelection::s_raceRestrictions[10];

// PROVEN: eff88660 IDENTICAL/NONE
// OFFSET: 0x00807E60
void CharSelectAttachDeathVisuals(CM2Model* model) {
    CGlueMgr::ApplySpellVisualKit(CCharacterSelection::m_modelFrame, model, 989, 0);
}

// PROVEN: ccb98158 IDENTICAL/NONE
// OFFSET: 0x00807E80
void LinkModelAndTexturesToAsyncHead(CM2Model* model) {
    if (model) {
        model->LinkToAsyncHead();
    }
}

// PROVEN: 2f559bd8 IDENTICAL/NONE
// OFFSET: 0x00807EA0/0x14051D820
void CharSelectGetBGModelPathFromRaceStr(const char* raceString, char* path) {
    strcpy(path, "Interface\\Glues\\Models\\UI_");
    strcat(path, raceString);
    strcat(path, "\\UI_");
    strcat(path, raceString);
    strcat(path, ".m2");
}

// OFFSET: 0x00807FA0/0x14051D970
int32_t CharSelectIsStartRaceComplete() {
    if (CCharacterSelection::m_startRacePreloadList) {
        return CCharacterSelection::m_startRacePreloadList->IsComplete();
    }

    return 1;
}

// INLINED
bool CCharacterSelection::IsStreamingDataReady() {
    return CharSelectIsStartRaceComplete() != 0;
}

// OFFSET: 0x00807FC0
void CharSelectStartRaceShutdown() {
    PreloadList::StopDownload(CCharacterSelection::m_startRacePreloadList);
    CCharacterSelection::m_startRacePreloadList = NULL;
}

// OFFSET: 0x00807FE0
void CharSelectSwitchStartRaceDownload(int32_t raceID, int32_t classID) {
    PreloadList* list = CCharacterSelection::m_startRacePreloadList;

    if (list) {
        if (!list->ListNameChanged(raceID, classID)) {
            return;
        }

        list = CCharacterSelection::m_startRacePreloadList;
    }

    PreloadList::StopDownload(list);
    CCharacterSelection::m_startRacePreloadList = PreloadList::StartDownload(raceID, classID, 1);
}

// PROVEN: 671bc0ca IDENTICAL/NONE
// OFFSET: 0x00808030/0x14051DA10
void CCharacterSelection::Initialize() {
    CoordinateSetAspectRatio(g_theGxDevicePtr->CapsAspectRatio(0));
}

// OFFSET: 0x00808050
void CCharacterSelection::TransferComponentToWorld() {
    if (static_cast<uint32_t>(CCharacterSelection::m_enterWorldIndex) >= CCharacterSelection::m_characterList.Count()) {
        return;
    }

    if (CCharacterComponent::m_activePlayerComponent) {
        CCharacterComponent::FreeComponent(CCharacterComponent::m_activePlayerComponent);
        CCharacterComponent::m_activePlayerComponent = NULL;
    }

    CharacterSelectionDisplay& display = CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_enterWorldIndex];

    if (!display.m_component) {
        return;
    }

    CM2Model* model = display.m_component->m_data.m_model;

    if (!model) {
        return;
    }

    model->SetLoadedCallback(NULL, NULL);
    model->SetSequenceCallback(NULL, 0);
    model->SetSequenceStartCallback(NULL, 0);
    model->SetEventCallback(NULL, 0);
    CCharacterComponent::RemoveVisuals(model);
    model->AttachToSceneRecursive(World::GetScene());
    CCharacterComponent::m_activePlayerComponent = display.m_component;
    CGPlayer_C::s_displayId = GetDisplayIdFromRaceAndGender(display.m_characterInfo.m_raceID, display.m_characterInfo.m_sexID);
    display.m_component = NULL;
}

// OFFSET: 0x00808130
void CCharacterSelection::UpdateCharacterName(WOWGUID guid, const char* name) {
    uint32_t index = 0;

    if (CCharacterSelection::m_characterList.Count() <= 0) {
        return;
    }

    while (CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_guid != guid) {
        if (++index >= CCharacterSelection::m_characterList.Count()) {
            return;
        }
    }

    char oldPath[260];
    char newPath[260];
    SStrPrintf(oldPath, sizeof(oldPath), "WTF\\Account\\%s\\%s\\%s", CGlueMgr::m_accountName, ClientServices::GetSelectedRealmName(), static_cast<const char*>(CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_name));
    SStrPrintf(newPath, sizeof(newPath), "WTF\\Account\\%s\\%s\\%s", CGlueMgr::m_accountName, ClientServices::GetSelectedRealmName(), name);
    OsMoveFile(oldPath, newPath);
    SStrPrintf(oldPath, sizeof(oldPath), "WTF\\Account\\%s\\%s", CGlueMgr::m_accountName, static_cast<const char*>(CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_name));
    SStrPrintf(newPath, sizeof(newPath), "WTF\\Account\\%s\\%s", CGlueMgr::m_accountName, name);
    OsMoveFile(oldPath, newPath);
    ClearAddOnEnableState(CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_name);
    LoadAddOnEnableState(name);
    SStrCopy(CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_name, name, sizeof(CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_name));
    CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_flags &= ~0x4000u;
    CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_flags &= ~0x2000000u;
    FrameScript_SignalEvent(GLUE_EVENT_CHARACTER_LIST_UPDATE, NULL);
}

// OFFSET: 0x008082B0
void CCharacterSelection::UpdateCharacterInfo(WOWGUID guid, const char* name, uint8_t sexID, uint8_t skinColorID, uint8_t hairColorID, uint8_t hairStyleID, uint8_t facialHairStyleID, uint8_t faceID, uint8_t raceID) {
    uint32_t index = 0;

    if (CCharacterSelection::m_characterList.Count() <= 0) {
        return;
    }

    while (CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_guid != guid) {
        if (++index >= CCharacterSelection::m_characterList.Count()) {
            return;
        }
    }

    if (_stricmp(name, CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_name)) {
        char oldPath[260];
        char newPath[260];
        SStrPrintf(oldPath, sizeof(oldPath), "WTF\\Account\\%s\\%s\\%s", CGlueMgr::m_accountName, ClientServices::GetSelectedRealmName(), static_cast<const char*>(CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_name));
        SStrPrintf(newPath, sizeof(newPath), "WTF\\Account\\%s\\%s\\%s", CGlueMgr::m_accountName, ClientServices::GetSelectedRealmName(), name);
        OsMoveFile(oldPath, newPath);
        SStrPrintf(oldPath, sizeof(oldPath), "WTF\\Account\\%s\\%s", CGlueMgr::m_accountName, static_cast<const char*>(CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_name));
        SStrPrintf(newPath, sizeof(newPath), "WTF\\Account\\%s\\%s", CGlueMgr::m_accountName, name);
        OsMoveFile(oldPath, newPath);
        ClearAddOnEnableState(CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_name);
        LoadAddOnEnableState(name);
        SStrCopy(CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_name, name, sizeof(CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_name));
    }

    CHARACTER_INFO& info = CCharacterSelection::m_characterList.m_data[index].m_characterInfo;
    info.m_sexID = sexID;
    info.m_skinColorID = skinColorID;
    info.m_hairColorID = hairColorID;
    info.m_hairStyleID = hairStyleID;
    info.m_facialHairStyleID = facialHairStyleID;
    info.m_faceID = faceID;

    if (raceID) {
        info.m_raceID = raceID;
    }

    info.m_customizeFlags &= 0xFFEEFFFE;
    FrameScript_SignalEvent(GLUE_EVENT_CHARACTER_LIST_UPDATE, NULL);
}

// OFFSET: 0x008084B0
uint32_t* CCharacterSelection::UpdateCharacterNameDeclension(WOWGUID guid) {
    uint32_t index = 0;

    if (CCharacterSelection::m_characterList.Count() <= 0) {
        return NULL;
    }

    while (CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_guid != guid) {
        if (++index >= CCharacterSelection::m_characterList.Count()) {
            return NULL;
        }
    }

    CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_flags |= 0x2000000;
    return &CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_flags;
}

// OFFSET: 0x00808510/0x14051DFE0
void CCharacterSelection::SetNextEmoteTimer() {
    ClientKillTimer(CCharacterSelection::m_emoteTimerEvent, CCharacterSelection::UpdateEmote, "UpdateEmote");
    CCharacterSelection::m_emoteTimerEvent = 0;

    if (!g_useAnimKitsOnGlueScreen) {
        return;
    }

    if (CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_selectionIndex].m_characterInfo.m_flags & 0x2000) {
        return;
    }

    uint32_t bits = (CRandom::uint32_(g_rndSeed) & 0x7FFFFF) | 0x3F800000;
    float random = *reinterpret_cast<float*>(&bits) - 1.0f;
    CCharacterSelection::m_emoteTimerEvent = ClientSetTimer(static_cast<uint32_t>((random * s_emoteIntervalMax + (1.0f - random) * s_emoteIntervalMin) * 1000.0f), CCharacterSelection::UpdateEmote, NULL);
}

// OFFSET: 0x008085D0/0x14051E0B0
void CharacterSelectionDisplay::SetCharacterFacing(float rotation) {
    if (!CCharacterSelection::m_characterCount) {
        return;
    }

    CCharacterSelection::m_charFacing = rotation;

    if (static_cast<uint32_t>(CCharacterSelection::m_selectionIndex) >= CCharacterSelection::m_characterList.Count()) {
        return;
    }

    CCharacterComponent* component = CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_selectionIndex].m_component;

    if (component && component->m_data.m_model) {
        C3Vector position;
        component->m_data.m_model->SetWorldTransform(position, rotation, 1.0f);
    }
}

// OFFSET: 0x00808650/0x14051E130
void CCharacterSelection::RenderPrep() {
    if (static_cast<uint32_t>(CCharacterSelection::m_selectionIndex) >= CCharacterSelection::m_characterList.Count()) {
        return;
    }

    CCharacterComponent* component = CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_selectionIndex].m_component;

    if (!component) {
        return;
    }

    CM2Model* attached = CCharacterSelection::m_modelFrame->m_model->m_attachList;

    while (attached && attached->m_attachmentId) {
        attached = attached->m_attachNext;
    }

    if (component->RenderPrep(0) && attached && (attached->m_fFlags & 0x2000) && attached->IsDrawableRecursive()) {
        attached->SetRenderDisabledRecursive(0);
    }
}

// OFFSET: 0x008086D0
CharacterSelectionDisplay* CCharacterSelection::GetCharacterDisplay(uint32_t index) {
    if (index < CCharacterSelection::m_characterList.Count()) {
        return &CCharacterSelection::m_characterList.m_data[index];
    }

    return NULL;
}

// OFFSET: 0x008086F0
int32_t CCharacterSelection::Idle(const void* data, void* param) {
    CCharacterComponent* component = NULL;

    if (CCharacterSelection::m_selectionIndex >= 0 && CCharacterSelection::m_selectionIndex < static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        CharacterSelectionDisplay& display = CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_selectionIndex];

        CM2Model* model = CCharacterSelection::m_modelFrame->m_model;

        if (model) {
            model->LinkToAsyncHead();
        }

        if (display.m_petModel) {
            display.m_petModel->LinkToAsyncHead();
        }

        component = display.m_component;

        if (component) {
            component->LinkToAsyncHead();

            if (component->m_data.m_model) {
                component->m_data.m_model->LinkToAsyncHead();
            }
        }
    }

    CGlueLoading::Idle(component);
    return 1;
}

// OFFSET: 0x00808770
const CharStartOutfitRec* CCharacterSelection::FindStartOutfit(uint8_t raceID, uint8_t classID, uint8_t sexID) {
    for (int32_t i = 0; i < g_charStartOutfitDB.m_numRecords; i++) {
        const CharStartOutfitRec* rec = g_charStartOutfitDB.GetRecordByIndex(i);

        if (rec->m_raceID == raceID && rec->m_classID == classID && rec->m_sexID == sexID) {
            return rec;
        }
    }

    return NULL;
}

// OFFSET: 0x008088D0
CharacterSelectionDisplay::~CharacterSelectionDisplay() {
    CAnimKitInst* emoteKit = this->m_emoteKit;

    if (emoteKit) {
        CGlueMgr::StopCharacterEmoteKits(this->m_animKitManager);
        emoteKit->Stop(1, 0);
        emoteKit->Release();
    }

    if (this->m_petModel) {
        this->m_petModel->SetLoadedCallback(NULL, NULL);
        this->m_petModel->SetSequenceCallback(NULL, 0);
        this->m_petModel->SetSequenceStartCallback(NULL, 0);
        this->m_petModel->SetEventCallback(NULL, 0);
        this->m_petModel->Release();
    }

    if (this->m_component) {
        CM2Model* model = this->m_component->m_data.m_model;

        if (model) {
            model->SetLoadedCallback(NULL, NULL);
            model->SetSequenceCallback(NULL, 0);
            model->SetSequenceStartCallback(NULL, 0);
            model->SetEventCallback(NULL, 0);
        }

        CCharacterComponent::FreeComponent(this->m_component);
        this->m_component = NULL;
    }
}

// OFFSET: 0x008089C0
CharacterSelectionDisplay::CharacterSelectionDisplay() {
    this->m_component = NULL;
    this->m_emoteKit = NULL;
    this->m_petModel = NULL;
    this->m_petAlpha = 1.0f;
    this->m_weaponLoadOut.m_offHand = 0;
    this->m_weaponLoadOut.m_mainHand = 0;
    this->m_animKitManager.SetGlueMode(1);
}

// PROVEN: 9dd21890 IDENTICAL/NONE
// OFFSET: 0x00808A30
void CCharacterSelection::OnShow() {
    EventRegisterEx(EVENT_ID_IDLE, CCharacterSelection::Idle, NULL, 0.0f);
}

// OFFSET: 0x00808A50
uint32_t CCharacterSelection::GetEquippedGuildTabardFlags(int32_t slot, int32_t index) {
    CharacterSelectionDisplay* display = index >= 0 && index < static_cast<int32_t>(CCharacterSelection::m_characterList.Count()) ? &CCharacterSelection::m_characterList.m_data[index] : NULL;
    ItemDisplayInfoRec displayRec;

    if (!display) {
        return 0;
    }

    if (display->m_characterInfo.m_items[slot].m_displayID && g_itemDisplayInfoDB.GetRecord(display->m_characterInfo.m_items[slot].m_displayID, &displayRec)) {
        return displayRec.m_flags & 0x19;
    }

    return 0;
}

// OFFSET: 0x00808AB0
void CCharacterSelection::GuildCallback(const uint64_t& guildGUID, const WOWGUID& guid, void* param, bool success) {
    if (!guildGUID || !success || static_cast<uint32_t>(CCharacterSelection::m_selectionIndex) >= CCharacterSelection::m_characterList.Count()) {
        return;
    }

    for (int32_t i = 0; i < static_cast<int32_t>(CCharacterSelection::m_characterList.Count()); i++) {
        if (CCharacterSelection::m_characterList.m_data[i].m_characterInfo.m_guildGUID.m_guid != guildGUID) {
            continue;
        }

        int32_t emblemStyle;
        int32_t emblemColor;
        int32_t borderStyle;
        int32_t borderColor;
        int32_t background;

        if (!GuildGetGuildTabard(WOWGUID(), WOWGUID::Make(guildGUID), NULL, NULL, emblemStyle, emblemColor, borderStyle, borderColor, background)) {
            continue;
        }

        CCharacterComponent* component = CCharacterSelection::m_characterList.m_data[i].m_component;

        if (!component) {
            continue;
        }

        uint32_t flags = CCharacterSelection::GetEquippedGuildTabardFlags(18, i);

        if (flags) {
            component->ApplyGuildColor(flags, emblemStyle, emblemColor, borderStyle, borderColor, background);
        }

        CGObject_C::ReplaceGuildTextures(component->m_data.m_model, guildGUID, -1);
        flags = CCharacterSelection::GetEquippedGuildTabardFlags(3, i);

        if (flags) {
            component->ApplyShirtGuildColor(flags, emblemStyle, emblemColor, borderStyle, borderColor, background);
        }
    }
}

// OFFSET: 0x00808BE0
void CCharacterSelection::ModelLoadedCallback(CM2Model* model, void* arg) {
    for (int32_t i = 0; i < static_cast<int32_t>(CCharacterSelection::m_characterList.Count()); i++) {
        CharacterSelectionDisplay* display = &CCharacterSelection::m_characterList.m_data[i];
        CCharacterComponent* component = display->m_component;

        if (!component || component->m_data.m_model != model) {
            continue;
        }

        int32_t displayID = display->m_characterInfo.m_items[18].m_displayID;
        ItemDisplayInfoRec displayRec;

        if (displayID && g_itemDisplayInfoDB.GetRecord(displayID, &displayRec) && (displayRec.m_flags & 0x19)) {
            int32_t emblemStyle;
            int32_t emblemColor;
            int32_t borderStyle;
            int32_t borderColor;
            int32_t background;
            GuildGetGuildTabard(display->m_characterInfo.m_guid, display->m_characterInfo.m_guildGUID, CCharacterSelection::GuildCallback, NULL, emblemStyle, emblemColor, borderStyle, borderColor, background);
        }

        CGObject_C::ReplaceGuildTextures(model, display->m_characterInfo.m_guildGUID.m_guid, CGPlayer_C::GetPVPFactionIndex(display->m_characterInfo.m_raceID));
    }
}

// OFFSET: 0x00808D10
void CCharacterSelection::StartAnimation(uint64_t param) {
    if (g_useAnimKitsOnGlueScreen) {
        CharacterSelectionDisplay& display = CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_selectionIndex];
        display.m_emoteKit = display.m_animKitManager.PlayAndMaintain(s_standAnimKitID, -1, 0);
    } else {
        CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_selectionIndex].m_component->m_data.m_model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }

    CCharacterSelection::SetNextEmoteTimer();
}

// OFFSET: 0x00808D90/0x14051E9C0
int32_t CCharacterSelection::UpdateEmote(const void* data, void* param) {
    CharacterSelectionDisplay& selected = CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_selectionIndex];
    CharacterSelectionDisplay* display = CCharacterSelection::m_selectionIndex < 0 || CCharacterSelection::m_selectionIndex >= static_cast<int32_t>(CCharacterSelection::m_characterList.Count()) ? NULL : &CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_selectionIndex];
    CGlueMgr::PlayCharacterEmoteKits(display->m_characterInfo.m_classID, display->m_characterInfo.m_raceID, display->m_characterInfo.m_sexID, selected.m_weaponLoadOut, selected.m_animKitManager, CCharacterSelection::m_modelFrame);
    CCharacterSelection::SetNextEmoteTimer();
    return 1;
}

// PROVEN: 38548fe0 IDENTICAL/NONE
// OFFSET: 0x00809590
void CCharacterSelection::OnHide() {
    EventUnregister(EVENT_ID_IDLE, CCharacterSelection::Idle);
    LoadingScreenMiniDisable();
    CCharacterSelection::m_selectionIndex = -1;
    ClientKillTimer(CCharacterSelection::m_emoteTimerEvent, CCharacterSelection::UpdateEmote, "UpdateEmote");
    CCharacterSelection::m_emoteTimerEvent = 0;
}

// OFFSET: 0x008095D0/0x14051F440
void CCharacterSelection::GenericLightingCallback(CM2Model* model, CM2Lighting* lighting, void* arg) {
    CM2LightArray* lights = static_cast<CM2LightArray*>(arg);
    CharacterSelectionDisplay* display = CCharacterSelection::m_selectionIndex < 0 || CCharacterSelection::m_selectionIndex >= static_cast<int32_t>(CCharacterSelection::m_characterList.Count()) ? NULL : &CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_selectionIndex];

    if (!lights->unk_01BC && display && (display->m_characterInfo.m_flags & 0x2000)) {
        lights = &lights[1];

        if (!lights->m_hasCustomLight) {
            CAaSphere sphere;
            sphere.c = model->GetWorldPosition();
            sphere.r = 0.0f;
            lighting->Initialize(model->m_scene, sphere);

            const DayNight::LightInfo* lightInfo = DayNight::GetDeathLightInfo();
            CM2Light light;
            light.SetLightType(M2LIGHT_DIRECT);
            light.m_ambColor = C3Vector(lightInfo->m_ambientColor);
            light.m_dirColor = C3Vector(lightInfo->m_directColor);
            light.SetDirection(C3Vector(0.0f, 0.0f, -1.0f));
            light.SetVisible(1);
            lighting->AddLight(&light);
            return;
        }
    }

    if (lights->m_hasCustomLight) {
        lighting->Reset();

        for (uint32_t i = 0; i < lights->m_lightCount; i++) {
            lighting->AddLight(&lights->m_lights[i]);
        }
    }

    if (lights->unk_01B5) {
        CSimpleModelFFX* frame = lights->m_modelFrame;

        if (frame->m_flags & 0x1) {
            lighting->SetFog(C3Vector(frame->m_fogColor), frame->m_fogNear, frame->m_fogFar);
        }
    }
}

// OFFSET: 0x00809830/0x14051F6C0
void CCharacterSelection::SetBackgroundModel(const char* modelName) {
    if (!CCharacterSelection::m_modelFrame || !modelName || !*modelName || !CCharacterSelection::m_characterListRcvd) {
        return;
    }

    CM2Model* current = CCharacterSelection::m_modelFrame->m_model;

    if (current) {
        if (!_stricmp(modelName, current->m_shared->m_fileName)) {
            return;
        }

        while (current->m_attachList) {
            current->m_attachList->DetachFromParent();
        }
    }

    CCharacterSelection::m_modelFrame->SetModel(modelName);
    CCharacterSelection::m_modelFrame->m_lightArray1[0].unk_01B5 = 1;

    CM2Model* model = CCharacterSelection::m_modelFrame->m_model;

    if (!model) {
        return;
    }

    char defaultName[260];

    while (true) {
        model->m_lightingCallback = CCharacterSelection::GenericLightingCallback;
        model->m_lightingArg = CCharacterSelection::m_modelFrame->m_lightArray1;

        if (modelName == defaultName) {
            break;
        }

        model->WaitUntilLoaded();

        if (model->TexturesAreLocal()) {
            break;
        }

        CharSelectGetBGModelPathFromRaceStr("CharacterSelect", defaultName);

        if (!_stricmp(modelName, defaultName)) {
            break;
        }

        modelName = defaultName;
        CCharacterSelection::m_modelFrame->SetModel(defaultName);
        CCharacterSelection::m_modelFrame->m_lightArray1[0].unk_01B5 = 1;
        model = CCharacterSelection::m_modelFrame->m_model;

        if (!model) {
            return;
        }
    }

    model->WaitUntilDrawable();

    if (static_cast<uint32_t>(CCharacterSelection::m_selectionIndex) >= CCharacterSelection::m_characterList.Count()) {
        return;
    }

    CharacterSelectionDisplay& display = CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_selectionIndex];

    if (!display.m_component) {
        return;
    }

    CM2Model* petModel = display.m_petModel;

    if (display.m_component->m_data.m_model) {
        display.m_component->m_data.m_model->AttachToParent(model, 0, NULL, 0);
    }

    if (petModel) {
        petModel->AttachToParent(model, 1, NULL, 0);
    }
}

// OFFSET: 0x008099C0
void CCharacterSelection::DeleteCharacter(int32_t index) {
    ClientKillTimer(CCharacterSelection::m_emoteTimerEvent, CCharacterSelection::UpdateEmote, "UpdateEmote");
    CCharacterSelection::m_emoteTimerEvent = 0;

    if (index >= static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        return;
    }

    int32_t lastIndex = g_lastCharacterIndexVar->m_intValue;

    if (index == lastIndex) {
        g_lastCharacterIndexVar->Set(lastIndex - 1, true, false, false, true);
    }

    CGlueMgr::DeleteCharacter(CCharacterSelection::m_characterList.m_data[index].m_characterInfo.m_guid);
}

// OFFSET: 0x00809A50
void CCharacterSelection::StartLoadAnimation(int32_t param) {
    CCharacterComponent* component = CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_selectionIndex].m_component;

    if (component) {
        CGlueLoading::StartLoad(component, 1, CCharacterSelection::StartAnimation, static_cast<int64_t>(param), 5);
    }
}

// OFFSET: 0x00809B30/0x14051FA90
void CCharacterSelection::Shutdown() {
    if (CCharacterSelection::m_selectionIndex >= 0 && CCharacterSelection::m_selectionIndex < static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        CharacterSelectionDisplay& display = CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_selectionIndex];

        if (display.m_component) {
            CGlueMgr::StopCharacterEmoteKits(display.m_animKitManager);
        }
    }

    LoadingScreenMiniDisable();
    CCharacterSelection::m_characterList.Clear();
    CCharacterSelection::m_characterListRcvd = 0;
    CCharacterSelection::m_selectionIndex = -1;
    ClientKillTimer(CCharacterSelection::m_emoteTimerEvent, CCharacterSelection::UpdateEmote, "UpdateEmote");
    CCharacterSelection::m_emoteTimerEvent = 0;
    CGlueLoading::CancelLoadCallback();
    CGlueMgr::GetSelectedCharInfo(NULL);
}

// OFFSET: 0x00809BE0
void CCharacterSelection::ShowCharacter(uint8_t raceOverride, int32_t noEffects) {
    if (CCharacterSelection::m_selectionIndex < 0 || CCharacterSelection::m_selectionIndex >= static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        return;
    }

    const CreatureDisplayInfoRec* displayRec = NULL;
    float petScale = 0.0f;
    ComponentData data;

    ClientKillTimer(CCharacterSelection::m_emoteTimerEvent, CCharacterSelection::UpdateEmote, "UpdateEmote");
    CCharacterSelection::m_emoteTimerEvent = 0;

    if (CCharacterSelection::m_modelFrame) {
        CM2Model* model = CCharacterSelection::m_modelFrame->m_model;

        if (model) {
            model->DetachAllChildrenById(0);
        }

        if (CCharacterSelection::m_modelFrame) {
            CM2Model* model = CCharacterSelection::m_modelFrame->m_model;

            if (model) {
                model->DetachAllChildrenById(1);
            }
        }
    }

    CCharacterSelection::m_modelFrame->GetScene()->GarbageCollectModels();

    CharacterSelectionDisplay& display = CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_selectionIndex];
    CCharacterSelection::m_charFacing = 0.0f;

    if (!display.m_component) {
        uint32_t raceID = raceOverride ? raceOverride : display.m_characterInfo.m_raceID;
        CreatureModelDataRec* modelDataRec = GetModelDataRec(raceID, display.m_characterInfo.m_sexID);

        if (!modelDataRec || !modelDataRec->m_modelName) {
            return;
        }

        display.m_component = CCharacterComponent::AllocComponent();
        data.m_preferences.m_raceID = raceID;
        data.m_preferences.m_sexID = display.m_characterInfo.m_sexID;
        data.m_preferences.m_classId = display.m_characterInfo.m_classID;
        data.m_preferences.m_skinID = display.m_characterInfo.m_skinColorID;
        data.m_preferences.m_faceId = display.m_characterInfo.m_faceID;
        data.m_preferences.m_hairStyleId = display.m_characterInfo.m_hairStyleID;
        data.m_preferences.m_hairColorId = display.m_characterInfo.m_hairColorID;
        data.m_preferences.m_facialHairId = display.m_characterInfo.m_facialHairStyleID;
        const char* modelName = modelDataRec->m_modelName;
        data.m_model = CCharacterSelection::m_modelFrame->GetScene()->CreateModel(modelName, 0, 3);
        data.m_flags |= 0x2;
        display.m_component->Init(&data, NULL);

        CM2Model* model = display.m_component->m_data.m_model;
        model->m_lightingCallback = CCharacterSelection::GenericLightingCallback;
        model->m_lightingArg = CCharacterSelection::m_modelFrame->m_lightArray2;
        model->SetLoadedCallback(CCharacterSelection::ModelLoadedCallback, NULL);
        display.m_animKitManager.SetModel(data.m_model, 1);
        display.m_weaponLoadOut = CGlueMgr::AddItemsFromCharInfo(&display.m_characterInfo, display.m_component, CCharacterSelection::GuildCallback, CCharacterCreation::BowModelLoadedCallback);

        if (!noEffects && (display.m_characterInfo.m_flags & 0x2000)) {
            CharSelectAttachDeathVisuals(display.m_component->m_data.m_model);
        }

        display.m_component->m_data.m_model->SetRenderDisabledRecursive(1);

        if (!noEffects) {
            int32_t totem = 0;
            const CreatureModelDataRec* petModelRec = NULL;
            int32_t petDisplayID = CGlueMgr::GetTotemDisplayId(data.m_preferences.m_classId, data.m_preferences.m_raceID);

            if (petDisplayID) {
                totem = 1;
            } else {
                petDisplayID = display.m_characterInfo.m_petDisplayInfoID;
            }

            if (petDisplayID > 0) {
                displayRec = g_creatureDisplayInfoDB.GetRecord(petDisplayID);

                if (displayRec) {
                    petModelRec = g_creatureModelDataDB.GetRecord(displayRec->m_modelID);

                    if (petModelRec) {
                        if (display.m_petModel) {
                            display.m_petModel->Release();
                        }

                        display.m_petModel = CCharacterSelection::m_modelFrame->GetScene()->CreateModel(petModelRec->m_modelName, 0, 3);
                    }
                }
            }

            if (display.m_petModel) {
                petScale = petModelRec->m_modelScale * displayRec->m_creatureModelScale;
                const CreatureFamilyRec* familyRec = g_creatureFamilyDB.GetRecord(display.m_characterInfo.m_petCreatureFamilyID);

                if (!totem && familyRec) {
                    uint32_t petLevel = display.m_characterInfo.m_petExperienceLevel;
                    uint32_t level = petLevel < static_cast<uint32_t>(familyRec->m_minScaleLevel) ? 0 : petLevel - familyRec->m_minScaleLevel;
                    uint32_t range = familyRec->m_maxScaleLevel - familyRec->m_minScaleLevel;

                    if (level > range) {
                        level = range;
                    }

                    petScale = static_cast<float>(level) / static_cast<float>(range) * (familyRec->m_maxScale - familyRec->m_minScale) + familyRec->m_minScale;

                    if (petModelRec->m_petScale > 0.0f) {
                        petScale = petModelRec->m_petScale * petScale;
                    }
                }

                display.m_petModel->m_lightingCallback = CCharacterSelection::GenericLightingCallback;
                display.m_petModel->m_lightingArg = CCharacterSelection::m_modelFrame->m_lightArray3;
                CCharacterComponent::ReplaceMonsterSkin(display.m_petModel, displayRec, petModelRec);

                C44Matrix transform(petScale);
                display.m_petModel->SetWorldTransform(transform);

                float alpha = static_cast<float>(displayRec->m_creatureModelAlpha) * 0.0039215689f;
                display.m_petAlpha = alpha;
                display.m_petModel->m_fadeAlpha = alpha;
            }
        }

        CCharacterSelection::m_characterCount++;
    }

    if (!noEffects) {
        if (display.m_characterInfo.m_flags & 0x2000) {
            FFX::SetEffect(1, CGlueMgr::m_deathEffect);
            int32_t glow = static_cast<uint8_t>(CMath::fuint8_n(s_ghostGlow));
            CGlueMgr::m_deathEffect->SetParameters(1, &glow);
        } else {
            FFX::SetEffect(1, CGlueMgr::m_glowEffect);
            int32_t params[3];
            params[0] = 0;
            params[1] = static_cast<uint8_t>(CMath::fuint8_n(s_characterGlow));
            params[2] = 0;
            CGlueMgr::m_glowEffect->SetParameters(3, params);
        }
    }

    CM2Model* background = CCharacterSelection::m_modelFrame->m_model;

    if (background) {
        display.m_component->m_data.m_model->AttachToParent(background, 0, NULL, 0);
        CharacterSelectionDisplay::SetCharacterFacing(0.0f);

        if (display.m_petModel) {
            display.m_petModel->AttachToParent(background, 1, NULL, 0);
        }
    }

    char loadingScreen[260];

    if (LoadingScreenGetBackgroundTexturePath(display.m_characterInfo.m_mapID, loadingScreen, sizeof(loadingScreen), NULL)) {
        AsyncFileReadCreateObject(loadingScreen, 5, 0, 0, NULL, NULL, NULL, AsyncFileReadDestroyObject, AsyncFileReadDestroyObject, 0, 0, 0);
    }

    CCharacterSelection::StartLoadAnimation(1);
}

// OFFSET: 0x0080A110/0x1405202E0
void CCharacterSelection::SelectCharacter(int32_t index) {
    if (CCharacterSelection::m_characterList.Count() <= 0) {
        return;
    }

    int32_t selection = index < 0 ? 0 : index;

    if (selection >= static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        selection = 0;
    }

    if (selection == CCharacterSelection::m_selectionIndex) {
        return;
    }

    if (CCharacterSelection::m_selectionIndex >= 0 && CCharacterSelection::m_selectionIndex < static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        CharacterSelectionDisplay& previous = CCharacterSelection::m_characterList.m_data[CCharacterSelection::m_selectionIndex];

        if (previous.m_component) {
            CGlueMgr::StopCharacterEmoteKits(previous.m_animKitManager);
        }
    }

    CCharacterSelection::m_selectionIndex = selection;
    g_lastCharacterIndexVar->Set(selection, true, false, false, true);
    CharSelectSwitchStartRaceDownload(CCharacterSelection::m_characterList.m_data[selection].m_characterInfo.m_raceID, CCharacterSelection::m_characterList.m_data[selection].m_characterInfo.m_classID);
    CCharacterSelection::ShowCharacter(0, 0);
    FrameScript_SignalEvent(GLUE_EVENT_UPDATE_SELECTED_CHARACTER, "%d", CCharacterSelection::m_selectionIndex + 1);
}

// OFFSET: 0x0080A370
CharacterSelectionDisplay::CharacterSelectionDisplay(const CharacterSelectionDisplay& source) : m_animKitManager(source.m_animKitManager) {
    memcpy(this, &source, offsetof(CharacterSelectionDisplay, unk_0194) + sizeof(uint32_t));
    this->m_emoteKit = source.m_emoteKit;
    this->m_petModel = source.m_petModel;
    this->m_petAlpha = source.m_petAlpha;
    this->m_weaponLoadOut = source.m_weaponLoadOut;
}

// OFFSET: 0x0080A670
void CCharacterSelection::EnumerateCharactersCallback(const CHARACTER_INFO& info, void* param) {
    CharacterSelectionDisplay* display = CCharacterSelection::m_characterList.New();
    memcpy(&display->m_characterInfo, &info, sizeof(info));

    if (info.m_firstLogin) {
        const CharStartOutfitRec* outfit = CCharacterSelection::FindStartOutfit(info.m_raceID, info.m_classID, info.m_sexID);

        if (outfit) {
            display->m_characterInfo.m_petDisplayInfoID = outfit->m_petDisplayID;
            display->m_characterInfo.m_petCreatureFamilyID = outfit->m_petFamilyID;
            display->m_characterInfo.m_petExperienceLevel = 1;
        }
    }

    LoadAddOnEnableState(info.m_name);
}

// INLINED
static void DetachCharacterModels() {
    if (!CCharacterSelection::m_modelFrame) {
        return;
    }

    CM2Model* model = CCharacterSelection::m_modelFrame->m_model;

    if (model) {
        model->DetachAllChildrenById(0);
    }

    if (CCharacterSelection::m_modelFrame) {
        CM2Model* model = CCharacterSelection::m_modelFrame->m_model;

        if (model) {
            model->DetachAllChildrenById(1);
        }
    }
}

// OFFSET: 0x0080A700
void CCharacterSelection::OnGetCharacterList() {
    CCharacterSelection::m_characterList.SetCount(0);
    CCharacterSelection::m_characterListRcvd = 0;
    ClientKillTimer(CCharacterSelection::m_emoteTimerEvent, CCharacterSelection::UpdateEmote, "UpdateEmote");
    CCharacterSelection::m_emoteTimerEvent = 0;
    CGlueLoading::CancelLoadCallback();
    DetachCharacterModels();
    CGlueMgr::GetCharacterList();
}

// OFFSET: 0x0080A780
void CCharacterSelection::UpdateCharacterList(const TSFixedArray<CHARACTER_INFO>& characters) {
    ClearAddOnEnableState(NULL);
    CCharacterSelection::m_characterList.SetCount(0);
    CCharacterSelection::m_selectionIndex = -1;
    CCharacterSelection::m_characterListRcvd = 0;
    ClientKillTimer(CCharacterSelection::m_emoteTimerEvent, CCharacterSelection::UpdateEmote, "UpdateEmote");
    CCharacterSelection::m_emoteTimerEvent = 0;
    CGlueLoading::CancelLoadCallback();

    if (CCharacterSelection::m_characterList.Count() + characters.m_count > CCharacterSelection::m_characterList.m_alloc) {
        CCharacterSelection::m_characterList.ReallocData(CCharacterSelection::m_characterList.Count() + characters.m_count);
    }

    for (uint32_t i = 0; i < characters.m_count; i++) {
        CCharacterSelection::EnumerateCharactersCallback(characters.m_data[i], NULL);
    }

    CCharacterSelection::m_characterListRcvd = 1;

    if (!CCharacterSelection::m_characterList.Count()) {
        FrameScript_SignalEvent(GLUE_EVENT_UPDATE_SELECTED_CHARACTER, "%d", 0);
        DetachCharacterModels();
    } else {
        s_unk_D07A04 = 0;
        int32_t index = g_lastCharacterIndexVar->m_intValue;

        if (index >= static_cast<int32_t>(CCharacterSelection::m_characterList.Count() - 1)) {
            index = CCharacterSelection::m_characterList.Count() - 1;
        }

        FrameScript_SignalEvent(GLUE_EVENT_UPDATE_SELECTED_CHARACTER, "%d", (index <= 0 ? 0 : index) + 1);
    }

    FrameScript_SignalEvent(GLUE_EVENT_CHARACTER_LIST_UPDATE, "%d", CCharacterSelection::m_characterList.Count());
}

// OFFSET: 0x0080A8C0/0x140520BC0
void CCharacterSelection::ClearCharacterList() {
    CCharacterSelection::m_characterList.SetCount(0);
    CCharacterSelection::m_characterListRcvd = 0;
    ClientKillTimer(CCharacterSelection::m_emoteTimerEvent, CCharacterSelection::UpdateEmote, "UpdateEmote");
    CCharacterSelection::m_emoteTimerEvent = 0;
    CGlueLoading::CancelLoadCallback();
    DetachCharacterModels();
    CCharacterSelection::SelectCharacter(-1);

    if (CCharacterSelection::m_modelFrame) {
        CCharacterSelection::m_modelFrame->SetCameraByIndex(0);
    }
}

// OFFSET: 0x00807E50
void CCharacterSelection::SetModelFrame(CSimpleModelFFX* modelFrame) {
    CCharacterSelection::m_modelFrame = modelFrame;
}
