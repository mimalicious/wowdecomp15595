#include "Effect_C.h"
#include <cmath>
#include <cstring>
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxVertexPC.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/ParticleSystem2.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/Magic/MagicClient/SpellVisuals.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/CreatureStats.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00DD5810
static CEffect* s_orphanedEffects;

// STATIC: 0x00DD5814
static CEffect* s_destroyedEffects;

// STATIC: 0x00BB0334
static const int32_t s_specialUnitEffectAttachments[] = { 53, 53, 17, 17, 19, 19, 19, 17, 19, 19, 34, 19, 19, 53, 19 };

// STATIC: 0x00DD5818
static CDataAllocator s_effectAllocator(sizeof(CEffect), 64);

// STATIC: 0x00DD582C
static CDataAllocator s_virtualItemAllocator(sizeof(CEffectVirtualItem), 16);

// STATIC: 0x00DD5840
static CDataAllocator s_curveAllocator(sizeof(CEffectCurve), 16);

// STATIC: 0x00DD5854
static CDataAllocator s_curvePointAllocator(sizeof(CEffectCurvePoint), 64);

// OFFSET: 0x0057FDC0
int32_t GetSpecialUnitEffectAttachment(int32_t id) {
    return s_specialUnitEffectAttachments[id];
}

// PROVEN: 19b4b8e6 IDENTICAL/NONE
// OFFSET: 0x0057FDD0
void CEffect::UnrootFromCaster() {
    if (this->m_flags & 0x4000) {
        this->m_flags &= ~0x4004000;
    }
}

// OFFSET: 0x0057FDF0
int32_t CEffect::GetCamouflage(float& alpha, float& fadeTime) const {
    if (!(this->m_flags & 0x10000000)) {
        return 0;
    }

    alpha = this->m_data.m_float[0];
    fadeTime = this->m_data.m_float[1];

    return 1;
}

// OFFSET: 0x0057FE30
int32_t CEffect::GetOrientationAngles(float& yaw, float& pitch) const {
    if (!(this->m_flags & 0x400)) {
        return 0;
    }

    yaw = this->m_data.m_float[0];
    pitch = this->m_data.m_float[1];

    return 1;
}

// PROVEN: ecd8fb4d IDENTICAL/NONE
// OFFSET: 0x0057FE70/0x1401E6BF0
void CEffect::MountTransitionSequenceCB(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    CEffect* effect = reinterpret_cast<CEffect*>(static_cast<uintptr_t>(param));

    if (sequenceId == 127 && effect->m_mountTransition) {
        SpellVisualsFlagMountTransitionFinished(effect->m_mountTransition);
    }
}

// PROVEN: 1e7f9327 IDENTICAL/NONE
// OFFSET: 0x0057FEA0
void CEffect::UnLinkSelf() {
    if (this->m_prevLink) {
        *this->m_prevLink = this->m_next;
    }

    if (this->m_next) {
        this->m_next->m_prevLink = this->m_prevLink;
    }

    this->m_next = NULL;
    this->m_prevLink = NULL;
}

// OFFSET: 0x0057FEE0
void CEffect::ClearLightning() {
    for (uint32_t i = 0; i < 12; i++) {
        if (this->m_lightning[i]) {
            SpellVisualClearLightning(this->m_lightning[i]);
        }

        this->m_lightning[i] = NULL;
    }
}

// PROVEN: 3bfc2479 IDENTICAL/NONE
// OFFSET: 0x0057FF10/0x1401E6C70
void CEffect::StopAnimKit(int32_t keepCallback, CAnimKitInst** stopKitInst) {
    if (!this->m_animKitInst) {
        return;
    }

    if (!keepCallback) {
        this->m_animKitInst->SetStopCallback(NULL, 0);
    }

    this->m_animKitInst->Stop(1, stopKitInst);
    this->m_animKitInst->Release();
    this->m_animKitInst = NULL;
}

// OFFSET: 0x0057FF60
static CEffectVirtualItem* NewVirtualItem(int32_t zero) {
    return static_cast<CEffectVirtualItem*>(s_virtualItemAllocator.GetData(zero, typeid(CEffectVirtualItem).raw_name(), SLOG_OBJECT));
}

// PROVEN: 58545550 IDENTICAL/NONE
// OFFSET: 0x0057FF90
void CEffectCurve::Clear() {
    CEffectCurvePoint* point = this->m_points;

    while (point) {
        CEffectCurvePoint* next = point->m_next;
        s_curvePointAllocator.PutData(point, NULL, 0);
        point = next;
    }

    this->m_points = NULL;
}

// PROVEN: a92dfabd IDENTICAL/NONE
// OFFSET: 0x0057FFD0
CEffectCurve* CEffectCurve::New() {
    CEffectCurve* curve = static_cast<CEffectCurve*>(s_curveAllocator.GetData(0, typeid(CEffectCurve).raw_name(), SLOG_OBJECT));

    if (curve) {
        curve->m_points = NULL;
    }

    return curve;
}

// OFFSET: 0x00580000
float CEffectCurve::Evaluate(float time) {
    CEffectCurvePoint* next = this->m_points;
    CEffectCurvePoint* point = NULL;

    if (!next) {
        return 0.0f;
    }

    do {
        if (time < next->m_time) {
            break;
        }

        point = next;
        next = next->m_next;
    } while (next);

    if (!point) {
        if (!next) {
            return 0.0f;
        }

        return next->m_value - (next->m_time - time) * next->m_inSlope;
    }

    if (!next) {
        return (point->m_time - time) * point->m_outSlope + point->m_value;
    }

    if (point->m_dirty) {
        float t0 = point->m_time;
        float t1 = next->m_time;

        C44Matrix matrix;
        matrix.a0 = t0 * t0 * t0;
        matrix.a1 = t0 * t0 * 3.0f;
        matrix.a2 = t1 * t1 * t1;
        matrix.a3 = t1 * t1 * 3.0f;
        matrix.b0 = t0 * t0;
        matrix.b1 = t0 * 2.0f;
        matrix.b2 = t1 * t1;
        matrix.b3 = t1 * 2.0f;
        matrix.c0 = t0;
        matrix.c1 = 1.0f;
        matrix.c2 = t1;
        matrix.c3 = 1.0f;
        matrix.d0 = 1.0f;
        matrix.d1 = 0.0f;
        matrix.d2 = 1.0f;
        matrix.d3 = 0.0f;

        float values[4] = { point->m_value, point->m_inSlope, next->m_value, point->m_outSlope };
        float determinant = matrix.Determinant();

        point->m_coefficients[0] = 0.0f;
        point->m_coefficients[1] = 0.0f;
        point->m_coefficients[2] = 0.0f;
        point->m_coefficients[3] = 0.0f;

        if (fabs(determinant) >= 0.0000001f) {
            C44Matrix inverse = matrix.Inverse(determinant);
            point->m_coefficients[0] = inverse.d0 * values[3] + inverse.c0 * values[2] + inverse.b0 * values[1] + inverse.a0 * values[0];
            point->m_coefficients[1] = inverse.d1 * values[3] + inverse.c1 * values[2] + inverse.b1 * values[1] + inverse.a1 * values[0];
            point->m_coefficients[2] = inverse.d2 * values[3] + inverse.c2 * values[2] + inverse.b2 * values[1] + inverse.a2 * values[0];
            point->m_coefficients[3] = inverse.d3 * values[3] + inverse.c3 * values[2] + inverse.b3 * values[1] + inverse.a3 * values[0];
        } else {
            point->m_coefficients[3] = point->m_value;
        }

        point->m_dirty = 0;
    }

    return time * point->m_coefficients[2] + time * time * point->m_coefficients[1] + time * time * time * point->m_coefficients[0] + point->m_coefficients[3];
}

// PROVEN: 7f0da729 IDENTICAL/NONE
// OFFSET: 0x005802D0
const SpellVisualEffectNameRec* GetSpecialSpellVisualEffectNameRec(int32_t id) {
    const SpellSpecialUnitEffectRec* rec = g_spellSpecialUnitEffectDB.GetRecord(id);

    if (!rec) {
        return NULL;
    }

    return g_spellVisualEffectNameDB.GetRecord(rec->m_spellVisualEffectNameID);
}

// OFFSET: 0x00580320/0x1401E7020
CEffect::CEffect() {
    this->m_scale = 1.0f;
    this->m_model = NULL;
    this->m_lineModel = NULL;
    this->m_sourceGuid = WOWGUID();
    this->m_targetGuid = WOWGUID();
    this->m_attachGuid = WOWGUID();
    this->m_spellId = 0;
    this->m_kitRec = NULL;
    this->m_effectNameRec = NULL;
    this->m_attachmentId = -1;
    this->m_offset.x = 0.0f;
    this->m_offset.y = 0.0f;
    this->m_offset.z = 0.0f;
    this->m_soundHandle = 0;
    this->m_soundGuid = WOWGUID();
    this->m_flags = 0;
    this->m_flags2 = 0;
    this->unk_0088 = 0;
    this->unk_008C = 0;
    this->m_facing = 0.0f;
    this->m_orientation.x = 0.0f;
    this->m_orientation.y = 0.0f;
    this->m_orientation.z = 0.0f;
    this->unk_00A0 = -1;
    this->m_mountTransition = NULL;
    this->m_pendingCacheId = 0;
    this->m_displayId = 0;
    this->m_inventoryType = 30;
    this->m_param = 0;
    this->m_attachRec = NULL;
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->m_worldPosition.x = 0.0f;
    this->m_worldPosition.y = 0.0f;
    this->m_worldPosition.z = 0.0f;
    this->m_transportGuid = WOWGUID();
    this->m_startTime = 0;
    this->m_expireTime = 0;
    this->unk_00F0 = 0;
    this->m_fadeAlpha = 0.0f;
    this->m_tintColor = 0;
    this->m_mapEntity = 0;
    this->m_refCount = 1;
    this->m_lightningRefCount = 0;
    this->m_fade = 0.0f;
    this->m_lightningState = 0;
    this->m_animKitInst = NULL;
    this->m_blizzard = NULL;
    this->m_prevLink = NULL;
    this->m_next = NULL;
    this->m_data.m_uint[0] = 0;
    this->m_data.m_uint[1] = 0;

    for (uint32_t i = 0; i < 12; i++) {
        this->m_lightning[i] = NULL;
    }
}

