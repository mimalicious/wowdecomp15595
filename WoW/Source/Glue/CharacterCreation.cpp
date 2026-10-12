#include "CharacterCreation.h"
#include <cstring>
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CharBaseInfoRec.h"
#include "WoW/Source/DB/Rec/CharSectionsRec.h"
#include "WoW/Source/DB/Rec/CharStartOutfitRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/FactionGroupRec.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/Game/GameClient/CDrawBowString.h"
#include "WoW/Source/Glue/CGlueLoading.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Glue/CharacterSelection.h"
#include "WoW/Source/Glue/ComponentData.h"
#include "WoW/Source/Glue/CSimpleModelFFX.h"
#include "WoW/Source/Glue/Types.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/Object/ObjectClient/AnimKitManager.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "WoW/Source/WowServices/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00D07A98/0x140BB8B80
int32_t CCharacterCreation::m_selectedClassID = 1;

// STATIC: 0x00D07A9C/0x140BB8B84
int32_t CCharacterCreation::m_existingCharacterIndex = -1;

// STATIC: 0x00D07C04/0x140BB8E30
static int32_t s_standAnimKitID = 541;

// STATIC: 0x00EBC0D8/0x140DACF70
CharacterPreferences* CCharacterCreation::m_charPreferences[24][2];

// STATIC: 0x00EBC198/0x140DAD0F0
uint32_t CCharacterCreation::m_raceIndex;

// STATIC: 0x00EBC19C/0x140DAD0F8
CSimpleModelFFX* CCharacterCreation::m_charCustomizeFrame;

// STATIC: 0x00EBC1A0/0x140DAD0F4
float CCharacterCreation::m_charFacing;

// STATIC: 0x00EBC1A4/0x140DAD100
int32_t CCharacterCreation::m_alteredFormAtFront;

// STATIC: 0x00EBC1A8/0x140DAD104
uint32_t CCharacterCreation::m_prevSkinIndex;

// STATIC: 0x00EBC1AC/0x140DAD108
uint32_t CCharacterCreation::m_prevFaceIndex;

// STATIC: 0x00EBC1B0/0x140DAD10C
uint32_t CCharacterCreation::m_prevHairColorIndex;

// STATIC: 0x00EBC1B4/0x140DAD110
uint32_t CCharacterCreation::m_prevHairStyleIndex;

// STATIC: 0x00EBC1B8/0x140DAD114
uint32_t CCharacterCreation::m_prevFacialFeatureIndex;

// STATIC: 0x00EBC1BC/0x140DAD120
CCharacterComponent* CCharacterCreation::m_character;

// STATIC: 0x00EBC1C0/0x140DAD128
CCharacterComponent* CCharacterCreation::m_alteredCharacter;

// STATIC: 0x00EBC1C4/0x140DAD130
CAnimKitManager* CCharacterCreation::m_animKitManager;

// STATIC: 0x00EBC1C8/0x140DAD138
CAnimKitInst* CCharacterCreation::m_standAnimKit;

// STATIC: 0x00EBC1CC/0x140DAD140
TSFixedArray<const ChrClassesRec*> CCharacterCreation::m_classes;

// STATIC: 0x00EBC1F4/0x140DAD170
CM2Model* CCharacterCreation::m_petModel;

// STATIC: 0x00EBC1F8/0x140DAD178
TSGrowableArray<int32_t> CCharacterCreation::m_races;

// STATIC: 0x00EBC208/0x140DAD190
CNameGen CCharacterCreation::m_nameGen(g_rndSeed);

// STATIC: 0x00EBC22C/0x140DAD1C8
TSGrowableArray<CAsyncObject*> CCharacterCreation::m_racialTexturesLoading;

// STATIC: 0x00EBC23C/0x140DAD1E0
CGlueMgr::WeaponLoadOut CCharacterCreation::m_weaponLoadOut;

// OFFSET: 0x0080C050/0x140522680
void CCharacterCreation::SetCharCustomizeFrame(CSimpleModelFFX* modelFrame) {
    CCharacterCreation::m_charCustomizeFrame = modelFrame;
}

// OFFSET: 0x0080C060/0x140522690
void CCharacterCreation::UndressComponent(CCharacterComponent* component) {
    if (!component) {
        return;
    }

    for (uint32_t i = 0; i < 12; i++) {
        component->RemoveItem(static_cast<ITEM_SLOT>(i));
    }

    component->RemoveHandItem(INVSLOT_MAINHAND, SHEATHE_NONE, false);
    component->RemoveHandItem(INVSLOT_OFFHAND, SHEATHE_NONE, false);
    component->RemoveHandItem(INVSLOT_OFFHAND, SHEATHE_NONE, true);
    component->RemoveHandItem(INVSLOT_RANGED, SHEATHE_NONE, false);
}

// OFFSET: 0x0080C0C0/0x140522710
int32_t CCharacterCreation::GetDefaultSlotByType(int32_t inventoryType) {
    switch (inventoryType) {
        case 1:
            return 0;

        case 3:
            return 2;

        case 4:
            return 3;

        case 5:
        case 20:
            return 4;

        case 6:
            return 5;

        case 7:
            return 6;

        case 8:
            return 7;

        case 9:
            return 8;

        case 10:
            return 9;

        case 13:
        case 17:
        case 21:
            return 15;

        case 14:
        case 22:
        case 23:
            return 16;

        case 15:
        case 25:
        case 26:
            return 17;

        case 16:
            return 14;

        case 19:
            return 18;

        default:
            return -1;
    }
}

// OFFSET: 0x0080C3F0/0x140522AB0
int32_t CCharacterCreation::GetSelectedSex() {
    if (CCharacterCreation::m_character) {
        return CCharacterCreation::m_character->m_data.m_preferences.m_sexID;
    }

    return 0;
}

// OFFSET: 0x0080C400/0x140522AD0
void CCharacterCreation::GetRandomRaceAndSex(ComponentData* data) {
    int32_t races[24];
    data->m_preferences.m_sexID = static_cast<uint32_t>((static_cast<uint64_t>(2u) * CRandom::uint32_(g_rndSeed)) >> 32);
    int32_t count = 0;
    int32_t expansion = ClientServices::Connection()->m_accountExpansion;

    for (uint32_t i = 0; i < CCharacterCreation::m_races.Count(); i++) {
        const ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(CCharacterCreation::m_races.m_data[i]);

        if (expansion >= raceRec->m_requiredExpansion) {
            races[count++] = raceRec->m_id;
        }
    }

    uint32_t index = count <= 0 ? 0 : static_cast<uint32_t>((static_cast<uint64_t>(static_cast<uint32_t>(count)) * CRandom::uint32_(g_rndSeed)) >> 32);
    data->m_preferences.m_raceID = races[index];
}

