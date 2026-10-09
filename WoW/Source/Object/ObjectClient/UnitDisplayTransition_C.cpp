#include "UnitDisplayTransition_C.h"
#include <new>
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoExtraRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/CreatureSoundDataRec.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/Object/ObjectClient/AnimKitManager.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/Missile_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/CGTabardCreationFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00CA8024
static const uint32_t s_reattachLinks[13] = { 0, 1, 2, 26, 27, 28, 30, 31, 32, 33, 35, 36, 49 };

// STATIC: 0x00DD8670
TSObjectAllocPool<CUnitDisplayTransition_C>* CUnitDisplayTransition_C::s_pool;

// PROVEN: 5a7acaa7 IDENTICAL/NONE
// OFFSET: 0x005FDB90
void CUnitDisplayTransition_C::HandleComponentRenderPrep() {
    if (this->m_state == 1 && this->m_component && this->m_component->RenderPrep(0)) {
        this->m_state = 2;
    }
}

// PROVEN: e638c01f IDENTICAL/NONE
// OFFSET: 0x005FDBC0
void CUnitDisplayTransition_C::HandleAnimSwapEvent() {
    if (this->m_state == 4) {
        this->m_flags |= 0x1;
    }
}

// PROVEN: d1ce6fa0 IDENTICAL/NONE
// OFFSET: 0x005FDBD0
int32_t CUnitDisplayTransition_C::GetTargetDisplayID() const {
    if (this->m_state <= 4 && this->m_displayInfo) {
        return this->m_displayInfo->m_id;
    }

    return 0;
}

// OFFSET: 0x005FDBF0
void CUnitDisplayTransition_C::MirrorTransformToSwapModel() {
    if (this->m_state != 4 && this->m_state != 5) {
        return;
    }

    CM2Model* unitModel = this->m_unit->m_worldModel;

    if (!unitModel || !(unitModel->m_fFlags & 0x4000)) {
        return;
    }

    C44Matrix matrix;
    CM2Model* parent;
    uint32_t attachmentId;

    if (this->m_attachment == -1) {
        parent = unitModel->m_attachParent;
        attachmentId = unitModel->m_attachmentId;
        matrix = unitModel->m_worldTransform;
    } else {
        parent = unitModel;
        attachmentId = this->m_attachment;
        matrix = C44Matrix();
    }

    if (this->m_model->m_attachParent != parent) {
        if (this->m_model->m_attachParent) {
            this->m_model->DetachFromParent();
        }

        if (parent) {
            this->m_model->AttachToParent(parent, attachmentId, NULL, 0);
        }
    }

    this->m_model->m_fFlags |= 0x4000;
    this->m_model->m_worldTransform = matrix;
}

// OFFSET: 0x005FDD40/0x140286440
void CUnitDisplayTransition_C::MirrorTransformToSwapWorldObject(const C44Matrix& matrix, const CAaBox& box, const CAaSphere& sphere, const C3Vector& position, int32_t a6, int32_t keyBone) {
    if (this->m_worldObject && (this->m_state == 4 || this->m_state == 5)) {
        World::ObjectUpdate(this->m_worldObject, matrix, box, sphere, position, a6, keyBone);
    }
}

// OFFSET: 0x005FDD80/0x140286490
void CUnitDisplayTransition_C::ApplyGuildColor(uint32_t flags, int32_t emblem, int32_t emblemColor, int32_t border, int32_t borderColor, int32_t background) {
    if (!this->m_component) {
        return;
    }

    this->m_component->ApplyGuildColor(flags, emblem, emblemColor, border, borderColor, background);

    CImVector backgroundColor(0);
    CImVector borderColorValue(0);
    CImVector emblemColorValue(0);
    char fileName[64];
    CGObject_C::GetGuildLogoInfo(0, borderColor, emblemColor, emblem, backgroundColor, borderColorValue, emblemColorValue, fileName, sizeof(fileName));
    CGObject_C::ReplaceGuildTextures(this->m_component->m_data.m_model, backgroundColor, borderColorValue, emblemColorValue, fileName);
}

