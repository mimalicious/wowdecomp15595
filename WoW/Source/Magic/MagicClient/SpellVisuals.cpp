#include "SpellVisuals.h"
#include <cmath>
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Common/Lightning.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellCastingRequirementsRec.h"
#include "WoW/Source/DB/Rec/SpellChainEffectsRec.h"
#include "WoW/Source/DB/Rec/SpellEffectCameraShakesRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitAreaModelRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitModelAttachRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "WoW/Source/Game/ViolenceLevels.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Magic/SpellCast.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/StaticSingleton.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/CameraManager.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00D0AFF8
static int32_t s_lastUpdateTime = -1;

// STATIC: 0x00D0B00C
static TSList<BlizzardObject> s_blizzardObjects;

// STATIC: 0x00D0B018
static TSList<BlizzardObject> s_blizzardObjectsDeleted;

// STATIC: 0x00ED6754
static CDataAllocator s_blizzardObjectHeap(sizeof(BlizzardObject), 8);

// STATIC: 0x00ED6768
static CDataAllocator s_shardHeap(sizeof(Shard), 32);

// STATIC: 0x00D0B024
static TSList<LightningObject> s_lightningObjects;

// STATIC: 0x00D0B030
static TSList<LightningObject> s_lightningObjectsDeleted;

// STATIC: 0x00D0B03C
static TSList<MountTransitionObject> s_mountTransitions;

// STATIC: 0x00D0B048
static TSList<MountTransitionObject> s_mountTransitionsDeleted;

// STATIC: 0x00D0B164
static const char* s_defaultBlizzardModel = "Spells\\Blizzard_Impact_Base.mdx";

// STATIC: 0x00ED6740
static CLightningManager* s_lightningManager;

// STATIC: 0x00ED6748
static TSGrowableArray<TSGrowableArray<const SpellVisualKitModelAttachRec*>*>* s_modelAttachLookup;

// STATIC: 0x00ED6790
static EclipseState s_eclipse;

// STATIC: 0x00ED67A4
static uint32_t s_shardEventCount;

// STATIC: 0x00ED67A8
static SpellVisualKitAreaModelRec s_defaultAreaModel;

// INLINED
LightningObject::LightningObject() : m_refCount(1) {
}

// INLINED
Shard::Shard() : m_model(NULL), m_started(0), m_loop(0), m_dissolve(0) {
}

// INLINED
Shard::~Shard() {
    if (this->m_model) {
        this->m_model->Release();
    }
}

// INLINED
C3Vector BlizzardObject::GetWorldPosition() const {
    if (this->m_hasFrame) {
        return this->m_position * this->m_frame;
    }

    return this->m_position;
}

// INLINED
void BlizzardObject::SetFrameOfReference(const C44Matrix* matrix) {
    this->m_hasFrame = matrix != NULL;

    if (matrix) {
        this->m_frame = *matrix;
        this->UpdateWorldObject();
    }
}

// OFFSET: 0x008BBFA0
int32_t BlizzardObject::GetAngleRange(float& minAngle, float& maxAngle) const {
    minAngle = 0.0f;
    maxAngle = 6.2831855f;

    if (this->m_angle > 0.00000011920929f) {
        minAngle = this->m_angle * -0.5f;
        maxAngle = this->m_angle * 0.5f;
        return 1;
    }

    if (this->m_angle < -0.00000011920929f) {
        minAngle = this->m_angle * 0.5f + 3.1415927f;
        maxAngle = 3.1415927f - this->m_angle * 0.5f;
        return 1;
    }

    return 0;
}

// OFFSET: 0x008BC040
void EclipseState::Update(uint32_t time) {
    if (this->m_start == this->m_end) {
        return;
    }

    float blend = 0.0f;

    if (time < this->m_fadeInEnd) {
        blend = static_cast<float>(time - this->m_start) / static_cast<float>(this->m_fadeInEnd - this->m_start);
    } else if (time < this->m_holdEnd) {
        blend = 1.0f;
    } else if (time < this->m_end) {
        blend = 1.0f - static_cast<float>(time - this->m_holdEnd) / static_cast<float>(this->m_end - this->m_holdEnd);
    } else {
        this->m_end = 0;
        this->m_start = 0;
    }

    DayNight::SetEclipse(*reinterpret_cast<const uint32_t*>(&this->m_color), blend);
}

// OFFSET: 0x008BC100/0x140604CE0
void SpellVisualsBlizzardSetDeletedCallback(BlizzardObject* object, void (*callback)(BlizzardObject* object, void* param), void* param) {
    object->m_deletedCallback = callback;
    object->m_deletedParam = param;
}

// PROVEN: b0403c10 IDENTICAL/NONE
// OFFSET: 0x008BC120/0x140604CF0
float SpellVisualsGetMountTransitionHeight(const MountTransitionObject* transition) {
    return transition->m_height;
}

// PROVEN: d81d3c7b IDENTICAL/NONE
// OFFSET: 0x008BC130
float SpellVisualsGetMountTransitionAlpha(const MountTransitionObject* transition) {
    return transition->m_alpha;
}

// PROVEN: 06742b3f IDENTICAL/NONE
// OFFSET: 0x008BC140/0x140604D10
void SpellVisualsGetMountTransitionPosAndNormal(const MountTransitionObject* transition, C3Vector* position, C3Vector* normal, float* facing) {
    *position = transition->m_position;
    *normal = transition->m_groundNormal;
    *facing = transition->m_facing;
}

// PROVEN: 54548d21 IDENTICAL/NONE
// OFFSET: 0x008BC180/0x140604D40
void SpellVisualsFlagMountTransitionFinished(MountTransitionObject* transition) {
    transition->m_flags |= 0x2;
}

// PROVEN: 484ab884 IDENTICAL/NONE
// OFFSET: 0x008BC190/0x140604D50
int32_t SpellVisualsIsMountTransitionFinished(const MountTransitionObject* transition) {
    return (transition->m_flags >> 1) & 1;
}