// OFFSET: 0x0080C4D0/0x140522BD0
bool IsRaceClassValid(int32_t raceID, int32_t classID) {
    for (uint32_t i = 0; i < static_cast<uint32_t>(g_charBaseInfoDB.m_numRecords); i++) {
        const CharBaseInfoRec* rec = g_charBaseInfoDB.GetRecordByIndex(i);

        if (rec && static_cast<uint8_t>(rec->m_raceID) == raceID && static_cast<uint8_t>(rec->m_classID) == classID) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x0080C530/0x140522C30
void CCharacterCreation::SetCharCustomizeModel(const char* modelName) {
    if (!CCharacterCreation::m_charCustomizeFrame || !modelName || !*modelName) {
        return;
    }

    CM2Model* current = CCharacterCreation::m_charCustomizeFrame->m_model;

    if (current) {
        if (!_stricmp(modelName, current->m_shared->m_fileName)) {
            return;
        }

        while (current->m_attachList) {
            current->m_attachList->DetachFromParent();
        }
    }

    CCharacterCreation::m_charCustomizeFrame->SetModel(modelName);
    CCharacterCreation::m_charCustomizeFrame->m_lightArray1[0].unk_01B5 = 1;

    CM2Model* model = CCharacterCreation::m_charCustomizeFrame->m_model;

    if (!model) {
        return;
    }

    char defaultName[260];

    while (true) {
        CCharacterCreation::m_charCustomizeFrame->m_lightArray1[0].unk_01BC = 1;
        model->m_lightingCallback = CCharacterSelection::GenericLightingCallback;
        model->m_lightingArg = CCharacterCreation::m_charCustomizeFrame->m_lightArray1;

        if (modelName == defaultName) {
            break;
        }

        model->WaitUntilLoaded();

        if (model->TexturesAreLocal()) {
            break;
        }

        model->MakeTexturesLocal();
        CharSelectGetBGModelPathFromRaceStr("CharacterSelect", defaultName);

        if (!_stricmp(modelName, defaultName)) {
            break;
        }

        modelName = defaultName;
        CCharacterCreation::m_charCustomizeFrame->SetModel(defaultName);
        CCharacterCreation::m_charCustomizeFrame->m_lightArray1[0].unk_01B5 = 1;
        model = CCharacterCreation::m_charCustomizeFrame->m_model;

        if (!model) {
            return;
        }
    }

    model->WaitUntilDrawable();

    if (!CCharacterCreation::m_character) {
        return;
    }

    CM2Model* characterModel = CCharacterCreation::m_character->m_data.m_model;
    CM2Model* alteredModel = CCharacterCreation::m_alteredCharacter ? CCharacterCreation::m_alteredCharacter->m_data.m_model : NULL;
    CM2Model* petModel = CCharacterCreation::m_petModel;

    if (alteredModel) {
        if (characterModel) {
            characterModel->AttachToParent(model, CCharacterCreation::m_alteredFormAtFront != 0, NULL, 0);
        }

        alteredModel->AttachToParent(model, CCharacterCreation::m_alteredFormAtFront == 0, NULL, 0);
    } else {
        CCharacterCreation::m_alteredFormAtFront = 0;

        if (characterModel) {
            characterModel->AttachToParent(model, 0, NULL, 0);
        }

        if (petModel) {
            petModel->AttachToParent(model, 1, NULL, 0);
        }
    }
}

// OFFSET: 0x0080C700/0x140522E90
void CCharacterCreation::BowModelLoadedCallback(CM2Model* model, void* arg) {
    if (model->HasEvent('TTW$') && model->HasEvent('BTW$')) {
        model->m_drawCallback = CDrawBowString::GlueDrawBowStringCB;
        model->m_drawArg = arg;
        model->m_fFlags = (model->m_fFlags & ~0x50000u) | 0x20000;
    }
}

// OFFSET: 0x0080C760/0x140522EF0
CGlueMgr::WeaponLoadOut CCharacterCreation::Dress() {
    CHARACTER_INFO info;

    if (CCharacterCreation::m_existingCharacterIndex >= 0) {
        CharacterSelectionDisplay* display = CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex);

        if (display) {
            memcpy(&info, &display->m_characterInfo, sizeof(info));
            info.m_flags |= 0x400;
            CGlueMgr::AddItemsFromCharInfo(&info, CCharacterCreation::m_alteredCharacter, NULL, CCharacterCreation::BowModelLoadedCallback);
            return CGlueMgr::AddItemsFromCharInfo(&info, CCharacterCreation::m_character, NULL, CCharacterCreation::BowModelLoadedCallback);
        }
    }

    const CharStartOutfitRec* outfit = CCharacterSelection::FindStartOutfit(CCharacterCreation::m_character->m_data.m_preferences.m_raceID, CCharacterCreation::m_selectedClassID, CCharacterCreation::m_character->m_data.m_preferences.m_sexID);
    CCharacterCreation::UndressComponent(CCharacterCreation::m_character);
    CCharacterCreation::UndressComponent(CCharacterCreation::m_alteredCharacter);

    if (!outfit) {
        CGlueMgr::WeaponLoadOut empty;
        empty.m_offHand = 0;
        empty.m_mainHand = 0;
        return empty;
    }

    memset(&info, 0, sizeof(info));
    info.m_flags |= 0x400;
    info.m_classID = CCharacterCreation::m_selectedClassID;

    for (int32_t i = 0; i < 24; i++) {
        int32_t displayID = outfit->m_displayItemID[i];
        int32_t inventoryType = outfit->m_inventoryType[i];
        int32_t slot = CCharacterCreation::GetDefaultSlotByType(inventoryType);

        if (slot != -1) {
            info.m_items[slot].m_inventoryType = inventoryType;
            info.m_items[slot].m_displayID = displayID;
        }
    }

    CGlueMgr::AddItemsFromCharInfo(&info, CCharacterCreation::m_alteredCharacter, NULL, CCharacterCreation::BowModelLoadedCallback);
    return CGlueMgr::AddItemsFromCharInfo(&info, CCharacterCreation::m_character, NULL, CCharacterCreation::BowModelLoadedCallback);
}

// OFFSET: 0x0080C8E0/0x1405230B0
void CCharacterCreation::SetCharacterFacing(float facing) {
    CCharacterComponent* component = CCharacterCreation::m_alteredCharacter && CCharacterCreation::m_alteredFormAtFront ? CCharacterCreation::m_alteredCharacter : CCharacterCreation::m_character;
    CCharacterCreation::m_charFacing = facing;

    if (component->m_data.m_model) {
        C3Vector position;
        component->m_data.m_model->SetWorldTransform(position, facing, 1.0f);
    }
}

// OFFSET: 0x0080C940/0x140523110
void CCharacterCreation::SetViewingAlteredForm(int32_t alteredForm) {
    if (!CCharacterCreation::m_alteredCharacter) {
        CCharacterCreation::m_alteredFormAtFront = 0;
        return;
    }

    if (alteredForm == CCharacterCreation::m_alteredFormAtFront) {
        return;
    }

    CCharacterCreation::m_alteredFormAtFront = alteredForm;
    CM2Model* alteredModel = CCharacterCreation::m_alteredCharacter->m_data.m_model;
    CM2Model* background = CCharacterCreation::m_charCustomizeFrame->m_model;

    if (!background || !CCharacterCreation::m_character->m_data.m_model || !alteredModel) {
        return;
    }

    CM2Model* characterModel = CCharacterCreation::m_character->m_data.m_model;
    characterModel->DetachFromParent();
    alteredModel->DetachFromParent();
    characterModel->AttachToParent(background, CCharacterCreation::m_alteredFormAtFront != 0, NULL, 0);
    alteredModel->AttachToParent(background, CCharacterCreation::m_alteredFormAtFront == 0, NULL, 0);

    C3Vector position;
    characterModel->SetWorldTransform(position, CCharacterCreation::m_alteredFormAtFront ? 0.0f : CCharacterCreation::m_charFacing, 1.0f);
    alteredModel->SetWorldTransform(position, CCharacterCreation::m_alteredFormAtFront ? CCharacterCreation::m_charFacing : 0.0f, 1.0f);
}

// OFFSET: 0x0080CA80/0x140523280
void CCharacterCreation::SetPet() {
    const CharStartOutfitRec* outfit = NULL;
    int32_t totem = 0;
    int32_t petDisplayID = CGlueMgr::GetTotemDisplayId(CCharacterCreation::m_selectedClassID, CCharacterCreation::m_character->m_data.m_preferences.m_raceID);

    if (petDisplayID) {
        totem = 1;
    } else {
        outfit = CCharacterSelection::FindStartOutfit(CCharacterCreation::m_character->m_data.m_preferences.m_raceID, CCharacterCreation::m_selectedClassID, CCharacterCreation::m_character->m_data.m_preferences.m_sexID);

        if (!outfit) {
            return;
        }

        petDisplayID = outfit->m_petDisplayID;
    }

    if (!petDisplayID) {
        return;
    }

    const CreatureDisplayInfoRec* displayRec = g_creatureDisplayInfoDB.GetRecord(petDisplayID);

    if (!displayRec) {
        return;
    }

    const CreatureModelDataRec* modelRec = g_creatureModelDataDB.GetRecord(displayRec->m_modelID);

    if (!modelRec) {
        return;
    }

    CCharacterCreation::m_petModel = CCharacterCreation::m_charCustomizeFrame->GetScene()->CreateModel(modelRec->m_modelName, 0, 3);

    if (!CCharacterCreation::m_petModel) {
        return;
    }

    float scale = modelRec->m_modelScale * displayRec->m_creatureModelScale;

    if (outfit) {
        const CreatureFamilyRec* familyRec = g_creatureFamilyDB.GetRecord(outfit->m_petFamilyID);

        if (familyRec && !totem) {
            scale = familyRec->m_minScale;

            if (modelRec->m_petScale > 0.0f) {
                scale = modelRec->m_petScale * scale;
            }
        }
    }

    CCharacterCreation::m_petModel->m_lightingCallback = CCharacterSelection::GenericLightingCallback;
    CCharacterCreation::m_petModel->m_lightingArg = CCharacterCreation::m_charCustomizeFrame->m_lightArray3;
    CCharacterComponent::ReplaceMonsterSkin(CCharacterCreation::m_petModel, displayRec, modelRec);
    CCharacterCreation::m_petModel->m_fFlags |= 0x4000;
    CCharacterCreation::m_petModel->SetWorldTransform(C44Matrix(scale));
    CCharacterCreation::m_petModel->m_fadeAlpha = static_cast<float>(displayRec->m_creatureModelAlpha) * 0.0039215689f;

    if (!CCharacterCreation::m_alteredCharacter) {
        CM2Model* background = CCharacterCreation::m_charCustomizeFrame->m_model;

        if (background) {
            CCharacterCreation::m_petModel->AttachToParent(background, 1, NULL, 0);
        }
    }
}

// OFFSET: 0x0080CCC0
void CCharacterCreation::UninitCharacterComponent(CCharacterComponent*& component) {
    if (!component) {
        return;
    }

    CM2Model* model = component->m_data.m_model;

    if (model) {
        if (model->m_attachParent) {
            model->DetachFromParent();
        }

        model->SetLoadedCallback(NULL, NULL);
        model->SetSequenceCallback(NULL, 0);
        model->SetSequenceStartCallback(NULL, 0);
        model->SetEventCallback(NULL, 0);
    }

    CCharacterComponent::FreeComponent(component);
    component = NULL;
}

// OFFSET: 0x0080CD30/0x140523540
void CCharacterCreation::InitCharacterComponent(CCharacterComponent*& component, ComponentData* data) {
    CreatureModelDataRec* modelRec = GetModelDataRec(data->m_preferences.m_raceID, data->m_preferences.m_sexID);

    if (!modelRec || !*modelRec->m_modelName) {
        return;
    }

    component = CCharacterComponent::AllocComponent();
    data->m_model = CCharacterCreation::m_charCustomizeFrame->GetScene()->CreateModel(modelRec->m_modelName, 0, 3);

    if (!data->m_model) {
        return;
    }

    CCharacterCreation::m_charCustomizeFrame->m_lightArray2[0].unk_01BC = 1;
    data->m_flags |= 0x2;
    component->Init(data, NULL);

    CM2Model* model = component->m_data.m_model;
    model->m_lightingCallback = CCharacterSelection::GenericLightingCallback;
    model->m_lightingArg = CCharacterCreation::m_charCustomizeFrame->m_lightArray2;
}

// OFFSET: 0x0080CDD0
bool CCharacterCreation::IsClassAvailable(int32_t classID) {
    for (uint32_t i = 0; i < CCharacterCreation::m_classes.Count(); i++) {
        if (CCharacterCreation::m_classes.m_data[i]->m_id == classID) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x0080CE10/0x140523630
void CCharacterCreation::RandomizeCharFeatures() {
    COMPONENT_CONTEXT context = static_cast<COMPONENT_CONTEXT>(0);

    if (CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex)) {
        context = static_cast<COMPONENT_CONTEXT>(1);
    }

    CCharacterCreation::m_character->SetRandomSkin(context);
    CCharacterCreation::m_character->SetRandomFace(context);
    CCharacterCreation::m_character->SetRandomHairColor(context);
    CCharacterCreation::m_character->SetRandomHairStyle(context);
    CCharacterCreation::m_character->SetRandomFacialFeature(context);
    CCharacterCreation::m_prevSkinIndex = CCharacterCreation::m_character->m_data.m_preferences.m_skinID;
    CCharacterCreation::m_prevFaceIndex = CCharacterCreation::m_character->m_data.m_preferences.m_faceId;
    CCharacterCreation::m_prevHairColorIndex = CCharacterCreation::m_character->m_data.m_preferences.m_hairColorId;
    CCharacterCreation::m_prevHairStyleIndex = CCharacterCreation::m_character->m_data.m_preferences.m_hairStyleId;
    CCharacterCreation::m_prevFacialFeatureIndex = CCharacterCreation::m_character->m_data.m_preferences.m_facialHairId;
}

// OFFSET: 0x0080CEA0/0x1405236E0
void CCharacterCreation::CreateCharacter(const char* name) {
    WOWGUID guid = { 0 };
    CharacterSelectionDisplay* display = NULL;

    if (CCharacterCreation::m_existingCharacterIndex >= 0) {
        display = CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex);

        if (!display) {
            return;
        }

        guid = display->m_characterInfo.m_guid;

        if (!name) {
            name = display->m_characterInfo.m_name;
        } else if (!_stricmp(display->m_characterInfo.m_name, name)) {
            goto validated;
        }
    }

    {
        int32_t result = 91;

        if (!name || (result = ClientServices::CharacterValidateName(name)) != 89) {
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText(ClientServices::GetErrorToken(result), -1, GENDER_NOT_APPLICABLE));
            return;
        }
    }

validated:
    CHARACTER_CREATE_INFO info;
    SStrCopy(info.m_name, name, sizeof(info.m_name));
    info.m_raceID = CCharacterCreation::m_character->m_data.m_preferences.m_raceID;
    info.m_sexID = CCharacterCreation::m_character->m_data.m_preferences.m_sexID;
    info.m_classID = CCharacterCreation::m_selectedClassID;
    info.m_skinID = CCharacterCreation::m_character->m_data.m_preferences.m_skinID;
    info.m_hairColorID = CCharacterCreation::m_character->m_data.m_preferences.m_hairColorId;
    info.m_hairStyleID = CCharacterCreation::m_character->m_data.m_preferences.m_hairStyleId;
    info.m_facialHairStyleID = CCharacterCreation::m_character->m_data.m_preferences.m_facialHairId;
    info.m_faceID = CCharacterCreation::m_character->m_data.m_preferences.m_faceId;
    info.m_outfitID = 0;

    if (!guid.m_guid || !display) {
        CGlueMgr::CreateCharacter(&info);
    } else if (display->m_characterInfo.m_customizeFlags & 0x100000) {
        CGlueMgr::RaceChange(guid, &info);
    } else if (display->m_characterInfo.m_customizeFlags & 0x10000) {
        CGlueMgr::FactionChange(guid, &info);
    } else {
        CGlueMgr::CustomizeCharacter(guid, &info);
    }
}