// OFFSET: 0x005FDE10
void CUnitDisplayTransition_C::ApplyShirtGuildColor(uint32_t flags, int32_t emblem, int32_t emblemColor, int32_t border, int32_t borderColor, int32_t background) {
    if (!this->m_component) {
        return;
    }

    this->m_component->ApplyShirtGuildColor(flags, emblem, emblemColor, border, borderColor, background);

    CImVector backgroundColor(0);
    CImVector borderColorValue(0);
    CImVector emblemColorValue(0);
    char fileName[64];
    CGObject_C::GetGuildLogoInfo(0, borderColor, emblemColor, emblem, backgroundColor, borderColorValue, emblemColorValue, fileName, sizeof(fileName));
    CGObject_C::ReplaceGuildTextures(this->m_component->m_data.m_model, backgroundColor, borderColorValue, emblemColorValue, fileName);
}

// PROVEN: c1689d8a IDENTICAL/NONE
// OFFSET: 0x005FDEA0/0x140286690
void CUnitDisplayTransition_C::ReleaseAnimKits() {
    if (this->m_swapAnimKit) {
        this->m_swapAnimKit->SetStopCallback(NULL, 0);
        this->m_swapAnimKit->Stop(1, NULL);
        this->m_swapAnimKit->Release();
        this->m_swapAnimKit = NULL;
    }

    if (this->m_animKit) {
        this->m_animKit->SetStopCallback(NULL, 0);
        this->m_animKit->Stop(1, NULL);
        this->m_animKit->Release();
        this->m_animKit = NULL;
    }

    if (this->m_state == 5) {
        this->m_state = 0;
    }
}

// OFFSET: 0x005FDF10/0x140286720
bool CUnitDisplayTransition_C::IsReadyToBegin() const {
    if (this->m_state != 2) {
        return false;
    }

    if (this->m_effectModel && !this->m_effectModel->IsLoaded()) {
        return false;
    }

    CM2Model* unitModel = this->m_unit->m_worldModel;

    return unitModel && unitModel->IsLoaded() && this->m_unit->m_worldObject && this->m_unit->m_displayInfo;
}

// PROVEN: b6323d67 IDENTICAL/NONE
// OFFSET: 0x005FDF60
void CUnitDisplayTransition_C::HandleComponentPrep() {
    if (this->m_state == 1 && this->m_component) {
        this->m_component->Prep();

        if (!(this->m_component->m_flags & 0x1)) {
            this->m_state = 2;
        }
    }
}

