#include "CharacterModelBase.h"
#include <cmath>
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Services/Camera.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Common/ComponentCore/Types.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoExtraRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/Glue/ComponentData.h"
#include "WoW/Source/Glue/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/PortraitButton.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00D17688/0x140BD2540
const char* CGCharacterModelBase::s_objectTypeName = "PlayerModel";

// STATIC: 0x00D1768C/0x140BD2548
int32_t CGCharacterModelBase::s_metatable = LUA_NOREF;

// STATIC: 0x00F76C70/0x140E6CDB8
int32_t CGCharacterModelBase::s_objectType;

CDataAllocator CGCharacterModelBase::s_characterModelHeap(sizeof(CGCharacterModelBase), 5);

// OFFSET: 0x00A02EE0
static int32_t IsUnitDrawable(CGUnit_C* unit) {
    if (unit->m_clientFlags & 0x400000) {
        return 0;
    }

    if (unit->m_characterComponent && !unit->m_characterComponent->RenderPrep(0)) {
        return 0;
    }

    if (!unit->m_worldModel) {
        return 0;
    }

    return unit->m_worldModel->IsDrawableRecursive();
}

// INLINED
void CGCharacterModelBase::operator delete(void* ptr) {
    CGCharacterModelBase::s_characterModelHeap.PutData(ptr, NULL, 0);
}