// OFFSET: 0x008BC1A0/0x140604D60
bool CanPlaySpellVisualForPlayer(uint64_t caster, uint64_t target, int32_t casterIsPlayer, int32_t targetIsPlayer) {
    if (caster) {
        if (casterIsPlayer && caster != ClntObjMgrGetActivePlayer().m_guid) {
            return false;
        }

        if (targetIsPlayer && caster == ClntObjMgrGetActivePlayer().m_guid) {
            return false;
        }
    }

    if (target) {
        if (targetIsPlayer && target != ClntObjMgrGetActivePlayer().m_guid) {
            return false;
        }

        if (casterIsPlayer && target == ClntObjMgrGetActivePlayer().m_guid) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x008BC220/0x140604DF0
bool CanPlaySpellVisual(uint64_t caster, uint64_t target, const SpellVisualRec* visual) {
    if (!visual) {
        return false;
    }

    return CanPlaySpellVisualForPlayer(caster, target, visual->m_flags & 0x800, visual->m_flags & 0x1000);
}

// OFFSET: 0x008BC260/0x140604E20
bool CanPlaySpellVisualKit(uint64_t caster, uint64_t target, const SpellVisualKitRec* kit, const SpellVisualRec* visual) {
    if (!kit || (visual && !CanPlaySpellVisualForPlayer(caster, target, visual->m_flags & 0x800, visual->m_flags & 0x1000))) {
        return false;
    }

    return CanPlaySpellVisualForPlayer(caster, target, kit->m_flags & 0x1000, kit->m_flags & 0x2000);
}

// OFFSET: 0x008BC4D0
CAaSphere BlizzardObject::GetBoundingSphere() {
    float radius = this->m_radius;

    if (this->m_model && this->m_model->IsLoaded()) {
        radius = this->m_model->GetBoundingSphere().r + radius;
    }

    CAaSphere sphere;
    sphere.c = C3Vector(0.0f, 0.0f, 0.0f);
    sphere.r = radius;
    return sphere;
}

// OFFSET: 0x008BC550/0x140605190
static void ShardEventCallback(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time, uint64_t param) {
    if ((++s_shardEventCount & 3) == 0 && eventId == 0x444E5324) {
        SE3SoundKitProperties properties;
        properties.m_maxAudibleBehavior = 1;
        properties.m_priority = 128;
        SE3::PlaySoundKit(data, position, &properties, NULL, __FILE__, __LINE__, "ShardEventCallback()", 0, NULL);
    }
}

// OFFSET: 0x008BC5E0
C3Vector BlizzardObject::GetPolarOffset(float radius, float angle, int32_t rotate) const {
    C3Vector offset;
    offset.z = 0.0f;
    offset.x = cos(angle) * radius;
    offset.y = radius * sin(angle);

    if (rotate && this->m_hasFrame) {
        C3Vector rotated = offset * this->m_frame;
        offset.x = rotated.x - this->m_frame.d0;
        offset.y = rotated.y - this->m_frame.d1;
        offset.z = rotated.z - this->m_frame.d2;
    }

    return offset;
}

// OFFSET: 0x008BC6D0/0x140605480
const SpellVisualRec* GetSpellVisual(const SpellRec* spell, const CGUnit_C* unit) {
    int32_t visualId = spell->m_spellVisualID[0];

    if (ViolenceGetLevel() < 2 && spell->m_spellVisualID[1]) {
        visualId = spell->m_spellVisualID[1];
    }

    const SpellVisualRec* visual = g_spellVisualDB.GetRecord(visualId);

    if (!unit || !visual || !visual->m_hostileSpellVisualID) {
        return visual;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->IsFriendly(unit)) {
        return visual;
    }

    const SpellVisualRec* hostile = g_spellVisualDB.GetRecord(visual->m_hostileSpellVisualID);
    return hostile ? hostile : visual;
}

// OFFSET: 0x008BC770
void SpellVisualsHandleCastStart(CGUnit_C* unit, const SpellCast* cast, const SpellRec* spell) {
    if (!unit) {
        SErrSetLastError(87);
        return;
    }

    if (spell->m_attributesExB & 0x20) {
        if (static_cast<uint8_t>(unit->m_animationState >> 3) & 1) {
            return;
        }

        unit->SetCastingAnim(true);
    }

    SpellVisualRec visualRec;
    const SpellVisualRec* visual = unit->GetAppropriateSpellVisual(spell, visualRec, 0, 0);

    if (!visual) {
        return;
    }

    uint64_t target = (cast->m_targetFlags & 0x2) ? cast->m_objectTarget.m_guid : 0;
    const SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_precastKit);

    if (!CanPlaySpellVisualKit(unit->m_obj->m_guid.m_guid, target, kit, visual)) {
        return;
    }

    if (kit->m_startAnimID > 0 || kit->m_animID > 0) {
        unit->SetCastingAutoRepeat(true);
    }

    PlaySpellVisuaKitData kitData(spell, kit, 4);
    kitData.m_someGUID = unit->m_obj->m_guid;
    unit->PlaySpellVisualKit(kitData);

    if (spell->m_attributes & 0x2) {
        unit->SetSheatheState(2, 1, 0);
    }

    if (visual->m_hasMissile && visual->m_missileModel) {
        const SpellVisualEffectNameRec* effectName = g_spellVisualEffectNameDB.GetRecord(visual->m_missileModel);

        if (effectName && (static_cast<uint8_t>(effectName->m_type) == 1 || static_cast<uint8_t>(effectName->m_type) == 2)) {
            unit->SetSheatheState(1, 1, 0);
        }
    }

    if (unit->IsActivePlayer() && (spell->m_attributes & 0x400000) && (cast->m_targetFlags & 0x2) && cast->m_objectTarget.m_guid) {
        const SpellCastingRequirementsRec* requirements = spell->m_spellCastingRequirementsID ? g_spellCastingRequirementsDB.GetRecord(spell->m_spellCastingRequirementsID) : NULL;

        if (!requirements || !(requirements->m_facingCasterFlags & 0x1) || unit->IsInFrontOfTargetWithGuid(cast->m_objectTarget)) {
            unit->SetTrackingTarget(cast->m_objectTarget, 0);
        }
    }

    if (visual->m_flags & 0x20) {
        unit->AttachPrecastMissileModel(visual, cast->m_castCount);
    }
}

// OFFSET: 0x008BC970
C3Vector LightningObject::GetCasterPosition(CGUnit_C* unit, uint32_t attachment, const C3Vector& offset, int32_t raw) {
    CM2Model* model = unit->m_worldModel;
    C3Vector base = unit->m_vehiclePassenger ? unit->GetModelPosition() : unit->GetPosition();
    C3Vector position;
    position.x = base.x;
    position.y = base.y;
    position.z = base.z + unit->m_scale * unit->m_height * 0.75f;

    if (model && model->IsLoaded() && (model->m_fFlags & 0x4000)) {
        if (attachment != 0xFFFFFFFF && unit->HasAttachmentForEffect(attachment, raw)) {
            return unit->GetAttachmentPositionForEffect(attachment, offset, raw);
        }

        if (model->HasEvent(0x4C534324)) {
            return offset * model->GetEventWorldTransform(0x4C534324);
        }

        if (offset.x != 0.0f || offset.y != 0.0f || offset.z != 0.0f) {
            C3Vector transformed = offset * (unit->m_vehiclePassenger ? unit->GetModelMatrix() : unit->GetMatrix());
            position.x = transformed.x;
            position.y = transformed.y;
            position.z = unit->m_scale * unit->m_height * 0.75f + transformed.z;
        }
    }

    return position;
}

// OFFSET: 0x008BCC50
C3Vector LightningObject::GetImpactPosition(CGUnit_C* unit, uint32_t spellId, CGUnit_C* caster) {
    CM2Model* model = unit->m_worldModel;
    C3Vector position = unit->m_vehiclePassenger ? unit->GetModelPosition() : unit->GetPosition();
    position.z = unit->GetStandHeight() * 0.75f + position.z;
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        if (model->IsLoaded() && (model->m_fFlags & 0x4000) && model->HasAttachment(34)) {
            return model->GetAttachmentPosition(34);
        }

        return position;
    }

    SpellVisualRec buffer;
    const SpellVisualRec* visual = (unit->m_obj->m_type & TYPEMASK_UNIT) ? unit->GetAppropriateSpellVisual(spell, buffer, 0, 0) : GetSpellVisual(spell, caster);

    if (!visual) {
        return position;
    }

    C3Vector offset;
    offset.x = visual->m_missileImpactOffset.x;
    offset.y = -visual->m_missileImpactOffset.y;
    offset.z = visual->m_missileImpactOffset.z;
    int32_t raw = visual->m_flags & 0x200;

    if (model->IsLoaded() && (model->m_fFlags & 0x4000) && unit->HasAttachmentForEffect(visual->m_missileDestinationAttachment, raw)) {
        return unit->GetAttachmentPositionForEffect(visual->m_missileDestinationAttachment, offset, raw);
    }

    if (visual->m_missileImpactOffset.x != 0.0f || visual->m_missileImpactOffset.y != 0.0f || visual->m_missileImpactOffset.z != 0.0f) {
        C44Matrix matrix = unit->m_vehiclePassenger ? unit->GetModelMatrix() : unit->GetMatrix();
        position.x = offset.z * matrix.c0 + matrix.a0 * offset.x + matrix.b0 * offset.y + position.x;
        position.y = matrix.c1 * offset.z + matrix.a1 * offset.x + matrix.b1 * offset.y + position.y;
        position.z = matrix.c2 * offset.z + matrix.a2 * offset.x + matrix.b2 * offset.y + position.z;
    }

    return position;
}

