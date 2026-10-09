#include "Missile_C.h"
#include <cfloat>
#include <cmath>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C3Segment.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/ObjectClient/SimpleScript.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "WoW/Source/Object/ObjectClient/UnitMissileTrajectory_C.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellMissileRec.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"
#include "WoW/Source/Magic/Spell.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Services/ParticleSystem2.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/DB/Rec/SpellMissileMotionRec.h"

// STATIC: 0x00DD58E0
TSObjectAllocPool<CMissile>* CMissile::s_pool;

// STATIC: 0x00DD58D8
CMissile* CMissile::s_inFlightMissileList;

// STATIC: 0x00DD58DC
CMissile* CMissile::s_missileDestroyList;

// STATIC: 0x00CA5840
static uint32_t s_worldIntersectFlags = 0x111;

// STATIC: 0x00CA590C
static float s_groundTraceHigh = 10.0f;

// STATIC: 0x00CA5910
static float s_groundTraceLow = 20.0f;

// STATIC: 0x00CA5914
static float s_groundTraceEpsilon = 0.1f;

// STATIC: 0x00DD5870
static double s_msfNumbers[13];

// STATIC: 0x00CA58B0
static SimpleScriptIO s_msfIO = { { 13, 0, s_msfNumbers, 0 }, 0, 0, NULL };

// STATIC: 0x00CA5844
static const char* s_msfArgs[] = { "progress", "time", "missileIndex", "missileCount", "distanceToFirePos", "distanceToImpactPos", "startDistance", "totalDistance", "rand1", "rand2", "rand3", "spellID", NULL };

// STATIC: 0x00CA5878
static const char* s_msfReturns[] = { "transAngle", "transMag", "transRight", "transFront", "transUp", "modelYaw", "modelPitch", "modelRoll", "speedAbs", "speedScalar", "speedOffset", "scale", "faceImpactPos", NULL };

// INLINED
static float SmoothStep(float t) {
    if (t < 0.0f) {
        return 0.0f;
    }

    if (t <= 1.0f) {
        return (3.0f - t * 2.0f) * t * t;
    }

    return 1.0f;
}

// PROVEN: cca048cf IDENTICAL/DIFFERENT
// OFFSET: 0x009FCE20/0x14090EBB0
static int32_t MissileObjectUpdateCallback(void* param, uintptr_t object, uint64_t param64, uintptr_t param32) {
    return 1;
}

// OFFSET: 0x005850A0
void CMissile::ApplyMotionScriptFunctionRotationAndScale(const SimpleScriptIO& io, C44Matrix& matrix) {
    if (0.0 != io.m_numbers.m_data[5]) {
        matrix.RotateAroundZ(io.m_numbers.m_data[5] * 0.017453292f);
    }

    if (0.0 != io.m_numbers.m_data[6]) {
        matrix.RotateAroundY(io.m_numbers.m_data[6] * 0.017453292f);
    }

    if (0.0 != io.m_numbers.m_data[7]) {
        matrix.RotateAroundX(io.m_numbers.m_data[7] * 0.017453292f);
    }

    if (io.m_numbers.m_data[11] > 0.0) {
        float scale = io.m_numbers.m_data[11];

        if (scale <= 0.0099999998f) {
            scale = 0.0099999998f;
        }

        matrix.Scale(scale);
    }
}

// PROVEN: 083c0994 IDENTICAL/NONE
// OFFSET: 0x00585160
void CMissile::SetRemoteCollisionPos(const C3Vector& position) {
    this->m_flags |= 0x400000;
    this->m_remoteCollisionPos = position;
}

// OFFSET: 0x00585190
C3Vector* CMissile::ResolveTargetPosition(C3Vector* position, WOWGUID* transport) {
    if (!this->m_transportGuid) {
        *transport = WOWGUID();
        *position = this->m_impactPos;
    } else if (MovementObjIsTransport(this->m_transportGuid)) {
        *transport = this->m_transportGuid;
        *position = this->m_impactPos;
    } else {
        *transport = WOWGUID();
        *position = this->m_worldImpactPos;
    }

    return position;
}

// OFFSET: 0x00585220
ChaserState::ChaserState(const C3Vector& position, const C3Vector& velocity) {
    this->m_position = position;
    this->m_velocity = velocity;
}

// OFFSET: 0x00585250
void ChaserState::ChaserImplementation(const C3Vector& target, float stiffness, float elapsed) {
    float x = stiffness * elapsed;
    NTempest::SmoothDamp(this->m_position, this->m_velocity, target, stiffness, elapsed, 1.0f / (x * x * x * 0.235f + x * x * 0.47999999f + x + 1.0f));
}

// PROVEN: 815252a4 IDENTICAL/NONE
// OFFSET: 0x005854A0
void CMissile::Link(CMissile** link) {
    if (this->m_prevLink) {
        *this->m_prevLink = this->m_next;
    }

    if (this->m_next) {
        this->m_next->m_prevLink = this->m_prevLink;
    }

    this->m_next = NULL;
    this->m_prevLink = link;
    this->m_next = *link;
    *link = this;

    if (this->m_next) {
        this->m_next->m_prevLink = &this->m_next;
    }
}