// PROVEN: 6ac7a362 DIFFERENT/IDENTICAL
// OFFSET: 0x008C64D0/0x140636540
CSimpleFrame* CGCharacterModelBase::Create(CSimpleFrame* parent) {
    void* data = CGCharacterModelBase::s_characterModelHeap.GetData(0, typeid(CGCharacterModelBase).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGCharacterModelBase(parent) : NULL;
}

// OFFSET: [0x00A02C70/0x1407A1D50, 0x00A03130/0x1407A22D0, 0x00A06020/0x1407A42E0]
CGCharacterModelBase::~CGCharacterModelBase() {
    if (this->m_component) {
        CCharacterComponent::FreeComponent(this->m_component);
    }
}

// PROVEN: 596b5867 IDENTICAL/IDENTICAL
// OFFSET: 0x00A02CA0/0x1407A1D90
const char* CGCharacterModelBase::GetObjectTypeName() {
    return CGCharacterModelBase::s_objectTypeName;
}

// PROVEN: 65e855b0 IDENTICAL/IDENTICAL
// OFFSET: 0x00A02CB0/0x1407A1DA0
int32_t CGCharacterModelBase::GetScriptMetaTable() {
    return CGCharacterModelBase::s_metatable;
}

// PROVEN: ba8c5656 IDENTICAL/IDENTICAL
// OFFSET: 0x00A02CC0/0x1407A1DB0
CM2Model* CGCharacterModelBase::GetDuplicateUnitCharacterModel(CGUnit_C* unit) {
    if (unit) {
        return unit->DuplicateModel(this->GetScene());
    }

    return NULL;
}

// PROVEN: 823ef583 IDENTICAL/DIFFERENT
// OFFSET: 0x00A02CF0/0x1407A1DE0
void CGCharacterModelBase::RotateModel(float facing, bool animate) {
    if (this->m_model && this->m_model->IsLoaded()) {
        if (this->m_sequence || !animate) {
            this->m_facing = facing;
            return;
        }

        if (facing != this->m_facing) {
            uint32_t sequence = 11;

            if (!(facing > this->m_facing)) {
                sequence = 12;
            }

            if (this->m_model->HasSequence(sequence) && sequence != this->m_model->GetBoneSequenceId(-1)) {
                this->m_model->SetBoneSequence(-1, sequence, -1, 0, 1.0f, 1, 1, 1000, 0, 0);
            }
        }
    }

    if (animate) {
        this->m_sequenceSet = 1;
        this->m_sequenceResetTime = FrameTime::GetCurTimeMs() + 100;
    }

    this->m_facing = facing;
}

// PROVEN: 9b7efb3f IDENTICAL/DIFFERENT
// OFFSET: 0x00A02DD0/0x1407A1F10
void CGCharacterModelBase::UpdateModel() {
    CSimpleModel::UpdateModel();

    if (!this->m_sequenceSet && FrameTime::GetCurTimeMs() > this->m_sequenceResetTime && this->m_model->IsLoaded()) {
        uint32_t sequence = this->m_sequence;

        if (sequence != this->m_model->GetBoneSequenceId(-1)) {
            this->m_model->SetBoneSequence(-1, sequence, this->m_sequenceVariation, 0, 1.0f, 1, 1, 0, 0, 0);
            this->m_sequenceResetTime = 0;
        }
    }

    this->m_sequenceSet = 0;
}

// PROVEN: f77a3fda IDENTICAL/IDENTICAL
// OFFSET: 0x00A02E60/0x1407A2010
void CGCharacterModelBase::HideThis() {
    this->SetModel(static_cast<CM2Model*>(NULL));
    CSimpleFrame::HideThis();
}

// PROVEN: 18675790 IDENTICAL/IDENTICAL
// OFFSET: 0x00A02F20/0x1407A20B0
bool CGCharacterModelBase::IsA(int32_t type) {
    return type == CGCharacterModelBase::s_objectType || type == CSimpleModel::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: fd18a837 IDENTICAL/DIFFERENT
// OFFSET: 0x00A02F60/0x1407A20E0
bool CGCharacterModelBase::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CGCharacterModelBase::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CSimpleModel::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// OFFSET: 0x00A02FC0/0x1407A2130
CGCharacterModelBase::CGCharacterModelBase(CSimpleFrame* parent) : CSimpleModel(parent) {
    this->m_sequenceVariation = -1;
    this->m_portraitZoom = 0.0f;
    this->m_customRace = -1;
    this->m_customSex = -1;
    this->m_unitGuid = WOWGUID::Make(0);
    this->m_sequenceSet = 0;
    this->m_sequenceResetTime = 0;
    this->m_sequence = 0;
    this->m_creatureStats = NULL;
    this->m_displayID = 0;
    this->m_component = NULL;
    this->m_camDistanceScale = 1.0f;
    this->m_modelAlpha = 1.0f;
    this->m_barberShopAlternateForm = false;
    this->m_light.SetLightType(M2LIGHT_DIRECT);
    this->m_light.SetDirection(C3Vector(0.0f, -0.70710599f, -0.70710599f));
    this->m_light.m_dirColor = C3Vector(0.8f, 0.8f, 0.64f);
    this->m_light.m_ambColor = C3Vector(0.7f, 0.7f, 0.7f);
    this->m_light.SetDirection(C3Vector(0.0f, 1.0f, 0.0f));
    this->m_light.SetVisible(1);
}

// OFFSET: 0x00A03180/0x1407A2340
void CGCharacterModelBase::SetUpModel(int32_t displayID, CGPlayer_C* owner) {
    CreatureDisplayInfoRec* displayRec = g_creatureDisplayInfoDB.GetRecord(displayID);

    if (!displayRec) {
        return;
    }

    CreatureModelDataRec* modelRec = g_creatureModelDataDB.GetRecord(displayRec->m_modelID);

    if (!modelRec) {
        return;
    }

    CreatureDisplayInfoExtraRec* extraRec = g_creatureDisplayInfoExtraDB.GetRecord(displayRec->m_extendedDisplayInfoID);
    CM2Model* model = this->GetScene()->CreateModel(modelRec->m_modelName, 0, 3);

    if (!model) {
        return;
    }

    if (this->m_component) {
        CCharacterComponent::FreeComponent(this->m_component);
        this->m_component = NULL;
    }

    if (extraRec) {
        this->m_component = CCharacterComponent::AllocComponent();
        ComponentData data;
        data.m_flags |= 1;
        data.m_preferences.m_sexID = extraRec->m_displaySexID;
        data.m_preferences.m_hairStyleId = extraRec->m_hairStyleID;
        data.m_preferences.m_skinID = extraRec->m_skinID;
        data.m_preferences.m_hairColorId = extraRec->m_hairColorID;
        data.m_preferences.m_raceID = extraRec->m_displayRaceID;
        data.m_preferences.m_faceId = extraRec->m_faceID;
        data.m_preferences.m_classId = 0;
        data.m_preferences.m_facialHairId = extraRec->m_facialHairID;
        SStrPrintf(data.m_npcSkinTexture, 260, "%s%s", "Textures\\BakedNpcTextures\\", static_cast<const char*>(extraRec->m_bakeName));
        data.m_model = model;
        model->m_refCount++;
        CCharacterComponent::ValidateComponentData(&data, static_cast<COMPONENT_CONTEXT>(2));
        this->m_component->Init(&data, NULL);

        if (this->m_component) {
            for (uint32_t i = 0; i < 11; i++) {
                int32_t item = extraRec->m_npcitemDisplay[i];

                if (!item) {
                    continue;
                }

                this->m_component->AddItem(static_cast<ITEM_SLOT>(i), item, 0);

                if (i == 10 && owner) {
                    CGObject_C::ReplaceGuildTextures(this->m_component->m_data.m_model, owner->GetGuildGUID().m_guid, owner->GetPVPFactionIndex());
                }
            }
        }

        this->m_component->RenderPrep(0);
    } else {
        CCharacterComponent::ReplaceMonsterSkin(model, displayRec, modelRec);
    }

    this->SetModel(model);
    this->m_sequence = 0;
    this->m_sequenceVariation = -1;
    model->SetBoneSequence(-1, 0, -1, 0, 1.0f, 1, 1, 0, 0, 0);
    model->Release();
}

// OFFSET: 0x00A033F0/0x1407A2670
void CGCharacterModelBase::RefreshUnit(WOWGUID guid) {
    this->m_customRace = -1;
    this->m_customSex = -1;
    this->m_unitGuid = guid;
    this->m_creatureStats = NULL;
    this->m_displayID = 0;
    this->m_modelAlpha = 1.0f;
    this->m_ownerGuid = WOWGUID::Make(0);
    this->m_barberShopAlternateForm = false;

    if (this->m_visible) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            this->SetUpModel(unit);
        }
    }
}