// OFFSET: 0x005804B0/0x1401E7170
CM2Model* CEffect::CreateModelFromEffect(int32_t effectId, float* scale, float* alpha, WOWGUID guid, uint32_t displayId, uint32_t inventoryType) {
    if (scale) {
        *scale = 1.0f;
    }

    if (alpha) {
        *alpha = 1.0f;
    }

    const SpellVisualEffectNameRec* rec = g_spellVisualEffectNameDB.GetRecord(effectId);

    if (!rec) {
        return NULL;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    float modelScale = 1.0f;
    CM2Model* model = NULL;

    switch (rec->m_type) {
        case 1:
            if (!unit) {
                return NULL;
            }

            model = unit->CreateItemModel(0, &modelScale);
            break;

        case 2:
            if (!unit) {
                return NULL;
            }

            model = unit->CreateItemModel(1, &modelScale);
            break;

        case 3:
            if (!unit) {
                return NULL;
            }

            model = unit->CreateItemModel(2, &modelScale);
            break;

        case 4:
        case 5:
            if (!unit) {
                return NULL;
            }

            model = unit->CreateItemModel(displayId, inventoryType, NULL);
            break;

        default:
            model = World::GetScene()->CreateModel(rec->m_fileName, 32, 3);
            modelScale = rec->m_scale;
            break;
    }

    if (model) {
        if (rec->m_alpha < 0.99f) {
            model->m_fadeAlpha = rec->m_alpha;

            if (alpha) {
                *alpha = rec->m_alpha;
            }
        }

        if (scale) {
            *scale = modelScale;
        }
    }

    return model;
}

// OFFSET: 0x00580600/0x1401E7340
void CEffect::ApplyItemVisual() {
    if (!(this->m_flags & 0x80000) || this->m_inventoryType >= 30 || !this->m_displayId || (this->m_flags & 0x20000080)) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    CGPlayer_C* player = NULL;
    int32_t canEquip;

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        player = static_cast<CGPlayer_C*>(unit);
        canEquip = unit->CanEquipItemsInThisForm();
    } else {
        canEquip = this->m_inventoryType == INVTYPE_HEAD;
    }

    if (!canEquip) {
        return;
    }

    ItemDisplayInfoRec displayInfo;

    if (!g_itemDisplayInfoDB.GetRecord(this->m_displayId, &displayInfo)) {
        return;
    }

    CCharacterComponent* component = unit->m_characterComponent;

    if (!component) {
        return;
    }

    INVENTORY_SLOTS slot = CCharacterComponent::GetInvSlotForInvType(static_cast<INVENTORY_TYPES>(this->m_inventoryType));
    uint32_t hand;

    switch (slot) {
        case INVSLOT_MAINHAND:
            hand = 0;
            break;

        case INVSLOT_OFFHAND:
            hand = 1;
            break;

        case INVSLOT_RANGED:
            hand = 2;
            break;

        default:
            goto component;
    }

    if (!player) {
        goto component;
    }

    {
        const VisibleItem_C* visible = player->GetVisibleItem(slot);
        VisibleItem_C oldItem;
        const VisibleItem_C* old = NULL;

        if (visible) {
            oldItem = *visible;
            old = &oldItem;
        }

        if (this->m_data.m_virtualItem) {
            s_virtualItemAllocator.PutData(this->m_data.m_virtualItem, NULL, 0);
            this->m_data.m_virtualItem = NULL;
        }

        CEffectVirtualItem* item = NewVirtualItem(0);
        this->m_data.m_virtualItem = item;
        item->m_hand = hand;
        item->m_item.m_itemId = this->m_pendingCacheId;
        item->m_item.m_enchantment[0] = 0;
        item->m_item.m_enchantment[1] = 0;
        unit->UpdateEffectVirtualItemFlag();
        player->UpdateInventoryComponent(slot, old);

        if (hand == 2) {
            unit->SetSheatheState(2, 1, 0);
        }

        return;
    }

component:
    component->RemoveItemByType(static_cast<INVENTORY_TYPES>(this->m_inventoryType));
    component->AddItemByType(static_cast<INVENTORY_TYPES>(this->m_inventoryType), this->m_displayId);
    CGGameUI::UnitModelUpdate(this->m_sourceGuid, 3);
}

// PROVEN: e9bb61b0 IDENTICAL/NONE
// OFFSET: 0x005807F0
void CEffect::SetEmissionRateIgnoresDistance(int32_t ignore) {
    if (!this->m_model || !this->m_model->IsLoaded()) {
        return;
    }

    for (uint32_t i = 0; ; i++) {
        CM2Model* model = this->m_model;

        if (!(model->m_fFlags & 0x1)) {
            model->WaitForLoad(NULL);
        }

        if (i >= model->m_shared->m_data->m_particles.m_count) {
            break;
        }

        model = this->m_model;

        if (!(model->m_fFlags & 0x1)) {
            model->WaitForLoad(NULL);
        }

        CParticleEmitter2* emitter = model->m_particleEmitters[i];

        if (ignore) {
            emitter->m_flags |= 0x400000;
        } else {
            emitter->m_flags &= ~0x400000;
        }
    }
}

// OFFSET: 0x00580880/0x1401E75E0
void CEffect::LoadGuildTextures() {
    if (!this->m_model || !this->m_model->IsLoaded()) {
        return;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    if (object->m_obj->m_dataType == 1 && object->m_obj->m_data) {
        WOWGUID requester = object->m_obj->m_guid;

        if (!g_guildInfoCache.GetRecord2(object->m_obj->m_data.m_guid, requester, &CGObject_C::GuildTextureCallback, NULL, false)) {
            return;
        }
    }

    int32_t factionIndex = -1;

    if (object->m_obj->m_type & TYPEMASK_PLAYER) {
        factionIndex = static_cast<CGPlayer_C*>(object)->GetPVPFactionIndex();
    }

    CGObject_C::ReplaceGuildTextures(this->m_model, object->GetGuildGUID().m_guid, factionIndex);
}

// PROVEN: 5e7aa5d4 IDENTICAL/NONE
// OFFSET: 0x00580950/0x1401E76D0
void CEffect::EffectModelLoadedCallback(CM2Model* model, void* arg) {
    CEffect* effect = static_cast<CEffect*>(arg);

    if (effect->m_flags & 0x400000) {
        effect->SetEmissionRateIgnoresDistance(1);
    }

    const SpellVisualKitModelAttachRec* attach = effect->m_attachRec;

    if (attach && attach->unk_002C != -1 && model->HasSequence(attach->unk_002C)) {
        model->SetBoneSequence(-1, effect->m_attachRec->unk_002C, -1, 0, 1.0f, 1, 1, 0, 0, 0);
        effect->LoadGuildTextures();
        return;
    }

    model->SetBoneSequence(-1, 0, -1, 0, 1.0f, 1, 1, 0, 0, 0);
    effect->LoadGuildTextures();
}

// PROVEN: 128332a5 IDENTICAL/NONE
// OFFSET: 0x005809F0/0x1401E7820
void CEffect::LinkTo(CEffect** head) {
    if (this->m_prevLink) {
        *this->m_prevLink = this->m_next;
    }

    if (this->m_next) {
        this->m_next->m_prevLink = this->m_prevLink;
    }

    this->m_next = NULL;
    this->m_prevLink = head;
    this->m_next = *head;
    *head = this;

    if (this->m_next) {
        this->m_next->m_prevLink = &this->m_next;
    }
}

// OFFSET: 0x00580A50/0x1401E7880
C3Vector CEffect::TransformPositionByTransport() {
    if (!this->m_transportGuid) {
        return this->m_position;
    }

    C44Matrix transport;

    if (MovementGetTransportMtxSafe(this->m_transportGuid, &transport)) {
        this->m_worldPosition = this->m_position * transport;
        return this->m_worldPosition;
    }

    this->m_position = this->m_worldPosition;
    this->m_transportGuid = WOWGUID();

    return this->m_worldPosition;
}

// PROVEN: d24b62de IDENTICAL/NONE
// OFFSET: 0x00580C10/0x1401E7A80
void CEffect::DetachFromParent() {
    if (this->m_model && this->m_model->m_attachParent) {
        this->m_model->DetachFromParent();
    }
}

// PROVEN: e2dc8e9b IDENTICAL/NONE
// OFFSET: 0x00580C30/0x1401E7AA0
void CEffect::SetEnabled(int32_t enabled) {
    if (!this->m_model) {
        return;
    }

    this->m_model->m_baseAlpha = enabled ? 1.0f : 0.0f;
    this->m_model->SetEmittersEnabled(enabled);
    this->m_model->SetRibbonsEnabled(enabled);
}

// PROVEN: e59d6707 IDENTICAL/NONE
// OFFSET: 0x00580C70
void CEffect::InitLightningEndpoint() {
    this->m_lightningState = 0;

    if (!this->m_model) {
        return;
    }

    uint32_t time = this->m_model->m_scene->m_time;

    if (!time) {
        time = 1;
    }

    this->m_model->m_lastAnimTime = time;
    this->SetEnabled(0);
}

// PROVEN: 153cce60 IDENTICAL/NONE
// OFFSET: 0x00580CC0/0x1401E7B50
void CEffect::ConnectToLightningThisFrame() {
    if (!this->m_lightningState && this->m_model) {
        this->m_model->m_lastAnimTime = 0;
        this->SetEnabled(1);
    }

    this->m_lightningState = 2;
}

// OFFSET: 0x00580D10
float __stdcall CEffect::ComputeModelScaleFromTarget(CGObject_C* object) {
    CM2Model* model = object->GetObjectModel();

    if (model && !model->IsLoadedRecursive()) {
        return 1.0f;
    }

    float modelScale = 1.0f;

    if (object->m_obj->m_type & TYPEMASK_UNIT) {
        const CreatureModelDataRec* modelData = static_cast<CGUnit_C*>(object)->GetModelData();

        if (modelData) {
            modelScale = modelData->m_modelScale;
        }
    }

    CAaBox box = model->GetBoundingBox();
    float size = box.b.x - box.t.x;

    if (box.b.y - box.t.y <= size) {
        size = box.b.y - box.t.y;
    }

    size *= 0.3f;

    if (size <= 1.0f) {
        return 1.0f * modelScale;
    }

    return size * modelScale;
}

// PROVEN: 24596baf IDENTICAL/NONE
// OFFSET: 0x00580DE0
void CEffect::MoveToOrphanList(int32_t stopAnimKit) {
    if (stopAnimKit && this->m_animKitInst) {
        this->m_animKitInst->Stop(1, NULL);
        this->m_animKitInst->Release();
        this->m_animKitInst = NULL;
    }

    this->LinkTo(&s_orphanedEffects);
}

// OFFSET: 0x00580E30
void CEffect::PlayAnimKit(CGUnit_C* unit, AnimKitStopCallback callback, uint64_t param) {
    if (!this->m_kitRec || this->m_kitRec->m_animKitID <= 0 || !unit || !(unit->m_obj->m_type & TYPEMASK_UNIT)) {
        return;
    }

    CAnimKitInst* inst = unit->m_animKitManager.PlayAndMaintain(this->m_kitRec->m_animKitID, -1, NULL);

    if (inst) {
        inst->SetStopCallback(callback, param);
    }

    if (this->m_flags & 0x1040) {
        this->m_animKitInst = inst;
    } else if (inst) {
        inst->Release();
    }
}

// OFFSET: 0x00580EB0
float CEffect::GetClampedImpactEffectScale(float scale, const CGObject_C* object) const {
    static float s_levelScale = 1.0f / 28.0f;

    CM2Model* model = object->m_worldModel;
    CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!caster || (!(caster->m_obj->m_type & TYPEMASK_PLAYER) && !caster->m_unit->m_unitFieldPetnumber) || !(object->m_obj->m_type & TYPEMASK_UNIT) || !model || (model->m_fFlags & 0x2000) || !object->m_worldObject || World::ObjectIsInvisibleStalker(object->m_worldObject)) {
        return scale;
    }

    float attachScale = model->GetAttachmentWorldScale(this->m_attachmentId) * scale;
    float factor = (static_cast<const CGUnit_C*>(object)->m_selectionRadius - 1.0f) * s_levelScale;

    if (factor > 0.0f) {
        if (factor >= 1.0f) {
            factor = 1.0f;
        }
    } else {
        factor = 0.0f;
    }

    float maxScale = factor * 4.0f + 1.0f;

    if (attachScale <= maxScale) {
        maxScale = attachScale;
    }

    if (attachScale > 0.00000011920929f) {
        return maxScale / attachScale * scale;
    }

    return scale;
}

