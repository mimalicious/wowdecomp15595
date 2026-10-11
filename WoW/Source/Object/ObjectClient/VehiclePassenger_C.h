#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_VEHICLEPASSENGER_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_VEHICLEPASSENGER_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"

class VehicleRec;
class CAnimKitInst;
class CDataStore;
class CM2Model;
class C44Matrix;
class CGUnit_C;
class CVehicle_C;
class VehicleSeatRec;

struct VehiclePassengerRescueTransition {
    /* 0000 */ WOWGUID m_passengerGuid;
    /* 0008 */ WOWGUID m_vehicleGuid;
    /* 0010 */ uint8_t m_seat;
    /* 0011 */ uint8_t m_instant;
};

struct VehicleAnimDeferredSpline {
    /* 0000 */ CDataStore* m_msg;
    /* 0008 */ WOWGUID m_transport;
    /* 0010 */ uint8_t m_seat;
};

class CVehiclePassenger_C {
    public:
    static TSObjectAllocPool<CVehiclePassenger_C>* s_passengerPool;
    static TSGrowableArray<CVehiclePassenger_C*>* s_pendingPassengers;
    static uint32_t s_addingPendingRescueTransitions;
    static CVehiclePassenger_C* s_headPassenger;
    static CVehiclePassenger_C* s_tailPassenger;
    static TSGrowableArray<VehiclePassengerRescueTransition>* s_rescueTransitions;

    static void ExecutePendingRescueTransitions();
    static void StartAddingPendingRescueTransitions();
    static void OnCancelControlEnd();
    static void InitSystem();
    static void OnReloadUI();
    static void UninitSystem();
    static void PostRenderUpdateAll();
    static void UpdateAll(uint32_t time);
    static void OnVehicleRideLoopAnimEnd(CGUnit_C* unit, int32_t& done, TSGrowableArray<CVehiclePassenger_C*>& passengers);
    static void OnVehicleTransitionAnimEnd(CGUnit_C* unit, int32_t& done, TSGrowableArray<CVehiclePassenger_C*>& passengers);
    static void AddPendingRescueTransition(WOWGUID passengerGuid, WOWGUID vehicleGuid, uint8_t seat, int32_t instant);
    static CVehiclePassenger_C* Alloc(CGUnit_C* unit);
    static void Init(CGUnit_C* unit);
    static void SendTransitionUIEvents(WOWGUID guid, WOWGUID vehicleGuid, const VehicleRec* vehicleRec, const VehicleSeatRec* seatRec, int32_t fromState, int32_t toState);
    static void SignalTransitionUIEvents(WOWGUID guid, WOWGUID vehicleGuid, const VehicleRec* vehicleRec, const VehicleSeatRec* seatRec, uint32_t events);
    static void OnSetActiveMover(CGUnit_C* unit);

    /* 0000 */ uint32_t m_handle;
    /* 0004 */ CVehiclePassenger_C* m_prev;
    /* 0008 */ CVehiclePassenger_C* m_next;
    /* 000C */ CGUnit_C* m_unit;
    /* 0010 */ uint32_t m_flags;
    /* 0014 */ uint32_t m_state;
    /* 0018 */ uint32_t m_prevState;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ WOWGUID m_vehicleGuid;
    /* 0028 */ WOWGUID m_transitionVehicleGuid;
    /* 0030 */ uint8_t m_seat;
    /* 0031 */ uint8_t m_transitionSeat;
    /* 0034 */ uint32_t m_startTime;
    /* 0038 */ uint32_t m_endTime;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ WOWGUID m_startRelativeGuid;
    /* 0048 */ float m_facingBlend;
    /* 004C */ uint32_t m_stopMovementTime;
    /* 0050 */ VehicleRec* m_vehicleRec;
    /* 0054 */ VehicleSeatRec* m_seatRec;
    /* 0058 */ WOWGUID m_rootVehicleGuid;
    /* 0060 */ TSGrowableArray<VehicleAnimDeferredSpline> m_deferredSplines;
    /* 0070 */ uint32_t m_transitionAnimTimeout;
    /* 0074 */ uint32_t unk_0074;
    /* 0078 */ WOWGUID m_transitionAnimVehicleGuid;
    /* 0080 */ WOWGUID m_boneAnimGuid;
    /* 0088 */ uint32_t m_boneAnimBone;
    /* 008C */ C3Vector m_startModelPosition;
    /* 0098 */ C3Vector m_startModelOffset;
    /* 00A4 */ C3Vector m_startPosition;
    /* 00B0 */ C3Vector m_startOffset;
    /* 00BC */ float m_startFacing;
    /* 00C0 */ float m_startTargetFacing;
    /* 00C4 */ float m_smoothFacing;
    /* 00C8 */ float m_targetFacing;
    /* 00CC */ float m_arcGravity;
    /* 00D0 */ float m_arcScale;
    /* 00D4 */ C3Vector m_seatOffset;
    /* 00E0 */ float m_lockedWorldFacing;
    /* 00E4 */ CAnimKitInst* m_animKit;
    /* 00E8 */ CAnimKitInst* m_seatAnimKit;
    /* 00EC */ C3Vector m_trainCarPosition;