// OFFSET: 0x008BCF20
bool LightningObject::Tick(uint32_t time) {
    for (uint32_t i = 0; i < this->m_bolts.Count(); i++) {
        Bolt& bolt = this->m_bolts[i];

        if (bolt.m_source == 0xFFFF || bolt.m_target == 0xFFFF) {
            continue;
        }

        CGObject_C* caster = static_cast<CGObject_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(this->m_targets[0]), TYPEMASK_OBJECT, __FILE__, __LINE__));
        CGObject_C* source = static_cast<CGObject_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(this->m_targets[bolt.m_source]), TYPEMASK_OBJECT, __FILE__, __LINE__));
        CGObject_C* target = static_cast<CGObject_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(this->m_targets[bolt.m_target]), TYPEMASK_OBJECT, __FILE__, __LINE__));
        CGUnit_C* casterUnit = caster && (static_cast<uint8_t>(static_cast<uint32_t>(static_cast<int16_t>(caster->m_obj->m_type)) >> 3) & 1) ? static_cast<CGUnit_C*>(caster) : NULL;
        CGUnit_C* sourceUnit = source && (static_cast<uint8_t>(static_cast<uint32_t>(static_cast<int16_t>(source->m_obj->m_type)) >> 3) & 1) ? static_cast<CGUnit_C*>(source) : NULL;
        CGUnit_C* targetUnit = target && (static_cast<uint8_t>(static_cast<uint32_t>(static_cast<int16_t>(target->m_obj->m_type)) >> 3) & 1) ? static_cast<CGUnit_C*>(target) : NULL;

        if ((this->m_flags & 0x1) || (time >= bolt.m_startTime && time < bolt.m_endTime)) {
            C3Vector sourcePos(0.0f, 0.0f, 0.0f);

            if (sourceUnit) {
                sourcePos = bolt.m_source ? LightningObject::GetImpactPosition(sourceUnit, this->m_spellId, casterUnit) : LightningObject::GetCasterPosition(sourceUnit, this->m_sourceAttachment, this->m_sourceOffset, this->m_flags & 0x4);
            } else if (source) {
                sourcePos = this->m_sourceOffset * source->GetMatrix();
            }

            C3Vector targetPos(0.0f, 0.0f, 0.0f);
            int32_t hidden = 0;

            if (targetUnit) {
                targetPos = LightningObject::GetImpactPosition(targetUnit, this->m_spellId, casterUnit);
            } else if (target) {
                targetPos = this->m_targetOffset * target->GetMatrix();
            } else if (this->m_flags & 0x2) {
                targetPos = this->m_targetOffset;
            } else {
                hidden = 1;
            }

            if (bolt.m_lightning == 0xFFFFFFFF) {
                bolt.m_lightning = s_lightningManager->Add(NULL, NULL, this->m_chainEffects);
                CLightning* lightning = s_lightningManager->GetTempPtr(bolt.m_lightning);
                lightning->SetSrcPos(sourcePos);
                lightning->SetDstPos(targetPos);
                lightning->SetTexture(this->m_texture);
            } else {
                s_lightningManager->Move(bolt.m_lightning, &sourcePos, &targetPos);
            }

            bool visible = true;

            if (!source || hidden || (targetPos.z - sourcePos.z) * (targetPos.z - sourcePos.z) + (targetPos.y - sourcePos.y) * (targetPos.y - sourcePos.y) + (targetPos.x - sourcePos.x) * (targetPos.x - sourcePos.x) >= 250000.0f) {
                visible = false;
            }

            s_lightningManager->SetBoltVisible(bolt.m_lightning, visible);
            CLightning* lightning = s_lightningManager->GetTempPtr(bolt.m_lightning);

            if (source && lightning) {
                if (lightning->m_flags & 0x10) {
                    source->ConnectToLightningThisFrame(this->m_spellId, -1);
                }

                if (target) {
                    if (lightning->m_flags & 0x20) {
                        target->ConnectToLightningThisFrame(this->m_spellId, -1);
                    }
                } else if (lightning->m_flags & 0x20) {
                    source->ConnectToLightningThisFrame(this->m_spellId, bolt.m_attachment);
                }
            }
        }

        if (!(this->m_flags & 0x1) && time >= bolt.m_endTime && bolt.m_lightning != 0xFFFFFFFF) {
            s_lightningManager->Remove(bolt.m_lightning);
            bolt.m_lightning = 0xFFFFFFFF;
        }
    }

    return (this->m_flags & 0x1) || time < this->m_endTime;
}

// OFFSET: 0x008BD450
void SpellVisualsHandleEclipse(const CImVector& color, float fadeIn, const SpellRec* spell) {
    uint32_t castTime = Spell_C_GetCastTime(spell, 0, 0, 0);
    s_eclipse.m_color = color;
    uint32_t now = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    s_eclipse.m_start = now;
    s_eclipse.m_holdEnd = now + castTime;
    s_eclipse.m_fadeInEnd = now + static_cast<uint32_t>(static_cast<float>(castTime) * fadeIn);
    s_eclipse.m_end = now + castTime + 100;
}

// OFFSET: 0x008BD4E0
void LightningObject::InitializeBolts(CGUnit_C* caster, const WOWGUID* targets, uint32_t count, bool reverse, int32_t attachment) {
    uint32_t time = this->m_chainEffects->m_segDelay + CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    this->m_endTime = time;

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        Bolt& bolt = this->m_bolts[i];
        bolt.m_lightning = 0xFFFFFFFF;
        bolt.m_target = 0xFFFF;
        bolt.m_source = 0xFFFF;
        this->m_targets[i + 1] = targets[count - i - 1].m_guid;

        uint16_t source = reverse ? 0 : i;
        uint16_t target = i + 1;

        if (this->m_targets[source] == this->m_targets[target]) {
            continue;
        }

        bolt.m_source = source;
        bolt.m_target = target;
        bolt.m_startTime = time;
        bolt.m_endTime = time + this->m_chainEffects->m_segDuration;

        if (static_cast<int32_t>(bolt.m_endTime - this->m_endTime) >= 0) {
            this->m_endTime = bolt.m_endTime;
        }

        time += static_cast<uint32_t>(this->m_chainEffects->m_delayBetweenEffects);
        bolt.m_attachment = !i ? attachment : -1;

        if (!this->m_targets[i + 1]) {
            continue;
        }

        CGObject_C* object = static_cast<CGObject_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(this->m_targets[i + 1]), TYPEMASK_OBJECT, __FILE__, __LINE__));

        if (!object) {
            continue;
        }

        for (CEffect* effect = object->m_effectList; effect; effect = effect->m_next) {
            if (effect->m_spellId == this->m_spellId && (effect->m_flags & 0x40000)) {
                effect->InitLightningEndpoint();
            }
        }
    }
}