// OFFSET: 0x005810F0
void CEffectCurve::AddPoint(float time, float value, float inSlope, float outSlope) {
    CEffectCurvePoint* point = static_cast<CEffectCurvePoint*>(s_curvePointAllocator.GetData(0, typeid(CEffectCurvePoint).raw_name(), SLOG_OBJECT));

    if (point) {
        point->m_coefficients[0] = 0.0f;
        point->m_coefficients[1] = 0.0f;
        point->m_coefficients[2] = 0.0f;
        point->m_coefficients[3] = 0.0f;
        point->m_time = 0.0f;
        point->m_value = 0.0f;
        point->m_inSlope = 0.0f;
        point->m_outSlope = 0.0f;
        point->m_dirty = 1;
        point->m_next = NULL;
    }

    point->m_value = value;
    point->m_inSlope = inSlope;
    point->m_time = time;
    point->m_outSlope = outSlope;

    CEffectCurvePoint* next = this->m_points;
    CEffectCurvePoint* prev = NULL;

    if (next) {
        do {
            if (time < next->m_time) {
                break;
            }

            prev = next;
            next = next->m_next;
        } while (next);

        if (prev) {
            prev->m_next = point;
            prev->m_dirty = 1;
            point->m_next = next;
            return;
        }
    }

    this->m_points = point;
    point->m_next = next;
}

// OFFSET: 0x005811B0/0x1401E8070
void CEffect::EffectEvent(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time, uint64_t param) {
    CEffect* effect = reinterpret_cast<CEffect*>(static_cast<uintptr_t>(param));
    CGObject_C* object = ClntObjMgrObjectPtr(effect->m_attachGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    if (eventId == 'DNS$') {
        if ((object->m_obj->m_type & TYPEMASK_UNIT) && static_cast<CGUnit_C*>(object)->IsActiveMover()) {
            SE3SoundKitProperties properties;
            properties.m_combinable = 1;
            properties.m_usage = 2;
            bool listenerAtCharacter = SI3::sm_pCVar_Sound_ListenerAtCharacter && SI3::sm_pCVar_Sound_ListenerAtCharacter->m_intValue;
            properties.m_priority = 110;
            SE3::PlaySoundKit(data, listenerAtCharacter ? NULL : position, &properties, 0, __FILE__, __LINE__, "CEffect::EffectEvent FOURCC_SOUND (On Player)", 0, 0);
        } else {
            SE3SoundKitProperties properties;
            properties.m_combinable = 1;
            properties.m_usage = 2;
            SE3::PlaySoundKit(data, position, &properties, 0, __FILE__, __LINE__, "CEffect::EffectEvent FOURCC_SOUND", 0, 0);
        }

        if (!(effect->m_flags & 0x200000)) {
            effect->m_soundGuid = object->m_obj->m_guid;
        }

        return;
    }

    if (eventId == 'TIH$' && (object->m_obj->m_type & TYPEMASK_UNIT)) {
        static_cast<CGUnit_C*>(object)->PlayWoundAnimKit(0);
    }
}

// OFFSET: 0x00581320/0x1401E8260
int32_t CEffect::CreateModelFromEffect(void (*callback)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7), uint32_t displayId, uint32_t inventoryType) {
    WOWGUID guid = this->m_targetGuid;

    if (!guid) {
        guid = this->m_sourceGuid;
    }

    this->m_model = CEffect::CreateModelFromEffect(this->m_effectNameRec->m_id, &this->m_scale, NULL, guid, displayId, inventoryType);

    if (!this->m_model) {
        return 0;
    }

    this->m_model->SetLoadedCallback(CEffect::EffectModelLoadedCallback, this);
    this->m_model->SetSequenceCallback(reinterpret_cast<void*>(callback), reinterpret_cast<uintptr_t>(this));
    this->m_model->SetEventCallback(reinterpret_cast<void*>(CEffect::EffectEvent), reinterpret_cast<uintptr_t>(this));

    if (this->m_flags & 0x400000) {
        this->SetEmissionRateIgnoresDistance(1);
    }

    return 1;
}

// OFFSET: 0x005813B0/0x1401E8390
void CEffect::AddEffectWithModel(CM2Model* model, uint32_t flags, uint32_t attachmentId, const WOWGUID& guid, void (*callback)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7)) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object || !model || !object->m_worldModel) {
        return;
    }

    this->m_sourceGuid = guid;
    this->m_attachGuid = guid;
    this->m_flags = 0;
    this->m_attachmentId = attachmentId;

    if (attachmentId == 53) {
        this->m_attachmentId = -1;
        this->m_flags = flags | 2;

        if (static_cast<CGUnit_C*>(object)->HasVehicleTransport()) {
            this->m_position = object->GetPosition();
            this->m_transportGuid = WOWGUID();
        } else {
            this->m_position = object->GetRawPosition();
            this->m_transportGuid = WOWGUID::Make(object->GetTransportGUID());
        }
    } else if (object->m_worldModel->IsLoaded() && !object->m_worldModel->HasAttachment(attachmentId)) {
        this->m_attachmentId = 19;
    }

    this->m_model = model;
    model->m_refCount++;
    model->SetLoadedCallback(CEffect::EffectModelLoadedCallback, this);
    model->SetSequenceCallback(reinterpret_cast<void*>(callback), reinterpret_cast<uintptr_t>(this));
    model->SetEventCallback(reinterpret_cast<void*>(CEffect::EffectEvent), reinterpret_cast<uintptr_t>(this));
    this->m_refCount++;
    this->LinkTo(&object->m_effectList);
}

// OFFSET: 0x00581530/0x1401E8530
void CEffect::AddEffectByPosition(C3Vector position, const C3Vector* orientation, uint32_t startTime, uint32_t spellId, const SpellVisualKitRec* kit, const SpellVisualEffectNameRec* effectName, const WOWGUID& source, const WOWGUID& target, uint32_t flags, void (*callback)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7), uint32_t param, const SpellVisualKitModelAttachRec* attach, WOWGUID transport, int32_t kitType, uint32_t displayId, uint32_t inventoryType) {
    CGObject_C* object = ClntObjMgrObjectPtr(source, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    this->m_effectNameRec = effectName;

    if (flags & 0x4000) {
        flags |= 0x4000000;
    }

    this->m_sourceGuid = source;
    this->m_targetGuid = target;
    this->m_flags = flags | 0x8002;
    this->m_position = position;
    this->m_transportGuid = transport;
    this->m_kitRec = kit;
    this->m_startTime = startTime;
    this->m_spellId = spellId;
    this->m_param = param;
    this->m_facing = static_cast<CGUnit_C*>(object)->GetSmoothFacing();

    if (orientation) {
        this->m_flags2 |= 2;
        this->m_orientation = *orientation;
    }

    this->m_attachRec = attach;
    this->CreateModelFromEffect(callback, displayId, inventoryType);

    if (this->m_model) {
        this->m_refCount++;

        if (attach) {
            const CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(attach->unk_0028);

            if (displayInfo) {
                CCharacterComponent::ApplyMonsterGeosets(this->m_model, displayInfo);
                const CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);

                if (modelData) {
                    CCharacterComponent::ReplaceMonsterSkin(this->m_model, displayInfo, modelData);
                }
            }

            if (attach->unk_0030 != -1 && this->m_model->IsLoaded() && this->m_model->HasSequence(attach->unk_0030)) {
                this->m_model->SetBoneSequence(-1, attach->unk_002C, -1, 0, 1.0f, 1, 1, 0, 0, 0);
            }
        }
    }

    this->LinkTo(&object->m_effectList);
}

// OFFSET: 0x005816F0
void CEffect::AddOrphanedEffect(uint32_t spellId, const SpellVisualKitRec* kit, const SpellVisualEffectNameRec* effectName, uint32_t flags, void (*callback)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7), const C3Vector& position, uint32_t param, WOWGUID transport) {
    this->m_effectNameRec = effectName;
    this->m_attachmentId = -1;
    this->m_sourceGuid = WOWGUID();
    this->m_targetGuid = WOWGUID();
    this->m_attachGuid = WOWGUID();
    this->m_position = position;
    this->m_effectNameRec = effectName;
    this->m_transportGuid = transport;
    this->m_kitRec = kit;
    this->m_spellId = spellId;
    this->m_flags = flags | 2;
    this->m_param = param;
    this->CreateModelFromEffect(callback, 0, 0);

    if (this->m_model) {
        this->m_refCount++;
    }

    this->LinkTo(&s_orphanedEffects);
}

// PROVEN: cce69c3d IDENTICAL/NONE
// OFFSET: 0x005817A0/0x1401E88E0
void CEffect::AddSoundEffect(uint32_t expireTime, uint32_t spellId, CGObject_C* object, const WOWGUID& target, const C3Vector* position, uint32_t param) {
    if (!object) {
        return;
    }

    this->LinkTo(&object->m_effectList);
    this->m_refCount++;
    this->m_expireTime = expireTime;
    this->m_spellId = spellId;

    if (position) {
        this->m_position = *position;
    }

    this->m_sourceGuid = object->m_obj->m_guid;
    this->m_targetGuid = target;
    this->m_flags |= 0x100000;
    this->m_param = param;
}