// OFFSET: 0x0080D020/0x1405238A0
void CCharacterCreation::StartAnimation(uint64_t param) {
    if (g_useAnimKitsOnGlueScreen) {
        if (!CCharacterCreation::m_standAnimKit) {
            CCharacterCreation::m_standAnimKit = CCharacterCreation::m_animKitManager->PlayAndMaintain(s_standAnimKitID, -1, 0);
        }

        if (param) {
            CGlueMgr::PlayCharacterEmoteKits(CCharacterCreation::m_selectedClassID, CCharacterCreation::m_raceIndex, CCharacterCreation::m_character->m_data.m_preferences.m_sexID, CCharacterCreation::m_weaponLoadOut, *CCharacterCreation::m_animKitManager, CCharacterCreation::m_charCustomizeFrame);
        }
    } else {
        CCharacterCreation::m_character->m_data.m_model->SetBoneSequence(0xFFFFFFFF, 0, 0, 0, 1.0f, 1, 1, 0, 0, 0);
    }

    if (CCharacterCreation::m_alteredCharacter) {
        CCharacterCreation::m_alteredCharacter->m_data.m_model->SetBoneSequence(0xFFFFFFFF, 0, 0, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// OFFSET: 0x0080D0E0/0x14051E1E0
int32_t CCharacterCreation::GetExistingRaceIndex() {
    CharacterSelectionDisplay* display = CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex);

    if (display) {
        for (uint32_t i = 0; i < CCharacterCreation::m_races.Count(); i++) {
            if (CCharacterCreation::m_races.m_data[i] == display->m_characterInfo.m_raceID) {
                return i;
            }
        }
    }

    return -1;
}

// OFFSET: 0x0080D120
int32_t CCharacterCreation::GetExistingClassIndex() {
    CharacterSelectionDisplay* display = CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex);

    if (display) {
        for (uint32_t i = 0; i < static_cast<uint32_t>(g_chrClassesDB.m_numRecords); i++) {
            if (g_chrClassesDB.GetRecordByIndex(i)->m_id == display->m_characterInfo.m_classID) {
                return i;
            }
        }
    }

    return -1;
}

// OFFSET: 0x0080D170/0x1405239D0
static int32_t FindFirstCharSection(int32_t raceID, int32_t sexID) {
    int32_t high = g_charSectionsDB.m_numRecords;
    int32_t low = 0;

    if (high <= 0) {
        return -1;
    }

    do {
        int32_t mid = (high + low) / 2;
        const CharSectionsRec* rec = g_charSectionsDB.GetRecordByIndex(mid);

        if (rec->m_raceID < raceID || (rec->m_raceID == raceID && rec->m_sexID < sexID)) {
            low = mid + 1;
        } else {
            high = mid;
        }
    } while (low < high);

    if (low < g_charSectionsDB.m_numRecords) {
        const CharSectionsRec* rec = g_charSectionsDB.GetRecordByIndex(low);

        if (rec->m_raceID == raceID && rec->m_sexID == sexID) {
            return low;
        }
    }

    return -1;
}

// OFFSET: 0x0080D200/0x140523A70
void CCharacterCreation::TextureMakeLocalFinishedCallback(CAsyncObject* object) {
    CCharacterCreation::m_racialTexturesLoading.m_data[reinterpret_cast<uintptr_t>(object->m_userArg)] = NULL;
    AsyncFileReadDestroyObject(object);
}

// OFFSET: 0x0080D230/0x140523A90
int32_t CCharacterCreation::Idle(const void* data, void* param) {
    if (CCharacterCreation::m_charCustomizeFrame) {
        LinkModelAndTexturesToAsyncHead(CCharacterCreation::m_charCustomizeFrame->m_model);
    }

    AsyncFileReadEnterQueueLock();

    uint32_t count = CCharacterCreation::m_racialTexturesLoading.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (CCharacterCreation::m_racialTexturesLoading.m_data[i]) {
            AsyncFileReadLinkToPriority(CCharacterCreation::m_racialTexturesLoading.m_data[i], 1);
        }
    }

    AsyncFileReadLeaveQueueLock();
    LinkModelAndTexturesToAsyncHead(CCharacterCreation::m_petModel);

    if (CCharacterCreation::m_alteredFormAtFront) {
        if (CCharacterCreation::m_character) {
            CCharacterCreation::m_character->LinkToAsyncHead();
            LinkModelAndTexturesToAsyncHead(CCharacterCreation::m_character->m_data.m_model);
        }

        if (CCharacterCreation::m_alteredCharacter) {
            CCharacterCreation::m_alteredCharacter->LinkToAsyncHead();
            LinkModelAndTexturesToAsyncHead(CCharacterCreation::m_alteredCharacter->m_data.m_model);
        }
    } else {
        if (CCharacterCreation::m_alteredCharacter) {
            CCharacterCreation::m_alteredCharacter->LinkToAsyncHead();
            LinkModelAndTexturesToAsyncHead(CCharacterCreation::m_alteredCharacter->m_data.m_model);
        }

        if (CCharacterCreation::m_character) {
            CCharacterCreation::m_character->LinkToAsyncHead();
            LinkModelAndTexturesToAsyncHead(CCharacterCreation::m_character->m_data.m_model);
        }
    }

    CCharacterComponent* components[2];
    components[0] = CCharacterCreation::m_character;
    components[1] = CCharacterCreation::m_alteredCharacter;
    CGlueLoading::Idle(components, 2);
    return 1;
}