    CVehiclePassenger_C();
    void ClearFlag8();
    void UpdateSmoothFacing();
    void LockWorldFacingNextFrame(float facing);
    int32_t GetLowerAnim(const VehicleSeatRec* seat) const;
    void StopAnimKit(int32_t resetUserFlags, int32_t playStopKit);
    void FreeAnimKit();
    uint32_t IsMirroringVehicleAnims() const;
    uint32_t MirrorsVehicleAnimTier() const;
    void UpdateBlend(uint32_t time, const VehicleSeatRec* seatRec);
    C3Vector ComputeStartWorldPosition(const C3Vector& offset, const C3Vector& fallback) const;
    void StopMovement();
    CGUnit_C* GetVehicleUnit() const;
    CGUnit_C* GetStoredRootVehicleUnit() const;
    void StartWaitingForVehicleTransitionAnim(const VehicleSeatRec* seatRec, int32_t entering, CVehicle_C* vehicle);
    int32_t IsWaitingForSameVehicleTransitionAnim(WOWGUID transport, uint8_t seat) const;
    int32_t IsWaitingForVehicleTransitionAnimGrabEvent(uint32_t eventId) const;
    int32_t IsWaitingForVehicleTransitionAnimThrowEvent(uint32_t eventId) const;
    void ComputeSeatOffset(const VehicleSeatRec* seat, CM2Model* model);
    float GetLockedWorldFacing() const;
    void OnAnimEnd(uint32_t anim);
    void PlayRideAnimKit(const VehicleSeatRec* seatRec, int32_t resetUserFlags, uint8_t seat);
    void UpdateAnimKitAfterObjectModelChange();
    void StopMirroringVehicleAnims(int32_t unblockExpressions);
    int32_t IsCurrentlyMirroringVehicleAnims() const;
    void UpdateVehicleAnimMirroring();
    void InitTrainCarPosition();
    float ComputeTrainCarRawFacing(const C3Vector& position, float facing);
    CVehicle_C* GetVehicle() const;
    void ComputeModelTransform(C44Matrix& matrix, CM2Model*& model, CGUnit_C* vehicle, const VehicleSeatRec* seat, uint32_t attachment, float facing);
    void ComputeSeatPos(CGUnit_C* vehicle, CM2Model* model, const VehicleSeatRec* seat, C3Vector& position, C3Vector* up);
    void PlayTransitionAnimKit(const VehicleSeatRec* seatRec, int32_t entering, uint8_t seat);
    void StartMirroringVehicleAnims(int32_t mirrorAnimTier);
    void SetState(uint32_t state, CGUnit_C* vehicle, uint8_t seat, const VehicleSeatRec* seatRec, uint32_t time, WOWGUID relativeGuid, int32_t a8);
    void StoreRootVehicle(WOWGUID guid);
    C3Vector GetEndPosition();
    void ComputeEndPos(CGUnit_C* vehicle, const VehicleSeatRec* seat, C3Vector& position, C3Vector* up);
    void ComputeEndValues(CGUnit_C* vehicle, const VehicleSeatRec* seatRec, uint32_t time, C3Vector& endPosition);
    void StartDeferringVehicleTransitionAnimSpline(CDataStore* msg, WOWGUID transport, uint8_t seat);
    void DeleteAllDeferredVehicleTransitionAnimSplines();
    int32_t ExecuteAllDeferredVehicleTransitionAnimSplines();
    void UpdateModel();
    int32_t PostSetState(CGUnit_C* vehicle, const VehicleSeatRec* seatRec, uint32_t time);
    int32_t UpdateState(CGUnit_C* vehicle, const VehicleSeatRec* seatRec, uint32_t time);
    int32_t StopWaitingForVehicleTransitionAnim();
    void Free();
    void Uninit();
    void Update(uint32_t time);
    void BeginTransition(WOWGUID vehicleGuid, uint8_t seat, uint8_t instant);
    int32_t CheckForVehicleTransitionAnimTimeout();
    void OnUpdateTransport(WOWGUID transport, uint8_t seat, CGUnit_C* vehicle, int32_t deferred);

    int32_t GetUpperAnim(const VehicleSeatRec* seat) const;
    C3Vector ComputeInterpolatedUnitPosition() const;
    int32_t IsInTransitionMotion() const;
    int32_t IsEntering() const;
};

#endif