// OFFSET: 0x005FDF90/0x1402867D0
int32_t CUnitDisplayTransition_C::MaybeCreateCharacterComponent() {
    CGPlayer_C* player;
    bool useExtra;

    if ((this->m_unit->m_obj->m_type & TYPEMASK_PLAYER) && (this->m_modelData->m_flags & 0x4)) {
        player = static_cast<CGPlayer_C*>(this->m_unit);
        useExtra = this->m_displayInfoExtra && (this->m_displayInfoExtra->m_flags & 0x1);
    } else {
        if (!this->m_displayInfoExtra) {
            return 0;
        }

        player = NULL;
        useExtra = true;
    }

    this->m_component = CCharacterComponent::AllocComponent();
    ComponentData data;

    if (useExtra) {
        data.m_preferences.m_raceID = this->m_displayInfoExtra->m_displayRaceID;
        data.m_preferences.m_sexID = this->m_displayInfoExtra->m_displaySexID;
    } else {
        data.m_preferences.m_raceID = this->m_race;
        data.m_preferences.m_sexID = this->m_unit->m_unit->m_unitFieldBytes0[2];
    }

    data.m_preferences.m_classId = this->m_unit->m_unit->m_unitFieldBytes0[1];

    if (player) {
        data.m_preferences.m_skinID = player->m_player->m_playerBytes[0];
        data.m_preferences.m_faceId = player->m_player->m_playerBytes[1];
        data.m_preferences.m_hairStyleId = player->m_player->m_playerBytes[2];
        data.m_preferences.m_hairColorId = player->m_player->m_playerBytes[3];
        data.m_preferences.m_facialHairId = player->m_player->m_playerBytes2[0];
    } else {
        data.m_preferences.m_skinID = this->m_displayInfoExtra->m_skinID;
        data.m_preferences.m_faceId = this->m_displayInfoExtra->m_faceID;
        data.m_preferences.m_hairStyleId = this->m_displayInfoExtra->m_hairStyleID;
        data.m_preferences.m_hairColorId = this->m_displayInfoExtra->m_hairColorID;
        data.m_preferences.m_facialHairId = this->m_displayInfoExtra->m_facialHairID;

        if (!CGPlayer_C::IsXRayVisionActive() || !this->m_unit->UsesNpcBakedTexture()) {
            const char* bakeName = this->m_unit->GetDisplayBakeName();

            if (!bakeName || !*bakeName) {
                return 0;
            }

            data.m_flags |= 0x1;
            SStrPrintf(data.m_npcSkinTexture, sizeof(data.m_npcSkinTexture), "%s%s", "Textures\\BakedNpcTextures\\", bakeName);
        }
    }

    data.m_model = this->m_model;
    bool activePlayer = this->m_unit->m_obj->m_guid == ClntObjMgrGetActivePlayer();
    data.m_flags = (data.m_flags & ~0x2) | (activePlayer << 1);
    data.m_model->m_refCount++;
    CCharacterComponent::ValidateComponentData(&data, player ? COMPONENT_CONTEXT_1 : COMPONENT_CONTEXT_2);
    this->m_component->Init(&data, NULL);

    if ((this->m_unit->m_obj->m_type & TYPEMASK_PLAYER) && this->m_unit->CanEquipItemsInThisForm()) {
        CGPlayer_C* owner = static_cast<CGPlayer_C*>(this->m_unit);

        if (CGPlayer_C::IsXRayVisionActive() && owner->UsesNpcBakedTexture()) {
            return 1;
        }

        for (uint32_t slot = 0; slot <= INVSLOT_TABARD; slot++) {
            VisibleItem_C* item = const_cast<VisibleItem_C*>(owner->GetVisibleItem(slot));

            if (item->m_itemId <= 0 && (slot == INVSLOT_MAINHAND || slot == INVSLOT_OFFHAND || slot == INVSLOT_RANGED)) {
                continue;
            }

            if (!item->GetDisplayID()) {
                continue;
            }

            ItemDisplayInfoRec rec;

            if (slot == INVSLOT_TABARD) {
                if (!g_itemDisplayInfoDB.GetRecord(item->GetDisplayID(), &rec) && !(this->m_component->m_flags & 0x40)) {
                    continue;
                }

                this->m_component->AddItemBySlot(INVSLOT_TABARD, item->GetDisplayID(), 0);

                if (rec.m_flags & 0x19) {
                    int32_t emblemStyle;
                    int32_t emblemColor;
                    int32_t borderStyle;
                    int32_t borderColor;
                    int32_t background;

                    if (GuildGetGuildTabard(owner->m_obj->m_guid, owner->GetGuildGUID(), &CGPlayer_C::TabardDrawCallback, owner, emblemStyle, emblemColor, borderStyle, borderColor, background)) {
                        this->m_component->ApplyGuildColor(rec.m_flags, emblemStyle, emblemColor, borderStyle, borderColor, background);
                    }
                }
            } else if (slot == INVSLOT_BODY) {
                if (!g_itemDisplayInfoDB.GetRecord(item->GetDisplayID(), &rec)) {
                    continue;
                }

                this->m_component->AddItemBySlot(INVSLOT_BODY, item->GetDisplayID(), 0);

                if (rec.m_flags & 0x19) {
                    int32_t emblemStyle;
                    int32_t emblemColor;
                    int32_t borderStyle;
                    int32_t borderColor;
                    int32_t background;

                    if (GuildGetGuildTabard(owner->m_obj->m_guid, owner->GetGuildGUID(), &CGPlayer_C::ShirtTabardDrawCallback, owner, emblemStyle, emblemColor, borderStyle, borderColor, background)) {
                        this->m_component->ApplyShirtGuildColor(rec.m_flags, emblemStyle, emblemColor, borderStyle, borderColor, background);
                    }
                }
            } else {
                bool hidden;

                if (slot == INVSLOT_HEAD) {
                    hidden = (owner->m_player->m_playerFlags & 0x400) != 0;
                } else if (slot == INVSLOT_BACK) {
                    hidden = (owner->m_player->m_playerFlags & 0x800) != 0;
                } else {
                    hidden = false;
                }

                if (hidden) {
                    continue;
                }

                this->m_component->AddItemBySlot(static_cast<INVENTORY_SLOTS>(slot), item->GetDisplayID(), 0);

                if (slot == INVSLOT_BACK) {
                    CGObject_C::ReplaceGuildTextures(this->m_component->m_data.m_model, owner->GetGuildGUID().m_guid, owner->GetPVPFactionIndex());
                }
            }
        }

        return 1;
    }

    if (this->m_displayInfoExtra) {
        for (int32_t slot = 0; slot < 11; slot++) {
            int32_t displayID = this->m_displayInfoExtra->m_npcitemDisplay[slot];

            if (displayID) {
                this->m_component->AddItem(static_cast<ITEM_SLOT>(slot), displayID, 0);
            }
        }
    }

    return 1;
}