// OFFSET: 0x0080DE90/0x140524A90
int32_t CCharacterCreation::GetRandomClassID() {
    int32_t classes[10];
    int32_t count = 0;
    int32_t expansion = ClientServices::Connection()->m_accountExpansion;

    for (uint32_t i = 0; i < CCharacterCreation::m_classes.Count(); i++) {
        const ChrClassesRec* classRec = i >= CCharacterCreation::m_classes.Count() ? NULL : CCharacterCreation::m_classes.m_data[i];

        if (expansion >= classRec->m_requiredExpansion) {
            classes[count++] = classRec->m_id;
        }
    }

    uint32_t index = count <= 0 ? 0 : static_cast<uint32_t>((static_cast<uint64_t>(static_cast<uint32_t>(count)) * CRandom::uint32_(g_rndSeed)) >> 32);
    return classes[index];
}

// OFFSET: 0x0080DF10/0x140524B30
void CCharacterCreation::OnShow() {
    EventRegisterEx(EVENT_ID_IDLE, CCharacterCreation::Idle, NULL, 0.0f);
}

// OFFSET: 0x0080DF30/0x140524B50
void CCharacterCreation::UninitCharacterComponents() {
    if (CCharacterCreation::m_standAnimKit) {
        CGlueMgr::StopCharacterEmoteKits(*CCharacterCreation::m_animKitManager);
        CCharacterCreation::m_standAnimKit->Stop(1, 0);
        CCharacterCreation::m_standAnimKit->Release();
        CCharacterCreation::m_standAnimKit = NULL;
    }

    if (CCharacterCreation::m_animKitManager) {
        CAnimKitManager::Free(CCharacterCreation::m_animKitManager);
        CCharacterCreation::m_animKitManager = NULL;
    }

    CCharacterCreation::UninitCharacterComponent(CCharacterCreation::m_character);
    CCharacterCreation::UninitCharacterComponent(CCharacterCreation::m_alteredCharacter);

    if (CCharacterCreation::m_petModel) {
        if (CCharacterCreation::m_petModel->m_attachParent) {
            CCharacterCreation::m_petModel->DetachFromParent();
        }

        CCharacterCreation::m_petModel->Release();
        CCharacterCreation::m_petModel = NULL;
    }

    if (CCharacterCreation::m_charCustomizeFrame) {
        CCharacterCreation::m_charCustomizeFrame->GetScene()->GarbageCollectModels();
    }
}