// OFFSET: 0x00585500
int32_t CMissile::AttachToVictim(CGUnit_C* victim, uint32_t duration) {
    if (!this->m_model || !this->m_model->IsLoaded() || !(this->m_model->m_fFlags & 0x4000)) {
        return 0;
    }

    if (!victim || !victim->m_worldModel || !victim->m_worldModel->IsLoaded() || !victim->m_worldModel->HasAttachment(this->m_attachmentID) || this->m_fizzleTime) {
        return 0;
    }

    CMissile** link = &victim->m_pendingMissiles;
    uint32_t destroyTime = duration + CGWorldFrame::s_currentWorldFrame->GetSceneTime();

    for (CMissile* missile = victim->m_pendingMissiles; missile; missile = missile->m_next) {
        if (static_cast<int32_t>(missile->m_destroyTime - destroyTime) >= 0) {
            break;
        }

        link = &missile->m_next;
    }

    if (this->m_mapEntity) {
        World::ObjectDestroy(this->m_mapEntity);
    }

    this->m_mapEntity = 0;
    this->Link(link);
    this->m_destroyTime = destroyTime;
    this->m_model->SetBoneSequence(0xFFFFFFFF, 191, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    this->m_model->AttachToParent(victim->m_worldModel, this->m_attachmentID, NULL, 0);
    C44Matrix attach = victim->m_worldModel->GetAttachmentWorldTransform(this->m_attachmentID);
    this->m_model->SetWorldTransform(this->m_model->m_worldTransform * attach.AffineInverse(sqrt(attach.a1 * attach.a1 + attach.a2 * attach.a2 + attach.a0 * attach.a0)));
    this->m_flags |= 2;
    return 1;
}

// OFFSET: 0x005856B0
void CMissile::ApplyTargetTransportOffset(CGObject_C* target, C3Vector* position, float elapsed) {
    WOWGUID transport;

    if (target) {
        transport.m_guid = target->GetTransportGUID();
    } else {
        transport = this->m_transportGuid;
    }

    if (!transport) {
        return;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(transport, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    C3Vector transportPos = (object->m_obj->m_type & TYPEMASK_UNIT) ? static_cast<CGUnit_C*>(object)->BuildSmoothPosition() : object->GetPosition();

    if (this->m_flags & 0x4000000) {
        C3Vector delta(transportPos.x - this->m_lastTransportPos.x, transportPos.y - this->m_lastTransportPos.y, transportPos.z - this->m_lastTransportPos.z);
        this->m_transportOffset.ChaserImplementation(delta, 5.0f, elapsed);
        position->x = position->x + this->m_transportOffset.m_position.x;
        position->y = this->m_transportOffset.m_position.y + position->y;
        position->z = this->m_transportOffset.m_position.z + position->z;
    } else {
        this->m_transportOffset.m_position = C3Vector(0.0f, 0.0f, 0.0f);
        this->m_transportOffset.m_velocity = C3Vector(0.0f, 0.0f, 0.0f);
        this->m_flags |= 0x4000000;
    }

    this->m_lastTransportPos = transportPos;
}

// PROVEN: 0c5e8127 IDENTICAL/NONE
// OFFSET: 0x00585850
int32_t CMissile::IsHit() const {
    if (!(this->m_flags & 0x200) || (this->m_flags & 0x400)) {
        return this->m_flags & 1;
    }

    return this->m_currentTarget < this->m_targetHitResults.m_count && !this->m_targetHitResults.m_data[this->m_currentTarget];
}

// PROVEN: 1b3bb3d3 IDENTICAL/NONE
// OFFSET: 0x00585890
void CMissile::SetHitResult(uint32_t result) {
    if (!(this->m_flags & 0x200) || (this->m_flags & 0x400) || this->m_currentTarget >= this->m_targetHitResults.m_count) {
        this->m_hitResult = result;
    } else {
        this->m_targetHitResults.m_data[this->m_currentTarget] = result;
    }
}

// OFFSET: 0x005858E0
uint32_t CMissile::GetHitResult() const {
    if (!(this->m_flags & 0x200) || (this->m_flags & 0x400)) {
        return this->m_hitResult;
    }

    if (this->m_currentTarget >= this->m_targetHitResults.m_count) {
        return 0;
    }

    return this->m_targetHitResults.m_data[this->m_currentTarget];
}

// OFFSET: 0x00585920
uint32_t CMissile::GetMissReason() const {
    if (!(this->m_flags & 0x200)) {
        return this->m_missReason;
    }

    if (this->m_currentTarget >= this->m_targetMissReasons.m_count) {
        return 0;
    }

    return this->m_targetMissReasons.m_data[this->m_currentTarget];
}

// OFFSET: 0x00585950
C3Vector* MissileFindGroundPosition(C3Vector* result, const C3Vector& position, C3Vector& normal) {
    normal = C3Vector(0.0f, 0.0f, 1.0f);
    C4Plane plane;
    C3Segment segment;
    segment.b = C3Vector(position.x, position.y, position.z + s_groundTraceEpsilon);
    segment.t = C3Vector(position.x, position.y, position.z - s_groundTraceLow);
    float t = 1.0f;

    if (World::IntersectSegment(segment, t, plane, s_worldIntersectFlags)) {
        normal = plane.n;
        *result = segment.Lerp(t);
        return result;
    }

    segment.b = C3Vector(position.x, position.y, position.z + s_groundTraceHigh);
    segment.t = C3Vector(position.x, position.y, position.z - s_groundTraceEpsilon);
    t = 1.0f;

    if (World::IntersectSegment(segment, t, plane, s_worldIntersectFlags)) {
        normal = plane.n;
        *result = segment.Lerp(t);
        return result;
    }

    *result = position;
    return result;
}

// PROVEN: 2362d293 IDENTICAL/NONE
// OFFSET: 0x00585C00
void CMissile::InstallMotionScriptFunction(int32_t motionId) {
    this->m_motionScriptFunction = CSimpleScriptManager::s_instance->FindFunction(motionId);

    if (this->m_motionScriptFunction != -1) {
        return;
    }

    SpellMissileMotionRec* motion = g_spellMissileMotionDB.GetRecord(motionId);

    if (!motion) {
        return;
    }

    SimpleScriptFunctionDesc desc;
    desc.m_name = motion->m_name;
    desc.m_args = s_msfArgs;
    desc.m_stringArgs = NULL;
    desc.m_body = motion->m_scriptBody;
    desc.m_returns = s_msfReturns;
    desc.m_stringReturns = NULL;
    this->m_motionScriptFunction = CSimpleScriptManager::s_instance->CreateFunction(motionId, desc);
}

// OFFSET: 0x00585C90
void CMissile::AdjustTrajectoryDuration(uint32_t duration) {
    if (!(this->m_flags & 0x10000)) {
        return;
    }

    this->m_flags |= 0x40000;
    uint32_t now = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    float remaining = static_cast<float>(this->m_arrivalTime - now);
    this->m_adjustTime = now;

    if (remaining <= 0.0f) {
        this->m_adjustedDuration = 1;
        return;
    }

    if (!duration) {
        duration = 1;
    }

    float ratio = duration / remaining;

    if (ratio < 0.5f) {
        ratio = 0.5f;
    } else if (ratio >= 2.0f) {
        ratio = 2.0f;
    }

    this->m_adjustedDuration = static_cast<uint32_t>(static_cast<int64_t>(remaining * ratio));

    if (!this->m_adjustedDuration) {
        this->m_adjustedDuration = 1;
    }
}

// PROVEN: e2921f85 IDENTICAL/NONE
// OFFSET: 0x00585D80
void CMissile::AdjustAllMissileTrajectoryDurations(uint8_t castID, uint32_t duration) {
    for (CMissile* missile = CMissile::s_inFlightMissileList; missile; missile = missile->m_next) {
        if (missile->m_castID == castID) {
            missile->AdjustTrajectoryDuration(duration);
        }
    }
}

// OFFSET: 0x00585DC0
void CMissile::StartFizzling(WOWGUID caster, uint8_t castID, uint32_t spellID) {
    for (CMissile* missile = CMissile::s_inFlightMissileList; missile; missile = missile->m_next) {
        if (missile->m_casterGuid == caster && missile->m_castID == castID && missile->m_spellID == spellID && !missile->m_fizzleTime) {
            missile->m_fizzleTime = CGWorldFrame::s_currentWorldFrame->GetSceneTime() + 500;

            if (!missile->m_fizzleTime) {
                missile->m_fizzleTime = 1;
            }
        }
    }
}

// PROVEN: 341803e8 IDENTICAL/NONE
// OFFSET: 0x00585E30
void CMissile::OnRemoteMissileTrajectoryCollision(const MissileCollisionData& data) {
    for (CMissile* missile = CMissile::s_inFlightMissileList; missile; missile = missile->m_next) {
        if (missile->m_casterGuid == data.m_casterGuid && missile->m_castID == data.m_castID) {
            missile->m_flags |= 0x400000;
            missile->m_remoteCollisionPos = data.m_position;
        }
    }
}

// OFFSET: 0x00585E90
C3Vector CMissile::TransformPositionByTransport() {
    if (!this->m_transportGuid) {
        return this->m_impactPos;
    }

    C44Matrix matrix;

    if (MovementGetTransportMtxSafe(this->m_transportGuid, &matrix)) {
        this->m_worldImpactPos = this->m_impactPos * matrix;
        return this->m_worldImpactPos;
    }

    this->m_impactPos = this->m_worldImpactPos;
    this->m_transportGuid = WOWGUID();
    return this->m_worldImpactPos;
}

// OFFSET: 0x00586010
void CMissile::InitializeTrajectory() {
    C3Vector impact = this->TransformPositionByTransport();
    C3Vector delta(impact.x - this->m_launchPos.x, impact.y - this->m_launchPos.y, impact.z - this->m_launchPos.z);
    float pitch = this->m_pitch;
    C3Vector velocity(0.0f, 0.0f, 0.0f);

    if (pitch > 1.5707964f) {
        pitch = pitch - (pitch - 1.5707964f) * 2.0f;
    } else if (pitch < -1.5707964f) {
        pitch = (-1.5707964f - pitch) * 2.0f + pitch;
    }

    const SpellRec* spell = g_spellDB.GetRecord(this->m_spellID);
    const SpellMissileRec* missile = spell ? g_spellMissileDB.GetRecord(spell->m_spellMissileID) : NULL;
    this->m_gravity = missile ? missile->m_gravity : 19.291105f;
    float speed;
    int32_t result = SpellUtils_ComputeMissileTrajectory(pitch, &delta, this->m_gravity, &speed, &this->m_travelTime, &velocity);

    if (!result) {
        pitch = (this->m_gravity < 0.0f ? -0.0099999998f : 0.0099999998f) + this->m_pitch;

        if (pitch > 1.5707964f) {
            pitch = pitch - (pitch - 1.5707964f) * 2.0f;
        } else if (pitch < -1.5707964f) {
            pitch = (-1.5707964f - pitch) * 2.0f + pitch;
        }

        result = SpellUtils_ComputeMissileTrajectory(pitch, &delta, this->m_gravity, &speed, &this->m_travelTime, &velocity);
    }

    if (!result) {
        this->m_flags &= ~0x10000;
    } else if (result == 1) {
        if (spell && missile) {
            speed = this->m_trajectorySpeed;
            this->m_travelTime = SpellUtils_ComputeMissileTrajectoryTravelTime(pitch, speed, &delta, this->m_gravity);

            if (this->m_travelTime < 0.000099999997f) {
                speed = missile->m_defaultSpeedMin;
                this->m_travelTime = SpellUtils_ComputeMissileTrajectoryTravelTime(pitch, speed, &delta, this->m_gravity);

                if (this->m_travelTime < 0.000099999997f) {
                    speed = missile->m_defaultSpeedMax;
                    this->m_travelTime = SpellUtils_ComputeMissileTrajectoryTravelTime(pitch, speed, &delta, this->m_gravity);

                    if (this->m_travelTime < 0.000099999997f) {
                        this->m_flags &= ~0x10000;
                    }
                }
            }

            if (this->m_flags & 0x10000) {
                float inv = 1.0f / this->m_travelTime;
                velocity = C3Vector(delta.x * inv, delta.y * inv, inv * delta.z);
            }
        } else {
            this->m_flags &= ~0x10000;
        }
    }

    float remaining = static_cast<int32_t>(this->m_arrivalTime - CGWorldFrame::s_currentWorldFrame->GetSceneTime()) * 0.001f;

    if (remaining < 0.000099999997f) {
        remaining = 0.000099999997f;
    }

    if (this->m_flags & 0x10000) {
        this->m_velocity = velocity;
        float timeScale = this->m_travelTime / remaining;
        this->m_timeScale = timeScale;

        if (timeScale < 0.0f) {
            timeScale = 0.0f;
        } else if (timeScale >= 2.0f) {
            timeScale = 2.0f;
        }

        this->m_timeScale = timeScale;
    } else {
        float newSpeed = sqrt(delta.z * delta.z + delta.y * delta.y + delta.x * delta.x) / remaining;
        this->m_speed = newSpeed;
        this->m_baseSpeed = newSpeed;
    }

    if ((this->m_flags & 0x200000) && (missile->m_flags & 0x3C)) {
        CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_casterGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
        this->m_missileCollision = CMissileCollision::Alloc();
        CGUnit_C* root = caster && caster->m_vehiclePassenger && caster->m_vehiclePassenger->m_transitionVehicleGuid ? caster->m_vehiclePassenger->GetStoredRootVehicleUnit() : caster;
        C3Vector end = this->TransformPositionByTransport();
        this->m_missileCollision->Init(&this->m_launchPos, &end, pitch, speed, missile, root);
    }
}

// OFFSET: 0x00586470/0x1401EE740
void CMissile::ClearTargetingEffects() {
    if (!this->m_targetingUnit) {
        return;
    }

    CGObject_C* unit = ClntObjMgrObjectPtr(this->m_targetingUnit, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (unit) {
        if (this->m_spellID) {
            unit->ClearEffectKit(this->m_spellID, 7, this->m_targetingEffectTime, 1, 1);
        } else {
            const SpellVisualRec* visual = g_spellVisualDB.GetRecord(this->m_spellVisualID);

            if (visual && visual->m_missileTargetingKit) {
                unit->ClearEffectKitByID(0, visual->m_missileTargetingKit, this->m_targetingEffectTime, 1, 1);
            }
        }
    }

    this->m_targetingUnit = WOWGUID();
}

// PROVEN: 7599236b IDENTICAL/NONE
// OFFSET: 0x00586660
void CMissile::SetReadyToFire(CMissile** list) {
    this->Link(list);
}

// OFFSET: 0x00586670/0x1401EE9F0
void CMissile::UpdateImpactPos() {
    CGObject_C* target = ClntObjMgrObjectPtr(this->m_targetGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!target) {
        return;
    }

    C3Vector position;
    CM2Model* model = target->m_worldModel;

    if (this->m_attachmentID != -1 && model && model->HasAttachment(this->m_attachmentID) && (model->m_fFlags & 0x4000)) {
        position = this->m_attachOffset * model->GetAttachmentWorldTransform(this->m_attachmentID);
    } else {
        position = this->m_attachOffset;
        position.z = target->m_height * target->m_scale * 0.75f + position.z;

        if ((target->m_obj->m_type & TYPEMASK_UNIT) && static_cast<CGUnit_C*>(target)->m_vehiclePassenger && model && (model->m_fFlags & 0x4000)) {
            position *= model->GetAnimatedWorldTransform();
        } else {
            position *= target->GetMatrix();
        }
    }

    if ((this->m_motionFlags & 1) && (this->m_motionFlags & 8)) {
        position.z = target->GetPosition().z + this->m_groundOffset;
    }

    if (this->m_flags & 8) {
        C3Vector delta(position.x - this->m_position.x, position.y - this->m_position.y, position.z - this->m_position.z);
        float length = sqrt(delta.y * delta.y + delta.z * delta.z + delta.x * delta.x);

        if (fabs(length) < 2.3841858e-7f) {
            return;
        }

        float inv = 1.0f / length;
        float dirZ = inv * delta.z;

        if (fabs(fabs(dirZ) - 1.0f) < 2.3841858e-7f) {
            return;
        }

        float extended = this->m_speed * 0.5f + length;
        C3Vector offset(extended * (inv * delta.x), delta.y * inv * extended, extended * dirZ);
        C3Vector axis(-(delta.z * delta.x), delta.z * -delta.y, delta.x * delta.x - -delta.y * delta.y);
        float angle = sin(CRandom::sfloat_(g_rndSeed) * 1.5707964f) * 0.052359879f;
        C3Vector rotated = offset * C33Matrix::Rotation(angle, axis, false);
        this->m_impactPos.x = this->m_position.x + rotated.x;
        this->m_impactPos.y = this->m_position.y + rotated.y;
        this->m_impactPos.z = this->m_position.z + rotated.z;
        this->m_transportGuid = WOWGUID();
        return;
    }

    if (target->m_obj->m_type & TYPEMASK_UNIT) {
        CMovement_C* movement = static_cast<CGUnit_C*>(target)->m_movement;
        C3Vector toTarget(position.x - this->m_position.x, position.y - this->m_position.y, position.z - this->m_position.z);
        float speed = movement->m_currentSpeed;
        C3Vector velocity(speed * movement->m_moveDir.x, movement->m_moveDir.y * speed, movement->m_moveDir.z * speed);
        float missileSpeed = this->m_speed;
        float speedSq = missileSpeed * missileSpeed;

        if (velocity.z * velocity.z + velocity.y * velocity.y + velocity.x * velocity.x > missileSpeed * missileSpeed) {
            velocity.NormalizeSafe();
            velocity.x = velocity.x * missileSpeed;
            velocity.y = velocity.y * missileSpeed;
            velocity.z = velocity.z * missileSpeed;
        }

        C3Vector half(velocity.x * 0.5f, velocity.y * 0.5f, velocity.z * 0.5f);
        float a = half.z * half.z + half.y * half.y + half.x * half.x - speedSq;
        float b = (velocity.z * 0.5f * toTarget.z + half.y * toTarget.y + velocity.x * 0.5f * toTarget.x) * 2.0f;
        float c = (toTarget.z * toTarget.z + toTarget.y * toTarget.y + toTarget.x * toTarget.x) * a * 4.0f;
        float discriminant = b * b - c;

        if (discriminant >= 0.0f && a * a >= 0.0000099999997f) {
            float root = sqrt(discriminant);
            float t1 = (-b - root) * (0.5f / a);
            float t2 = (root - b) * (0.5f / a);
            float best = FLT_MAX;

            if (t1 > 0.0f && t1 < FLT_MAX) {
                best = t1;
            }

            if (t2 > 0.0f && best > t2) {
                best = t2;
            }

            if (FLT_MAX == best) {
                this->m_impactPos = position;
            } else {
                this->m_impactPos.x = half.x * best + position.x;
                this->m_impactPos.y = half.y * best + position.y;
                this->m_impactPos.z = half.z * best + position.z;
            }

            this->m_transportGuid = WOWGUID();
            return;
        }
    }

    this->m_impactPos = position;
    this->m_transportGuid = WOWGUID();
}

// OFFSET: 0x00586DF0
void CMissile::DetermineMissType() {
    if (this->IsHit()) {
        return;
    }

    switch (this->GetHitResult()) {
        case 1:
        case 3:
        case 6: {
            this->m_flags |= 0x8;
            break;
        }

        case 2:
        case 7:
        case 8: {
            this->m_flags |= 0x20;
            break;
        }

        case 4: {
            this->m_flags |= 0x40;
            this->SetHitResult(9);
            break;
        }

        case 5: {
            this->m_flags |= 0x10;
            break;
        }

        case 11: {
            if (this->m_flags & 0x8000000) {
                this->m_flags = (this->m_flags & ~0x8000040) | 0x40;
            } else {
                this->m_flags |= 0x80;
            }

            break;
        }

        default: {
            this->m_flags |= 0x40;
            this->m_hitResult = 9;
            break;
        }
    }
}

// OFFSET: 0x00586F20
void CMissile::CalcMissFade() {
    C3Vector impact = this->TransformPositionByTransport();
    float dx = impact.x - this->m_position.x;
    float dy = impact.y - this->m_position.y;
    float dz = impact.z - this->m_position.z;
    float fadeDistance = 0.0f;

    if (this->m_flags & 8) {
        fadeDistance = this->m_speed * 0.5f;
    }

    if (!this->m_model || this->m_fizzleTime) {
        return;
    }

    float distSq = dz * dz + dy * dy + dx * dx;

    if (fadeDistance * fadeDistance > distSq) {
        this->m_model->m_fadeAlpha = this->m_alpha * SmoothStep(sqrt(distSq) / fadeDistance);
    }
}

// OFFSET: 0x00587020
void CMissile::UpdateProgress(float distance) {
    C3Vector impact = this->TransformPositionByTransport();
    float dz = impact.z - this->m_launchPos.z;
    float dy = impact.y - this->m_launchPos.y;
    float dxSq = (impact.x - this->m_launchPos.x) * (impact.x - this->m_launchPos.x);
    float dySq = dy * dy;
    float progress = 1.0f;
    float remaining = sqrt(dz * dz + dxSq + dySq);

    if (remaining > 1.0e-6f) {
        progress = 1.0f - distance / remaining;
    }

    if (this->m_progress > progress) {
        progress = this->m_progress;
    }

    this->m_progress = progress;

    impact = this->TransformPositionByTransport();
    float hx = impact.x - this->m_position.x;
    float total = sqrt(dxSq + dySq);
    float hy = impact.y - this->m_position.y;
    float horizontalProgress = 1.0f;

    if (total > 1.0e-6f) {
        horizontalProgress = 1.0f - sqrt(hy * hy + hx * hx) / total;
    }

    if (this->m_horizontalProgress <= horizontalProgress) {
        this->m_horizontalProgress = horizontalProgress;
    }
}

// OFFSET: 0x00587140
void CMissile::FollowGround(const C3Vector& direction, float distance) {
    if (distance < 0.0f) {
        distance = 0.0f;
    } else if (distance >= 50.0f) {
        distance = 50.0f;
    }

    C3Vector target(this->m_position.x + direction.x * distance * 2.0f, this->m_position.y + direction.y * distance * 2.0f, this->m_position.z + direction.z * distance * 2.0f);
    C3Vector flat(direction.x, direction.y, direction.z);

    if (fabs(flat.z) <= 0.99900001f) {
        float inv = 1.0f / sqrt(flat.y * flat.y + flat.x * flat.x);
        flat.x = flat.x * inv;
        flat.y = flat.y * inv;
        flat.z = inv * 0.0f;
    } else {
        flat = C3Vector(1.0f, 0.0f, 0.0f);
    }

    float remaining = distance;
    C3Vector start(0.0f, 0.0f, 0.0f);
    C3Vector end(0.0f, 0.0f, 0.0f);
    int32_t steps;

    for (steps = 4; steps > 0; steps--) {
        if (remaining <= 0.001f) {
            break;
        }

        C3Vector step(flat.x * remaining, flat.y * remaining, remaining * flat.z);
        start = this->m_position;
        end = C3Vector(this->m_position.x + step.x, this->m_position.y + step.y, this->m_position.z + step.z);
        float t = 1.0f;
        C3Vector hit(0.0f, 0.0f, 0.0f);

        if (!World::Intersect(start, end, &hit, &t, s_worldIntersectFlags, NULL)) {
            break;
        }

        this->m_position.x = this->m_position.x + step.x * t;
        this->m_position.y = this->m_position.y + step.y * t;
        this->m_position.z = this->m_position.z + step.z * t;
        remaining = (1.0f - t) * remaining;
        this->m_position.z = this->m_groundOffset + this->m_position.z;

        if (t > 0.99900001f) {
            break;
        }
    }

    this->m_position = end;

    if (steps == 4) {
        start = this->m_position;
        end = C3Vector(this->m_position.x, this->m_position.y, this->m_position.z - (this->m_groundOffset + 20.0f));
        C3Vector hit(0.0f, 0.0f, 0.0f);
        float t = 1.0f;

        if (World::Intersect(start, end, &hit, &t, s_worldIntersectFlags, NULL)) {
            float ground = this->m_groundOffset + hit.z;
            float z = this->m_position.z - (this->m_position.z - ground) * this->m_followGroundRate * distance;

            if (z > ground) {
                ground = z;
            }

            this->m_position.z = ground;
        }
    }

    if (this->m_horizontalProgress > this->m_followGroundStart) {
        float blend = 1.0f / (1.0f - this->m_followGroundStart) * (this->m_horizontalProgress - this->m_followGroundStart);
        blend = blend * blend;

        if (this->m_motionFlags & 4) {
            this->m_position.x = this->m_position.x * (1.0f - blend) + target.x * blend;
            this->m_position.y = this->m_position.y * (1.0f - blend) + target.y * blend;
            this->m_position.z = this->m_position.z * (1.0f - blend) + target.z * blend;
        } else {
            C3Vector impact = this->TransformPositionByTransport();
            this->m_position.x = this->m_position.x * (1.0f - blend) + impact.x * blend;
            this->m_position.y = this->m_position.y * (1.0f - blend) + impact.y * blend;
            this->m_position.z = this->m_position.z * (1.0f - blend) + impact.z * blend;
        }
    }
}

// OFFSET: 0x005875E0
int32_t CMissile::CallMotionScriptFunction(SimpleScriptIO& io) {
    io.m_numbers.m_count = 12;
    io.m_numbers.m_data[0] = this->m_progress;
    io.m_numbers.m_data[1] = static_cast<double>(static_cast<int32_t>(CGWorldFrame::s_currentWorldFrame->GetSceneTime() - this->m_fireTime)) * 0.001f;
    io.m_numbers.m_data[2] = this->m_missileIndex;
    io.m_numbers.m_data[3] = this->m_missileCount;

    float dz = this->m_launchPos.z - this->m_position.z;
    float dy = this->m_launchPos.y - this->m_position.y;
    float dx = this->m_launchPos.x - this->m_position.x;
    io.m_numbers.m_data[4] = sqrt(dx * dx + (dy * dy + dz * dz));

    C3Vector impact = this->TransformPositionByTransport();
    dx = impact.x - this->m_position.x;
    dy = impact.y - this->m_position.y;
    dz = impact.z - this->m_position.z;
    io.m_numbers.m_data[5] = sqrt(dx * dx + dy * dy + dz * dz);
    io.m_numbers.m_data[6] = this->m_initialDistance;

    impact = this->TransformPositionByTransport();
    dy = impact.y - this->m_launchPos.y;
    dx = impact.x - this->m_launchPos.x;
    dz = impact.z - this->m_launchPos.z;
    io.m_numbers.m_data[7] = sqrt(dx * dx + dy * dy + dz * dz);
    io.m_numbers.m_data[8] = this->m_random[0];
    io.m_numbers.m_data[9] = this->m_random[1];
    io.m_numbers.m_data[10] = this->m_random[2];
    io.m_numbers.m_data[11] = this->m_spellID;

    if (!CSimpleScriptManager::s_instance->CallFunction(this->m_motionScriptFunction, io, io)) {
        return 0;
    }

    for (int32_t i = 0; i < 13; i++) {
        if (!_finite(io.m_numbers.m_data[i])) {
            io.m_numbers.m_data[i] = 0.0;
        }
    }

    if (io.m_numbers.m_data[8] > 0.0) {
        this->m_flags |= 0x1000000;
        this->m_speed = io.m_numbers.m_data[8];
    }

    if (io.m_numbers.m_data[9] > 0.0) {
        this->m_flags |= 0x1000000;
        this->m_speed = io.m_numbers.m_data[9] * this->m_baseSpeed;
    }

    if (0.0 != io.m_numbers.m_data[10]) {
        float speed = io.m_numbers.m_data[10] + this->m_baseSpeed;
        this->m_speed = speed;
        this->m_flags |= 0x1000000;
        this->m_speed = speed <= 0.0099999998f ? 0.0099999998f : speed;
    }

    return 1;
}

// OFFSET: 0x005877F0
int32_t CMissile::ApplyMotionScriptFunctionTranslation(const SimpleScriptIO& io, C44Matrix& matrix) {
    C3Vector impact = this->TransformPositionByTransport();
    C3Vector dir(impact.x - this->m_launchPos.x, impact.y - this->m_launchPos.y, 0.0f);
    float lengthSq = dir.y * dir.y + dir.x * dir.x;

    if (lengthSq > 2.3841858e-7f) {
        float scale = 1.0f / sqrt(lengthSq);
        dir.x = dir.x * scale;
        dir.y = dir.y * scale;
        dir.z = scale * 0.0f;
    }

    C3Vector side(-dir.y, dir.x, dir.z);
    C3Vector start(matrix.d0, matrix.d1, matrix.d2);
    const double* numbers = io.m_numbers.m_data;

    if (0.0 != numbers[1]) {
        double angle = numbers[0] * 0.017453292f;
        float s = sin(angle);
        float c = cos(angle);
        matrix.d0 = matrix.d0 + numbers[1] * (s * side.x);
        matrix.d1 = matrix.d1 + side.y * s * numbers[1];
        matrix.d2 = matrix.d2 + numbers[1] * c;
    }

    matrix.d0 = numbers[2] * side.x + matrix.d0;
    matrix.d1 = matrix.d1 + numbers[2] * side.y;
    matrix.d2 = matrix.d2 + side.z * numbers[2];

    matrix.d0 = matrix.d0 + numbers[3] * dir.x;
    matrix.d1 = matrix.d1 + dir.y * numbers[3];
    matrix.d2 = matrix.d2 + dir.z * numbers[3];

    matrix.d2 = numbers[4] + matrix.d2;

    if ((matrix.d2 - start.z) * (matrix.d2 - start.z) + (matrix.d1 - start.y) * (matrix.d1 - start.y) + (matrix.d0 - start.x) * (matrix.d0 - start.x) <= 0.2f) {
        this->m_flags &= ~0x8000;
    } else {
        this->m_flags |= 0x8000;
    }

    int32_t faceImpact = io.m_numbers.m_data[12] > 0.0;

    if (io.m_numbers.m_data[12] <= 0.0) {
        this->m_flags &= ~0x20000000;
    } else {
        this->m_flags |= 0x20000000;
    }

    return faceImpact;
}

// OFFSET: 0x00587A90
void CMissile::BeginWaitingForModelFX(bool disableEffects) {
    this->ClearTargetingEffects();

    if (disableEffects) {
        this->m_flags |= 0x10000000;
    }

    this->Link(&CMissile::s_missileDestroyList);
    CGObject_C* object = ClntObjMgrObjectPtr(this->m_targetGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);
    CGUnit_C* target = object && (object->m_obj->m_type & TYPEMASK_UNIT) ? static_cast<CGUnit_C*>(object) : NULL;

    if (this->m_model && this->m_model->IsLoaded() && (this->m_model->m_fFlags & 0x4000) && !(this->m_flags & 0x40000000)) {
        if (!this->m_fizzleTime) {
            this->m_model->SetBoneSequence(0xFFFFFFFF, 191, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
        }

        if (target) {
            CM2Model* targetModel = target->m_worldModel;

            if (targetModel && targetModel->IsLoaded() && targetModel->HasAttachment(this->m_attachmentID)) {
                this->m_model->AttachToParent(targetModel, this->m_attachmentID, NULL, 0);
                C44Matrix attach = targetModel->GetAttachmentWorldTransform(this->m_attachmentID);
                this->m_model->SetWorldTransform(this->m_model->m_worldTransform * attach.AffineInverse(sqrt(attach.a1 * attach.a1 + attach.a2 * attach.a2 + attach.a0 * attach.a0)));
            }
        }
    }

    if (this->m_mapEntity) {
        World::ObjectSetUpdateCallback(this->m_mapEntity, NULL, NULL);
    }
}

// OFFSET: 0x00587EA0
CMissile::CMissile() {
    this->m_casterGuid = WOWGUID();
    this->m_sourceGuid = WOWGUID();
    this->m_targetGuid = WOWGUID();
    this->m_transportGuid = WOWGUID();
    this->m_model = NULL;
    this->m_spellID = 0;
    this->m_spellVisualID = 0;
    this->m_launchSpeed = 0.0f;
    this->m_speed = 0.0f;
    this->m_baseSpeed = 0.0f;
    this->m_position = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_launchPos = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_impactPos = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_worldImpactPos = C3Vector(0.0f, 0.0f, 0.0f);
    this->unk_0074 = C3Vector(0.0f, 0.0f, 1.0f);
    this->m_lastModelPos = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_flags = 0;
    this->m_attachmentID = -1;
    this->m_attachOffset = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_soundKitID = 0;
    this->m_missileSound = 0;
    this->m_hitResult = 0;
    this->m_destroyTime = 0;
    this->m_victimAnimTime = 0;
    this->m_castTime = 0;
    this->m_duration = 1000;
    this->m_lastUpdateTime = 0;
    this->m_currentTarget = 0;
    this->m_scale = 1.0f;
    this->m_alpha = 1.0f;
    this->m_progress = 0.0f;
    this->m_horizontalProgress = 0.0f;
    this->m_groundOffset = 0.0f;
    this->m_followGroundRate = 0.0f;
    this->m_followGroundStart = 0.0f;
    this->m_motionFlags = 0;
    this->m_missileIndex = 0;
    this->m_missileCount = 1;
    this->m_velocity = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_timeScale = 1.0f;
    this->m_fizzleTime = 0;
    this->m_missileCollision = NULL;
    this->m_remoteCollisionPos = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_mapEntity = 0;
    this->m_mapEntity2 = 0;
    this->m_prevLink = NULL;
    this->m_next = NULL;
    this->m_targetingUnit = WOWGUID();
    this->m_targetingEffectTime = -1;
    this->m_motionScriptFunction = -1;
    this->m_initialDistance = 0.0f;
    this->m_random[0] = 0.0f;
    this->m_random[1] = 0.0f;
    this->m_random[2] = 0.0f;
    this->m_smoothDirection = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_lastTransportPos = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_transportOffset.m_position = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_transportOffset.m_velocity = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_targets.SetCount(0);
    this->m_targetHitResults.SetCount(0);
    this->m_targetMissReasons.SetCount(0);
    this->m_targetPoints.SetCount(0);
}

// OFFSET: 0x00588160/0x1401F0890
CMissile::~CMissile() {
    this->ClearTargetingEffects();

    if (this->m_model) {
        if (this->m_model->m_attachParent) {
            this->m_model->DetachFromParent();
        }

        this->m_model->Release();
        this->m_model = NULL;
    }

    if (this->m_mapEntity) {
        World::ObjectDestroy(this->m_mapEntity);
        this->m_mapEntity = 0;
    }

    if (this->m_mapEntity2) {
        World::ObjectDestroy(this->m_mapEntity2);
        this->m_mapEntity2 = 0;
    }

    if (this->m_prevLink) {
        *this->m_prevLink = this->m_next;
    }

    if (this->m_next) {
        this->m_next->m_prevLink = this->m_prevLink;
    }

    this->m_next = NULL;
    this->m_prevLink = NULL;
    SE3::StopSound(this->m_missileSound, 150, __FILE__, __LINE__, "CMissile::~CMissile");
    this->m_targets.SetCount(0);
    this->m_targetHitResults.SetCount(0);
    this->m_targetMissReasons.SetCount(0);
    this->m_targetPoints.SetCount(0);
}

// OFFSET: 0x005882C0
void CMissile::HitTargetAreaEffect(const SpellRec* spell, const SpellVisualRec* visual) {
    CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_casterGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    const SpellVisualKitRec* areaKit = g_spellVisualKitDB.GetRecord(visual->m_impactAreaKit);
    bool canPlay = CanPlaySpellVisual(this->m_casterGuid.m_guid, 0, visual);

    if (areaKit && !this->m_fizzleTime && canPlay) {
        C3Vector position;
        WOWGUID transport;
        this->ResolveTargetPosition(&position, &transport);

        if (caster) {
            if (CanPlaySpellVisualKit(caster->m_obj->m_guid.m_guid, 0, areaKit, NULL)) {
                PlaySpellVisuaKitData data(spell, areaKit, 3);
                data.m_transportGUID = transport;
                data.m_position = &position;
                data.m_someGUID = this->m_casterGuid;
                caster->PlaySpellVisualKit(data);
            }
        } else {
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (player && CanPlaySpellVisualKit(this->m_casterGuid.m_guid, 0, areaKit, NULL)) {
                PlaySpellVisuaKitData data(spell, areaKit, 3);
                data.m_position = &position;
                data.m_transportGUID = transport;
                data.m_someGUID = this->m_casterGuid;
                data.m_animParam = 0;
                player->PlaySpellVisualKit(data);
            }
        }
    }

    const SpellVisualKitRec* casterKit = Spell_C_GetAppropriateImpactKit(visual, 1);
    const SpellVisualKitRec* targetKit = Spell_C_GetAppropriateImpactKit(visual, 0);
    const SpellVisualKitRec* stateKit = g_spellVisualKitDB.GetRecord(visual->m_stateKit);
    uint32_t hitResultCount = this->m_targetHitResults.m_count;

    if (canPlay) {
        for (uint32_t i = 0; i < this->m_targets.m_count; i++) {
            WOWGUID targetGuid;
            targetGuid.m_guid = this->m_targets.m_data[i];
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(targetGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (!unit) {
                continue;
            }

            if (hitResultCount) {
                uint32_t result = this->m_targetHitResults.m_data[i];

                if (result && !(this->m_flags & 0x100) && !this->m_missileIndex) {
                    if (spell && !(spell->m_attributes & 0x100)) {
                        unit->AddWorldText(this->m_casterGuid, result, spell, 0);
                        CGGameUI::ShowSpellMissFeedback(unit->m_obj->m_guid, result);
                    }

                    UnitCombatLogSpellMissed(this->m_casterGuid, unit->m_obj->m_guid, spell ? spell->m_id : 0, result, 0, 0, 0);
                }
            } else if (!this->m_fizzleTime && CanPlaySpellVisual(0, unit->m_obj->m_guid.m_guid, visual)) {
                const SpellVisualKitRec* kit = unit == caster ? casterKit : targetKit;

                if (CanPlaySpellVisualKit(this->m_casterGuid.m_guid, unit->m_obj->m_guid.m_guid, kit, NULL)) {
                    PlaySpellVisuaKitData data(spell, kit, 1);
                    data.m_someGUID = this->m_casterGuid;
                    unit->PlaySpellVisualKit(data);
                }

                if (unit->m_unit->m_unitFieldHealth > 0 && unit->HasAura(spell->m_id, WOWGUID()) && CanPlaySpellVisualKit(this->m_casterGuid.m_guid, unit->m_obj->m_guid.m_guid, stateKit, NULL) && (static_cast<int8_t>(stateKit->m_flags) >= 0 || unit->HasAuraActive(spell->m_id, WOWGUID()))) {
                    PlaySpellVisuaKitData data(spell, stateKit, 2);
                    data.m_someGUID = this->m_casterGuid;
                    unit->PlaySpellVisualKit(data);
                }
            }
        }

        if (caster && CanPlaySpellVisualKit(this->m_casterGuid.m_guid, 0, targetKit, NULL)) {
            for (uint32_t i = 0; i < this->m_targetPoints.m_count; i++) {
                PlaySpellVisuaKitData data(spell, targetKit, 1);
                SpellCastTargetPoint* point = &this->m_targetPoints.m_data[i];

                if (point->m_transport && MovementObjIsTransport(point->m_transport)) {
                    data.m_transportGUID = point->m_transport;
                    data.m_position = &point->m_position;
                } else {
                    data.m_transportGUID = WOWGUID();
                    data.m_position = &point->m_worldPosition;
                }

                caster->PlaySpellVisualKit(data);
            }
        }
    }

    if (hitResultCount) {
        this->m_flags |= 0x100;
    }

    this->BeginWaitingForModelFX(false);
}

// OFFSET: 0x005887F0
void CMissile::ComputeModelMatrix(C44Matrix* matrix, const C3Vector* position, const C3Vector* direction) {
    matrix->d0 = position->x;
    matrix->d1 = position->y;
    matrix->d2 = position->z;

    if (this->m_motionScriptFunction != -1) {
        if (this->CallMotionScriptFunction(s_msfIO)) {
            this->ApplyMotionScriptFunctionTranslation(s_msfIO, *matrix);
        } else {
            this->m_motionScriptFunction = -1;
        }
    }

    uint32_t flags = this->m_flags;
    int32_t animated = 0;

    if (flags & 0x2000000) {
        CM2Model* model = this->m_model;
        C3Vector translation(matrix->d0, matrix->d1, matrix->d2);
        model->Animate();
        *matrix = model->m_matrixF4 * model->m_scene->m_viewInv;
        matrix->d0 = translation.x;
        matrix->d1 = translation.y;
        matrix->d2 = translation.z;
        this->m_flags &= ~0x2000000;
        animated = 1;
    } else {
        if (flags & 0x20000000) {
            C3Vector impact = this->TransformPositionByTransport();
            C3Vector facing(impact.x - this->m_launchPos.x, impact.y - this->m_launchPos.y, 0.0f);
            float lengthSq = facing.y * facing.y + facing.x * facing.x;

            if (lengthSq > 2.3841858e-7f) {
                float inv = 1.0f / sqrt(lengthSq);
                facing.x = facing.x * inv;
                facing.y = facing.y * inv;
                facing.z = inv * 0.0f;
            }

            if (sqrt(facing.y * facing.y + facing.z * facing.z + facing.x * facing.x) > 2.3841858e-7f) {
                matrix->a0 = facing.x;
                matrix->a1 = facing.y;
                matrix->a2 = facing.z;
            } else {
                matrix->a0 = direction->x;
                matrix->a1 = direction->y;
                matrix->a2 = direction->z;
            }
        } else {
            bool haveForward = false;

            if (this->m_model && (flags & 0x800)) {
                C3Vector delta(matrix->d0 - this->m_lastModelPos.x, matrix->d1 - this->m_lastModelPos.y, matrix->d2 - this->m_lastModelPos.z);
                float length = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);

                if (length > 2.3841858e-7f) {
                    float scaleLength = length;

                    if (length < 0.0099999998f && (flags & 0x100000)) {
                        float weight = length * 100.0f;

                        if (length * 100.0f < 0.050000001f) {
                            weight = 0.050000001f;
                        }

                        delta.z = this->m_smoothDirection.z * (1.0f - weight) + delta.z * weight;
                        delta.y = this->m_smoothDirection.y * (1.0f - weight) + delta.y * weight;
                        delta.x = (1.0f - weight) * this->m_smoothDirection.x + delta.x * weight;
                        scaleLength = sqrt(delta.z * delta.z + delta.y * delta.y + delta.x * delta.x);
                    }

                    float inv = 1.0f / scaleLength;
                    this->m_smoothDirection = delta;
                    this->m_flags = flags | 0x100000;
                    matrix->a0 = inv * delta.x;
                    matrix->a1 = delta.y * inv;
                    matrix->a2 = delta.z * inv;
                    haveForward = true;
                }
            }

            if (!haveForward) {
                matrix->a0 = direction->x;
                matrix->a1 = direction->y;
                matrix->a2 = direction->z;
            }
        }

        if (this->m_flags & 0x40000000) {
            matrix->a0 = matrix->a0 * -1.0f;
            matrix->a1 = matrix->a1 * -1.0f;
            matrix->a2 = matrix->a2 * -1.0f;
        }

        float forwardX = matrix->a0;
        float sideX = -matrix->a1;
        matrix->b0 = sideX;
        matrix->b1 = forwardX;
        matrix->b2 = 0.0f;

        if (sideX * sideX + forwardX * forwardX <= 0.0000099999997f) {
            if (this->m_model && (this->m_flags & 0x800) && (matrix->a0 = direction->x, matrix->a1 = direction->y, matrix->a2 = direction->z, matrix->b0 = -direction->y, matrix->b1 = direction->x, matrix->b2 = 0.0f, matrix->b0 * matrix->b0 + matrix->b1 * matrix->b1 > 0.0000099999997f)) {
                reinterpret_cast<C2Vector*>(&matrix->b0)->Normalize();
                C2Vector side(*reinterpret_cast<C2Vector*>(&matrix->b0));
                matrix->c0 = -(side.y * matrix->a2);
                matrix->c1 = side.x * matrix->a2;
                matrix->c2 = side.y * matrix->a0 - side.x * matrix->a1;
            } else {
                matrix->a0 = 0.0f;
                matrix->a1 = 0.0f;
                matrix->a2 = 1.0f;
                matrix->b0 = 1.0f;
                matrix->b1 = 0.0f;
                matrix->b2 = 0.0f;
                matrix->c2 = 0.0f;
                matrix->c1 = 1.0f;
                matrix->c0 = 0.0f;
            }
        } else {
            float b1 = matrix->b1;
            float inv = 1.0f / sqrt(matrix->b0 * matrix->b0 + b1 * b1);
            matrix->b0 = matrix->b0 * inv;
            matrix->b1 = b1 * inv;
            C2Vector side(*reinterpret_cast<C2Vector*>(&matrix->b0));
            matrix->c0 = -(matrix->a2 * side.y);
            matrix->c1 = side.x * matrix->a2;
            matrix->c2 = side.y * matrix->a0 - side.x * matrix->a1;
        }
    }

    if (this->m_motionScriptFunction != -1) {
        CMissile::ApplyMotionScriptFunctionRotationAndScale(s_msfIO, *matrix);
    }

    if (!animated) {
        matrix->Scale(this->m_scale);
    }
}

// OFFSET: 0x00588E40
void CMissile::CalcPosition(int32_t advance) {
    float elapsed = 0.0f;

    if (advance) {
        elapsed = CGWorldFrame::s_currentWorldFrame->m_elapsedSec;
    }

    uint32_t now = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    this->m_lastUpdateTime = now;

    if (0.0f == this->m_speed) {
        return;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(this->m_targetGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);
    CGUnit_C* target = static_cast<CGUnit_C*>(object);
    int32_t targetIsUnit;

    if (object && (object->m_obj->m_type & TYPEMASK_UNIT)) {
        targetIsUnit = 1;

        if (!(this->m_flags & 8)) {
            this->UpdateImpactPos();
        }
    } else {
        targetIsUnit = 0;
    }

    if (this->m_victimAnimTime && static_cast<int32_t>(now - this->m_victimAnimTime) >= 0) {
        this->m_victimAnimTime = 0;
    }

    if (this->m_flags & 0x2000) {
        this->ApplyTargetTransportOffset(object, &this->m_launchPos, elapsed);
    }

    this->ApplyTargetTransportOffset(object, &this->m_position, elapsed);
    float step = this->m_speed * elapsed;
    C3Vector impact = this->TransformPositionByTransport();
    C3Vector delta(impact.x - this->m_position.x, impact.y - this->m_position.y, impact.z - this->m_position.z);
    uint32_t flags = this->m_flags;
    uint32_t arrival = this->m_arrivalTime;
    float timeScale = this->m_timeScale;
    float distance = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
    int32_t timed = (flags & 0x1024000) == 0x20000 && elapsed > 0.0f && this->m_duration;
    uint32_t trajectory = flags & 0x10000;

    if (trajectory) {
        if (flags & 0x40000) {
            float t = this->m_adjustedDuration ? static_cast<float>(now - this->m_adjustTime) / static_cast<float>(this->m_adjustedDuration) : 1.0f;
            arrival += static_cast<int64_t>((static_cast<float>(this->m_adjustedDuration) - (arrival - this->m_adjustTime)) * t);
            float targetScale = arrival == this->m_fireTime ? 1.0f : this->m_travelTime / ((arrival - this->m_fireTime) * 0.001f);
            timeScale = (targetScale - timeScale) * t + timeScale;
        }

        if (static_cast<int32_t>(now - arrival) >= 0) {
            this->ReachedTarget(targetIsUnit ? target : NULL, &delta);
            return;
        }
    } else if (timed) {
        if (static_cast<int32_t>(now - this->m_fireTime - this->m_duration) >= 0) {
            this->ReachedTarget(targetIsUnit ? target : NULL, &delta);
            return;
        }
    } else if (step >= distance && !(flags & 2)) {
        this->ReachedTarget(targetIsUnit ? target : NULL, &delta);
        return;
    }

    if (step <= 0.0f && this->m_mapEntity) {
        return;
    }

    C3Vector dir;

    if (fabs(distance) < 2.3841858e-7f) {
        dir = C3Vector(0.0f, 0.0f, 1.0f);
    } else {
        dir.x = 1.0f / distance * delta.x;
        dir.y = delta.y * (1.0f / distance);
        dir.z = delta.z * (1.0f / distance);
    }

    C3Vector previous = this->m_position;

    if (trajectory) {
        float total = (arrival - static_cast<float>(this->m_fireTime)) * 0.001f;
        float left = 0.001f * static_cast<int32_t>(arrival - now);
        float t = (total - left) * timeScale;
        this->m_position.x = this->m_velocity.x * t + this->m_launchPos.x;
        this->m_position.y = this->m_launchPos.y + this->m_velocity.y * t;
        this->m_position.z = this->m_launchPos.z + this->m_velocity.z * t;
        this->m_position.z = this->m_position.z - this->m_gravity * t * t * 0.5f;

        if (flags & 0x1000) {
            float progress = 1.0f - left / total;
            this->m_progress = progress;
            this->m_horizontalProgress = progress;
        }
    } else if (timed) {
        float t = static_cast<float>(now - this->m_fireTime) / static_cast<float>(this->m_duration);
        this->m_progress = t;
        this->m_horizontalProgress = t;
        C3Vector targetPos = this->TransformPositionByTransport();
        float keep = 1.0f - this->m_progress;
        this->m_position.x = keep * this->m_launchPos.x + targetPos.x * t;
        this->m_position.y = this->m_launchPos.y * keep + targetPos.y * t;
        this->m_position.z = this->m_launchPos.z * keep + targetPos.z * t;
    } else {
        if (step > 0.0f) {
            if (flags & 0x4000) {
                this->FollowGround(dir, step);
            } else {
                this->m_position.x = this->m_position.x + dir.x * step;
                this->m_position.y = this->m_position.y + dir.y * step;
                this->m_position.z = this->m_position.z + dir.z * step;
            }
        }

        if (this->m_flags & 0x1000) {
            this->UpdateProgress(distance - step);
        }
    }

    if (this->m_flags & 0x400000) {
        float dz = this->m_position.z - this->m_remoteCollisionPos.z;
        float dy = this->m_position.y - this->m_remoteCollisionPos.y;
        float dx = this->m_position.x - this->m_remoteCollisionPos.x;
        float pz = previous.z - this->m_remoteCollisionPos.z;
        float py = previous.y - this->m_remoteCollisionPos.y;
        float px = previous.x - this->m_remoteCollisionPos.x;

        if (dz * dz + dy * dy + dx * dx > pz * pz + py * py + px * px) {
            this->m_impactPos = previous;
            this->m_transportGuid = WOWGUID();
            this->ReachedTarget(NULL, &delta);
            return;
        }
    }

    if (this->m_missileCollision) {
        float t = this->m_missileCollision->ComputeSegmentImpactTime(&previous, &this->m_position);

        if (t >= 0.0f) {
            this->m_position.x = (this->m_position.x - previous.x) * t + previous.x;
            this->m_position.y = (this->m_position.y - previous.y) * t + previous.y;
            this->m_position.z = (this->m_position.z - previous.z) * t + previous.z;
            this->HandleMissileTrajectoryCollision(&delta);
            return;
        }
    }

    if (this->m_model) {
        C44Matrix matrix;
        this->ComputeModelMatrix(&matrix, &this->m_position, &dir);
        this->m_model->m_fFlags |= 0x4000;
        this->m_model->m_worldTransform = matrix;
        this->m_lastModelPos = C3Vector(matrix.d0, matrix.d1, matrix.d2);

        if (this->m_fizzleTime) {
            float fade = (this->m_fizzleTime - now) * 0.0020000001f;
            float alpha;

            if (fade < 0.0f) {
                alpha = 0.0f;
            } else {
                alpha = 1.0f;

                if (fade < 1.0f) {
                    alpha = (this->m_fizzleTime - now) * 0.0020000001f;
                }
            }

            this->m_model->m_fadeAlpha = this->m_alpha * alpha;

            if (fade <= 0.0f) {
                this->BeginWaitingForModelFX(false);
                return;
            }
        }

        if (!this->m_mapEntity && (this->m_model->m_fFlags & 0x4000)) {
            this->m_mapEntity = World::ObjectCreate(this->m_model, &MissileObjectUpdateCallback, NULL, 0, reinterpret_cast<uintptr_t>(this), 6, __FILE__, __LINE__);
        }

        CAaBox box;
        CAaSphere sphere;

        if (this->m_model->IsLoaded()) {
            box = this->m_model->GetBoundingBox();
            sphere = this->m_model->GetBoundingSphere();

            if ((this->m_flags & 0x90000) == 0x10000) {
                for (uint32_t i = 0; ; i++) {
                    CM2Model* emitterModel = this->m_model;

                    if (!(emitterModel->m_fFlags & 1)) {
                        emitterModel->WaitForLoad(NULL);
                    }

                    if (i >= emitterModel->m_shared->m_data->m_particles.m_count) {
                        break;
                    }

                    emitterModel = this->m_model;

                    if (!(emitterModel->m_fFlags & 1)) {
                        emitterModel->WaitForLoad(NULL);
                    }

                    emitterModel->m_particleEmitters[i]->m_flags |= 0x400000;
                }

                this->m_flags |= 0x80000;
            }
        }

        C3Vector zero(0.0f, 0.0f, 0.0f);
        World::ObjectUpdate(this->m_mapEntity, matrix, box, sphere, zero, 0, 0xFFFFFFFF);
        this->m_model->SetAnimating(1);
    }

    if (SE3::IsPlaying(this->m_missileSound)) {
        C3Vector position = this->m_model ? this->m_model->GetWorldPosition() : this->m_position;
        SE3::Set3DPosition(this->m_missileSound, position);
    }
}

// PROVEN: c7d0addc IDENTICAL/NONE
// OFFSET: 0x00589910
CMissile* CMissile::NewMissile() {
    return CMissile::s_pool->New("", 0);
}

// OFFSET: 0x00589930
CMissile* CMissile::CopyMissile(const CMissile* source) {
    CMissile* missile = CMissile::s_pool->New("", 0);
    missile->m_casterGuid = source->m_casterGuid;
    missile->m_sourceGuid = source->m_sourceGuid;
    missile->m_targetGuid = source->m_targetGuid;
    missile->m_spellID = source->m_spellID;
    missile->m_spellVisualID = source->m_spellVisualID;
    missile->m_speed = source->m_speed;
    missile->m_baseSpeed = source->m_baseSpeed;
    missile->m_launchSpeed = source->m_launchSpeed;
    missile->m_position = source->m_position;
    missile->m_transportGuid = source->m_transportGuid;
    missile->m_impactPos = source->m_impactPos;
    missile->m_flags = source->m_flags;
    missile->m_attachmentID = source->m_attachmentID;
    missile->m_attachOffset = source->m_attachOffset;
    missile->m_soundKitID = source->m_soundKitID;
    missile->m_hitResult = source->m_hitResult;
    missile->m_missReason = source->m_missReason;
    missile->m_destroyTime = source->m_destroyTime;
    missile->m_victimAnimTime = source->m_victimAnimTime;
    missile->m_castTime = source->m_castTime;
    missile->m_duration = source->m_duration;
    missile->m_currentTarget = source->m_currentTarget;
    missile->m_scale = source->m_scale;
    missile->m_progress = source->m_progress;
    missile->m_horizontalProgress = source->m_horizontalProgress;
    missile->m_groundOffset = source->m_groundOffset;
    missile->m_followGroundRate = source->m_followGroundRate;
    missile->m_followGroundStart = source->m_followGroundStart;
    missile->m_motionFlags = source->m_motionFlags;
    missile->m_missileIndex = source->m_missileIndex;
    missile->m_missileCount = source->m_missileCount;
    missile->m_motionScriptFunction = source->m_motionScriptFunction;
    missile->m_initialDistance = source->m_initialDistance;
    missile->m_random[0] = source->m_random[0];
    missile->m_random[1] = source->m_random[1];
    missile->m_random[2] = source->m_random[2];
    missile->m_fizzleTime = source->m_fizzleTime;
    missile->m_model = World::GetScene()->DuplicateModel(source->m_model, 0);
    missile->m_targets.Set(source->m_targets.m_count, source->m_targets.m_data);
    missile->m_targetHitResults.Set(source->m_targetHitResults.m_count, source->m_targetHitResults.m_data);
    missile->m_targetMissReasons.Set(source->m_targetMissReasons.m_count, source->m_targetMissReasons.m_data);
    missile->m_targetPoints.Set(source->m_targetPoints.m_count, source->m_targetPoints.m_data);
    return missile;
}

// OFFSET: 0x00589BA0/0x1401F2770
void CMissile::FireMissiles(const C3Vector* position) {
    if (!this->m_currentTarget) {
        this->m_lastUpdateTime = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    }

    this->DetermineMissType();
    this->m_launchPos = *position;
    this->m_position = *position;
    this->Link(&CMissile::s_inFlightMissileList);

    if (this->m_flags & 0x10000) {
        this->InitializeTrajectory();
    }

    if (this->m_flags & 0x10) {
        this->m_attachmentID = 0;
    }

    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_targetGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (target && target->m_worldModel && target->m_worldModel->IsLoaded()) {
        if (!target->m_worldModel->HasAttachment(this->m_attachmentID)) {
            if (target->m_worldModel->HasAttachment(15)) {
                this->m_attachmentID = 15;
            } else {
                this->m_attachmentID = target->m_worldModel->HasAttachment(19) ? 19 : -1;
            }
        }
    } else {
        this->m_attachmentID = -1;
    }

    if (!this->m_targetGuid) {
        this->m_flags |= 0x20000;
    }

    this->m_progress = 0.0f;
    this->m_horizontalProgress = 0.0f;
    C3Vector impact = this->TransformPositionByTransport();
    float dz = impact.z - this->m_position.z;
    this->m_lastModelPos.x = this->m_position.x - (impact.x - this->m_position.x);
    this->m_lastModelPos.y = this->m_position.y - (impact.y - this->m_position.y);
    this->m_lastModelPos.z = this->m_position.z - dz;
    this->m_smoothDirection = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_fireTime = CGWorldFrame::s_currentWorldFrame->GetSceneTime();

    uint32_t bits = (CRandom::uint32_(g_rndSeed) & 0x7FFFFF) | 0x3F800000;
    this->m_random[0] = *reinterpret_cast<float*>(&bits) - 1.0f;
    bits = (CRandom::uint32_(g_rndSeed) & 0x7FFFFF) | 0x3F800000;
    this->m_random[1] = *reinterpret_cast<float*>(&bits) - 1.0f;
    bits = CRandom::uint32_(g_rndSeed);
    this->m_flags &= ~0x100000;
    bits = (bits & 0x7FFFFF) | 0x3F800000;
    this->m_random[2] = *reinterpret_cast<float*>(&bits) - 1.0f;

    this->UpdateImpactPos();
    this->CalcPosition(0);
    impact = this->TransformPositionByTransport();
    float iy = impact.y - this->m_position.y;
    float iz = impact.z - this->m_position.z;
    float ix = impact.x - this->m_position.x;
    this->m_initialDistance = sqrt(ix * ix + iz * iz + iy * iy);

    const SpellRec* spell = NULL;
    const SpellVisualRec* visual;

    if (this->m_spellID) {
        spell = g_spellDB.GetRecord(this->m_spellID);

        if (!spell) {
            return;
        }

        visual = g_spellVisualDB.GetRecord(spell->m_spellVisualID[0]);
    } else {
        visual = g_spellVisualDB.GetRecord(this->m_spellVisualID);

        if (!visual) {
            return;
        }
    }

    if (this->m_model) {
        if (this->m_model->m_attachParent) {
            this->m_model->DetachFromParent();
        }

        if (spell && (spell->m_attributesExE & 0x40000000)) {
            this->m_model->m_fFlags |= 4;
        }

        float distance;

        if (this->m_speed <= 2.3841858e-7f || this->m_currentTarget) {
            distance = this->m_initialDistance;
        } else {
            CGUnit_C* source = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_sourceGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (visual && visual->m_flags < 0) {
                if (source) {
                    C3Vector targetPos = this->TransformPositionByTransport();
                    float ox = targetPos.x - this->m_launchPos.x;
                    float oy = targetPos.y - this->m_launchPos.y;
                    float oz = targetPos.z - this->m_launchPos.z;
                    distance = sqrt(ox * ox + oy * oy + oz * oz);
                } else {
                    distance = this->m_initialDistance;
                }
            } else if (source) {
                C3Vector sourcePos = source->GetPosition();
                C3Vector targetPos = this->TransformPositionByTransport();
                float ox = targetPos.x - sourcePos.x;
                float oy = targetPos.y - sourcePos.y;
                float oz = targetPos.z - sourcePos.z;
                distance = sqrt(ox * ox + oy * oy + oz * oz);
            } else {
                distance = this->m_initialDistance;
            }

            float travelTime;
            float speed;

            if (!spell || !(spell->m_attributesExI & 0x10)) {
                speed = this->m_speed;
                travelTime = distance / speed;
            } else {
                travelTime = this->m_speed;

                if (target) {
                    if (distance < 10.0f) {
                        distance = 10.0f;
                    }

                    this->m_speed = distance / travelTime;
                } else if (distance > 2.3841858e-7f) {
                    this->m_speed = distance / travelTime;
                }

                speed = this->m_speed;
                this->m_baseSpeed = speed;
            }

            float remaining = travelTime - static_cast<float>(this->m_fireTime - this->m_castTime) * 0.001f;

            if (remaining <= 0.0f) {
                remaining = 0.0f;
            }

            float maxSpeed = speed * 2.0f;

            if (distance <= 2.3841858e-7f) {
                this->m_speed = maxSpeed;
                this->m_duration = 0;
            } else {
                if (remaining > 2.3841858e-7f && distance / remaining <= maxSpeed) {
                    maxSpeed = distance / remaining;
                }

                this->m_speed = maxSpeed;
                this->m_duration = static_cast<int32_t>(distance / this->m_speed * 1000.0f);
            }

            if (target && (this->m_flags & 0x50)) {
                uint32_t animId = (this->m_flags & 0x10) ? 24 : 30;
                M2SequenceInfo info;
                info.Clear();
                target->m_worldModel->GetSequenceInfo(animId, 0, info);
                int32_t delay = static_cast<int32_t>(remaining * 1000.0f) - (info.m_duration >> 1);

                if (delay > 0) {
                    this->m_victimAnimTime = delay + CGWorldFrame::s_currentWorldFrame->GetSceneTime();
                }
            }
        }

        if (!this->m_currentTarget) {
            if (distance <= 0.0f) {
                this->m_model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
            } else {
                float speedScale = 1.0f;
                M2SequenceInfo info;

                if (this->m_model->IsLoaded() && this->m_model->HasSequence(144) && (info.Clear(), this->m_model->GetSequenceInfo(144, 0, info), info.m_flags & 1)) {
                    speedScale = info.m_duration * 0.001 * this->m_speed / distance;
                }

                this->m_model->SetBoneSequence(0xFFFFFFFF, 144, 0, 0, speedScale, 1, 1, 0, 0, 0);
            }
        }
    }

    this->ClearTargetingEffects();

    if (visual && visual->m_missileTargetingKit > 0) {
        const SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_missileTargetingKit);

        if (kit) {
            CGUnit_C* unit = target;

            if (!unit && this->m_casterGuid && !this->m_missileIndex) {
                unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_casterGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
            }

            if (unit && CanPlaySpellVisualKit(this->m_casterGuid.m_guid, unit->m_obj->m_guid.m_guid, kit, visual)) {
                this->m_targetingEffectTime = -1;
                this->m_targetingEffectTime = unit->NextVisualKitSequence();

                if (!target) {
                    C3Vector targetPos;
                    WOWGUID transport;
                    this->ResolveTargetPosition(&targetPos, &transport);
                    C3Vector orientation(0.0f, 0.0f, 1.0f);
                    int32_t hasOrientation;

                    if (visual->m_flags & 0x400) {
                        orientation = this->unk_0074;
                        hasOrientation = 1;
                    } else {
                        hasOrientation = 0;
                    }

                    PlaySpellVisuaKitData data(spell, kit, 7);
                    data.m_position = &targetPos;
                    data.m_transportGUID = transport;
                    data.m_someGUID = this->m_casterGuid;
                    data.unk_0038 = this->m_targetingEffectTime;
                    data.m_orientation = hasOrientation ? &orientation : NULL;
                    data.m_animParam = 0;
                    unit->PlaySpellVisualKit(data);
                } else {
                    PlaySpellVisuaKitData data(spell, kit, 2);
                    data.m_someGUID = this->m_casterGuid;
                    data.unk_0038 = this->m_targetingEffectTime;
                    unit->PlaySpellVisualKit(data);
                }

                this->m_targetingUnit = unit->m_obj->m_guid;
            }
        }
    }

    if (!SE3::IsPlaying(this->m_missileSound) && this->m_soundKitID) {
        SE3SoundKitProperties properties;
        properties.m_usage = 1;
        properties.m_maxAudibleBehavior = 1;
        properties.m_bit0 = 1;
        properties.m_noRandomStart = 1;
        properties.m_checkRange = 0;
        properties.m_fadeInTime = 500;
        properties.m_priority = 128;

        if (this->m_casterGuid == CGUnit_C::s_activeMover || this->m_targetGuid == CGUnit_C::s_activeMover) {
            properties.m_priority = 110;
        }

        SE3::PlaySoundKit(this->m_soundKitID, &this->m_position, &properties, &this->m_missileSound, __FILE__, __LINE__, "CMissile::FireMissiles");
    }
}

// OFFSET: 0x0058A510
int32_t CMissile::PossiblyHandleBounce() {
    if (!(this->m_flags & 0x200)) {
        return 0;
    }

    int32_t fired = 0;
    uint32_t index = ++this->m_currentTarget;

    if (index < this->m_targets.m_count) {
        this->m_targetGuid.m_guid = this->m_targets.m_data[index];
        this->m_flags &= 0xF7FFFA04;
        this->FireMissiles(&this->m_position);
        fired = 1;
    }

    if (this->m_currentTarget >= this->m_targetPoints.m_count) {
        return fired;
    }

    SpellCastTargetPoint* point = &this->m_targetPoints.m_data[this->m_currentTarget];
    this->m_impactPos = point->m_position;
    this->m_transportGuid = point->m_transport;
    this->m_flags &= 0xF7FFFA04;
    this->FireMissiles(&this->m_position);
    return 1;
}

// OFFSET: 0x0058A5D0
void CMissile::Reflect() {
    this->m_flags |= 0x40000000;
    this->ClearTargetingEffects();
    WOWGUID target = this->m_targetGuid;
    C3Vector launchPos = this->m_launchPos;
    float launchSpeed = this->m_launchSpeed;
    this->m_targetGuid = this->m_casterGuid;
    this->m_casterGuid = target;
    this->m_sourceGuid = target;
    C3Vector impactPos = this->m_impactPos;
    this->m_impactPos = launchPos;
    this->m_launchPos = impactPos;
    this->m_speed = launchSpeed;
    this->m_baseSpeed = launchSpeed;
    C3Vector impact = this->TransformPositionByTransport();
    float dz = impact.z - this->m_position.z;
    this->m_lastModelPos.x = this->m_position.x - (impact.x - this->m_position.x);
    this->m_lastModelPos.y = this->m_position.y - (impact.y - this->m_position.y);
    this->m_lastModelPos.z = this->m_position.z - dz;
    this->m_smoothDirection = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_flags &= ~0x100000;

    if ((this->m_flags & 0x1024000) == 0x20000 && this->m_duration) {
        uint32_t now = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
        float progress = 1.0f - this->m_progress;
        this->m_progress = progress;
        this->m_horizontalProgress = progress;
        this->m_attachmentID = -1;
        this->m_fireTime = now - static_cast<int64_t>(static_cast<float>(this->m_duration) * progress);
        this->UpdateImpactPos();

        if (this->m_progress < 1.0f) {
            C3Vector targetPos = this->TransformPositionByTransport();
            float t = this->m_progress;
            float x = this->m_position.x - targetPos.x * t;
            float y = this->m_position.y - targetPos.y * t;
            float z = this->m_position.z - targetPos.z * t;
            float inv = 1.0f / (1.0f - t);
            this->m_launchPos = C3Vector(inv * x, inv * y, inv * z);
        }
    }

    this->CalcPosition(0);

    if (this->m_flags & 8) {
        this->CalcMissFade();
    }
}

// PROVEN: ab4fab00 IDENTICAL/NONE
// OFFSET: 0x0058A840/0x1401F37D0
void CMissile::DeleteSelf() {
    if (this->m_missileCollision) {
        this->m_missileCollision->Free();
    }

    CMissile::s_pool->Delete(this);
}

// OFFSET: 0x0058A890
void CMissile::HitTarget(CGUnit_C* target, const SpellRec* spell, const SpellVisualRec* visual) {
    if (this->m_flags & 0x40000000) {
        this->BeginWaitingForModelFX(false);
        return;
    }

    if (target) {
        if (target->m_unit->m_unitFieldHealth > 0) {
            if (!visual || !(visual->m_flags & 0x40)) {
                target->PlayWoundAnimKit(0);
            }
        } else {
            target->PlayDeathAnim(0);
        }

        if (spell && !this->m_fizzleTime && CanPlaySpellVisual(this->m_casterGuid.m_guid, 0, visual)) {
            CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_casterGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
            const SpellVisualKitRec* kit = Spell_C_GetAppropriateImpactKit(visual, target == caster);
            WOWGUID transport = WOWGUID();
            C3Vector position(0.0f, 0.0f, 0.0f);
            C3Vector* positionPtr = NULL;

            if (this->m_model && (this->m_flags & 0x8000)) {
                position = (this->m_model->m_fFlags & 0x4000) ? this->m_model->GetWorldPosition() : target->GetPosition();
                positionPtr = &position;
            } else if ((this->m_motionFlags & 1) && (this->m_motionFlags & 8)) {
                C3Vector resolved;
                position = *this->ResolveTargetPosition(&resolved, &transport);
                positionPtr = &position;
            }

            if (kit && CanPlaySpellVisualKit(this->m_casterGuid.m_guid, target->m_obj->m_guid.m_guid, kit, NULL)) {
                PlaySpellVisuaKitData data(spell, kit, 1);
                data.m_transportGUID = transport;
                data.m_position = positionPtr;
                data.m_someGUID = this->m_casterGuid;
                target->PlaySpellVisualKit(data);
            }

            const SpellVisualKitRec* stateKit = g_spellVisualKitDB.GetRecord(visual->m_stateKit);

            if (stateKit && target->m_unit->m_unitFieldHealth > 0 && target->HasAura(spell->m_id, this->m_casterGuid) && CanPlaySpellVisualKit(this->m_casterGuid.m_guid, target->m_obj->m_guid.m_guid, stateKit, NULL) && (static_cast<int8_t>(stateKit->m_flags) >= 0 || target->HasAuraActive(spell->m_id, this->m_casterGuid))) {
                PlaySpellVisuaKitData data(spell, stateKit, 2);
                data.m_someGUID = this->m_casterGuid;
                target->PlaySpellVisualKit(data);
            }
        }

        if (this->m_attachmentID != -1 && target->GetObjectModel() && (this->m_flags & 4) && this->AttachToVictim(target, 5000)) {
            return;
        }
    }

    if (!this->PossiblyHandleBounce()) {
        this->BeginWaitingForModelFX(false);
    }
}

// OFFSET: 0x0058AB30
void CMissile::MissedTarget(CGUnit_C* target, const C3Vector* direction) {
    if (target) {
        if (!(this->m_flags & 0x100)) {
            const SpellRec* spell = g_spellDB.GetRecord(this->m_spellID);

            if (spell && !(spell->m_attributes & 0x100) && !this->m_missileIndex) {
                target->AddWorldText(this->m_casterGuid, this->GetHitResult(), spell, 0);
                CGGameUI::ShowSpellMissFeedback(target->m_obj->m_guid, this->GetHitResult());
                UnitCombatLogSpellMissed(this->m_casterGuid, target->m_obj->m_guid, spell->m_id, this->GetHitResult(), 0, 0, 0);
            }

            this->m_flags |= 0x100;
        }

        uint32_t flags = this->m_flags;

        if (flags & 0x10) {
            if (this->AttachToVictim(target, 5000)) {
                this->PossiblyHandleBounce();
                return;
            }

            if (!this->PossiblyHandleBounce()) {
                this->BeginWaitingForModelFX(false);
            }

            return;
        }

        if (flags & 0x20) {
            CEffect* effect = CEffect::Create();
            effect->AddSpecialByPosition(10, this->m_position, target->m_obj->m_guid, 32, &CGObject_C::EffectOnceHandler);
            effect->m_flags |= 0x8000;
            C44Matrix matrix = this->m_model ? this->m_model->m_worldTransform : this->GetPositionMatrix();
            effect->m_facing = atan2(matrix.b1, matrix.b0) - 1.5707964f;
            effect->Release();

            if (!this->PossiblyHandleBounce()) {
                this->BeginWaitingForModelFX(false);
            }

            return;
        }
    }

    uint32_t flags = this->m_flags;

    if (!(flags & 8)) {
        if (flags & 0x40) {
            C3Vector dir(-direction->x, -direction->y, -direction->z);
            float length = sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

            if (fnotequal(length, 0.0f) && fnotequal(length, fabs(dir.z))) {
                dir.x = 1.0f / length * dir.x;
                dir.y = 1.0f / length * dir.y;
                dir.z = 1.0f / length * dir.z;
                C33Matrix first = C33Matrix::Rotation(CRandom::sfloat_(g_rndSeed) * 1.5707964f, dir, true);
                C3Vector side(dir.y, -dir.x, 0.0f);
                side *= first;
                C33Matrix second = C33Matrix::Rotation(CRandom::sfloat_(g_rndSeed) * 1.5707964f, side, false);
                dir *= second;
                float speed = this->m_speed * 0.5f;
                this->m_speed = speed;
                float reach = speed * 0.5f;
                this->m_impactPos.x = this->m_position.x + reach * dir.x;
                this->m_impactPos.y = this->m_position.y + reach * dir.y;
                this->m_impactPos.z = this->m_position.z + reach * dir.z;
                this->m_transportGuid = WOWGUID();

                if (this->m_model) {
                    this->m_model->SetBoneSequence(0xFFFFFFFF, 198, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
                }

                this->m_flags = (this->m_flags & ~0x48) | 8;
                this->PossiblyHandleBounce();
                return;
            }
        } else if (flags & 0x80) {
            this->m_sourceGuid = this->m_targetGuid;
            this->m_targetGuid = this->m_casterGuid;
            this->m_flags = (flags & 0xFFFFFA7F) | 0x400;
            uint32_t reason = this->GetMissReason();
            this->SetHitResult(reason);

            if (!reason) {
                this->m_flags |= 1;
            }

            this->m_speed = this->m_launchSpeed;
            this->m_baseSpeed = this->m_launchSpeed;
            this->FireMissiles(&this->m_position);
            return;
        }
    }

    if (!this->PossiblyHandleBounce()) {
        this->BeginWaitingForModelFX(false);
    }
}

// OFFSET: 0x0058AFC0/0x1401F4320
void CMissile::ReachedTarget(CGUnit_C* target, const C3Vector* direction) {
    this->m_progress = 1.0f;
    this->m_horizontalProgress = 1.0f;
    this->m_position = this->TransformPositionByTransport();
    C3Vector dir = *direction;
    float length = sqrt(direction->z * direction->z + direction->y * direction->y + direction->x * direction->x);

    if (fabs(length) < 2.3841858e-7f) {
        dir = C3Vector(0.0f, 0.0f, 1.0f);
    } else {
        dir.x = 1.0f / length * dir.x;
        dir.y = dir.y * (1.0f / length);
        dir.z = dir.z * (1.0f / length);
    }

    if (this->m_model && (this->m_model->m_fFlags & 0x4000)) {
        C44Matrix matrix;
        this->ComputeModelMatrix(&matrix, &this->m_position, &dir);
        this->m_model->m_fFlags |= 0x4000;
        this->m_model->m_worldTransform = matrix;
        this->m_lastModelPos = C3Vector(matrix.d0, matrix.d1, matrix.d2);
    }

    SE3::StopSound(this->m_missileSound, 150, __FILE__, __LINE__, "CMissile::ReachedTarget");
    this->m_flags &= ~0x10000;
    const SpellRec* spell = g_spellDB.GetRecord(this->m_spellID);

    if (!spell) {
        if (target && this->IsHit()) {
            this->HitTarget(target, NULL, NULL);
            return;
        }

        this->MissedTarget(target, direction);
        return;
    }

    const SpellVisualRec* visual = GetSpellVisual(spell, static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_casterGuid, TYPEMASK_UNIT, __FILE__, __LINE__)));

    if (target) {
        if (this->IsHit()) {
            this->HitTarget(target, spell, visual);
            return;
        }

        this->MissedTarget(target, direction);
        return;
    }

    if (!this->m_targetGuid) {
        if (visual) {
            this->HitTargetAreaEffect(spell, visual);
            return;
        }

        this->MissedTarget(target, direction);
        return;
    }

    if (this->m_flags & 0x40000000) {
        this->BeginWaitingForModelFX(false);
        return;
    }

    if (!this->PossiblyHandleBounce()) {
        this->BeginWaitingForModelFX(false);
    }
}

// PROVEN: 29187e7b IDENTICAL/NONE
// OFFSET: 0x0058B260/0x1401F4760
void CMissile::RemoveMissiles() {
    while (CMissile::s_inFlightMissileList) {
        CMissile::s_inFlightMissileList->DeleteSelf();
    }

    while (CMissile::s_missileDestroyList) {
        CMissile::s_missileDestroyList->DeleteSelf();
    }
}

// OFFSET: 0x0058B320
int32_t CMissile::WaitForModelFX() {
    if (this->m_model && this->m_model->IsLoaded() && (this->m_model->m_fFlags & 0x4000)) {
        CM2Model* model = this->m_model;

        if (this->m_flags & 0x800000) {
            if (this->m_flags & 0x10000000) {
                model->m_baseAlpha = 0.0f;
                model->SetEmittersEnabled(0);
                model->SetRibbonsEnabled(0);
                this->m_flags &= ~0x10000000;
            }
        } else {
            if (!(this->m_flags & 0x10000000)) {
                model->m_baseAlpha = 0.0f;
                model->SetEmittersEnabled(0);
                model->SetRibbonsEnabled(0);
            }

            this->m_flags |= 0x800000;
        }

        if (this->m_model->m_scene->m_frameStamp - this->m_model->m_frameStamp < 2 && (this->m_model->HasLiveParticles() || this->m_model->HasLiveRibbons())) {
            return 1;
        }
    }

    this->DeleteSelf();
    return 0;
}

// OFFSET: 0x0058B430/0x1401F4970
void CMissile::UpdateAttachments(WOWGUID guid) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    CM2Model* unitModel = unit->m_worldModel;

    for (CMissile* missile = CMissile::s_missileDestroyList; missile; ) {
        CMissile* next = missile->m_next;

        if (missile->m_targetGuid == guid && missile->m_model && missile->m_model->m_attachParent) {
            if (unitModel && unitModel->IsLoaded() && unitModel->HasAttachment(missile->m_attachmentID)) {
                missile->m_model->DetachFromParent();
                missile->m_model->AttachToParent(unitModel, missile->m_attachmentID, NULL, 0);
                C44Matrix attach = unitModel->GetAttachmentWorldTransform(missile->m_attachmentID);
                missile->m_model->SetWorldTransform(missile->m_model->m_worldTransform * attach.AffineInverse(sqrt(attach.a1 * attach.a1 + attach.a2 * attach.a2 + attach.a0 * attach.a0)));
            } else {
                missile->DeleteSelf();
            }
        }

        missile = next;
    }
}

// PROVEN: 53adc96b IDENTICAL/NONE
// OFFSET: 0x0058B570
void CMissile::ClearInFlightMissiles(int32_t reflect, WOWGUID caster, int32_t spellID) {
    for (CMissile* missile = CMissile::s_inFlightMissileList; missile; ) {
        CMissile* next = missile->m_next;

        if ((!spellID || missile->m_spellID == spellID) && (!caster || missile->m_casterGuid == caster)) {
            if (reflect) {
                missile->Reflect();
            } else {
                missile->DeleteSelf();
            }
        }

        missile = next;
    }
}

// PROVEN: d45801f8 IDENTICAL/NONE
// OFFSET: 0x0058B610
void CMissile::ClearInFlightMissilesWithTarget(WOWGUID target) {
    for (CMissile* missile = CMissile::s_inFlightMissileList; missile; ) {
        CMissile* next = missile->m_next;

        if (missile->m_targetGuid == target) {
            missile->DeleteSelf();
        }

        missile = next;
    }
}

// OFFSET: 0x0058B690
void CMissile::HandleMissileTrajectoryCollision(const C3Vector* direction) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_UPDATE_PROJECTILE_POSITION));
    msg.Put(this->m_casterGuid.m_guid);
    msg.Put(this->m_spellID);
    msg.Put(this->m_castID);
    msg.Put(this->m_position.x);
    msg.Put(this->m_position.y);
    msg.Put(this->m_position.z);
    msg.Finalize();
    ClientServices::Send2(&msg);
    this->m_impactPos = this->m_position;
    this->m_transportGuid = WOWGUID();
    this->ReachedTarget(NULL, direction);
}