// OFFSET: 0x005FE460/0x140286DF0
void CUnitDisplayTransition_C::PerformSwap() {
    if (this->m_state != 4) {
        return;
    }

    if (this->m_model->m_attachParent) {
        this->m_model->DetachFromParent();
    }

    CM2Model* unitModel = this->m_unit->m_worldModel;

    if (unitModel->m_attachParent) {
        this->m_model->AttachToParent(unitModel->m_attachParent, unitModel->m_attachmentId, NULL, 0);
        this->m_unit->m_worldModel->DetachFromParent();
    }

    this->m_attachment = -1;

    CreatureDisplayInfoRec* displayInfo = this->m_unit->m_displayInfo;
    this->m_unit->m_displayInfo = this->m_displayInfo;
    this->m_displayInfo = displayInfo;

    CreatureDisplayInfoExtraRec* displayInfoExtra = this->m_unit->m_displayInfoExtra;
    this->m_unit->m_displayInfoExtra = this->m_displayInfoExtra;
    this->m_displayInfoExtra = displayInfoExtra;

    CreatureModelDataRec* modelData = this->m_unit->m_modelData;
    this->m_unit->m_modelData = this->m_modelData;
    this->m_modelData = modelData;

    CreatureSoundDataRec* soundData = this->m_unit->m_soundData;
    this->m_unit->m_soundData = this->m_soundData;
    this->m_soundData = soundData;

    CM2Model* model = this->m_unit->m_worldModel;
    this->m_unit->m_worldModel = this->m_model;
    this->m_model = model;

    this->m_unit->SetModelCallbacks(this->m_unit->m_worldModel);

    if (!this->m_unit->m_worldModel->IsLoaded()) {
        this->m_unit->m_worldModel->SetLoadedCallback(&CGObject_C::ModelLoadedCallback, this->m_unit);
    }

    this->m_unit->m_animKitManager.SetModel(this->m_unit->m_worldModel, 0);
    this->m_model->SetLoadedCallback(NULL, NULL);
    this->m_model->SetSequenceCallback(NULL, 0);
    this->m_model->SetSequenceStartCallback(NULL, 0);
    this->m_model->SetEventCallback(NULL, 0);
    this->m_unit->m_worldModel->m_refCount++;
    this->m_model->m_refCount++;

    if (!this->m_unit->m_mountModel && this->m_unit->m_worldObject) {
        World::ObjectSetModel(this->m_unit->m_worldObject, NULL);
    }

    if (this->m_worldObject) {
        World::ObjectSetModel(this->m_worldObject, NULL);
    }

    if (!this->m_unit->m_mountModel && this->m_unit->m_worldObject) {
        World::ObjectSetModel(this->m_unit->m_worldObject, this->m_unit->m_worldModel);
    }

    if (this->m_worldObject) {
        World::ObjectSetModel(this->m_worldObject, this->m_model);
    }

    this->m_unit->m_worldModel->Release();
    this->m_model->Release();

    CCharacterComponent* component = this->m_unit->m_characterComponent;
    this->m_unit->m_characterComponent = this->m_component;
    this->m_component = component;

    this->m_unit->UpdateModelScale(this->m_modelData);

    if (this->m_unit->m_mountModel) {
        float scale = 1.0f / this->m_unit->m_mountScale;
        this->m_unit->m_worldModel->m_fFlags |= 0x4000;
        C44Matrix matrix(scale, 0.0f, 0.0f, 0.0f, 0.0f, scale, 0.0f, 0.0f, 0.0f, 0.0f, scale, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        this->m_unit->m_worldModel->m_worldTransform = matrix;
    }

    for (CEffect* effect = this->m_unit->m_effectList; effect;) {
        CEffect* next = effect->m_next;
        effect->DetachFromParent();
        effect->UpdateAttachment();
        effect = next;
    }

    this->m_unit->ApplyComponentItemsFromEffects();
    CMissile::UpdateAttachments(this->m_unit->m_obj->m_guid);

    CM2Model* precastModel = this->m_unit->m_precastMissileModel;

    if (precastModel && precastModel->m_attachParent) {
        precastModel->m_refCount++;
        uint32_t attachmentId = this->m_unit->m_precastMissileModel->m_attachmentId;
        this->m_unit->m_precastMissileModel->DetachFromParent();

        if (this->m_unit->m_worldModel->HasAttachment(attachmentId)) {
            this->m_unit->m_precastMissileModel->AttachToParent(this->m_unit->m_worldModel, attachmentId, NULL, 0);
        }

        this->m_unit->m_precastMissileModel->Release();
    }

    CVehicle_C* vehicle = this->m_unit->m_vehicle;

    if (vehicle) {
        for (CPassenger* passenger = vehicle->m_passengers.Head(); passenger; passenger = vehicle->m_passengers.Next(passenger)) {
            CGObject_C* object = ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__);
            CM2Model* passengerModel = object->GetObjectModel();

            if (!passengerModel || !passengerModel->m_attachParent) {
                continue;
            }

            passengerModel->m_refCount++;
            uint32_t attachmentId = passengerModel->m_attachmentId;
            passengerModel->DetachFromParent();

            if (this->m_unit->m_worldModel->HasAttachment(attachmentId)) {
                passengerModel->AttachToParent(this->m_unit->m_worldModel, attachmentId, NULL, 0);
            }

            passengerModel->Release();
        }
    }

    for (uint32_t i = 0; i < sizeof(s_reattachLinks) / sizeof(s_reattachLinks[0]); i++) {
        uint32_t link = s_reattachLinks[i];
        CM2Model* child = this->m_model->m_attachList;

        while (child && child->m_attachmentId != link) {
            child = child->m_attachNext;
        }

        while (child) {
            child->m_refCount++;
            child->DetachFromParent();

            if (this->m_unit->m_worldModel->HasAttachment(link)) {
                child->AttachToParent(this->m_unit->m_worldModel, link, NULL, 0);
            }

            child->Release();
            child = this->m_model->m_attachList;

            while (child && child->m_attachmentId != link) {
                child = child->m_attachNext;
            }
        }
    }

    uint32_t clearLinks[2] = { 51, 52 };

    for (uint32_t i = 0; i < 2; i++) {
        uint32_t link = clearLinks[i];

        if (!this->m_unit->m_worldModel->HasAttachment(link)) {
            continue;
        }

        CM2Model* child = this->m_unit->m_worldModel->m_attachList;

        while (child && child->m_attachmentId != link) {
            child = child->m_attachNext;
        }

        while (child) {
            child->DetachFromParent();
            child->Release();
            child = this->m_unit->m_worldModel->m_attachList;

            while (child && child->m_attachmentId != link) {
                child = child->m_attachNext;
            }
        }
    }

    if (this->m_swapAnimKit) {
        this->m_swapAnimKit->SetStopCallback(NULL, 0);
        this->m_swapAnimKit->Stop(1, NULL);
        this->m_swapAnimKit->Release();
        this->m_swapAnimKit = NULL;
    }

    if (this->m_unit->m_worldModel->IsLoaded()) {
        this->m_endAnim = this->m_unit->m_animKitManager.FilterAnim(this->m_endAnim, 0);

        if (this->m_endAnim != -1) {
            if (this->m_animKit) {
                this->m_animKit->SetAnim(0, this->m_endAnim, 0xFF, 0, 0, 0);
            } else {
                this->m_animKit = this->m_unit->m_animKitManager.PlayAndMaintain(281, this->m_endAnim, NULL);
            }
        }
    }

    CM2Model* worldModel = this->m_unit->m_worldModel;
    CM2Model* rightHand = worldModel->m_attachList;

    while (rightHand && rightHand->m_attachmentId != ATTACH_HANDRIGHT) {
        rightHand = rightHand->m_attachNext;
    }

    if (rightHand) {
        CCharacterComponent::ComponentCloseFingers(worldModel, COMP_HAND_RIGHT);
    } else {
        CCharacterComponent::ComponentOpenFingers(worldModel, COMP_HAND_RIGHT);
    }

    worldModel = this->m_unit->m_worldModel;
    CM2Model* leftHand = worldModel->m_attachList;

    while (leftHand && leftHand->m_attachmentId != ATTACH_HANDLEFT) {
        leftHand = leftHand->m_attachNext;
    }

    if (leftHand) {
        CCharacterComponent::ComponentCloseFingers(worldModel, COMP_HAND_LEFT);
    } else {
        CCharacterComponent::ComponentOpenFingers(worldModel, COMP_HAND_LEFT);
    }

    this->m_model->m_fadeAlpha = 0.0f;

    if (this->m_startAnim != -1) {
        for (CEffect* effect = this->m_unit->m_effectList; effect; effect = effect->m_next) {
            if (effect->m_model == this->m_effectModel) {
                effect->m_flags &= ~0x4000000;
                break;
            }
        }
    }

    if (this->m_effectModel) {
        this->m_effectModel->Release();
        this->m_effectModel = NULL;
    }

    this->m_unit->SetupFootprints();
    WOWGUID guid = this->m_unit->m_obj->m_guid;
    CGGameUI::UnitModelUpdate(guid, 3);
    this->m_state = 5;
    this->m_unit->FlipDisplayTransitionState();

    if (this->m_animKit) {
        this->m_animKit->SetStopCallback(&CUnitDisplayTransition_C::AnimKitStopCallback, reinterpret_cast<intptr_t>(this));
        return;
    }

    if (this->m_swapAnimKit) {
        this->m_swapAnimKit->SetStopCallback(NULL, 0);
        this->m_swapAnimKit->Stop(1, NULL);
        this->m_swapAnimKit->Release();
        this->m_swapAnimKit = NULL;
    }

    if (this->m_animKit) {
        this->m_animKit->SetStopCallback(NULL, 0);
        this->m_animKit->Stop(1, NULL);
        this->m_animKit->Release();
        this->m_animKit = NULL;
    }

    if (this->m_state == 5) {
        this->m_state = 0;
    }
}