// OFFSET: 0x0080DFE0/0x140524CF0
void CCharacterCreation::InitCharacterComponents(ComponentData* data, bool randomize) {
    CCharacterCreation::InitCharacterComponent(CCharacterCreation::m_character, data);
    CCharacterCreation::m_animKitManager = CAnimKitManager::Alloc();
    CCharacterCreation::m_animKitManager->SetGlueMode(1);
    CCharacterCreation::m_animKitManager->SetModel(data->m_model, 1);

    if (!g_useAnimKitsOnGlueScreen) {
        CCharacterCreation::m_character->m_data.m_model->SetBoneSequence(0xFFFFFFFF, 0, 0, 0, 1.0f, 1, 1, 0, 0, 0);
    }

    if (randomize) {
        CCharacterCreation::RandomizeCharFeatures();
    }

    const ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(data->m_preferences.m_raceID);

    if (raceRec && raceRec->m_alteredFormRaceID) {
        CharacterPreferences preferences;
        data->GetPreferences(&preferences);

        ComponentData alteredData;
        alteredData.m_preferences.m_sexID = preferences.m_sexID;
        alteredData.m_preferences.m_classId = preferences.m_classId;
        alteredData.m_preferences.m_hairColorId = preferences.m_hairColorId;
        alteredData.m_preferences.m_skinID = preferences.m_skinID;
        alteredData.m_preferences.m_faceId = preferences.m_faceId;
        alteredData.m_preferences.m_facialHairId = preferences.m_facialHairId;
        alteredData.m_preferences.m_hairStyleId = preferences.m_hairStyleId;
        alteredData.m_preferences.m_raceID = raceRec->m_alteredFormRaceID;
        COMPONENT_CONTEXT validateContext = static_cast<COMPONENT_CONTEXT>(0);

        if (CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex)) {
            validateContext = static_cast<COMPONENT_CONTEXT>(1);
        }

        CCharacterComponent::ValidateComponentData(&alteredData, validateContext);
        CCharacterCreation::InitCharacterComponent(CCharacterCreation::m_alteredCharacter, &alteredData);
    } else {
        CCharacterCreation::m_alteredFormAtFront = 0;
    }

    if (CCharacterCreation::m_alteredCharacter) {
        CCharacterCreation::m_alteredCharacter->m_data.m_model->SetBoneSequence(0xFFFFFFFF, 0, 0, 0, 1.0f, 1, 1, 0, 0, 0);

        if (CCharacterCreation::m_character && CCharacterCreation::m_alteredCharacter) {
            COMPONENT_CONTEXT copyContext = static_cast<COMPONENT_CONTEXT>(0);

            if (CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex)) {
                copyContext = static_cast<COMPONENT_CONTEXT>(1);
            }

            CCharacterCreation::m_character->CopyChoicesToAlteredFormComponent(CCharacterCreation::m_alteredCharacter, copyContext);
        }
    }

    CCharacterComponent* front = CCharacterCreation::m_alteredCharacter && CCharacterCreation::m_alteredFormAtFront ? CCharacterCreation::m_alteredCharacter : CCharacterCreation::m_character;

    if (front->m_data.m_model) {
        C3Vector position;
        front->m_data.m_model->SetWorldTransform(position, CCharacterCreation::m_charFacing, 1.0f);
    }

    CCharacterCreation::m_weaponLoadOut = CCharacterCreation::Dress();
    CCharacterCreation::m_character->RenderPrep(0);

    if (CCharacterCreation::m_alteredCharacter) {
        CCharacterCreation::m_alteredCharacter->RenderPrep(0);
    }

    CM2Model* background = CCharacterCreation::m_charCustomizeFrame->m_model;

    if (!background) {
        return;
    }

    front = CCharacterCreation::m_alteredCharacter && CCharacterCreation::m_alteredFormAtFront ? CCharacterCreation::m_alteredCharacter : CCharacterCreation::m_character;
    front->m_data.m_model->AttachToParent(background, 0, NULL, 0);

    if (CCharacterCreation::m_alteredCharacter) {
        CCharacterComponent* back = CCharacterCreation::m_alteredFormAtFront ? CCharacterCreation::m_character : CCharacterCreation::m_alteredCharacter;

        if (back) {
            back->m_data.m_model->AttachToParent(background, 1, NULL, 0);
        }
    }
}

// OFFSET: 0x0080E5E0/0x1405253F0
bool CCharacterCreation::SetSelectedClass(int32_t classID) {
    if (!CCharacterCreation::IsClassAvailable(classID)) {
        return false;
    }

    CGlueMgr::StopCharacterEmoteKits(*CCharacterCreation::m_animKitManager);
    CCharacterCreation::m_selectedClassID = classID;

    ComponentData data;
    data.m_preferences.m_raceID = CCharacterCreation::m_character->m_data.m_preferences.m_raceID;
    data.m_preferences.m_sexID = CCharacterCreation::m_character->m_data.m_preferences.m_sexID;
    data.m_preferences.m_classId = classID;
    data.m_preferences.m_skinID = CCharacterCreation::m_character->m_data.m_preferences.m_skinID;
    data.m_preferences.m_hairStyleId = CCharacterCreation::m_character->m_data.m_preferences.m_hairStyleId;
    data.m_preferences.m_hairColorId = CCharacterCreation::m_character->m_data.m_preferences.m_hairColorId;
    data.m_preferences.m_facialHairId = CCharacterCreation::m_character->m_data.m_preferences.m_facialHairId;
    data.m_preferences.m_faceId = CCharacterCreation::m_character->m_data.m_preferences.m_faceId;
    COMPONENT_CONTEXT validateContext = static_cast<COMPONENT_CONTEXT>(0);

    if (CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex)) {
        validateContext = static_cast<COMPONENT_CONTEXT>(1);
    }

    CCharacterComponent::ValidateComponentData(&data, validateContext);

    if (CCharacterCreation::m_character->CheckForComponentChanges(&data)) {
        CCharacterCreation::UninitCharacterComponents();
        CCharacterCreation::InitCharacterComponents(&data, false);
    }

    if (CCharacterCreation::m_petModel) {
        if (CCharacterCreation::m_petModel->m_attachParent) {
            CCharacterCreation::m_petModel->DetachFromParent();
        }

        CCharacterCreation::m_petModel->Release();
        CCharacterCreation::m_petModel = NULL;
    }

    CharSelectSwitchStartRaceDownload(data.m_preferences.m_raceID, data.m_preferences.m_classId);
    CCharacterCreation::SetPet();
    CCharacterCreation::m_weaponLoadOut = CCharacterCreation::Dress();

    CCharacterComponent* components[2];
    components[0] = CCharacterCreation::m_character;
    components[1] = CCharacterCreation::m_alteredCharacter;
    CGlueLoading::StartLoad(components, 2, 1, CCharacterCreation::StartAnimation, 1, 5);
    return true;
}