// OFFSET: 0x00581830/0x1401E89C0
void CEffect::AddAnimEffect(uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags, uint32_t param) {
    CGObject_C* object = ClntObjMgrObjectPtr(source, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    this->LinkTo(&object->m_effectList);
    this->m_refCount++;
    this->m_spellId = spellId;
    this->m_sourceGuid = source;
    this->m_targetGuid = target;
    this->m_flags |= flags | 0x800;
    this->m_kitRec = kit;
    this->m_param = param;
}

// OFFSET: 0x005818B0/0x1401E8AB0
void CEffect::AddAnimKitEffect(uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags, uint32_t param, AnimKitStopCallback callback, uint64_t callbackParam) {
    CGObject_C* object = ClntObjMgrObjectPtr(source, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    this->LinkTo(&object->m_effectList);
    this->m_refCount++;
    this->m_spellId = spellId;
    this->m_sourceGuid = source;
    this->m_targetGuid = target;
    this->m_flags |= flags;
    this->m_kitRec = kit;
    this->m_param = param;
    this->PlayAnimKit(static_cast<CGUnit_C*>(object), callback, callbackParam);
    this->m_flags |= 0x800;
}

// OFFSET: 0x00581950
void CEffect::AddLightning(uint32_t expireTime, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags) {
    CGObject_C* object = ClntObjMgrObjectPtr(source, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    this->LinkTo(&object->m_effectList);
    this->m_refCount++;
    this->m_spellId = spellId;
    this->m_expireTime = expireTime;
    this->m_kitRec = kit;
    this->m_sourceGuid = source;
    this->m_targetGuid = target;
    this->m_flags = flags;
}

// OFFSET: 0x005819D0
void CEffect::AddFreeze(uint32_t expireTime, float duration, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags) {
    CGObject_C* object = ClntObjMgrObjectPtr(source, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    this->LinkTo(&object->m_effectList);
    this->m_refCount++;

    CGUnit_C* unit = static_cast<CGUnit_C*>(object);
    CAnimKitManager* mountManager = unit->m_mountAnimKitManager;

    if (mountManager) {
        if (mountManager->IsFrozen()) {
            this->m_flags2 &= ~8;
        } else {
            this->m_flags2 |= 8;
        }

        mountManager->Freeze(duration > 0.0f ? static_cast<uint32_t>(static_cast<int64_t>(duration * 1000.0f)) : -1);
    } else {
        this->m_flags2 &= ~8;
    }

    CAnimKitManager* manager = &unit->m_animKitManager;

    if (manager) {
        if (manager->IsFrozen()) {
            this->m_flags2 &= ~4;
        } else {
            this->m_flags2 |= 4;
        }

        manager->Freeze(duration > 0.0f ? static_cast<uint32_t>(static_cast<int64_t>(duration * 1000.0f)) : -1);
    } else {
        this->m_flags2 &= ~4;
    }

    this->m_kitRec = kit;
    this->m_expireTime = expireTime;
    this->m_spellId = spellId;
    this->m_sourceGuid = source;
    this->m_targetGuid = target;
    this->m_flags |= flags | 4;
}

// PROVEN: 95ae8c4f IDENTICAL/NONE
// OFFSET: 0x00581B30
void CEffect::AddCamouflage(uint32_t expireTime, float alpha, float fadeTime, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags) {
    CGObject_C* object = ClntObjMgrObjectPtr(source, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    this->m_expireTime = expireTime;
    this->m_spellId = spellId;
    this->m_kitRec = kit;
    this->m_sourceGuid = source;
    this->m_targetGuid = target;
    this->m_flags |= flags | 0x10000000;
    this->m_data.m_float[0] = alpha;
    this->m_data.m_float[1] = fadeTime;
    this->LinkTo(&object->m_effectList);
    this->m_refCount++;
}

// PROVEN: f73f2609 IDENTICAL/NONE
// OFFSET: 0x00581BD0
void CEffect::AddOrientation(uint32_t expireTime, float yaw, float pitch, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags) {
    CGObject_C* object = ClntObjMgrObjectPtr(source, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    this->m_expireTime = expireTime;
    this->m_spellId = spellId;
    this->m_kitRec = kit;
    this->m_sourceGuid = source;
    this->m_targetGuid = target;
    this->m_data.m_float[0] = yaw * 0.017453292f;
    this->m_flags |= flags | 0x400;
    this->m_data.m_float[1] = pitch * 0.017453292f;
    this->LinkTo(&object->m_effectList);
    this->m_refCount++;
}

// OFFSET: 0x00581C80
void CEffect::AddCurveEffect(uint32_t duration, CEffectCurve* curve, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags) {
    CGObject_C* object = ClntObjMgrObjectPtr(source, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object || !curve || !duration) {
        return;
    }

    uint32_t now = FrameTime::GetCurTimeMs();
    this->m_startTime = now;
    this->m_expireTime = duration + now;
    this->m_spellId = spellId;
    this->m_kitRec = kit;
    this->m_sourceGuid = source;
    this->m_targetGuid = target;
    this->m_flags |= flags;
    this->m_data.m_curve = curve;
    this->m_flags2 |= 1;
    this->m_data.m_uint[1] = 0;
    this->LinkTo(&object->m_effectList);
    this->m_refCount++;
}

// OFFSET: 0x00581D30/0x1401E91C0
int32_t CEffect::LoadMountTransitionModel() {
    if (!this->m_displayId) {
        return 0;
    }

    const CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(this->m_displayId);

    if (!displayInfo) {
        return 0;
    }

    const CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);

    if (!modelData) {
        return 0;
    }

    this->m_model = World::GetScene()->CreateModel(modelData->m_modelName, 0, 3);
    this->m_model->SetSequenceCallback(reinterpret_cast<void*>(CEffect::MountTransitionSequenceCB), reinterpret_cast<uintptr_t>(this));
    this->m_model->SetEventCallback(reinterpret_cast<void*>(CEffect::EffectEvent), reinterpret_cast<uintptr_t>(this));
    CCharacterComponent::ReplaceMonsterSkin(this->m_model, displayInfo, modelData);
    this->m_model->SetBoneSequence(-1, 127, -1, 0, 1.0f, 1, 1, 0, 0, 0);

    CM2Model* model = this->m_model;

    if (model->m_attachParent) {
        model->m_fFlags &= ~0x40;
    } else {
        model->m_fFlags &= ~0x8;
    }

    if (model->m_attachParent) {
        model->m_fFlags &= ~0x10000;
    } else {
        model->m_fFlags &= ~0x8000;
    }

    model->SetAnimating(1);

    return 1;
}

// OFFSET: 0x00581E50/0x1401E9390
CEffect* CEffect::Create() {
    CEffect* effect = static_cast<CEffect*>(s_effectAllocator.GetData(0, typeid(CEffect).raw_name(), SLOG_OBJECT));

    if (!effect) {
        return NULL;
    }

    new (effect) CEffect();

    return effect;
}

// OFFSET: 0x00581E80
void CEffect::ApplyModelAttachRecToMatrix(const SpellVisualKitModelAttachRec* attach, C44Matrix& matrix, float facing) {
    float x = matrix.d0;
    float y = matrix.d1;
    float z = matrix.d2;
    matrix.d0 = 0.0f;
    matrix.d1 = 0.0f;
    matrix.d2 = 0.0f;

    if (attach->m_yaw != 0.0f) {
        matrix.RotateAroundZ(-attach->m_yaw);
    }

    if (attach->m_pitch != 0.0f) {
        matrix.RotateAroundY(attach->m_pitch);
    }

    if (attach->m_roll != 0.0f) {
        matrix.RotateAroundX(attach->m_roll);
    }

    if (facing == 0.0f) {
        matrix.d0 = attach->m_offset[0] + x;
        matrix.d1 = y - attach->m_offset[1];
    } else {
        float s;
        float c;
        CMath::sincos_(facing - 1.5707964f, &s, &c);
        matrix.d0 = c * attach->m_offset[1] + x - attach->m_offset[0] * s;
        matrix.d1 = attach->m_offset[0] * c + s * attach->m_offset[1] + y;
    }

    matrix.d2 = attach->m_offset[2] + z;
}

// OFFSET: 0x00581FE0
C44Matrix CEffect::TransformPosition(CGObject_C* object) {
    C44Matrix matrix;
    C3Vector position = this->TransformPositionByTransport();
    matrix.d0 = position.x;
    matrix.d1 = position.y;
    matrix.d2 = position.z;

    float facing = 0.0f;

    if (object) {
        if (object->m_obj->m_type & TYPEMASK_UNIT) {
            facing = static_cast<CGUnit_C*>(object)->GetSmoothFacing();
        } else {
            facing = object->GetFacing();
        }
    }

    if (this->m_flags & 0x8000) {
        matrix.RotateAroundZ(this->m_facing);
    } else if (object) {
        matrix.RotateAroundZ(facing);
    }

    if (this->m_flags2 & 2) {
        float rightX = matrix.a0;
        float rightY = matrix.a1;
        float dirX = this->m_orientation.x;
        float dirY = this->m_orientation.y;
        float dirZ = this->m_orientation.z;
        float length = sqrt(dirY * dirY + dirX * dirX + dirZ * dirZ);

        if (length > 0.00000023841858f) {
            dirX = 1.0f / length * dirX;
            dirY = 1.0f / length * dirY;
            dirZ = 1.0f / length * dirZ;

            if (fabs(dirZ) > 0.00000023841858f) {
                float rightZ = -((dirY * rightY + dirX * rightX) / dirZ);
                float rightLength = sqrt(rightX * rightX + rightY * rightY + rightZ * rightZ);

                if (rightLength > 0.00000023841858f) {
                    rightX = 1.0f / rightLength * rightX;
                    rightZ = 1.0f / rightLength * rightZ;
                    rightY = 1.0f / rightLength * rightY;

                    float upX = rightZ * dirY - rightY * dirZ;
                    float upZ = rightY * dirX - dirY * rightX;
                    float upY = rightX * dirZ - rightZ * dirX;
                    float upLength = sqrt(upZ * upZ + upY * upY + upX * upX);

                    if (upLength > 0.00000023841858f) {
                        matrix.a0 = rightX;
                        matrix.a1 = rightY;
                        matrix.a2 = rightZ;
                        matrix.b0 = upX * (1.0f / upLength);
                        matrix.b1 = upY * (1.0f / upLength);
                        matrix.b2 = upZ * (1.0f / upLength);
                        matrix.c0 = dirX;
                        matrix.c1 = dirY;
                        matrix.c2 = dirZ;
                    }
                }
            }
        }
    }

    float scale = 1.0f;
    float clamped = 1.0f;

    if ((this->m_flags & 0x200) && object) {
        float targetScale = CEffect::ComputeModelScaleFromTarget(object);
        clamped = object->GetTrueScale() * targetScale;
        scale = clamped;
    }

    if (this->m_effectNameRec) {
        clamped = this->m_scale * scale;
        scale = clamped;

        if (this->m_effectNameRec->m_minAllowedScale <= clamped) {
            if (this->m_effectNameRec->m_maxAllowedScale <= clamped) {
                scale = this->m_effectNameRec->m_maxAllowedScale;
                clamped = scale;
            }
        } else {
            scale = this->m_effectNameRec->m_minAllowedScale;
            clamped = scale;
        }
    }

    if (scale <= 0.0f) {
        clamped = 1.0f;
    }

    matrix.Scale(clamped);

    if (this->m_attachRec) {
        CEffect::ApplyModelAttachRecToMatrix(this->m_attachRec, matrix, (this->m_flags & 0x8000) ? this->m_facing : facing);
    }

    return matrix;
}

// OFFSET: 0x00582410
void CEffect::DrawFishingString(CM2Model* model, const CM2Lighting* lighting, void* arg) {
    if (!model->HasEvent('HCC$')) {
        return;
    }

    CEffect* effect = static_cast<CEffect*>(arg);
    CGObject_C* bobber = ClntObjMgrObjectPtr(effect->m_attachGuid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (!bobber) {
        return;
    }

    C3Vector start = model->GetEventPosition('HCC$');
    C3Vector end;

    if (bobber->m_worldModel && bobber->m_worldModel->IsLoaded()) {
        end = bobber->m_worldModel->GetEventPosition('HCC$');
    } else {
        end = bobber->GetPosition();
        end.z = bobber->GetScale() * bobber->m_height * 0.02f + end.z;
    }

    C3Vector delta(end.x - start.x, end.y - start.y, end.z - start.z);

    float fade = 1.0f - model->m_alpha3;

    if (fade < 0.1f) {
        fade = 0.1f;
    }

    float alpha = model->m_effectiveAlpha * fade * 0.8f;
    float blue = lighting->m_sunAmbient.z * 0.7f;
    float red = lighting->m_sunAmbient.x * 0.7f;
    float green = lighting->m_sunAmbient.y * 0.7f;
    blue = blue >= 0.0f ? (blue >= 1.0f ? 1.0f : blue) : 0.0f;
    green = green >= 0.0f ? (green >= 1.0f ? 1.0f : green) : 0.0f;
    red = red >= 0.0f ? (red >= 1.0f ? 1.0f : red) : 0.0f;
    alpha = alpha >= 0.0f ? (alpha >= 1.0f ? 1.0f : alpha) : 0.0f;

    CImVector color;
    color.b = static_cast<int32_t>(blue * 255.0f);
    color.g = static_cast<int32_t>(green * 255.0f);
    color.r = static_cast<int32_t>(red * 255.0f);
    color.a = static_cast<int32_t>(alpha * 255.0f);
    GxFormatColor(color);

    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPC), 65);
    CGxVertexPC* vertices = reinterpret_cast<CGxVertexPC*>(GxBufLock(buf));

    for (int32_t i = 0; i < 65; i++) {
        float t = i * 0.015625f;
        vertices[i].p.x = delta.x * t + start.x;
        vertices[i].p.y = delta.y * t + start.y;
        vertices[i].p.z = delta.z * t + start.z;
        vertices[i].p.z = vertices[i].p.z - sin(t * 3.1415927f) * 0.5f;
        vertices[i].c = color;
    }

    GxBufUnlock(buf, 0);
    GxPrimVertexPtr(buf, GxVBF_PC);
    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Texture0, static_cast<void*>(NULL));
    GxRsSet(GxRs_BlendingMode, 2);

    float fogBlue = lighting->m_fogColor.z;
    fogBlue = fogBlue >= 0.0f ? (fogBlue >= 1.0f ? 1.0f : fogBlue) : 0.0f;
    float fogGreen = lighting->m_fogColor.y;
    fogGreen = fogGreen >= 0.0f ? (fogGreen >= 1.0f ? 1.0f : fogGreen) : 0.0f;
    float fogRed = lighting->m_fogColor.x;
    fogRed = fogRed >= 0.0f ? (fogRed >= 1.0f ? 1.0f : fogRed) : 0.0f;

    CImVector fogColor;
    fogColor.g = static_cast<int32_t>(fogGreen * 255.0f);
    fogColor.r = static_cast<int32_t>(fogRed * 255.0f);
    fogColor.b = static_cast<int32_t>(fogBlue * 255.0f);
    fogColor.a = static_cast<int32_t>(255.0f);
    DefaultShaders::SetFogParams(lighting->m_fogStart, lighting->m_fogEnd, lighting->m_fogDensity, fogColor);
    DefaultShaders::SetFogEnabled(1);
    DefaultShaders::SetDefaultShaders(static_cast<EGxVertexBufferFormat>(22), -1);

    CGxBatch batch;
    batch.m_minIndex = 0;
    batch.m_maxIndex = 64;
    batch.m_primType = GxPrim_LineStrip;
    batch.m_start = 0;
    batch.m_count = 65;
    g_theGxDevicePtr->DrawNonIndexed(batch);
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x005828F0/0x1401EA010
void CEffect::AddFishingLine(CGObject_C* object, const WOWGUID& target) {
    CM2Model* model = object->m_worldModel;

    if (!model) {
        return;
    }

    CM2Model* line = model->m_attachList;

    while (line && line->m_attachmentId != 1) {
        line = line->m_attachNext;
    }

    this->m_lineModel = line;
    line->m_refCount++;
    this->m_lineModel->m_drawCallback = &CEffect::DrawFishingString;
    this->m_lineModel->m_drawArg = this;
    this->m_lineModel->m_drawFlags = (this->m_lineModel->m_drawFlags & 0xFFF8) | 2;
    this->m_sourceGuid = object->m_obj->m_guid;
    this->m_attachGuid = target;
}

// OFFSET: 0x00582E20
void CEffect::ItemCacheCB(int32_t id, const WOWGUID& guid, void* param, bool success) {
    CEffect* effect = static_cast<CEffect*>(param);
    ItemRec* item = g_itemDB.GetRecord(id, WOWGUID(), NULL, NULL, false);

    if (!item) {
        return;
    }

    effect->m_inventoryType = item->m_inventoryType;
    effect->m_displayId = item->m_displayInfoID;
    effect->ApplyItemVisual();
}

// OFFSET: 0x00582E70
void CEffect::AddItemVisual(uint32_t expireTime, int32_t itemId, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags) {
    CGObject_C* object = ClntObjMgrObjectPtr(source, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    this->m_expireTime = expireTime;
    this->m_spellId = spellId;
    this->m_kitRec = kit;
    this->m_sourceGuid = source;
    this->m_targetGuid = target;
    this->m_flags |= flags | 0x80000;
    this->m_pendingCacheId = itemId;
    this->LinkTo(&object->m_effectList);
    this->m_refCount++;

    if (g_itemSparseDB.GetRecord(this->m_pendingCacheId, target, CEffect::ItemCacheCB, this, false)) {
        CEffect::ItemCacheCB(itemId, target, this, true);
    }
}

// OFFSET: 0x00582F30
void CEffect::ClearItemVisualEffect() {
    if (!(this->m_flags & 0x80000)) {
        return;
    }

    if (this->m_inventoryType < 30) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            if (unit->m_characterComponent) {
                INVENTORY_SLOTS slot = CCharacterComponent::GetInvSlotForInvType(static_cast<INVENTORY_TYPES>(this->m_inventoryType));

                if (this->m_data.m_virtualItem) {
                    VisibleItem_C oldItem = this->m_data.m_virtualItem->m_item;
                    s_virtualItemAllocator.PutData(this->m_data.m_virtualItem, NULL, 0);
                    this->m_data.m_virtualItem = NULL;
                    unit->UpdateEffectVirtualItemFlag();
                    static_cast<CGPlayer_C*>(unit)->UpdateInventoryComponent(slot, &oldItem);
                } else if (static_cast<uint32_t>(slot) <= INVSLOT_TABARD) {
                    unit->RefreshComponentItem(slot);
                    CGGameUI::UnitModelUpdate(this->m_sourceGuid, 3);
                }
            }

            this->m_inventoryType = 30;
        }
    }

    if (this->m_data.m_virtualItem) {
        s_virtualItemAllocator.PutData(this->m_data.m_virtualItem, NULL, 0);
        this->m_data.m_virtualItem = NULL;
    }

    if (this->m_pendingCacheId) {
        g_itemSparseDB.CancelCallback(this->m_pendingCacheId, CEffect::ItemCacheCB, this);
        this->m_pendingCacheId = 0;
    }

    this->m_flags &= 0x80000;
    this->m_inventoryType = 30;
}

// OFFSET: 0x00583060/0x1401EA9A0
void CEffect::MoveToDestroyList() {
    if ((this->m_flags & 0x80000) && this->m_data.m_virtualItem && this->m_data.m_virtualItem->m_hand == 2) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            unit->SetSheatheState(0, 1, 0);
        }
    }

    this->LinkTo(&s_destroyedEffects);
    this->m_flags |= 0x900;

    if (this->m_flags & 0x4) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            if ((this->m_flags2 & 8) && unit->m_mountModel) {
                if (unit->m_mountAnimKitManager) {
                    unit->m_mountAnimKitManager->Unfreeze();
                }

                this->m_flags2 &= ~8;
            }

            if ((this->m_flags2 & 4) && unit->m_worldModel) {
                unit->m_animKitManager.Unfreeze();
                this->m_flags2 &= ~4;
            }
        }
    }

    if (SE3::IsLoopingOrPeriodic(this->m_soundHandle)) {
        SE3::StopSound(this->m_soundHandle, 500, __FILE__, __LINE__, "CEffect::MoveToDestroyList");
    } else {
        this->m_soundHandle = 0;
    }

    if (this->m_mountTransition) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            if (SpellVisualsIsMountTransitionFinished(this->m_mountTransition)) {
                unit->CompleteMountTransition();
            } else {
                unit->StopMountTransition();
            }
        }

        SpellVisualsCancelMountTransition(this->m_mountTransition);
        this->m_mountTransition = NULL;
    }

    if (this->m_model) {
        this->m_model->SetSequenceCallback(NULL, 0);
        this->m_model->SetEventCallback(NULL, 0);
        this->m_model->SetLoadedCallback(NULL, NULL);

        CM2Model* model = this->m_model;
        CM2Model* parent = model->m_attachParent;

        if (parent) {
            model->m_fFlags &= ~0x40;
        } else {
            model->m_fFlags &= ~0x8;
        }

        if (parent) {
            model->m_fFlags &= ~0x10000;
        } else {
            model->m_fFlags &= ~0x8000;
        }

        if (this->m_model->m_attachParent) {
            this->m_model->m_fFlags |= 0x10000;
        } else {
            this->m_model->m_fFlags |= 0x8000;
        }

        this->m_model->SetEmittersEnabled(0);
    }

    this->ClearItemVisualEffect();
    this->ClearLightning();

    if (this->m_animKitInst) {
        this->m_animKitInst->Stop(1, NULL);
        this->m_animKitInst->Release();
        this->m_animKitInst = NULL;
    }

    if (!this->m_prevLink) {
        this->m_refCount++;
    }
}