// PROVEN: 6f088cf1 IDENTICAL/NONE
// OFFSET: 0x005FEBA0/0x140287670
void CUnitDisplayTransition_C::AnimKitStopCallback(CAnimKitInst* inst, uint64_t param) {
    CUnitDisplayTransition_C* transition = reinterpret_cast<CUnitDisplayTransition_C*>(param);

    if (transition->m_swapAnimKit) {
        transition->m_swapAnimKit->SetStopCallback(NULL, 0);
        transition->m_swapAnimKit->Stop(1, NULL);
        transition->m_swapAnimKit->Release();
        transition->m_swapAnimKit = NULL;
    }

    if (transition->m_animKit) {
        transition->m_animKit->SetStopCallback(NULL, 0);
        transition->m_animKit->Stop(1, NULL);
        transition->m_animKit->Release();
        transition->m_animKit = NULL;
    }

    if (transition->m_state == 4) {
        transition->PerformSwap();
    }

    if (transition->m_state == 5) {
        transition->ReleaseAnimKits();
    }
}

// OFFSET: 0x005FEC20/0x140287710
void CUnitDisplayTransition_C::Expedite(int32_t swap) {
    if (this->m_state == 5) {
        this->ReleaseAnimKits();
        return;
    }

    if (!swap) {
        this->m_unit->FlipDisplayTransitionState();
        this->m_state = 5;
        this->ReleaseAnimKits();
        return;
    }

    if (!this->m_component) {
        this->MaybeCreateCharacterComponent();
    }

    this->m_state = 4;
    this->PerformSwap();
    this->ReleaseAnimKits();
}