// PROVEN: 18217234 IDENTICAL/NONE
// OFFSET: 0x0058B7B0/0x1401F4DC0
void CMissile::ScriptManagerCallback(uint32_t event) {
    if (event == 1) {
        CMissile::RemoveMissiles();
    }
}

// OFFSET: 0x0058B7D0
void CMissile::UpdateStaticLists() {
    for (CMissile* missile = CMissile::s_inFlightMissileList; missile; ) {
        CMissile* next = missile->m_next;
        missile->CalcPosition(CGWorldFrame::s_currentWorldFrame->m_elapsedSec > 0.0f);

        if (missile->m_flags & 8) {
            missile->CalcMissFade();
        }

        missile = next;
    }

    for (CMissile* missile = CMissile::s_missileDestroyList; missile; ) {
        CMissile* next = missile->m_next;
        missile->WaitForModelFX();
        missile = next;
    }
}

// OFFSET: 0x0058B850/0x1401F4E80
void CMissile::Initialize() {
    void* pool = SMemAlloc(sizeof(TSObjectAllocPool<CMissile>), __FILE__, __LINE__, 0);
    CMissile::s_pool = pool ? new (pool) TSObjectAllocPool<CMissile>("CMissile", 64) : NULL;
    CSimpleScriptManager::RegisterCallback(CMissile::ScriptManagerCallback);
}

// OFFSET: 0x0058B8E0
void CMissile::Destroy() {
    CSimpleScriptManager::UnregisterCallback(CMissile::ScriptManagerCallback);
    delete CMissile::s_pool;
    CMissile::s_pool = NULL;
}

// INLINED
C44Matrix CMissile::GetPositionMatrix() const {
    C44Matrix matrix;
    matrix.a0 = 1.0f;
    matrix.a1 = 0.0f;
    matrix.a2 = 0.0f;
    matrix.a3 = 0.0f;
    matrix.b0 = 0.0f;
    matrix.b2 = 0.0f;
    matrix.b3 = 0.0f;
    matrix.c0 = 0.0f;
    matrix.c1 = 0.0f;
    matrix.c3 = 0.0f;
    matrix.b1 = 1.0f;
    matrix.c2 = 1.0f;
    matrix.d0 = this->m_position.x;
    matrix.d1 = this->m_position.y;
    matrix.d2 = this->m_position.z;
    matrix.d3 = 1.0f;
    return matrix;
}