// OFFSET: 0x00583280/0x1401EAC70
void CEffect::UpdateWorldObject(float elapsed, uint32_t flags) {
    if ((!this->m_startTime || static_cast<int32_t>(CGWorldFrame::s_currentWorldFrame->GetSceneTime() - this->m_startTime) > 0) && this->m_model) {
        this->m_model->SetAnimating(1);

        if (!(this->m_flags & 0x100) && this->m_model->IsLoaded()) {
            CM2Model* model = this->m_model;
            CM2Model* parent = model->m_attachParent;
            uint32_t visible = flags & 1;

            if (parent) {
                model->m_fFlags = (model->m_fFlags & ~0x40) | (visible << 6);
            } else {
                model->m_fFlags = (model->m_fFlags & ~0x8) | (visible << 3);
            }

            if (parent) {
                model->m_fFlags = (model->m_fFlags & ~0x10000) | (visible << 16);
            } else {
                model->m_fFlags = (model->m_fFlags & ~0x8000) | (visible << 15);
            }

            this->m_startTime = 0;
        }
    }

    if (this->m_expireTime && static_cast<int32_t>(CGWorldFrame::s_currentWorldFrame->GetSceneTime() - this->m_expireTime) > 0) {
        this->MoveToDestroyList();
    }
}

// PROVEN: b70ee3d7 IDENTICAL/NONE
// OFFSET: 0x00583370
void __stdcall CEffect::DestroyMatchingEffects(CGObject_C* object, const SpellVisualEffectNameRec* effectName, int32_t attachType, int32_t spellId, int32_t kitId, const SpellVisualKitModelAttachRec* attach) {
    if (attachType == -1) {
        return;
    }

    CEffect* effect = object->m_effectList;

    while (effect) {
        CEffect* next = effect->m_next;

        if (effect->m_effectNameRec && effect->m_effectNameRec->m_id == effectName->m_id && (!effect->m_kitRec || effect->m_kitRec->m_id == kitId) && (!effect->m_attachRec || effect->m_attachRec == attach) && (!spellId || effect->m_spellId == static_cast<uint32_t>(spellId)) && effect->m_attachmentId == attachType) {
            effect->MoveToDestroyList();
        }

        effect = next;
    }
}