// OFFSET: 0x005FEC80/0x140287770
void CUnitDisplayTransition_C::BeginAnimation() {
    this->m_state = 4;

    if (this->m_startAnim != -1) {
        this->m_swapAnimKit = NULL;
        this->m_animKit = this->m_unit->m_animKitManager.PlayAndMaintain(281, this->m_startAnim, NULL);
    }

    if (this->m_swapAnimKit) {
        this->m_swapAnimKit->SetStopCallback(&CUnitDisplayTransition_C::AnimKitStopCallback, reinterpret_cast<intptr_t>(this));
    } else if (!this->m_animKit) {
        this->m_unit->OnDisplayTransitionBeginAnimation();
        this->PerformSwap();
        return;
    }

    if (this->m_animKit) {
        this->m_animKit->SetStopCallback(&CUnitDisplayTransition_C::AnimKitStopCallback, reinterpret_cast<intptr_t>(this));
    }

    this->m_model->SetBoneSequence(-1, this->m_startAnim, 0, 0, 1.0f, 0, 1, 0, 0, 0);
    this->m_unit->OnDisplayTransitionBeginAnimation();
}

// PROVEN: 8289983d IDENTICAL/NONE
// OFFSET: 0x005FED30/0x140287860
void CUnitDisplayTransition_C::Destroy() {
    if (this->m_swapAnimKit) {
        this->m_swapAnimKit->SetStopCallback(NULL, 0);
        this->m_swapAnimKit->Stop(1, NULL);
        this->m_swapAnimKit->Release();
        this->m_swapAnimKit = NULL;
    }

    if (this->m_animKit) {
        this->m_animKit->SetStopCallback(NULL, 0);
        this->m_animKit->Stop(1, NULL);
        this->m_animKit->Release();
        this->m_animKit = NULL;
    }

    if (this->m_model) {
        if (this->m_model->m_attachParent) {
            this->m_model->DetachFromParent();
        }

        this->m_model->Release();
    }

    if (this->m_worldObject) {
        World::ObjectDestroy(this->m_worldObject);
    }

    if (this->m_component) {
        CCharacterComponent::FreeComponent(this->m_component);
    }

    if (this->m_effectModel && this->m_startAnim != -1) {
        for (CEffect* effect = this->m_unit->m_effectList; effect; effect = effect->m_next) {
            if (effect->m_model == this->m_effectModel) {
                effect->MoveToDestroyList();
                break;
            }
        }
    }

    if (this->m_effectModel) {
        if (this->m_effectModel->m_attachParent) {
            this->m_effectModel->DetachFromParent();
        }

        this->m_effectModel->Release();
    }

    CUnitDisplayTransition_C::s_pool->Delete(this);
}

