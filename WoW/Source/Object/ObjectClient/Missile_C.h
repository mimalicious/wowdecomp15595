#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_MISSILE_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_MISSILE_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/SoundInterface/SI3.h"

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
    static void UpdateStaticLists();
    static void RemoveMissiles();
    static CMissile* s_inFlightMissileList;
    static CMissile* s_missileDestroyList;
    static CMissile* NewMissile();
    static CMissile* CopyMissile(const CMissile* source);
    static void ClearInFlightMissilesWithTarget(WOWGUID target);
    static uint32_t* s_missileHeap;

    static void AdjustAllMissileTrajectoryDurations(uint8_t castID, uint32_t duration);
    static void Destroy();
    static void Initialize();
    static void OnRemoteMissileTrajectoryCollision(const MissileCollisionData& data);
    static void ScriptManagerCallback(uint32_t event);
    static void StartFizzling(WOWGUID caster, uint8_t castID, uint32_t spellID);
    static void UpdateAttachments(WOWGUID guid);

    /* 0000 */ WOWGUID m_casterGuid;
    /* 0008 */ WOWGUID m_targetGuid;
    /* 0010 */ WOWGUID m_transportGuid;
    /* 0018 */ CM2Model* m_model;
    /* 001C */ uint32_t m_spellID;
    /* 0020 */ uint8_t m_castID;
    /* 0021 */ uint8_t unk_0021[3];
    /* 0024 */ float m_speed;
    /* 0028 */ float m_baseSpeed;
    /* 002C */ C3Vector m_position;
    /* 0038 */ C3Vector m_launchPos;
    /* 0044 */ C3Vector m_impactPos;
    /* 0050 */ C3Vector m_worldImpactPos;
    /* 005C */ C3Vector m_lastModelPos;
    /* 0068 */ uint32_t m_flags;
    /* 006C */ int32_t m_attachmentID;
    /* 0070 */ C3Vector m_attachOffset;
    /* 007C */ int32_t m_soundKitID;
    /* 0080 */ uint32_t m_missileSound;
    /* 0084 */ uint32_t m_hitResult;
    /* 0088 */ uint32_t m_missReason;
    /* 008C */ uint32_t m_destroyTime;
    /* 0090 */ TSGrowableArray<uint64_t> m_targets;
    /* 00A0 */ TSGrowableArray<SpellCastTargetPoint> m_targetPoints;
    /* 00B0 */ TSGrowableArray<uint32_t> m_targetHitResults;
    /* 00C0 */ TSGrowableArray<uint32_t> m_targetMissReasons;
    /* 00D0 */ uint32_t m_victimAnimTime;
    /* 00D4 */ uint32_t m_castTime;
    /* 00D8 */ uint32_t m_duration;
    /* 00DC */ uint32_t m_lastUpdateTime;
    /* 00E0 */ uint32_t m_currentTarget;
    /* 00E4 */ float m_scale;
    /* 00E8 */ float m_progress;
    /* 00EC */ float m_horizontalProgress;
    /* 00F0 */ float m_groundOffset;
    /* 00F4 */ float m_followGroundRate;
    /* 00F8 */ float m_followGroundStart;
    /* 00FC */ uint32_t m_motionFlags;
    /* 0100 */ int32_t unk_0110;
    /* 0104 */ int32_t unk_0114;
    /* 0108 */ float m_pitch;
    /* 010C */ float m_trajectorySpeed;
    /* 0110 */ C3Vector m_velocity;
    /* 011C */ float m_travelTime;
    /* 0120 */ float m_timeScale;
    /* 0124 */ uint32_t m_arrivalTime;
    /* 0128 */ uint32_t m_adjustTime;
    /* 012C */ uint32_t m_adjustedDuration;
    /* 0130 */ float m_gravity;
    /* 0134 */ uint32_t m_fizzleTime;
    /* 0138 */ CMissileCollision* m_missileCollision;
    /* 013C */ C3Vector m_remoteCollisionPos;
    /* 0148 */ uintptr_t m_mapEntity;
    /* 014C */ uintptr_t m_mapEntity2;
    /* 0150 */ CMissile** m_prevLink;
    /* 0154 */ CMissile* m_next;
    /* 0158 */ uint32_t m_memHandle;
    /* 015C */ uint32_t unk_016C;
    /* 0160 */ WOWGUID m_targetingUnit;
    /* 0168 */ uint32_t m_targetingEffectTime;
    /* 016C */ int32_t m_motionScriptFunction;
    /* 0170 */ uint32_t m_fireTime;
    /* 0174 */ float m_initialDistance;
    /* 0178 */ float m_random[3];
    /* 0184 */ C3Vector m_smoothDirection;
    /* 0190 */ C3Vector m_lastTransportPos;
    /* 019C */ ChaserState m_transportOffset;
    /* 01B4 */ uint32_t unk_01C4;

    void AdjustTrajectoryDuration(uint32_t duration);
    void ApplyMotionScriptFunctionRotationAndScale(const SimpleScriptIO& io, C44Matrix& matrix);
    int32_t ApplyMotionScriptFunctionTranslation(const SimpleScriptIO& io, C44Matrix& matrix);
    int32_t CallMotionScriptFunction(SimpleScriptIO& io);
    void DetermineMissType();
    uint32_t GetHitResult() const;
    uint32_t GetMissReason() const;
    int32_t IsHit() const;
    void SetHitResult(uint32_t result);
    void SetRemoteCollisionPos(const C3Vector& position);
    C3Vector TransformPositionByTransport();
    void UpdateProgress(float distance);
    CMissile();
    ~CMissile();
    C44Matrix GetPositionMatrix() const;
    void BeginWaitingForModelFX(int32_t disableEffects);
    int32_t WaitForModelFX();
    C3Vector* ResolveTargetPosition(C3Vector* position, WOWGUID* transport);
    void Link(CMissile** link);
    int32_t AttachToVictim(CGUnit_C* victim, uint32_t duration);
    void ClearTargetingEffects();
    void SetReadyToFire(CMissile** list);
    void UpdateImpactPos();
    void CalcMissFade();
    void ApplyTargetTransportOffset(CGObject_C* target, C3Vector* position, float elapsed);
    void FollowGround(const C3Vector& direction, float distance);
    void FireMissiles(const C3Vector* position);
    int32_t PossiblyHandleBounce();
    void HitTarget(CGUnit_C* target, const SpellRec* spell, const SpellVisualRec* visual);
    void HitTargetAreaEffect(const SpellRec* spell, const SpellVisualRec* visual);
    void MissedTarget(CGUnit_C* target, const C3Vector* direction);
    void ReachedTarget(CGUnit_C* target, const C3Vector* direction);
    void HandleMissileTrajectoryCollision(const C3Vector* direction);
    void ComputeModelMatrix(C44Matrix* matrix, const C3Vector* position, const C3Vector* direction);
    void InitializeTrajectory();
    void InstallMotionScriptFunction(int32_t motionId);
    void CalcPosition(int32_t advance);
    void DeleteSelf();
};

#include "WoW/Source/Object/ObjectClient/Missile_C.inl"

#endif