// OFFSET: 0x0080E750/0x140525580
void CCharacterCreation::CycleCharCustomization(CHAR_CUSTOMIZATION_TYPE type, int32_t delta) {
    if (!delta) {
        return;
    }

    COMPONENT_CONTEXT context = static_cast<COMPONENT_CONTEXT>(0);

    if (CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex)) {
        context = static_cast<COMPONENT_CONTEXT>(1);
    }

    switch (type) {
        case CHAR_CUSTOMIZATION_SKIN:
            CCharacterCreation::m_character->SetNextSkin(context, delta);
            CCharacterCreation::m_prevSkinIndex = CCharacterCreation::m_character->m_data.m_preferences.m_skinID;
            break;

        case CHAR_CUSTOMIZATION_FACE:
            CCharacterCreation::m_character->SetNextFace(context, delta, CCharacterCreation::m_prevSkinIndex);
            break;

        case CHAR_CUSTOMIZATION_HAIR_STYLE:
            CCharacterCreation::m_character->SetNextHairStyle(context, delta);
            break;

        case CHAR_CUSTOMIZATION_HAIR_COLOR:
            CCharacterCreation::m_character->SetNextHairColor(context, delta);
            break;

        case CHAR_CUSTOMIZATION_FACIAL_FEATURE:
            CCharacterCreation::m_character->SetNextFacialFeature(context, delta);
            break;
    }

    if (CCharacterCreation::m_character && CCharacterCreation::m_alteredCharacter) {
        COMPONENT_CONTEXT copyContext = static_cast<COMPONENT_CONTEXT>(0);

        if (CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex)) {
            copyContext = static_cast<COMPONENT_CONTEXT>(1);
        }

        CCharacterCreation::m_character->CopyChoicesToAlteredFormComponent(CCharacterCreation::m_alteredCharacter, copyContext);
    }

    CCharacterComponent* components[2];
    components[0] = CCharacterCreation::m_character;
    components[1] = CCharacterCreation::m_alteredCharacter;
    CGlueLoading::StartLoad(components, 2, 1, CCharacterCreation::StartAnimation, 0, 5);
}

// OFFSET: 0x0080E870/0x1405256D0
void CCharacterCreation::RandomizeCharCustomization() {
    CCharacterCreation::RandomizeCharFeatures();

    if (CCharacterCreation::m_character && CCharacterCreation::m_alteredCharacter) {
        COMPONENT_CONTEXT copyContext = static_cast<COMPONENT_CONTEXT>(0);

        if (CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex)) {
            copyContext = static_cast<COMPONENT_CONTEXT>(1);
        }

        CCharacterCreation::m_character->CopyChoicesToAlteredFormComponent(CCharacterCreation::m_alteredCharacter, copyContext);
    }

    CCharacterCreation::m_weaponLoadOut = CCharacterCreation::Dress();

    CCharacterComponent* components[2];
    components[0] = CCharacterCreation::m_character;
    components[1] = CCharacterCreation::m_alteredCharacter;
    CGlueLoading::StartLoad(components, 2, 1, CCharacterCreation::StartAnimation, 0, 5);
}

// OFFSET: 0x0080E910/0x140525780
void CCharacterCreation::CancelRaceSexComponentTexturesPreload() {
    uint32_t count = CCharacterCreation::m_racialTexturesLoading.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (CCharacterCreation::m_racialTexturesLoading.m_data[i]) {
            AsyncFileReadCancel(CCharacterCreation::m_racialTexturesLoading.m_data[i], AsyncFileReadDestroyObject);
        }
    }

    CCharacterCreation::m_racialTexturesLoading.m_count = 0;
}

// OFFSET: 0x0080E960/0x1405257F0
void CCharacterCreation::PreloadRaceSexComponentTextures() {
    int32_t index = FindFirstCharSection(CCharacterCreation::m_character->m_data.m_preferences.m_raceID, CCharacterCreation::m_character->m_data.m_preferences.m_sexID);

    if (index < 0) {
        return;
    }

    CCharacterCreation::CancelRaceSexComponentTexturesPreload();

    for (; index < g_charSectionsDB.m_numRecords; index++) {
        const CharSectionsRec* rec = g_charSectionsDB.GetRecordByIndex(index);

        if (rec->m_raceID != CCharacterCreation::m_character->m_data.m_preferences.m_raceID || rec->m_sexID != CCharacterCreation::m_character->m_data.m_preferences.m_sexID) {
            break;
        }

        for (int32_t i = 0; i < 3; i++) {
            const char* textureName = rec->m_textureName[i];

            if (!textureName || !*textureName) {
                break;
            }

            CAsyncObject* object;
            AsyncFileReadCreateObject(textureName, 5, 0, 0, &object, NULL, reinterpret_cast<void*>(static_cast<uintptr_t>(CCharacterCreation::m_racialTexturesLoading.Count())), CCharacterCreation::TextureMakeLocalFinishedCallback, CCharacterCreation::TextureMakeLocalFinishedCallback, 0, 0, 0);

            CAsyncObject* loading = object;

            if (object) {
                CCharacterCreation::m_racialTexturesLoading.Add(1, &loading);
            }
        }
    }
}

// OFFSET: 0x0080EB90/0x140525B10
void CCharacterCreation::Initialize() {
    CCharacterCreation::m_charFacing = 0.0f;
    CCharacterCreation::m_existingCharacterIndex = -1;
    CCharacterCreation::m_alteredFormAtFront = 0;
    memset(CCharacterCreation::m_charPreferences, 0, sizeof(CCharacterCreation::m_charPreferences));
    CCharacterCreation::m_races.m_count = 0;

    for (int32_t faction = 0; faction <= 1; faction++) {
        for (int32_t i = 0; i < g_chrRacesDB.m_numRecords; i++) {
            const ChrRacesRec* raceRec = g_chrRacesDB.GetRecordByIndex(i);

            if (raceRec && (raceRec->m_flags & 0x1)) {
                continue;
            }

            const FactionTemplateRec* templateRec = g_factionTemplateDB.GetRecord(raceRec->m_factionID);

            if (!templateRec) {
                continue;
            }

            for (int32_t j = 0; j < g_factionGroupDB.m_numRecords; j++) {
                const FactionGroupRec* groupRec = g_factionGroupDB.GetRecordByIndex(j);

                if (!groupRec || !groupRec->m_maskID || !((1 << groupRec->m_maskID) & templateRec->m_factionGroup)) {
                    continue;
                }

                int32_t compare;

                if (faction == 0) {
                    compare = _stricmp(groupRec->m_internalName, "Alliance");
                } else if (faction == 1) {
                    compare = _stricmp(groupRec->m_internalName, "Horde");
                } else {
                    continue;
                }

                if (!compare) {
                    int32_t raceID = raceRec->m_id;
                    CCharacterCreation::m_races.Add(1, &raceID);
                }
            }
        }
    }
}