// OFFSET: 0x005FEE40/0x1402879A0
void CUnitDisplayTransition_C::Cancel() {
    if (this->m_swapAnimKit) {
        uint32_t count = this->m_swapAnimKit->GetSegmentCount();

        for (uint32_t i = 0; i < count; i++) {
            this->m_swapAnimKit->SetUserFlags(i, 2);
        }
    }

    if (this->m_animKit) {
        uint32_t count = this->m_animKit->GetSegmentCount();

        for (uint32_t i = 0; i < count; i++) {
            this->m_animKit->SetUserFlags(i, 2);
        }
    }

    this->Destroy();
}

// OFFSET: 0x005FEEA0/0x140287A30
void CUnitDisplayTransition_C::BeginTransition() {
    if (this->m_effectModel) {
        CEffect* effect = CEffect::Create();
        WOWGUID guid = this->m_unit->m_obj->m_guid;
        effect->AddEffectWithModel(this->m_effectModel, 0xC008000, this->m_effectAttachment, guid, &CGObject_C::EffectOnceHandler);
        effect->m_flags |= 0x20;
        effect->Release();

        if (this->m_delay > 0) {
            this->m_state = 3;
            this->m_delayEndTime = this->m_delay + FrameTime::GetCurTimeMs();
            return;
        }
    }

    this->BeginAnimation();
}

// OFFSET: 0x005FEF20
void CUnitDisplayTransition_C::UninitSystem() {
    if (CUnitDisplayTransition_C::s_pool) {
        delete CUnitDisplayTransition_C::s_pool;
        CUnitDisplayTransition_C::s_pool = NULL;
    }
}

// OFFSET: 0x005FEF40/0x140287B70
bool CUnitDisplayTransition_C::UpdateState(int32_t begin) {
    if (this->m_state == 1 && this->m_model->IsLoaded() && !this->m_component && !this->MaybeCreateCharacterComponent()) {
        this->m_state = 2;
    }

    if (this->IsReadyToBegin() && begin) {
        this->BeginTransition();
    }

    if (this->m_state == 3 && static_cast<int32_t>(FrameTime::GetCurTimeMs() - this->m_delayEndTime) > 0) {
        this->BeginAnimation();
    }

    if (this->m_flags & 0x1) {
        this->PerformSwap();
        this->m_flags &= ~0x1;
    }

    return this->m_state != 0;
}

// OFFSET: 0x005FEFC0/0x140287C10
void CUnitDisplayTransition_C::InitSystem() {
    delete CUnitDisplayTransition_C::s_pool;
    void* pool = SMemAlloc(sizeof(TSObjectAllocPool<CUnitDisplayTransition_C>), __FILE__, __LINE__, 0);
    CUnitDisplayTransition_C::s_pool = pool ? new (pool) TSObjectAllocPool<CUnitDisplayTransition_C>("UnitDisplayTransition", 8) : NULL;
}