// OFFSET: 0x008BD660
void SpellVisualsRender() {
    s_lightningManager->Render(StaticSingleton<CCameraManager>::Instance()->m_shot.m_position, World::FrustumCull);
}

// OFFSET: 0x008BD6B0
void SpellVisualsPlayCameraShakeID(uint32_t id, const C3Vector* position) {
    SpellEffectCameraShakesRec* rec = g_spellEffectCameraShakesDB.GetRecord(id);

    if (!rec) {
        return;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (rec->m_cameraShake[i]) {
            StaticSingleton<CCameraManager>::Instance()->AddShakeById(rec->m_cameraShake[i], *position);
        }
    }
}

// OFFSET: 0x008BD730
int32_t MountTransitionObject::UpdateCharacterData() {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_unitGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    CM2Model* model = unit->m_worldModel;
    CM2Model* mountModel = NULL;

    if (unit->m_mountTransitionEffect) {
        mountModel = unit->m_mountTransitionEffect->m_model;
    }

    if (!model || !model->IsLoaded() || !mountModel || !mountModel->IsLoaded() || !mountModel->HasAttachment(0)) {
        return 0;
    }

    this->m_attachPivot = mountModel->GetAttachmentPivot(0);
    M2BoneSequenceInfo info;
    model->GetBoneSequenceInfo(0xFFFFFFFF, info);
    float rate = 1.0f / static_cast<float>(info.m_endTime - info.m_startTime);
    this->m_animRate = rate;
    uint32_t anim = unit->GetCurrentTorsoAnimId();
    uint32_t riseStart = model->GetEventSequenceTime(anim, 0x42545324);
    uint32_t riseEnd = model->GetEventSequenceTime(anim, 0x45545324);
    this->m_riseStart = static_cast<float>(riseStart) * rate;
    this->m_riseEnd = rate * static_cast<float>(riseEnd);
    this->m_riseRate = 1.0f / (this->m_riseEnd - this->m_riseStart);

    if (mountModel->HasEvent(0x42545324)) {
        this->m_eventPivot = mountModel->GetEventPivot(0x42545324);
    }

    const C44Matrix& world = unit->m_worldModel->m_worldTransform;
    this->m_eventPivot *= world;
    this->m_jumpVector.x = world.d0 - this->m_eventPivot.x;
    this->m_jumpVector.y = world.d1 - this->m_eventPivot.y;
    this->m_jumpVector.z = world.d2 - this->m_eventPivot.z;
    uint32_t jumpStart = mountModel->GetEventSequenceTime(127, 0x42545324);
    uint32_t jumpEnd = mountModel->GetEventSequenceTime(127, 0x45545324);
    this->m_jumpStart = static_cast<float>(jumpStart) * rate;
    this->m_jumpEnd = rate * static_cast<float>(jumpEnd);
    float jumpLength = this->m_jumpEnd - this->m_jumpStart;
    this->m_jumpRate = jumpLength;

    if (jumpLength <= 0.0f) {
        return 0;
    }

    this->m_jumpRate = 1.0f / jumpLength;
    this->m_facing = unit->GetDisplayFacing();
    this->m_flags |= 0x1;
    this->m_verticalSpeed = 0.0f;
    return 1;
}

// OFFSET: 0x008BD9B0
void SpellVisualsComputeMountTransitionJumpVec(const MountTransitionObject* transition, C3Vector* jump) {
    *jump = transition->m_jumpVector;

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(transition->m_unitGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit || !unit->m_mountTransitionEffect) {
        return;
    }

    CM2Model* model = unit->m_mountTransitionEffect->m_model;

    if (!model) {
        return;
    }

    C3Vector world = model->GetWorldPosition();
    C3Vector attach = model->GetAttachmentPosition(0);
    jump->x = attach.x - world.x;
    jump->y = attach.y - world.y;
    jump->z = attach.z - world.z;

    float height = transition->m_height;
    jump->x = jump->x * height;
    jump->y = jump->y * height;
    jump->z = jump->z * height;
}

// OFFSET: 0x008BDAA0
uint32_t SpellVisualsGetModelAttachRecCount(uint32_t kitId) {
    if (kitId < s_modelAttachLookup->m_count && s_modelAttachLookup->m_data[kitId]) {
        return s_modelAttachLookup->m_data[kitId]->m_count;
    }

    return 0;
}

// PROVEN: 356e42a8 IDENTICAL/NONE
// OFFSET: 0x008BDAD0/0x140606C80
const SpellVisualKitModelAttachRec* SpellVisualsGetModelAttachRec(uint32_t kitId, uint32_t index) {
    return s_modelAttachLookup->m_data[kitId]->m_data[index];
}

// OFFSET: 0x008BDAF0
void BlizzardObject::UpdateWorldObject() {
    C44Matrix transform;
    C3Vector position = this->GetWorldPosition();
    transform.d0 = position.x;
    transform.d1 = position.y;
    transform.d2 = position.z;
    CAaSphere sphere = this->GetBoundingSphere();
    CAaBox box = sphere.Box();
    World::ObjectUpdate(this->m_entity, transform, box, sphere, sphere.c, 0, 0xFFFFFFFF);
}

// OFFSET: 0x008BDCC0
void BlizzardObject::ModelLoadedCallback(CM2Model* model, void* param) {
    if (param) {
        static_cast<BlizzardObject*>(param)->UpdateWorldObject();
    }
}

// OFFSET: 0x008BDCE0
C3Vector BlizzardObject::GetRandomShardOffset() {
    float radius = sqrt(CRandom::ufloat_(g_rndSeed)) * this->m_radius;
    float minAngle;
    float maxAngle;
    int32_t rotate = this->GetAngleRange(minAngle, maxAngle);
    float angle = (maxAngle - minAngle) * CRandom::ufloat_(g_rndSeed) + minAngle;
    return this->GetPolarOffset(radius, angle, rotate);
}

