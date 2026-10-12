#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_MISSILE_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_MISSILE_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"

class C44Matrix;
class CGObject_C;
class CGUnit_C;
class CMapEntity;
class CMissileCollision;
class CM2Model;
struct SimpleScriptIO;

struct MissileCollisionData {
    /* 0000 */ WOWGUID m_casterGuid;
    /* 0008 */ uint8_t m_castID;
    /* 0009 */ uint8_t unk_0009[3];
    /* 000C */ C3Vector m_position;
};

struct ChaserState {
    /* 0000 */ C3Vector m_position;
    /* 000C */ C3Vector m_velocity;

    ChaserState();
    ChaserState(const C3Vector& position, const C3Vector& velocity);
    void ChaserImplementation(const C3Vector& target, float stiffness, float elapsed);
};

class CMissile {
    public:
    static TSObjectAllocPool<CMissile>* s_pool;
    static CMissile* s_inFlightMissileList;
    static CMissile* s_missileDestroyList;

    static void AdjustAllMissileTrajectoryDurations(uint8_t castID, uint32_t duration);
    static void ApplyMotionScriptFunctionRotationAndScale(const SimpleScriptIO& io, C44Matrix& matrix);
    static void ClearInFlightMissiles(int32_t reflect, WOWGUID caster, int32_t spellID);
    static void ClearInFlightMissilesWithTarget(WOWGUID target);
    static CMissile* CopyMissile(const CMissile* source);
    static void Destroy();
    static void Initialize();
    static CMissile* NewMissile();
    static void OnRemoteMissileTrajectoryCollision(const MissileCollisionData& data);
    static void RemoveMissiles();
    static void ScriptManagerCallback(uint32_t event);
    static void StartFizzling(WOWGUID caster, uint8_t castID, uint32_t spellID);
    static void UpdateAttachments(WOWGUID guid);
    static void UpdateStaticLists();

    /* 0000 */ uint32_t m_handle;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ WOWGUID m_casterGuid;
    /* 0010 */ WOWGUID m_sourceGuid;
    /* 0018 */ WOWGUID m_targetGuid;
    /* 0020 */ WOWGUID m_transportGuid;
    /* 0028 */ CM2Model* m_model;
    /* 002C */ uint32_t m_spellID;
    /* 0030 */ uint8_t m_castID;
    /* 0034 */ int32_t m_spellVisualID;
    /* 0038 */ float m_launchSpeed;
    /* 003C */ float m_speed;
    /* 0040 */ float m_baseSpeed;
    /* 0044 */ C3Vector m_position;
    /* 0050 */ C3Vector m_launchPos;
    /* 005C */ C3Vector m_impactPos;
    /* 0068 */ C3Vector m_worldImpactPos;
    /* 0074 */ C3Vector unk_0074;
    /* 0080 */ C3Vector m_lastModelPos;
    /* 008C */ uint32_t m_flags;
    /* 0090 */ int32_t m_attachmentID;
    /* 0094 */ C3Vector m_attachOffset;
    /* 00A0 */ int32_t m_soundKitID;
    /* 00A4 */ uint32_t m_missileSound;
    /* 00A8 */ uint32_t m_hitResult;
    /* 00AC */ uint32_t m_missReason;
    /* 00B0 */ uint32_t m_destroyTime;
    /* 00B4 */ TSGrowableArray<uint64_t> m_targets;
    /* 00C4 */ TSGrowableArray<SpellCastTargetPoint> m_targetPoints;
    /* 00D4 */ TSGrowableArray<uint32_t> m_targetHitResults;
    /* 00E4 */ TSGrowableArray<uint32_t> m_targetMissReasons;
    /* 00F4 */ uint32_t m_victimAnimTime;
    /* 00F8 */ uint32_t m_castTime;
    /* 00FC */ uint32_t m_duration;
    /* 0100 */ uint32_t m_lastUpdateTime;
    /* 0104 */ uint32_t m_currentTarget;
    /* 0108 */ float m_scale;
    /* 010C */ float m_alpha;
    /* 0110 */ float m_progress;
    /* 0114 */ float m_horizontalProgress;
    /* 0118 */ float m_groundOffset;
    /* 011C */ float m_followGroundRate;
    /* 0120 */ float m_followGroundStart;
    /* 0124 */ uint32_t m_motionFlags;
    /* 0128 */ int32_t m_missileIndex;
    /* 012C */ int32_t m_missileCount;
    /* 0130 */ float m_pitch;
    /* 0134 */ float m_trajectorySpeed;
    /* 0138 */ C3Vector m_velocity;
    /* 0144 */ float m_travelTime;
    /* 0148 */ float m_timeScale;
    /* 014C */ uint32_t m_arrivalTime;
    /* 0150 */ uint32_t m_adjustTime;
    /* 0154 */ uint32_t m_adjustedDuration;
    /* 0158 */ float m_gravity;
    /* 015C */ uint32_t m_fizzleTime;
    /* 0160 */ CMissileCollision* m_missileCollision;
    /* 0164 */ C3Vector m_remoteCollisionPos;
    /* 0170 */ uintptr_t m_mapEntity;
    /* 0174 */ uintptr_t m_mapEntity2;
    /* 0178 */ CMissile** m_prevLink;
    /* 017C */ CMissile* m_next;
    /* 0180 */ WOWGUID m_targetingUnit;
    /* 0188 */ uint32_t m_targetingEffectTime;
    /* 018C */ int32_t m_motionScriptFunction;
    /* 0190 */ uint32_t m_fireTime;
    /* 0194 */ float m_initialDistance;
    /* 0198 */ float m_random[3];
    /* 01A4 */ C3Vector m_smoothDirection;
    /* 01B0 */ C3Vector m_lastTransportPos;
    /* 01BC */ ChaserState m_transportOffset;
    /* 01D4 */ uint32_t unk_01D4;