// OFFSET: 0x005FF040/0x140287D10
CUnitDisplayTransition_C* CUnitDisplayTransition_C::Create(CGUnit_C* unit, int32_t race, int32_t displayID, int32_t attachment, uint32_t startAnim, uint32_t endAnim, const char* effectModelName, int32_t effectAttachment, int32_t delay) {
    CUnitDisplayTransition_C* transition = CUnitDisplayTransition_C::s_pool->New("", 0);
    transition->m_unit = unit;
    transition->m_flags = 0;
    transition->m_race = race;
    transition->m_state = 0;
    transition->m_model = NULL;
    transition->m_component = NULL;
    transition->m_worldObject = 0;
    transition->m_swapAnimKit = NULL;
    transition->m_animKit = NULL;
    transition->m_effectModel = NULL;

    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(displayID);
    transition->m_displayInfo = displayInfo;

    if (!displayInfo) {
        DebugLog("Error: Invalid dstCreatureDisplayInfoID provided (%d) for CUnitDisplayTransition_C::Create()", displayID);
        transition->Destroy();
        return NULL;
    }

    transition->m_displayInfoExtra = g_creatureDisplayInfoExtraDB.GetRecord(displayInfo->m_extendedDisplayInfoID);
    CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);
    transition->m_modelData = modelData;

    if (!modelData) {
        DebugLog("Error: Invalid CreatureModelData ID encountered (%d) from display info %d in CUnitDisplayTransition_C::Create()", displayInfo->m_modelID);
        transition->Destroy();
        return NULL;
    }

    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(race);
    transition->m_soundData = NULL;
    int32_t soundID;

    switch (unit->m_unit->m_unitFieldBytes0[2]) {
        case 0:
            soundID = raceRec->m_maleSoundDataID;
            break;

        case 1:
            soundID = raceRec->m_femaleSoundDataID;
            break;

        default:
            soundID = 0;
            break;
    }

    if (soundID) {
        transition->m_soundData = g_creatureSoundDataDB.GetRecord(soundID);
    }

    if (!transition->m_soundData) {
        transition->m_soundData = g_creatureSoundDataDB.GetRecord(displayInfo->m_soundID);

        if (!transition->m_soundData) {
            transition->m_soundData = g_creatureSoundDataDB.GetRecord(modelData->m_soundID);
        }
    }

    transition->m_model = World::GetScene()->CreateModel(modelData->m_modelName, 0, 3);

    if (!transition->m_model) {
        DebugLog("Error: Unable to create model \"%s\" from display info %d in CUnitDisplayTransition_C::Create()", transition->m_modelData->m_modelName);
        transition->Destroy();
        return NULL;
    }

    CCharacterComponent::ApplyMonsterGeosets(transition->m_model, transition->m_displayInfo);
    CCharacterComponent::ReplaceMonsterSkin(transition->m_model, transition->m_displayInfo, transition->m_modelData);
    transition->m_attachment = attachment;

    if (attachment == -1) {
        uint32_t flags = 0x20;

        if (unit->IsLinkAll()) {
            flags = 0x28;
        }

        if (unit->HasNoShadowBlob()) {
            flags |= 0x2;
        }

        transition->m_worldObject = World::ObjectCreate(transition->m_model, NULL, NULL, 0, 0, flags, __FILE__, __LINE__);

        if (!transition->m_worldObject) {
            DebugLog("Error: Unable to create world object from model \"%s\" in CUnitDisplayTransition_C::Create()", transition->m_modelData->m_modelName);
            transition->Destroy();
            return NULL;
        }
    }

    transition->m_startAnim = startAnim;
    transition->m_endAnim = endAnim;

    if (effectModelName) {
        transition->m_effectModel = World::GetScene()->CreateModel(effectModelName, 0, 3);

        if (!transition->m_effectModel) {
            DebugLog("Error: Unable to create effect model \"%s\" in CUnitDisplayTransition_C::Create()", effectModelName);
            transition->Destroy();
            return NULL;
        }
    }

    transition->m_effectAttachment = effectAttachment;
    transition->m_delay = delay;
    transition->m_state = 1;
    transition->UpdateState(0);

    return transition;
}