// OFFSET: 0x005833F0/0x1401EAE20
int32_t CEffect::UpdateAttachment() {
    if (this->m_attachmentId == -1) {
        if (this->m_model && this->m_model->m_attachParent) {
            this->m_model->DetachFromParent();
        }

        return 1;
    }

    CGObject_C* object;

    if (this->m_attachGuid) {
        object = ClntObjMgrObjectPtr(this->m_attachGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);
    } else {
        if (!this->m_sourceGuid) {
            goto detach;
        }

        object = ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);
    }

    if (!object) {
        goto detach;
    }

    {
        CM2Model* parent = object->m_worldModel;

        if (parent && !parent->IsLoaded()) {
            DebugLog("**** CEffect::UpdateAttachment() called before %s finished loading\n", parent->m_shared->m_fileNameWithoutPath);

            if (this->m_model && this->m_model->m_attachParent) {
                this->m_model->DetachFromParent();
            }

            this->MoveToDestroyList();
            return 0;
        }

        const CreatureModelDataRec* modelData = (object->m_obj->m_type & TYPEMASK_UNIT) ? static_cast<CGUnit_C*>(object)->GetModelData() : NULL;

        if ((this->m_attachmentId == 2 || this->m_attachmentId == 1) && modelData) {
            const CreatureModelDataRec* data = static_cast<CGUnit_C*>(object)->GetModelData();

            if (data && (data->m_flags & 0x10)) {
                this->DetachFromParent();
                return 0;
            }
        }

        if (!parent || !parent->HasAttachment(this->m_attachmentId)) {
            goto detach;
        }

        float scale = 1.0f;

        if (modelData) {
            scale = modelData->m_attachedEffectScale;
        }

        if (this->m_effectNameRec) {
            scale = this->m_scale * scale;
            float worldScale = parent->GetAttachmentWorldScale(this->m_attachmentId) * scale;

            if (worldScale > 0.000001f) {
                float limit = this->m_effectNameRec->m_maxAllowedScale;

                if (worldScale > limit || (limit = this->m_effectNameRec->m_minAllowedScale, limit > worldScale)) {
                    scale = limit / worldScale * scale;
                }
            }
        }

        if (this->m_model && (this->m_flags & 0x40000000) && !(this->m_flags2 & 1)) {
            this->m_model->m_fFlags |= 0x10000000;
            float squelch = object->AddSquelchedEffect();

            if (squelch > 0.0f) {
                CM2Model* model = this->m_model;
                this->m_fade = squelch;
                float alpha = (1.0f - squelch) * model->m_fadeAlpha;
                model->m_fadeAlpha = alpha;
                uint32_t now = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
                this->m_startTime = now;
                this->m_expireTime = now - static_cast<int64_t>(alpha * -10000.0f);
            }
        }

        if (this->m_flags & 0x20000000) {
            scale = this->GetClampedImpactEffectScale(scale, object);
        }

        if ((this->m_flags & 0x8) && modelData && modelData->unk_0070 > 0.0f) {
            scale = modelData->unk_0070;
        }

        if (this->m_model) {
            C44Matrix transform;

            if (this->m_attachRec) {
                CEffect::ApplyModelAttachRecToMatrix(this->m_attachRec, transform, 0.0f);
            }

            transform.Scale(scale);
            this->m_model->SetWorldTransform(transform);
        }

        if (this->m_flags & 0x10000) {
            if (this->m_model && (this->m_model->m_attachParent != parent || this->m_model->m_attachmentId != static_cast<uint32_t>(this->m_attachmentId))) {
                this->DetachFromParent();
                this->m_model->AttachToParent(parent, this->m_attachmentId, &this->m_offset, 0);
            }

            this->m_position = this->m_offset;
        } else {
            if (this->m_model && (this->m_model->m_attachParent != parent || this->m_model->m_attachmentId != static_cast<uint32_t>(this->m_attachmentId))) {
                this->DetachFromParent();
                this->m_model->AttachToParent(parent, this->m_attachmentId, NULL, 0);
            }

            this->m_position = parent->GetAttachmentPosition(this->m_attachmentId);
        }

        this->m_transportGuid = WOWGUID();

        if (!(this->m_flags & 0x2000000)) {
            this->m_model->m_fFlags |= 0x80000;
        }

        if (this->m_flags2 & 0x10) {
            this->m_model->m_fFlags |= 0x40000;
        }

        object->SetBoundsDirty();
        return 1;
    }

detach:
    if (this->m_model && this->m_model->m_attachParent) {
        this->m_model->DetachFromParent();
    }

    return 0;
}

// PROVEN: b9317e7c IDENTICAL/NONE
// OFFSET: 0x00583840
void CEffect::ReleaseLightningRef() {
    if (--this->m_lightningRefCount == 0) {
        this->MoveToDestroyList();
    }
}

// PROVEN: b55583c2 IDENTICAL/NONE
// OFFSET: 0x00583850
void CEffect::BlizzardStoppedCallback(BlizzardObject* blizzard, void* param) {
    CEffect* effect = static_cast<CEffect*>(param);

    if (blizzard == effect->m_blizzard) {
        effect->m_blizzard = NULL;
        effect->MoveToDestroyList();
    }
}

// OFFSET: 0x00583880
void CEffect::AddSpecialEffect(uint32_t type, const WOWGUID& guid, void (*callback)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7)) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object || type >= 15) {
        return;
    }

    int32_t attachment = s_specialUnitEffectAttachments[type];

    if (attachment == 53 || !object->m_worldModel) {
        return;
    }

    this->m_sourceGuid = guid;
    this->m_attachGuid = guid;
    this->m_flags = 0;
    this->m_attachmentId = attachment;

    if (object->m_worldModel->IsLoaded() && !object->m_worldModel->HasAttachment(this->m_attachmentId)) {
        this->m_attachmentId = 19;
    }

    this->m_effectNameRec = GetSpecialSpellVisualEffectNameRec(type);

    if (!this->m_effectNameRec) {
        return;
    }

    CEffect::DestroyMatchingEffects(object, this->m_effectNameRec, attachment, 0, 0, NULL);
    this->CreateModelFromEffect(callback, 0, 0);

    if (this->m_model) {
        this->m_refCount++;
    }

    this->LinkTo(&object->m_effectList);
}

// OFFSET: 0x00583980
void CEffect::AddSpecialByPosition(uint32_t type, C3Vector position, const WOWGUID& guid, uint32_t flags, void (*callback)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7)) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    this->m_sourceGuid = guid;
    this->m_flags = flags | 2;
    this->m_position = position;
    this->m_transportGuid = WOWGUID();
    this->m_effectNameRec = GetSpecialSpellVisualEffectNameRec(type);

    if (!this->m_effectNameRec) {
        return;
    }

    this->CreateModelFromEffect(callback, 0, 0);

    if (this->m_model) {
        this->m_refCount++;
    }

    this->LinkTo(&object->m_effectList);
}

// OFFSET: 0x00583A30/0x1401EB6F0
void CEffect::AddEffect(int32_t attachType, uint32_t spellId, const SpellVisualKitRec* kit, const SpellVisualEffectNameRec* effectName, const WOWGUID& source, const WOWGUID& target, const WOWGUID& attach, uint32_t flags, void (*callback)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7), const C3Vector* offset, uint32_t param, const SpellVisualKitModelAttachRec* attachRec, int32_t kitType, uint32_t displayId, uint32_t inventoryType) {
    CGObject_C* object = ClntObjMgrObjectPtr(attach, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    this->m_effectNameRec = effectName;
    CEffect::DestroyMatchingEffects(object, effectName, attachType, spellId, kit ? kit->m_id : 0, attachRec);

    if (attachType == -1) {
        DebugLog("**** CEffect::AddEffect called with ATTACH_NONE\n");
    }

    this->m_attachmentId = attachType;
    this->m_sourceGuid = source;
    this->m_targetGuid = target;
    this->m_attachGuid = attach;
    this->m_effectNameRec = effectName;
    this->m_param = param;
    this->m_spellId = spellId;
    this->m_kitRec = kit;
    this->m_flags = flags;
    this->m_attachRec = attachRec;

    if (offset) {
        this->m_offset = *offset;
        this->m_flags = flags | 0x10000;
    }

    this->CreateModelFromEffect(callback, displayId, inventoryType);

    if (this->m_model) {
        this->m_refCount++;

        if (attachRec) {
            const CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(attachRec->unk_0028);

            if (displayInfo) {
                CCharacterComponent::ApplyMonsterGeosets(this->m_model, displayInfo);
                const CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);

                if (modelData) {
                    CCharacterComponent::ReplaceMonsterSkin(this->m_model, displayInfo, modelData);
                }
            }

            if (attachRec->unk_0030 != -1 && this->m_model->IsLoaded() && this->m_model->HasSequence(attachRec->unk_0030)) {
                this->m_model->SetBoneSequence(-1, attachRec->unk_002C, -1, 0, 1.0f, 1, 1, 0, 0, 0);
            }
        }
    }

    this->LinkTo(&object->m_effectList);
}