// OFFSET: 0x008BDEB0
void BlizzardObject::Render(int32_t visible) {
    C44Matrix transform;
    float scale = this->m_areaModel->m_modelScale;

    if (scale > 0.00000011920929f) {
        transform.Scale(scale);
    }

    for (Shard* shard = this->m_shards.Head(); shard; shard = this->m_shards.Next(shard)) {
        if (CGWorldFrame::s_currentWorldFrame->GetSceneTime() < shard->m_startTime) {
            continue;
        }

        CM2Model* model = shard->m_model;

        if (!shard->m_started) {
            model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
            shard->m_started = 1;
        }

        model->SetAnimating(1);
        uint32_t flag = visible & 1;

        if (model->m_attachParent) {
            model->m_fFlags = (model->m_fFlags & ~0x40u) | (flag << 6);
        } else {
            model->m_fFlags = (model->m_fFlags & ~0x8u) | (flag << 3);
        }

        if (model->m_attachParent) {
            model->m_fFlags = (model->m_fFlags & ~0x10000u) | (flag << 16);
        } else {
            model->m_fFlags = (model->m_fFlags & ~0x8000u) | (flag << 15);
        }

        transform.d0 = shard->m_position.x;
        transform.d1 = shard->m_position.y;
        transform.d2 = shard->m_position.z;

        if (this->m_hasFrame) {
            C3Vector position(transform.d0, transform.d1, transform.d2);
            position = position * this->m_frame;
            transform.d0 = position.x;
            transform.d1 = position.y;
            transform.d2 = position.z;

            if (model->m_fFlags & 0x1) {
                model->m_frameOfReferenceMatrix = this->m_frame;
                model->ChangeFrameOfReference(&model->m_frameOfReferenceMatrix);
            }
        }

        model->m_fFlags |= 0x4000;
        model->SetWorldTransform(transform);
    }
}

// OFFSET: 0x008BE1D0
void SpellVisualsBlizzardSetFrameOfReference(BlizzardObject* object, const C44Matrix* matrix) {
    object->SetFrameOfReference(matrix);
}

// OFFSET: 0x008BE200
BlizzardObject::BlizzardObject() : m_entity(0), m_radius(0.0f), m_angle(0.0f), m_endTime(0), m_shardRate(0.0f), m_shardAccum(0.0f), m_hasFrame(0), m_deletedCallback(NULL), m_deletedParam(NULL), m_model(NULL) {
    this->unk_0030[0] = 0.0f;
    this->unk_0030[1] = 0.0f;
    this->unk_0030[2] = 0.0f;
    this->unk_0030[3] = 0.0f;
}

// PROVEN: a303902f IDENTICAL/NONE
// OFFSET: 0x008BE2F0
int32_t BlizzardObject::WorldObjectRender(void* param, uintptr_t flags, uint64_t param64, uintptr_t param32) {
    static_cast<BlizzardObject*>(param)->Render(flags & 1);
    return 1;
}

// OFFSET: 0x008BE310
void BlizzardObject::Init(const C3Vector* position, const SpellVisualKitAreaModelRec* areaModel, float radius, float angle) {
    this->m_position = *position;
    this->m_areaModel = areaModel;
    this->m_radius = radius;

    if (areaModel->m_lifeTime) {
        this->m_endTime = areaModel->m_lifeTime + CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    } else {
        this->m_endTime = 0;
    }

    this->m_angle = angle;
    float rate = this->m_areaModel->m_emissionRate;
    this->m_shardRate = rate;

    if (s_spellEffectLevelVar) {
        this->m_shardRate = (static_cast<float>(s_spellEffectLevelVar->m_intValue) * 0.11111111f * 0.89999998f + 0.1f) * rate;
    }

    this->m_entity = World::ObjectCreate(NULL, BlizzardObject::WorldObjectRender, this, 0, 0, 14, __FILE__, __LINE__);
    this->UpdateWorldObject();
    this->m_model = World::GetScene()->CreateModel(this->m_areaModel->m_name, 0, 3);
    CM2Model* model = this->m_model;

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

    model->SetLoadedCallback(BlizzardObject::ModelLoadedCallback, this);
}

// OFFSET: 0x008BE430
LightningObject::~LightningObject() {
    for (uint32_t i = 0; i < this->m_bolts.Count(); i++) {
        if (this->m_bolts[i].m_lightning != 0xFFFFFFFF) {
            s_lightningManager->Remove(this->m_bolts[i].m_lightning);
        }
    }

    this->m_bolts.Clear();
    this->m_targets.Clear();

    if (this->m_texture) {
        TextureRelease(this->m_texture);
        this->m_texture = NULL;
    }

    if (this->m_effect) {
        this->m_effect->ReleaseLightningRef();
        this->m_effect->Release();
        this->m_effect = NULL;
    }
}

// OFFSET: 0x008BE560
bool MountTransitionObject::Tick() {
    uint32_t now = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    float elapsed = static_cast<float>(static_cast<double>(now - this->m_lastTime) * 0.001);
    this->m_lastTime = now;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_unitGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return false;
    }

    if (!(this->m_flags & 0x1) && !this->UpdateCharacterData()) {
        return true;
    }

    float t = static_cast<float>(CGWorldFrame::s_currentWorldFrame->GetSceneTime() - this->m_startTime) * this->m_animRate;

    if (this->m_riseStart > 0.0f && t > this->m_riseStart) {
        this->m_facing = unit->GetDisplayFacing();
        this->m_height = (t - this->m_riseStart) * this->m_riseRate;

        if (this->m_height >= 1.0f) {
            this->m_flags |= 0x2;
            this->m_height = 1.0f;
        }
    }

    C3Vector normal = unit->m_movementData.GetGroundNormal();

    if (t <= this->m_jumpEnd) {
        if (t > this->m_jumpStart) {
            float jump = (t - this->m_jumpStart) * this->m_jumpRate;

            if (jump > 1.0f) {
                jump = 1.0f;
            }

            float range = sqrt(1.0f - jump) * 10.0f;
            this->m_position.x = this->m_jumpVector.x * jump;
            this->m_position.y = this->m_jumpVector.y * jump;
            this->m_position.z = this->m_jumpVector.z * jump;
            this->m_position.x = this->m_eventPivot.x + this->m_position.x;
            this->m_position.y = this->m_eventPivot.y + this->m_position.y;
            this->m_position.z = this->m_eventPivot.z + this->m_position.z;
            C3Vector hit(0.0f, 0.0f, 0.0f);
            C3Vector top(this->m_position.x, this->m_position.y, range + this->m_position.z);
            C3Vector bottom(this->m_position.x, this->m_position.y, this->m_position.z - range);
            CAaBox box;
            box.b = bottom;
            box.t = top;
            float distance = 1.0f;
            World::FacetData facets;
            unit->m_movementData.GetGroundNormal();

            if (World::Intersect(top, bottom, &hit, &distance, 0x110, NULL)) {
                this->m_flags |= 0x4;
                this->m_verticalSpeed = range - distance * range * 2.0f;
            }

            float speed = this->m_verticalSpeed;

            if (-range > speed) {
                speed = -range;
            } else if (range <= speed) {
                speed = range;
            }

            this->m_verticalSpeed = speed;
            this->m_position.z = this->m_position.z + speed;

            if (World::GetFacets(box, facets, 0x110) && facets.facets.m_count && facets.facets.m_data->plane.n.z >= 0.35721236f) {
                normal = facets.facets.m_data->plane.n;
            }
        }
    } else {
        this->m_position = unit->GetPosition();
    }

    normal.x = this->m_groundNormal.x + (normal.x - this->m_groundNormal.x) * 0.25f;
    normal.y = this->m_groundNormal.y + (normal.y - this->m_groundNormal.y) * 0.25f;
    normal.z = this->m_groundNormal.z + (normal.z - this->m_groundNormal.z) * 0.25f;
    this->m_groundNormal = normal;
    float scale = 1.0f / sqrt(this->m_groundNormal.x * this->m_groundNormal.x + this->m_groundNormal.y * this->m_groundNormal.y + this->m_groundNormal.z * this->m_groundNormal.z);
    this->m_groundNormal.x = this->m_groundNormal.x * scale;
    this->m_groundNormal.y = this->m_groundNormal.y * scale;
    this->m_groundNormal.z = this->m_groundNormal.z * scale;

    if (!(this->m_flags & 0x4)) {
        return true;
    }

    this->m_alpha = elapsed * 2.0f + this->m_alpha;

    if (this->m_alpha > 1.0f) {
        this->m_alpha = 1.0f;
    }

    return true;
}