// OFFSET: 0x0080ED80
void CCharacterCreation::Shutdown() {
    CCharacterCreation::UninitCharacterComponents();
    CCharacterCreation::m_nameGen.Destroy();

    for (uint32_t i = 0; i < 24; i++) {
        for (uint32_t j = 0; j < 2; j++) {
            if (CCharacterCreation::m_charPreferences[i][j]) {
                SMemFree(CCharacterCreation::m_charPreferences[i][j], "delete", -1, 0);
            }

            CCharacterCreation::m_charPreferences[i][j] = NULL;
        }
    }

    CCharacterCreation::CancelRaceSexComponentTexturesPreload();
}

// OFFSET: 0x0080EDE0/0x140525F10
void CCharacterCreation::OnHide() {
    CCharacterCreation::CancelRaceSexComponentTexturesPreload();
    EventUnregister(EVENT_ID_IDLE, CCharacterCreation::Idle);
    LoadingScreenMiniDisable();
}

// INLINED
static CharacterPreferences*& GetCurrentPreferences() {
    return CCharacterCreation::m_charPreferences[CCharacterCreation::m_races.m_data[CCharacterCreation::m_raceIndex]][CCharacterCreation::m_character->m_data.m_preferences.m_sexID];
}

// OFFSET: 0x0080EE10/0x140525F40
void CCharacterCreation::SetSelectedSex(uint32_t sexID) {
    if (sexID >= 3 || sexID == CCharacterCreation::m_character->m_data.m_preferences.m_sexID) {
        return;
    }

    CharacterPreferences*& preferences = GetCurrentPreferences();

    if (!preferences) {
        preferences = new CharacterPreferences;
    }

    CCharacterCreation::m_character->GetPreferences(preferences);

    ComponentData data;
    data.m_preferences.m_raceID = CCharacterCreation::m_character->m_data.m_preferences.m_raceID;
    data.m_preferences.m_sexID = sexID;
    data.m_preferences.m_classId = CCharacterCreation::m_selectedClassID;

    if (CCharacterCreation::m_existingCharacterIndex >= 0) {
        CharacterSelectionDisplay* display = CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex);

        if (display && display->m_characterInfo.m_sexID == sexID && (display->m_characterInfo.m_customizeFlags & 0x1)) {
            data.m_preferences.m_raceID = display->m_characterInfo.m_raceID;
            data.m_preferences.m_sexID = display->m_characterInfo.m_sexID;
            data.m_preferences.m_classId = display->m_characterInfo.m_classID;
            data.m_preferences.m_skinID = display->m_characterInfo.m_skinColorID;
            data.m_preferences.m_hairStyleId = display->m_characterInfo.m_hairStyleID;
            data.m_preferences.m_hairColorId = display->m_characterInfo.m_hairColorID;
            data.m_preferences.m_facialHairId = display->m_characterInfo.m_facialHairStyleID;
            data.m_preferences.m_faceId = display->m_characterInfo.m_faceID;
            CCharacterCreation::UninitCharacterComponents();
            CCharacterCreation::InitCharacterComponents(&data, false);
            goto loaded;
        }
    }

    {
        CharacterPreferences* stored = CCharacterCreation::m_charPreferences[CCharacterCreation::m_races.m_data[CCharacterCreation::m_raceIndex]][sexID];

        if (stored) {
            data.SetPreferences(stored);
            data.m_preferences.m_classId = CCharacterCreation::m_selectedClassID;
            COMPONENT_CONTEXT validateContext = static_cast<COMPONENT_CONTEXT>(0);

            if (CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex)) {
                validateContext = static_cast<COMPONENT_CONTEXT>(1);
            }

            CCharacterComponent::ValidateComponentData(&data, validateContext);
            CCharacterCreation::UninitCharacterComponents();
            CCharacterCreation::InitCharacterComponents(&data, false);
        } else {
            data.m_preferences.m_raceID = CCharacterCreation::m_character->m_data.m_preferences.m_raceID;
            data.m_preferences.m_sexID = sexID;
            data.m_preferences.m_classId = CCharacterCreation::m_selectedClassID;
            CCharacterCreation::UninitCharacterComponents();
            CCharacterCreation::InitCharacterComponents(&data, true);
        }
    }

loaded:
    CCharacterCreation::SetPet();
    CCharacterCreation::m_nameGen.LoadNames(CCharacterCreation::m_races.m_data[CCharacterCreation::m_raceIndex], sexID);

    CCharacterComponent* components[2];
    components[0] = CCharacterCreation::m_character;
    components[1] = CCharacterCreation::m_alteredCharacter;
    CGlueLoading::StartLoad(components, 2, 1, CCharacterCreation::StartAnimation, 1, 5);
    CCharacterCreation::PreloadRaceSexComponentTextures();
}

// OFFSET: 0x0080F0C0/0x1405262B0
void CCharacterCreation::CalcClasses(int32_t raceID) {
    uint32_t count = 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(g_charBaseInfoDB.m_numRecords); i++) {
        const CharBaseInfoRec* rec = g_charBaseInfoDB.GetRecordByIndex(i);

        if (rec && static_cast<uint8_t>(rec->m_raceID) == raceID) {
            count++;
        }
    }

    CCharacterCreation::m_classes.SetCount(count);
    uint32_t index = 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(g_charBaseInfoDB.m_numRecords); i++) {
        const CharBaseInfoRec* rec = g_charBaseInfoDB.GetRecordByIndex(i);

        if (rec && static_cast<uint8_t>(rec->m_raceID) == raceID) {
            CCharacterCreation::m_classes.m_data[index++] = g_chrClassesDB.GetRecord(static_cast<uint8_t>(rec->m_classID));
        }
    }
}

// OFFSET: 0x0080F1D0/0x140526400
void CCharacterCreation::ResetCharCustomizeInfo() {
    if (!CCharacterCreation::m_charCustomizeFrame) {
        return;
    }

    CCharacterCreation::m_existingCharacterIndex = -1;

    CM2Model* model = CCharacterCreation::m_charCustomizeFrame->m_model;

    if (model) {
        model->DetachAllChildrenById(0);
    }

    ComponentData data;
    CCharacterCreation::GetRandomRaceAndSex(&data);
    CCharacterCreation::CalcClasses(data.m_preferences.m_raceID);
    int32_t classID = CCharacterCreation::GetRandomClassID();
    data.m_preferences.m_classId = CCharacterCreation::m_selectedClassID;
    CCharacterCreation::UninitCharacterComponents();
    CCharacterCreation::InitCharacterComponents(&data, true);
    CCharacterCreation::SetSelectedClass(classID);

    CCharacterCreation::m_raceIndex = -1;

    for (uint32_t i = 0; i < CCharacterCreation::m_races.Count(); i++) {
        if (CCharacterCreation::m_races.m_data[i] == CCharacterCreation::m_character->m_data.m_preferences.m_raceID) {
            CCharacterCreation::m_raceIndex = i;
            break;
        }
    }

    CCharacterCreation::m_nameGen.LoadNames(CCharacterCreation::m_character->m_data.m_preferences.m_raceID, CCharacterCreation::m_character->m_data.m_preferences.m_sexID);
    CCharacterCreation::PreloadRaceSexComponentTextures();
}