    CMissile();
    ~CMissile();
    void AdjustTrajectoryDuration(uint32_t duration);
    void ApplyTargetTransportOffset(CGObject_C* target, C3Vector* position, float elapsed);
    int32_t ApplyMotionScriptFunctionTranslation(const SimpleScriptIO& io, C44Matrix& matrix);
    int32_t AttachToVictim(CGUnit_C* victim, uint32_t duration);
    void BeginWaitingForModelFX(bool disableEffects);
    void CalcMissFade();
    void CalcPosition(int32_t advance);
    int32_t CallMotionScriptFunction(SimpleScriptIO& io);
    void ClearTargetingEffects();
    void ComputeModelMatrix(C44Matrix* matrix, const C3Vector* position, const C3Vector* direction);
    void DeleteSelf();
    void DetermineMissType();
    void FireMissiles(const C3Vector* position);
    void FollowGround(const C3Vector& direction, float distance);
    uint32_t GetHitResult() const;
    uint32_t GetMissReason() const;
    C44Matrix GetPositionMatrix() const;
    void HandleMissileTrajectoryCollision(const C3Vector* direction);
    void HitTarget(CGUnit_C* target, const SpellRec* spell, const SpellVisualRec* visual);
    void HitTargetAreaEffect(const SpellRec* spell, const SpellVisualRec* visual);
    void InitializeTrajectory();
    void InstallMotionScriptFunction(int32_t motionId);
    int32_t IsHit() const;
    void Link(CMissile** link);
    void MissedTarget(CGUnit_C* target, const C3Vector* direction);
    int32_t PossiblyHandleBounce();
    void ReachedTarget(CGUnit_C* target, const C3Vector* direction);
    void Reflect();
    C3Vector* ResolveTargetPosition(C3Vector* position, WOWGUID* transport);
    void SetHitResult(uint32_t result);
    void SetReadyToFire(CMissile** list);
    void SetRemoteCollisionPos(const C3Vector& position);
    C3Vector TransformPositionByTransport();
    void UpdateImpactPos();
    void UpdateProgress(float distance);
    int32_t WaitForModelFX();
};

C3Vector* MissileFindGroundPosition(C3Vector* result, const C3Vector& position, C3Vector& normal);

#include "WoW/Source/Object/ObjectClient/Missile_C.inl"

#endif