// PROVEN: c77124a8 IDENTICAL/IDENTICAL
// OFFSET: 0x00A03490/0x1407A2700
void CGCharacterModelBase::UpdateAlpha() {
    CSimpleModel::UpdateAlpha();

    if (this->m_model) {
        this->m_model->m_fadeAlpha = this->m_modelAlpha * this->m_model->m_fadeAlpha;
    }
}

// OFFSET: 0x00A034D0/0x1407A2740
void CGCharacterModelBase::SetCreature(CreatureStats_C* stats, CGPlayer_C* owner) {
    this->m_unitGuid = WOWGUID::Make(0);
    this->m_creatureStats = stats;
    this->m_displayID = 0;
    this->m_modelAlpha = 1.0f;
    this->m_ownerGuid = owner ? owner->m_obj->m_guid : WOWGUID::Make(0);
    this->m_customRace = -1;
    this->m_customSex = -1;
    this->m_barberShopAlternateForm = false;

    if (this->m_visible && stats) {
        this->SetUpModel(stats->m_displayId[0], owner);
    }
}

// OFFSET: 0x00A03560/0x1407A27C0
void CGCharacterModelBase::SetDisplayInfo(int32_t displayID, CGPlayer_C* owner) {
    this->m_unitGuid = WOWGUID::Make(0);
    this->m_creatureStats = NULL;
    this->m_displayID = displayID;
    this->m_modelAlpha = 1.0f;
    this->m_ownerGuid = owner ? owner->m_obj->m_guid : WOWGUID::Make(0);
    this->m_customRace = -1;
    this->m_customSex = -1;
    this->m_barberShopAlternateForm = false;

    if (this->m_visible && displayID) {
        this->SetUpModel(displayID, owner);
    }
}