// OFFSET: 0x008BE990/0x140608060
void SpellVisualsShutdownModelAttachLookup() {
    if (!s_modelAttachLookup) {
        return;
    }

    uint32_t count = s_modelAttachLookup->Count();

    for (uint32_t i = 0; i < count; i++) {
        if ((*s_modelAttachLookup)[i]) {
            TSGrowableArray<const SpellVisualKitModelAttachRec*>* list = (*s_modelAttachLookup)[i];

            if (list) {
                list->~TSGrowableArray();
                SMemFree(list, "delete", -1, 0);
            }
        }
    }

    TSGrowableArray<TSGrowableArray<const SpellVisualKitModelAttachRec*>*>* lookup = s_modelAttachLookup;

    if (lookup) {
        lookup->~TSGrowableArray();
        SMemFree(lookup, "delete", -1, 0);
    }

    s_modelAttachLookup = NULL;
}

// OFFSET: 0x008BEB20/0x140608160
BlizzardObject* SpellVisualsBlizzardCreate(const C3Vector* position, float radius, float angle, int32_t spellId, int32_t areaModelID) {
    const SpellVisualKitAreaModelRec* areaModel = g_spellVisualKitAreaModelDB.GetRecord(areaModelID);

    if (!areaModel) {
        if (!static_cast<const char*>(s_defaultAreaModel.m_name)[0]) {
            s_defaultAreaModel.m_name = s_defaultBlizzardModel;
            s_defaultAreaModel.m_emissionRate = 5.0f;
        }

        areaModel = &s_defaultAreaModel;
    }

    BlizzardObject* object = s_blizzardObjectHeap.New<BlizzardObject>(0);
    s_blizzardObjects.LinkToTail(object);
    object->Init(position, areaModel, radius, angle);
    return object;
}

// OFFSET: 0x008BEBD0
MountTransitionObject* SpellVisualsStartMountTransition(CGUnit_C* unit) {
    MountTransitionObject* transition = s_mountTransitions.NewNode(2, 0, 0);

    if (!transition) {
        return NULL;
    }

    transition->m_flags = 0;
    transition->m_unitGuid = unit->m_obj->m_guid;
    transition->m_groundNormal = unit->m_movementData.GetGroundNormal();
    transition->m_startTime = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    transition->m_eventPivot.x = 0.0f;
    transition->m_eventPivot.y = 0.0f;
    transition->m_eventPivot.z = 0.0f;
    transition->m_alpha = 0.0f;
    transition->m_lastTime = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    return transition;
}

// OFFSET: 0x008BED30/0x1406084A0
void SpellVisualsInitializeModelAttachLookup() {
    SpellVisualsShutdownModelAttachLookup();

    uint32_t maxKitID = g_spellVisualKitDB.m_maxID <= 0 ? 0 : g_spellVisualKitDB.m_maxID;
    uint32_t maxAttachID = g_spellVisualKitModelAttachDB.m_maxID <= 0 ? 0 : g_spellVisualKitModelAttachDB.m_maxID;

    s_modelAttachLookup = new (SMemAlloc(sizeof(TSGrowableArray<TSGrowableArray<const SpellVisualKitModelAttachRec*>*>), __FILE__, __LINE__, 0)) TSGrowableArray<TSGrowableArray<const SpellVisualKitModelAttachRec*>*>();
    s_modelAttachLookup->GrowToFit(maxKitID, 1);

    for (uint32_t id = 1; id <= maxAttachID; id++) {
        const SpellVisualKitModelAttachRec* rec = g_spellVisualKitModelAttachDB.GetRecord(id);

        if (!rec || !rec->m_parentSpellVisualKitID || static_cast<uint32_t>(rec->m_parentSpellVisualKitID) > maxKitID) {
            continue;
        }

        TSGrowableArray<const SpellVisualKitModelAttachRec*>*& kitAttachments = (*s_modelAttachLookup)[rec->m_parentSpellVisualKitID];

        if (!kitAttachments) {
            kitAttachments = new (SMemAlloc(sizeof(TSGrowableArray<const SpellVisualKitModelAttachRec*>), __FILE__, __LINE__, 0)) TSGrowableArray<const SpellVisualKitModelAttachRec*>();
        }

        kitAttachments->New(rec);
    }
}

// OFFSET: 0x008BF000
BlizzardObject::~BlizzardObject() {
    Shard* shard;

    while ((shard = this->m_shards.Head()) != NULL) {
        s_shardHeap.Delete(shard);
    }

    if (this->m_entity) {
        World::ObjectDestroy(this->m_entity);
    }

    if (this->m_deletedCallback) {
        this->m_deletedCallback(this, this->m_deletedParam);
    }

    if (this->m_model) {
        this->m_model->Release();
        this->m_model = NULL;
    }
}