// OFFSET: 0x00583BF0
void CEffect::AddBlizzard(uint32_t expireTime, const C3Vector* position, uint32_t spellId, uint32_t index, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags) {
    if (!kit) {
        return;
    }

    CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(source, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!caster) {
        return;
    }

    CGUnit_C* victim = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));
    const SpellRec* spell = g_spellDB.GetRecord(spellId);
    uint32_t level = victim ? victim->m_unit->m_unitFieldLevel : caster->m_unit->m_unitFieldLevel;
    float radius = Spell_C_GetEffectRadius(spell, level);

    if (radius < 0.00000011920929f) {
        return;
    }

    float angle = Spell_C_GetConeAngle(spell);

    if ((flags & 0x4000) || !(flags & 0x20002080)) {
        flags |= 0x4000000;
    }

    int32_t areaModelID = static_cast<int32_t>(kit->m_charParamZero[index]);

    if (flags & 0x20) {
        flags &= ~0x20;

        if (!expireTime && g_spellVisualKitAreaModelDB.GetRecord(static_cast<int32_t>(kit->m_charParamZero[index]))->m_lifeTime <= 0) {
            expireTime = 2000;
        }
    }

    this->LinkTo(&caster->m_effectList);
    this->m_refCount++;
    this->m_kitRec = kit;
    this->m_expireTime = expireTime;
    this->m_spellId = spellId;
    this->m_sourceGuid = source;
    this->m_targetGuid = target;
    this->m_flags = flags;

    C44Matrix matrix = caster->GetMatrix();
    C44Matrix inverse = matrix.AffineInverse();
    C3Vector local = *position * inverse;

    this->m_blizzard = SpellVisualsBlizzardCreate(&local, radius, angle * 0.017453292f, this->m_spellId, areaModelID);
    SpellVisualsBlizzardSetFrameOfReference(this->m_blizzard, &matrix);
    SpellVisualsBlizzardSetDeletedCallback(this->m_blizzard, CEffect::BlizzardStoppedCallback, this);
}

// OFFSET: 0x00583E70
float CEffect::GetCurveAlpha(float elapsed) {
    if (!(this->m_flags2 & 1) || (this->m_flags & 0x100)) {
        return 1.0f;
    }

    double duration = static_cast<double>(this->m_expireTime - this->m_startTime) * 0.001;
    float time = this->m_data.m_float[1] + elapsed;
    this->m_data.m_float[1] = time;

    if (time >= static_cast<float>(duration)) {
        this->MoveToDestroyList();
        return 1.0f;
    }

    return this->m_data.m_curve->Evaluate(static_cast<float>(time / duration));
}

// OFFSET: 0x00583EF0/0x1401EBD50
void CEffect::MountTransitionCacheCB(int32_t id, const WOWGUID& guid, void* param, bool success) {
    CEffect* effect = static_cast<CEffect*>(param);
    WOWGUID key = { static_cast<uint64_t>(effect->m_spellId) | 0x1FE0000000000000ull };
    CreatureStats_C* stats = g_creatureDBCache.GetRecord(id, key, NULL, NULL, false);

    if (stats) {
        effect->m_displayId = stats->m_displayId[0];
        effect->m_pendingCacheId = 0;
        effect->LoadMountTransitionModel();
        effect->UpdateAttachment();
    }
}

// OFFSET: 0x00583F50
void CEffect::AddMountTransition(uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target) {
    this->m_sourceGuid = source;
    this->m_targetGuid = target;
    this->m_spellId = spellId;
    this->m_kitRec = kit;
    this->m_attachmentId = 19;

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spellId, count);

    if (!count) {
        return;
    }

    this->m_pendingCacheId = 0;
    this->m_displayId = 0;

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effect != 6 || effect->m_effectAura != 78) {
            continue;
        }

        this->m_pendingCacheId = effect->m_effectMiscValue;
        WOWGUID key = { static_cast<uint64_t>(spellId) | 0x1FE0000000000000ull };
        CreatureStats_C* stats = g_creatureDBCache.GetRecord(this->m_pendingCacheId, key, CEffect::MountTransitionCacheCB, this, false);

        if (stats) {
            this->m_displayId = stats->m_displayId[0];
            this->m_pendingCacheId = 0;
            break;
        }
    }

    if (this->m_displayId) {
        if (!this->LoadMountTransitionModel()) {
            return;
        }
    } else if (!this->m_pendingCacheId) {
        return;
    }

    this->LinkTo(&unit->m_effectList);
    this->m_refCount++;
    this->m_mountTransition = SpellVisualsStartMountTransition(unit);
    unit->StartMountTransition(this->m_mountTransition, this);
}

// OFFSET: 0x005840B0/0x1401EBFD0
void CEffect::PostInitialize() {
    if (this->m_model && !this->m_model->IsLoaded()) {
        return;
    }

    this->m_flags |= 0x800;

    if ((this->m_flags & 0x2) && !this->m_mapEntity && this->m_model) {
        CGObject_C* source = ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);
        C44Matrix matrix = this->TransformPosition(source);
        this->m_model->m_fFlags |= 0x4000;
        this->m_model->SetWorldTransform(matrix);
        this->m_mapEntity = World::ObjectCreate(this->m_model, this->m_model->m_worldTransform, CGWorldFrame::SpellEffectEnumProc, NULL, 0, reinterpret_cast<uintptr_t>(this), 6, __FILE__, __LINE__);

        CAaBox box;
        CAaSphere sphere;
        C3Vector light;
        memset(&box, 0, sizeof(box));
        memset(&sphere, 0, sizeof(sphere));
        light = C3Vector();

        if (this->m_model->IsLoaded()) {
            box = this->m_model->GetBoundingBox();
            sphere = this->m_model->GetBoundingSphere();
            light = sphere.c;
        }

        light.z = 1.0f;
        World::ObjectUpdate(this->m_mapEntity, this->m_model->m_worldTransform, box, sphere, light, 0, -1);

        int32_t animating = !this->m_startTime || static_cast<int32_t>(CGWorldFrame::s_currentWorldFrame->GetSceneTime() - this->m_startTime) > 0;
        this->m_model->SetAnimating(animating);

        CM2Model* model = this->m_model;
        CM2Model* parent = model->m_attachParent;
        uint32_t visible = animating;

        if (parent) {
            model->m_fFlags = (model->m_fFlags & ~0x40) | (visible << 6);
        } else {
            model->m_fFlags = (model->m_fFlags & ~0x8) | (visible << 3);
        }

        if (parent) {
            model->m_fFlags = (model->m_fFlags & ~0x10000) | (visible << 16);
        } else {
            model->m_fFlags = (model->m_fFlags & ~0x8000) | (visible << 15);
        }
    }

    const SpellRec* spell = g_spellDB.GetRecord(this->m_spellId);

    if (spell && this->m_model && (spell->m_attributesEx & 0x40000000)) {
        this->m_model->m_fFlags |= 0x4;
    }

    if (!this->UpdateAttachment()) {
        return;
    }

    if ((this->m_flags & 0x10) && this->m_kitRec && this->m_kitRec->m_shakeID > 0) {
        C3Vector position = this->TransformPositionByTransport();
        SpellVisualsPlayCameraShakeID(this->m_kitRec->m_shakeID, &position);
    }

    CGObject_C* object = NULL;

    if (this->m_attachGuid) {
        object = ClntObjMgrObjectPtr(this->m_attachGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);
    } else if (this->m_sourceGuid && !(this->m_flags & 0x2000)) {
        object = ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);
    }

    if (!(this->m_flags & 0x100000) || !this->m_kitRec || !this->m_kitRec->m_soundID || this->m_kitRec->m_soundID == -1) {
        return;
    }

    SE3SoundKitProperties properties;
    int32_t focus = CGObject_C::GetFocusObject() == object;
    int32_t listenerAtCharacter = focus && SI3::sm_pCVar_Sound_ListenerAtCharacter && SI3::sm_pCVar_Sound_ListenerAtCharacter->m_intValue && !(this->m_flags & 0x200000);

    if (focus || this->m_sourceGuid == CGUnit_C::s_activeMover) {
        properties.m_priority = 110;
    }

    if (CGGameUI::SequenceMusicIsPlaying()) {
        return;
    }

    if (this->m_flags & 0x1) {
        properties.m_checkRange = 0;

        if (!object) {
            C3Vector position = this->TransformPositionByTransport();
            SE3::PlaySoundKit(this->m_kitRec->m_soundID, &position, &properties, &this->m_soundHandle, __FILE__, __LINE__, "CEffect::PostInitialize (looped no target)", 0, 0);
            return;
        }

        C3Vector position = object->GetPosition();

        if (this->m_flags & 0x1000) {
            properties.m_bit0 = 1;
            properties.m_uniqueness = 1;
            properties.m_priority = 128;
        } else {
            properties.m_noRandomStart = 1;
        }

        SE3::PlaySoundKit(this->m_kitRec->m_soundID, listenerAtCharacter ? NULL : &position, &properties, &this->m_soundHandle, __FILE__, __LINE__, "CEffect::PostInitialize (looped with target)", 0, 0);

        if (!listenerAtCharacter && !(this->m_flags & 0x200000)) {
            this->m_soundGuid = object->m_obj->m_guid;
        }

        return;
    }

    properties.m_combinable = 1;
    properties.m_usage = 2;
    properties.m_uniqueness = 1;

    C3Vector position = C3Vector();
    const C3Vector* soundPosition = NULL;

    if (listenerAtCharacter) {
        properties.m_priority = 110;
    } else {
        properties.m_priority = 128;
        position = this->TransformPositionByTransport();

        if (object && position.z * position.z + position.y * position.y + position.x * position.x == 0.0f) {
            position = object->GetPosition();
        }

        if (this->m_targetGuid) {
            CGObject_C* target = ClntObjMgrObjectPtr(this->m_targetGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (target && position.z * position.z + position.y * position.y + position.x * position.x == 0.0f) {
                position = target->GetPosition();
            }
        }

        soundPosition = &position;
    }

    SE3::PlaySoundKit(this->m_kitRec->m_soundID, soundPosition, &properties, NULL, __FILE__, __LINE__, "CEffect::PostInitialize (one shot)", 0, 0);

    if (!listenerAtCharacter && !(this->m_flags & 0x200000) && object) {
        this->m_soundGuid = object->m_obj->m_guid;
    }
}