// OFFSET: 0x0080F2B0/0x1405264F0
void CCharacterCreation::SetSelectedRace(uint32_t raceIndex) {
    if (raceIndex >= CCharacterCreation::m_races.Count() || raceIndex == CCharacterCreation::m_raceIndex) {
        return;
    }

    CharacterPreferences*& preferences = GetCurrentPreferences();

    if (!preferences) {
        preferences = new CharacterPreferences;
    }

    CCharacterCreation::m_character->GetPreferences(preferences);
    CCharacterCreation::m_raceIndex = raceIndex;

    ComponentData data;
    CharacterSelectionDisplay* display;

    if (CCharacterCreation::m_existingCharacterIndex >= 0 && (display = CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex)) != NULL && display->m_characterInfo.m_raceID == CCharacterCreation::m_races.m_data[CCharacterCreation::m_raceIndex] && (display->m_characterInfo.m_customizeFlags & 0x110000)) {
        data.m_preferences.m_raceID = display->m_characterInfo.m_raceID;
        data.m_preferences.m_sexID = display->m_characterInfo.m_sexID;
        data.m_preferences.m_classId = display->m_characterInfo.m_classID;
        data.m_preferences.m_skinID = display->m_characterInfo.m_skinColorID;
        data.m_preferences.m_hairStyleId = display->m_characterInfo.m_hairStyleID;
        data.m_preferences.m_hairColorId = display->m_characterInfo.m_hairColorID;
        data.m_preferences.m_facialHairId = display->m_characterInfo.m_facialHairStyleID;
        data.m_preferences.m_faceId = display->m_characterInfo.m_faceID;
        CCharacterCreation::UninitCharacterComponents();
        CCharacterCreation::InitCharacterComponents(&data, false);
        CCharacterCreation::SetSelectedSex(display->m_characterInfo.m_sexID);
    } else {
        CharacterPreferences* stored = CCharacterCreation::m_charPreferences[CCharacterCreation::m_races.m_data[raceIndex]][CCharacterCreation::m_character->m_data.m_preferences.m_sexID];

        if (stored) {
            data.SetPreferences(stored);
            CCharacterCreation::CalcClasses(data.m_preferences.m_raceID);
            int32_t classID = CCharacterCreation::m_selectedClassID;

            if (!IsRaceClassValid(data.m_preferences.m_raceID, CCharacterCreation::m_selectedClassID)) {
                classID = CCharacterCreation::GetRandomClassID();
                CCharacterCreation::m_selectedClassID = classID;
            }

            data.m_preferences.m_classId = classID;
            COMPONENT_CONTEXT validateContext = static_cast<COMPONENT_CONTEXT>(0);

            if (CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex)) {
                validateContext = static_cast<COMPONENT_CONTEXT>(1);
            }

            CCharacterComponent::ValidateComponentData(&data, validateContext);
            CCharacterCreation::UninitCharacterComponents();
            CCharacterCreation::InitCharacterComponents(&data, false);
        } else {
            data.m_preferences.m_sexID = CCharacterCreation::m_character->m_data.m_preferences.m_sexID;
            data.m_preferences.m_raceID = CCharacterCreation::m_races.m_data[raceIndex];
            CCharacterCreation::CalcClasses(data.m_preferences.m_raceID);

            if (!IsRaceClassValid(data.m_preferences.m_raceID, CCharacterCreation::m_selectedClassID)) {
                CCharacterCreation::SetSelectedClass(CCharacterCreation::GetRandomClassID());
            }

            data.m_preferences.m_classId = CCharacterCreation::m_selectedClassID;
            CCharacterCreation::UninitCharacterComponents();
            CCharacterCreation::InitCharacterComponents(&data, true);
        }
    }

    CharSelectSwitchStartRaceDownload(data.m_preferences.m_raceID, data.m_preferences.m_classId);
    CCharacterCreation::SetPet();
    CCharacterCreation::m_nameGen.LoadNames(CCharacterCreation::m_races.m_data[CCharacterCreation::m_raceIndex], CCharacterCreation::m_character->m_data.m_preferences.m_sexID);
    CCharacterCreation::m_weaponLoadOut = CCharacterCreation::Dress();

    CCharacterComponent* components[2];
    components[0] = CCharacterCreation::m_character;
    components[1] = CCharacterCreation::m_alteredCharacter;
    CGlueLoading::StartLoad(components, 2, 1, CCharacterCreation::StartAnimation, 1, 5);
    CCharacterCreation::PreloadRaceSexComponentTextures();
}

// OFFSET: 0x0080F5B0/0x140526870
void CCharacterCreation::SetToExistingCharacter(uint32_t index) {
    CharacterSelectionDisplay* display = CCharacterSelection::GetCharacterDisplay(index);

    if (!display) {
        return;
    }

    CCharacterCreation::m_existingCharacterIndex = index;

    ComponentData data;
    data.m_preferences.m_raceID = display->m_characterInfo.m_raceID;
    data.m_preferences.m_sexID = display->m_characterInfo.m_sexID;
    data.m_preferences.m_classId = display->m_characterInfo.m_classID;
    data.m_preferences.m_skinID = display->m_characterInfo.m_skinColorID;
    data.m_preferences.m_hairStyleId = display->m_characterInfo.m_hairStyleID;
    data.m_preferences.m_hairColorId = display->m_characterInfo.m_hairColorID;
    data.m_preferences.m_facialHairId = display->m_characterInfo.m_facialHairStyleID;
    data.m_preferences.m_faceId = display->m_characterInfo.m_faceID;
    CCharacterCreation::UninitCharacterComponents();
    CCharacterCreation::InitCharacterComponents(&data, false);
    CCharacterCreation::CalcClasses(data.m_preferences.m_raceID);
    CCharacterCreation::UninitCharacterComponents();
    CCharacterCreation::InitCharacterComponents(&data, false);
    CCharacterCreation::m_selectedClassID = display->m_characterInfo.m_classID;

    for (uint32_t i = 0; i < CCharacterCreation::m_races.Count(); i++) {
        if (CCharacterCreation::m_races.m_data[i] == display->m_characterInfo.m_raceID) {
            CCharacterCreation::m_raceIndex = i;
            break;
        }
    }

    CCharacterCreation::m_nameGen.LoadNames(CCharacterCreation::m_character->m_data.m_preferences.m_raceID, CCharacterCreation::m_character->m_data.m_preferences.m_sexID);

    CCharacterComponent* components[2];
    components[0] = CCharacterCreation::m_character;
    components[1] = CCharacterCreation::m_alteredCharacter;
    CGlueLoading::StartLoad(components, 2, 1, CCharacterCreation::StartAnimation, 0, 5);
}