// OFFSET: 0x008BF120
static void ShardSequenceCallback(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    Shard* shard = reinterpret_cast<Shard*>(static_cast<uintptr_t>(param));

    if (!shard->m_started) {
        return;
    }

    if (!interrupted && shard->m_loop) {
        model->SetBoneSequence(0xFFFFFFFF, 158, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    } else if (!shard->m_loop && shard->m_dissolve && model->IsLoaded() && model->HasSequence(159)) {
        shard->m_dissolve = 0;
        model->SetBoneSequence(0xFFFFFFFF, 159, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    } else if (!interrupted) {
        s_shardHeap.Delete(shard);
    }
}

// OFFSET: 0x008BF1D0
void BlizzardObject::AddShard(const C3Vector& center, const C3Vector& offset, float delayRange) {
    Shard* shard = s_shardHeap.New<Shard>(0);
    this->m_shards.LinkToTail(shard);

    C3Vector target;
    target.x = center.x + offset.x;
    target.y = center.y + offset.y;
    target.z = center.z + offset.z;

    if (!(this->m_areaModel->m_enumID & 0x8)) {
        C3Vector end(target.x, target.y, target.z + 16.666666f);
        C3Vector start(center.x, center.y, center.z + 0.16666667f);
        C3Vector hit(0.0f, 0.0f, 0.0f);
        float distance = 1.0f;
        C3Vector ground;

        if (World::Intersect(start, end, &hit, &distance, 0x120111, NULL)) {
            float t = distance * 0.95f;
            ground.x = (end.x - start.x) * t + start.x;
            ground.y = (end.y - start.y) * t + start.y;
            ground.z = (end.z - start.z) * t + start.z;
        } else {
            ground = end;
        }

        C3Vector top = ground;
        C3Vector bottom(ground.x, ground.y, ground.z - 33.333332f);
        float groundDistance = 1.0f;

        if (World::Intersect(top, bottom, &shard->m_position, &groundDistance, 0x120111, NULL)) {
            if (this->m_hasFrame) {
                shard->m_position = shard->m_position * this->m_frame.AffineInverse();
            }
        } else {
            shard->m_position = this->m_position;
        }
    } else {
        shard->m_position = target;
    }

    shard->m_loop = this->m_areaModel->m_enumID >> 1;
    shard->m_dissolve = this->m_areaModel->m_enumID >> 2;
    shard->m_model = World::GetScene()->DuplicateModel(this->m_model, 0);
    shard->m_model->SetEventCallback(reinterpret_cast<void*>(&ShardEventCallback), 0);
    shard->m_model->SetSequenceCallback(reinterpret_cast<void*>(&ShardSequenceCallback), reinterpret_cast<uintptr_t>(shard));

    CM2Model* model = shard->m_model;

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

    World::ObjectSetModelLighting(this->m_entity, shard->m_model);
    int32_t delay = static_cast<int32_t>(CRandom::ufloat_(g_rndSeed) * delayRange);
    shard->m_startTime = delay + CGWorldFrame::s_currentWorldFrame->GetSceneTime();

    if (!shard->m_startTime) {
        shard->m_startTime = 1;
    }
}

// OFFSET: 0x008BF5A0/0x140608E90
void BlizzardObject::Stop() {
    this->m_shardRate = 0.0f;

    Shard* shard = this->m_shards.Head();

    while (!this->m_shards.IsEnd(shard)) {
        Shard* next = this->m_shards.RawNext(shard);

        if (shard->m_started) {
            shard->m_loop = 0;

            if (shard->m_dissolve) {
                shard->m_dissolve = 0;
                CM2Model* model = shard->m_model;

                if (model && model->IsLoaded() && model->HasSequence(159)) {
                    model->SetBoneSequence(0xFFFFFFFF, 159, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
                }
            }
        } else {
            s_shardHeap.Delete(shard);
        }

        shard = next;
    }
}

// OFFSET: 0x008BF6A0/0x140608FE0
void SpellVisualsInitialize() {
    s_lightningManager = new (SMemAlloc(sizeof(CLightningManager), __FILE__, __LINE__, 0)) CLightningManager();
    s_spellEffectLevelVar = CVar::Lookup("spellEffectLevel");
    SpellVisualsInitializeModelAttachLookup();
}

// OFFSET: 0x008BF6F0
void LightningObject::CreateFromSpell(CGUnit_C* caster, CEffect* effect, uint32_t chainEffectsId, const WOWGUID* targets, uint32_t count, uint32_t spellId, bool persist, bool reverse, LightningObject** lightning, uint32_t maxLightning, const C3Vector* location, int32_t attachment) {
    if (!caster || !spellId) {
        return;
    }

    SpellChainEffectsRec* root = g_spellChainEffectsDB.GetRecord(chainEffectsId);

    if (!root) {
        return;
    }

    SpellChainEffectsRec* chain = root;
    CStatus status;
    uint32_t sourceAttachment = 0xFFFFFFFF;
    C3Vector sourceOffset(0.0f, 0.0f, 0.0f);
    C3Vector targetOffset(0.0f, 0.0f, 0.0f);
    int32_t raw = 0;
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (spell) {
        const SpellVisualRec* visual = GetSpellVisual(spell, caster);

        if (visual) {
            sourceOffset.x = visual->m_missileCastOffset.x;
            sourceAttachment = visual->m_missileAttachment;
            sourceOffset.y = -visual->m_missileCastOffset.y;
            sourceOffset.z = visual->m_missileCastOffset.z;
            targetOffset.x = visual->m_missileImpactOffset.x;
            targetOffset.y = -visual->m_missileImpactOffset.y;
            targetOffset.z = visual->m_missileImpactOffset.z;
            raw = visual->m_flags & 0x200;
        }
    }

    bool useLocation = false;
    WOWGUID none[1];

    if (!count) {
        if (!location) {
            return;
        }

        count = 1;
        none[0] = WOWGUID::Make(0);
        targets = none;
        useLocation = true;
    }

    uint32_t created = 0;
    uint32_t offset = 0;

    do {
        CGxTexFlags texFlags;
        texFlags.m_wrapU = 1;
        texFlags.m_wrapV = 0;
        LightningObject* object = s_lightningObjects.NewNode(2, 0, 0);
        object->m_targets.SetCount(count + 1);
        object->m_bolts.SetCount(count);
        object->m_targets[0] = caster->m_obj->m_guid.m_guid;
        object->m_flags = 0;
        object->m_texture = TextureCreate(chain->m_texture, texFlags, &status, 0, NULL, 2);
        object->m_sourceOffset.x = sourceOffset.x;
        object->m_spellId = spellId;
        object->m_sourceAttachment = sourceAttachment;
        object->m_sourceOffset.y = sourceOffset.y;
        object->m_sourceOffset.z = sourceOffset.z;
        object->m_chainEffects = chain;

        if (!effect || persist) {
            object->m_effect = NULL;
        } else {
            object->m_effect = effect;
            effect->m_lightningRefCount++;
            object->m_effect->m_refCount++;
        }

        if (useLocation) {
            C44Matrix matrix = caster->m_vehiclePassenger ? caster->GetModelMatrix() : caster->GetMatrix();
            object->m_flags |= 0x2;
            object->m_targetOffset.x = location->x + (matrix.c0 * targetOffset.z + matrix.a0 * targetOffset.x + matrix.b0 * targetOffset.y);
            object->m_targetOffset.y = location->y + (matrix.c1 * targetOffset.z + matrix.a1 * targetOffset.x + matrix.b1 * targetOffset.y);
            object->m_targetOffset.z = location->z + (matrix.c2 * targetOffset.z + matrix.a2 * targetOffset.x + matrix.b2 * targetOffset.y);
        } else {
            object->m_targetOffset = targetOffset;
        }

        if (persist) {
            object->m_flags |= 0x1;
        }

        if (raw) {
            object->m_flags |= 0x4;
        }

        object->InitializeBolts(caster, targets, count, reverse, attachment);

        if (persist) {
            object->m_refCount++;

            if (lightning) {
                lightning[created++] = object;
            }
        }

        if (created >= maxLightning) {
            break;
        }

        uint32_t next = CLightning::DecodeChainEffectsID(*reinterpret_cast<const uint32_t*>(root->m_combo + offset));

        if (!next) {
            break;
        }

        chain = g_spellChainEffectsDB.GetRecord(next);

        if (!chain) {
            break;
        }

        offset += 4;
    } while (offset < 0x30);
}

// OFFSET: 0x008BFAF0
void SpellVisualGetLightning(CGUnit_C* caster, CEffect* effect, uint32_t chainEffectsId, bool persist, bool reverse, uint32_t spellId, const WOWGUID* targets, uint32_t count, LightningObject** lightning, uint32_t maxLightning, const C3Vector* location, int32_t attachment) {
    if (caster) {
        LightningObject::CreateFromSpell(caster, effect, chainEffectsId, targets, count, spellId, persist, reverse, lightning, maxLightning, location, attachment);
    }
}

// OFFSET: 0x008BFB30
void SpellVisualClearLightning(LightningObject* lightning) {
    uint32_t time = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    lightning->m_flags &= ~0x1;
    lightning->m_endTime = time;

    if (--lightning->m_refCount == 0) {
        lightning->~LightningObject();
        SMemFree(lightning, "delete", -1, 0);
    }
}

// OFFSET: 0x008BFB70/0x1406097B0
void SpellVisualsBlizzardStop(BlizzardObject* object) {
    object->Stop();
}

// OFFSET: 0x008BFB80
void SpellVisualsCancelMountTransition(MountTransitionObject* transition) {
    if (transition) {
        s_mountTransitions.DeleteNode(transition);
    }
}

// OFFSET: 0x008BFBA0
bool BlizzardObject::Update() {
    const SpellVisualKitAreaModelRec* areaModel = this->m_areaModel;
    int32_t ring = areaModel->m_enumID & 0x1;
    C3Vector center = this->GetWorldPosition();

    if (ring) {
        if (this->m_shardAccum == 0.0f) {
            float minAngle = 0.0f;
            float maxAngle = 6.2831855f;
            int32_t rotate = this->GetAngleRange(minAngle, maxAngle);
            float spacing = areaModel->m_spacing;

            if (spacing < 0.00000011920929f) {
                spacing = 1.0f;
            }

            float invSpacing = 1.0f / spacing;
            float rings = ceil(invSpacing * this->m_radius);
            float ringStep = this->m_radius / rings;
            float radius = 0.0f;

            if (this->m_radius + 0.00000011920929f >= 0.0f) {
                float arcScale = (maxAngle - minAngle) * 0.15915494f;

                do {
                    float arc = radius * 6.2831855f * arcScale;

                    if (fabs(arc) >= 0.00000023841858f) {
                        float step = arc / ceil(arc * invSpacing);
                        float limit = arc + 0.00000011920929f;

                        for (float s = 0.0f; limit >= s; s = s + step) {
                            float angle = 0.0f;

                            if (radius > 0.00000011920929f) {
                                angle = s / radius + minAngle;
                            }

                            C3Vector offset = this->GetPolarOffset(radius, angle, rotate);
                            this->AddShard(center, offset, 0.0f);
                            this->m_shardAccum = this->m_shardAccum + 1.0f;
                        }
                    }

                    radius = radius + ringStep;
                } while (this->m_radius + 0.00000011920929f >= radius);
            }
        }
    } else {
        float elapsed = CGWorldFrame::s_currentWorldFrame->m_elapsedSec;

        if (elapsed > 0.06666667f) {
            elapsed = 0.06666667f;
        }

        this->m_shardAccum = this->m_shardRate * elapsed + this->m_shardAccum;

        while (this->m_shardAccum >= 1.0f) {
            C3Vector offset = this->GetRandomShardOffset();
            this->AddShard(center, offset, 510.0f);
            this->m_shardAccum = this->m_shardAccum - 1.0f;
        }
    }

    if (this->m_endTime && this->m_endTime <= CGWorldFrame::s_currentWorldFrame->GetSceneTime()) {
        return false;
    }

    if ((this->m_shardRate == 0.0f || ring) && !this->m_shards.Head()) {
        return false;
    }

    return true;
}

// OFFSET: 0x008C0000
void SpellVisualsTick() {
    uint32_t time = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    LightningObject* lightning = s_lightningObjects.Head();

    while (!s_lightningObjects.IsEnd(lightning)) {
        LightningObject* next = s_lightningObjects.RawNext(lightning);

        if (!lightning->Tick(time)) {
            s_lightningObjects.UnlinkNode(lightning);
            s_lightningObjectsDeleted.LinkToTail(lightning);
        }

        lightning = next;
    }

    float elapsed = 0.0f;

    if (s_lastUpdateTime != -1) {
        elapsed = static_cast<float>(static_cast<int32_t>(time - s_lastUpdateTime)) * 0.001f;
    }

    s_lastUpdateTime = time;
    s_lightningManager->Update(elapsed);
    s_eclipse.Update(time);

    BlizzardObject* blizzard = s_blizzardObjects.Head();

    while (!s_blizzardObjects.IsEnd(blizzard)) {
        BlizzardObject* next = s_blizzardObjects.RawNext(blizzard);

        if (!blizzard->Update()) {
            s_blizzardObjects.UnlinkNode(blizzard);
            s_blizzardObjectsDeleted.LinkToTail(blizzard);
        }

        blizzard = next;
    }

    MountTransitionObject* transition = s_mountTransitions.Head();

    while (!s_mountTransitions.IsEnd(transition)) {
        MountTransitionObject* next = s_mountTransitions.RawNext(transition);

        if (transition->Tick()) {
            s_mountTransitions.UnlinkNode(transition);
            s_mountTransitionsDeleted.LinkToTail(transition);
        }

        transition = next;
    }
}

// OFFSET: 0x008C01D0
void SpellVisualsFreeDeleted() {
    LightningObject* lightning = s_lightningObjectsDeleted.Head();

    while (!s_lightningObjectsDeleted.IsEnd(lightning)) {
        LightningObject* next = s_lightningObjectsDeleted.RawNext(lightning);
        s_lightningObjectsDeleted.UnlinkNode(lightning);

        if (--lightning->m_refCount == 0) {
            lightning->~LightningObject();
            SMemFree(lightning, "delete", -1, 0);
        }

        lightning = next;
    }

    s_lightningObjectsDeleted.Clear();

    BlizzardObject* blizzard = s_blizzardObjectsDeleted.Head();

    while (!s_blizzardObjectsDeleted.IsEnd(blizzard)) {
        BlizzardObject* next = s_blizzardObjectsDeleted.RawNext(blizzard);
        blizzard->~BlizzardObject();
        s_blizzardObjectHeap.PutData(blizzard, NULL, 0);
        blizzard = next;
    }

    s_blizzardObjectsDeleted.Clear();

    MountTransitionObject* transition = s_mountTransitionsDeleted.Head();

    while (!s_mountTransitionsDeleted.IsEnd(transition)) {
        MountTransitionObject* next = s_mountTransitionsDeleted.RawNext(transition);
        s_mountTransitionsDeleted.DeleteNode(transition);
        transition = next;
    }

    s_mountTransitionsDeleted.Clear();
}

// OFFSET: 0x008C0360
void SpellVisualsClear() {
    SpellVisualsFreeDeleted();
    s_lightningObjects.Clear();

    BlizzardObject* object;

    while ((object = s_blizzardObjects.Head()) != NULL) {
        object->~BlizzardObject();
        s_blizzardObjectHeap.PutData(object, NULL, 0);
    }
}

// OFFSET: 0x008C03A0
void SpellVisualsShutdown() {
    SpellVisualsClear();

    CLightningManager* lightningManager = s_lightningManager;

    if (lightningManager) {
        lightningManager->~CLightningManager();
        SMemFree(lightningManager, "delete", -1, 0);
    }

    s_lastUpdateTime = -1;
    s_blizzardObjectHeap.Clear(typeid(BlizzardObject).raw_name(), -2, 1);
    s_shardHeap.Clear(typeid(Shard).raw_name(), -2, 1);
    s_spellEffectLevelVar = NULL;
    SpellVisualsShutdownModelAttachLookup();
    LightningDestroyBuffers();
}