// OFFSET: 0x005846F0/0x1401EC6F0
CEffect::~CEffect() {
    if (this->m_animKitInst) {
        this->m_animKitInst->Stop(1, NULL);
        this->m_animKitInst->Release();
        this->m_animKitInst = NULL;
    }

    if (this->m_blizzard) {
        SpellVisualsBlizzardSetDeletedCallback(this->m_blizzard, NULL, NULL);
        SpellVisualsBlizzardStop(this->m_blizzard);
        this->m_blizzard = NULL;
    }

    if ((this->m_flags & 0x80000) && this->m_data.m_virtualItem) {
        s_virtualItemAllocator.PutData(this->m_data.m_virtualItem, NULL, 0);
        this->m_data.m_virtualItem = NULL;
    }

    if (this->m_flags2 & 1) {
        CEffectCurve* curve = this->m_data.m_curve;

        if (curve) {
            curve->Clear();
            s_curveAllocator.PutData(curve, NULL, 0);
            this->m_data.m_curve = NULL;
        }
    }

    if (this->m_model) {
        if (this->m_model->m_attachParent) {
            this->m_model->DetachFromParent();
        }

        this->m_model->SetSequenceCallback(NULL, 0);
        this->m_model->SetEventCallback(NULL, 0);
        this->m_model->SetLoadedCallback(NULL, NULL);

        CM2Model* model = this->m_model;
        CM2Model* parent = model->m_attachParent;

        if (parent) {
            model->m_fFlags &= ~0x40;
        } else {
            model->m_fFlags &= ~0x8;
        }

        if (parent) {
            model->m_fFlags &= ~0x10000;
        } else {
            model->m_fFlags &= ~0x8000;
        }

        this->m_model->Release();
        this->m_model = NULL;
    }

    if (this->m_lineModel) {
        this->m_lineModel->m_drawCallback = NULL;
        this->m_lineModel->Release();
    }

    if (this->m_prevLink) {
        *this->m_prevLink = this->m_next;
    }

    if (this->m_next) {
        this->m_next->m_prevLink = this->m_prevLink;
    }

    this->m_next = NULL;
    this->m_prevLink = NULL;

    if (this->m_mapEntity) {
        World::ObjectDestroy(this->m_mapEntity);
    }

    if (SE3::IsLoopingOrPeriodic(this->m_soundHandle)) {
        SE3::StopSound(this->m_soundHandle, 500, __FILE__, __LINE__, "CEffect::~CEffect");
    } else {
        this->m_soundHandle = 0;
    }

    if (this->m_mountTransition) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            unit->StopMountTransition();
        }

        SpellVisualsCancelMountTransition(this->m_mountTransition);
        this->m_mountTransition = NULL;

        if (this->m_pendingCacheId) {
            g_creatureDBCache.CancelCallback(this->m_pendingCacheId, CEffect::MountTransitionCacheCB, this);
            this->m_pendingCacheId = 0;
        }
    }

    this->ClearItemVisualEffect();
    this->ClearLightning();
}

// OFFSET: 0x00584900
void CEffect::EffectUpdate() {
    if (!(this->m_flags & 0x900)) {
        this->PostInitialize();
    }

    if (this->m_flags2 & 1) {
        return;
    }

    if ((this->m_flags & 0x40000) && this->m_lightningState > 0) {
        if (this->m_lightningState-- == 1 && this->m_model) {
            this->m_model->m_baseAlpha = 0.0f;
            this->m_model->SetEmittersEnabled(0);
            this->m_model->SetRibbonsEnabled(0);
        }
    }

    if ((this->m_flags & 0x4000000) || ((this->m_flags & 0x2) && this->m_transportGuid)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            if ((this->m_flags & 0x4000000) && this->m_blizzard) {
                C44Matrix matrix = unit->GetMatrix();
                SpellVisualsBlizzardSetFrameOfReference(this->m_blizzard, &matrix);
            } else if (this->m_mapEntity && this->m_model) {
                if (this->m_flags & 0x4000000) {
                    this->m_position = unit->GetRawPosition();

                    if (unit->m_movement->m_flags & 0x10000000) {
                        this->m_position.z = this->m_position.z - unit->m_unit->m_unitFieldHoverheight;
                    }

                    this->m_transportGuid = WOWGUID::Make(unit->GetTransportGUID());
                }

                C44Matrix matrix = this->TransformPosition(unit);
                CAaBox box;
                CAaSphere sphere;
                C3Vector light;
                memset(&box, 0, sizeof(box));
                memset(&sphere, 0, sizeof(sphere));
                light = C3Vector();

                if (this->m_model->IsLoaded()) {
                    box = this->m_model->GetBoundingBox();
                    sphere = this->m_model->GetBoundingSphere();
                    light = sphere.c;
                }

                light.z = 1.0f;
                this->m_model->m_fFlags |= 0x4000;
                this->m_model->SetWorldTransform(matrix);
                World::ObjectUpdate(this->m_mapEntity, this->m_model->m_worldTransform, box, sphere, light, 0, -1);
            }
        }
    }

    if (this->m_soundGuid && SE3::IsPlaying(this->m_soundHandle)) {
        C3Vector position = C3Vector();
        CGObject_C* object = ClntObjMgrObjectPtr(this->m_soundGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object) {
            position = object->GetPosition();
        }

        SE3::Set3DPosition(this->m_soundHandle, position);
    }

    uint32_t now = CGWorldFrame::s_currentWorldFrame->GetSceneTime();

    if ((this->m_flags & 0x4) && this->m_expireTime && static_cast<int32_t>(now - this->m_expireTime) > 0) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit) {
            return;
        }

        if ((this->m_flags2 & 8) && unit->m_mountModel) {
            if (unit->m_mountAnimKitManager) {
                unit->m_mountAnimKitManager->Unfreeze();
            }

            this->m_flags2 &= ~8;
        }

        if ((this->m_flags2 & 4) && unit->m_worldModel) {
            unit->m_animKitManager.Unfreeze();
            this->m_flags2 &= ~4;
        }

        this->MoveToDestroyList();
    } else if (this->unk_00F0 && static_cast<int32_t>(now - this->unk_00F0) > 0) {
        CGObject_C* object = ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (!object) {
            return;
        }

        object->m_scaleModifier = 1.0f;
        object->m_scaleChangeTime = 0;
        this->MoveToDestroyList();
    } else if (!(this->m_flags & 0x20) && this->m_expireTime && static_cast<int32_t>(now - this->m_expireTime) > 0) {
        this->MoveToDestroyList();
    }

    if ((this->m_flags & 0x100000) && !SE3::IsPlaying(this->m_soundHandle)) {
        this->MoveToDestroyList();
    }

    if (this->m_fade > 0.0f && this->m_model && this->m_expireTime) {
        float remaining = static_cast<float>(static_cast<int32_t>(this->m_expireTime - now)) * 0.001f;

        if (remaining > 0.0f) {
            if (remaining > this->m_model->m_fadeAlpha) {
                remaining = this->m_model->m_fadeAlpha;
            }

            this->m_model->m_fadeAlpha = remaining;
        } else {
            this->MoveToDestroyList();
        }
    }
}

// OFFSET: 0x00584D90/0x1401ECF80
void CEffect::AddLootEffect(CGUnit_C* unit, void (*callback)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7)) {
    this->m_sourceGuid = unit->m_obj->m_guid;
    this->m_flags = 8;
    this->m_attachmentId = 19;
    this->m_effectNameRec = GetSpecialSpellVisualEffectNameRec(4);

    if (this->CreateModelFromEffect(callback, 0, 0)) {
        this->LinkTo(&unit->m_effectList);
        this->m_refCount++;
    } else {
        delete this;
    }
}

// OFFSET: 0x00584E50/0x1401ED0C0
void CEffect::Release() {
    if (this->m_refCount-- == 1) {
        this->~CEffect();
        s_effectAllocator.PutData(this, NULL, 0);
    }
}

// PROVEN: dc5d632f IDENTICAL/NONE
// OFFSET: 0x00584E80
void CEffect::Destroy() {
    while (s_destroyedEffects) {
        CEffect* effect = s_destroyedEffects;

        if (--effect->m_refCount == 0) {
            effect->~CEffect();
            s_effectAllocator.PutData(effect, NULL, 0);
        }
    }

    while (s_orphanedEffects) {
        CEffect* effect = s_orphanedEffects;

        if (--effect->m_refCount == 0) {
            effect->~CEffect();
            s_effectAllocator.PutData(effect, NULL, 0);
        }
    }

    s_effectAllocator.Clear(typeid(CEffect).raw_name(), SLOG_OBJECT, 1);
}

// OFFSET: 0x00584F20
void CEffect::CleanUpStaticLists() {
    uint32_t now = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    CEffect* effect = s_orphanedEffects;

    while (effect) {
        CEffect* next = effect->m_next;

        if (!effect->m_startTime || static_cast<int32_t>(now - effect->m_startTime) > 0) {
            effect->EffectUpdate();
        }

        if ((effect->m_expireTime && static_cast<int32_t>(now - effect->m_expireTime) > 0) || (effect->m_flags2 & 1)) {
            if (effect->m_prevLink) {
                *effect->m_prevLink = effect->m_next;
            }

            if (effect->m_next) {
                effect->m_next->m_prevLink = effect->m_prevLink;
            }

            effect->m_next = NULL;
            effect->m_prevLink = NULL;

            if (effect->m_refCount-- == 1) {
                effect->~CEffect();
                s_effectAllocator.PutData(effect, NULL, 0);
            }
        }

        effect = next;
    }

    effect = s_destroyedEffects;

    while (effect) {
        CEffect* next = effect->m_next;

        if (!effect->m_model || !(effect->m_model->m_fFlags & 0x200)) {
            if (effect->m_prevLink) {
                *effect->m_prevLink = effect->m_next;
            }

            if (effect->m_next) {
                effect->m_next->m_prevLink = effect->m_prevLink;
            }

            effect->m_next = NULL;
            effect->m_prevLink = NULL;

            if (effect->m_refCount-- == 1) {
                effect->~CEffect();
                s_effectAllocator.PutData(effect, NULL, 0);
            }
        }

        effect = next;
    }
}