// PROVEN: 6f00fe45 DIFFERENT/IDENTICAL
// OFFSET: 0x00A03730/0x1407A29C0
int32_t CGCharacterModelBase::IsDrawable() {
    if (this->m_model && this->m_model->IsDrawableRecursive()) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_unitGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit || IsUnitDrawable(unit)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00A03790/0x1407A2A50
void CGCharacterModelBase::ConfigureCamera() {
    float facing = this->m_facing;
    this->m_facing = 0.0f;
    this->UpdateModel();

    CM2Model* model = this->m_model;
    uint32_t hidden = (model->m_attachParent ? model->m_fFlags >> 6 : model->m_fFlags >> 3) & 1;

    if (model->m_attachParent) {
        model->m_fFlags &= ~0x40u;
    } else {
        model->m_fFlags &= ~0x8u;
    }

    if (model->m_attachParent) {
        model->m_fFlags &= ~0x10000u;
    } else {
        model->m_fFlags &= ~0x8000u;
    }

    model->SetAnimating(1);
    this->GetScene()->Animate(C3Vector(0.0f, 0.0f, 0.0f));

    model = this->m_model;

    if (model->m_attachParent) {
        model->m_fFlags = (model->m_fFlags & ~0x40u) | (hidden << 6);
        model->m_fFlags = (model->m_fFlags & ~0x10000u) | (hidden << 16);
    } else {
        model->m_fFlags = (model->m_fFlags & ~0x8u) | (hidden << 3);
        model->m_fFlags = (model->m_fFlags & ~0x8000u) | (hidden << 15);
    }

    this->m_facing = facing;
    this->SetCameraByID(1);
    CCamera* camera;

    if (this->m_camera) {
        camera = CameraDuplicate(this->m_camera);
        C3Vector position(0.0f, 0.0f, 0.0f);
        C3Vector target(0.0f, 0.0f, 0.0f);
        CameraGetCoord(camera, 8, &target);
        CameraGetCoord(camera, 7, &position);
        position.x -= this->m_position.x;
        position.y -= this->m_position.y;
        position.z -= this->m_position.z;
        target.x -= this->m_position.x;
        target.y -= this->m_position.y;
        target.z -= this->m_position.z;
        CameraSetCoord(camera, 8, target);
        CameraSetCoord(camera, 7, position);
    } else {
        CAaSphere sphere = this->m_model->GetBoundingSphere();
        camera = CameraCreate();
        CameraSetFloat(camera, 4, 0.5f);
        CameraSetCoord(camera, 8, sphere.c);
        CameraSetCoord(camera, 7, C3Vector(5.5555558f, 0.0f, 2.4166667f));
        CameraSetFloat(camera, 3, 0.027777778f);
    }

    if (this->m_portraitZoom != 0.0f) {
        CCamera* portrait = this->m_model->GetCameraById(0);

        if (portrait) {
            C3Vector position(0.0f, 0.0f, 0.0f);
            C3Vector target(0.0f, 0.0f, 0.0f);
            C3Vector portraitPosition(0.0f, 0.0f, 0.0f);
            C3Vector portraitTarget(0.0f, 0.0f, 0.0f);
            CameraGetCoord(camera, 8, &target);
            CameraGetCoord(camera, 7, &position);
            float fov = CameraGetFloat(camera, 4);
            CameraGetCoord(portrait, 8, &portraitTarget);
            CameraGetCoord(portrait, 7, &portraitPosition);
            portraitTarget.x -= this->m_position.x;
            portraitTarget.y -= this->m_position.y;
            portraitTarget.z -= this->m_position.z;
            portraitPosition.x -= this->m_position.x;
            portraitPosition.y -= this->m_position.y;
            portraitPosition.z -= this->m_position.z;
            float portraitFov = CameraGetFloat(portrait, 4);
            C3Vector value;
            value.x = (portraitPosition.x - position.x) * this->m_portraitZoom + position.x;
            value.y = (portraitPosition.y - position.y) * this->m_portraitZoom + position.y;
            value.z = (portraitPosition.z - position.z) * this->m_portraitZoom + position.z;
            CameraSetCoord(camera, 7, value);
            value.x = (portraitTarget.x - target.x) * this->m_portraitZoom + target.x;
            value.y = (portraitTarget.y - target.y) * this->m_portraitZoom + target.y;
            value.z = (portraitTarget.z - target.z) * this->m_portraitZoom + target.z;
            CameraSetCoord(camera, 8, value);
            CameraSetFloat(camera, 4, (portraitFov - fov) * this->m_portraitZoom + fov);
        }
    }

    if (this->m_camDistanceScale != 1.0f) {
        C3Vector position(0.0f, 0.0f, 0.0f);
        C3Vector target(0.0f, 0.0f, 0.0f);
        CameraGetCoord(camera, 8, &target);
        CameraGetCoord(camera, 7, &position);
        float scale = this->m_camDistanceScale;
        float dx = position.x - target.x;
        float dy = position.y - target.y;
        float dz = position.z - target.z;

        if (scale < 0.1f) {
            scale = 0.1f;
        }

        float sx = dx * scale;
        float sy = dy * scale;
        float sz = dz * scale;

        if (sz * sz + sy * sy + sx * sx < 0.010000001f) {
            float inv = 1.0f / sqrtf(dy * dy + dz * dz + dx * dx);
            sx = dx * inv * 0.1f;
            sy = dy * inv * 0.1f;
            sz = dz * inv * 0.1f;
        }

        CameraSetCoord(camera, 7, C3Vector(sx + target.x, sy + target.y, sz + target.z));
    }

    this->SetCamera(camera);
    CameraRelease(camera);
}

// OFFSET: 0x00A03DB0/0x1407A31E0
void CGCharacterModelBase::SetUpModel(CGUnit_C* unit) {
    if (!ReadyModelForUI(unit, 3)) {
        return;
    }

    CM2Model* model = this->GetDuplicateUnitCharacterModel(unit);

    if (!model) {
        return;
    }

    if (!(model->m_fFlags & 0x1)) {
        model->WaitForLoad(NULL);
    }

    if (this->m_component) {
        CCharacterComponent::FreeComponent(this->m_component);
        this->m_component = NULL;
    }

    CCharacterComponent::RemoveVisuals(model);
    model->DetachAllChildrenById(0);
    this->SetModel(model);
    float alpha = unit->GetBaseAlpha();
    this->m_modelAlpha = alpha;
    model->m_fadeAlpha = alpha * model->m_fadeAlpha;
    this->m_sequence = 0;
    this->m_sequenceVariation = -1;
    model->SetBoneSequence(-1, 0, -1, 0, 1.0f, 1, 1, 0, 0, 0);
    CM2Model* unitModel = unit->m_worldModel;

    if (unitModel->m_fFlags & 0x8000000) {
        model->CopyGuildColors(unitModel);
    }

    for (CM2Model* attached = model->m_attachList; attached; attached = attached->m_attachNext) {
        if (attached->m_attachmentId == 2) {
            CCharacterComponent::ComponentCloseFingers(model, COMP_HAND_LEFT);
            break;
        }
    }

    for (CM2Model* attached = model->m_attachList; attached; attached = attached->m_attachNext) {
        if (attached->m_attachmentId == 1) {
            CCharacterComponent::ComponentCloseFingers(model, COMP_HAND_RIGHT);
            break;
        }
    }

    model->Release();
}

// PROVEN: 55ff4808 IDENTICAL/IDENTICAL
// OFFSET: 0x00A03F60/0x1407A33F0
void CGCharacterModelBase::OnModelLoaded(CM2Model* model) {
    CSimpleModel::OnModelLoaded(model);
    this->ConfigureCamera();
}

// OFFSET: 0x00A042E0/0x1407A37C0
void CGCharacterModelBase::SetCustomRace(int32_t race, uint32_t sex) {
    if (sex > 1) {
        return;
    }

    CreatureModelDataRec* modelRec = GetModelDataRec(race, sex);

    if (!modelRec) {
        return;
    }

    CM2Model* model = this->GetScene()->CreateModel(modelRec->m_modelName, 0, 3);

    if (!model) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CharacterPreferences preferences;

    if (player && player->m_characterComponent) {
        player->m_characterComponent->GetPreferences(&preferences);
    } else {
        memset(&preferences, 0, sizeof(preferences));

        if (player) {
            preferences.m_classId = player->m_unit->m_unitFieldBytes0[1];
            preferences.m_skinID = player->m_player->m_playerBytes[0];
            preferences.m_faceId = player->m_player->m_playerBytes[1];
            preferences.m_hairStyleId = player->m_player->m_playerBytes[2];
            preferences.m_hairColorId = player->m_player->m_playerBytes[3];
            preferences.m_facialHairId = player->m_player->m_playerBytes2[0];
        }

        preferences.m_raceID = race;
        preferences.m_sexID = sex;
    }

    this->m_customSex = sex;
    this->m_customRace = race;

    if (this->m_component) {
        CCharacterComponent::FreeComponent(this->m_component);
        this->m_component = NULL;
    }

    this->m_component = CCharacterComponent::AllocComponent();
    ComponentData data;
    data.SetPreferences(&preferences);
    data.m_preferences.m_raceID = race;
    data.m_preferences.m_sexID = sex;
    data.m_model = model;
    model->m_refCount++;
    CCharacterComponent::ValidateComponentData(&data, static_cast<COMPONENT_CONTEXT>(1));
    this->m_component->Init(&data, NULL);

    if (player && player->GetBag()) {
        WOWGUID requester = WOWGUID::Make(0);

        for (uint32_t slot = 0; slot <= 18; slot++) {
            const VisibleItem_C* visible = player->GetVisibleItem(slot);
            ItemRecSparse* sparse = g_itemSparseDB.GetRecord(abs(visible->m_itemId), requester, NULL, NULL, false);
            ItemRec* item;

            if (sparse && (item = sparse->GetNonSparse()) && item->m_displayInfoID) {
                this->m_component->AddItemBySlot(static_cast<INVENTORY_SLOTS>(slot), item->m_displayInfoID, 0);
            }
        }
    }

    this->m_component->RenderPrep(1);
    this->SetModel(model);
    this->m_sequence = 0;
    this->m_sequenceVariation = 0;
    model->SetBoneSequence(-1, 0, 0, 0, 1.0f, 1, 1, 0, 0, 0);
    model->Release();
    this->UpdateModel();
}

// OFFSET: 0x00A04620/0x1407A3C50
void CGCharacterModelBase::SetBarberShopAlternateForm() {
    this->m_unitGuid = WOWGUID::Make(0);
    this->m_creatureStats = NULL;
    this->m_displayID = 0;
    this->m_modelAlpha = 1.0f;
    this->m_ownerGuid = WOWGUID::Make(0);
    this->m_barberShopAlternateForm = true;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(ClientServices::GetCharacterRace());

    if (this->m_visible) {
        this->SetCustomRace(raceRec->m_alteredFormRaceID, player->m_unit->m_unitFieldBytes0[2]);
    }
}

// OFFSET: 0x00A046D0/0x1407A3D20
void CGCharacterModelBase::ShowThis(bool loadPending) {
    if (!this->m_visible) {
        if (this->m_unitGuid) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_unitGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit) {
                this->SetUpModel(unit);
                CSimpleFrame::ShowThis(loadPending);
                return;
            }
        } else if (this->m_creatureStats) {
            CGPlayer_C* owner = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(this->m_ownerGuid, TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (this->m_creatureStats) {
                this->SetUpModel(this->m_creatureStats->m_displayId[0], owner);
                CSimpleFrame::ShowThis(loadPending);
                return;
            }
        } else if (this->m_displayID) {
            CGPlayer_C* owner = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(this->m_ownerGuid, TYPEMASK_PLAYER, __FILE__, __LINE__));
            this->SetUpModel(this->m_displayID, owner);
            CSimpleFrame::ShowThis(loadPending);
            return;
        } else if (this->m_barberShopAlternateForm) {
            this->SetBarberShopAlternateForm();
        }
    }

    CSimpleFrame::ShowThis(loadPending);
}
