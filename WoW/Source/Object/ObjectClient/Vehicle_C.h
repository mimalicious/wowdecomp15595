#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_VEHICLE_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_VEHICLE_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/Object/Passenger.h"
#include "Engine/Source/Base/ObjectAlloc.h"

class CAnimKitInst;
class CM2Model;
class CClientObjCreate;
class CGUnit_C;
class CVehiclePassenger_C;
class VehicleRec;
class VehicleSeatRec;

class CVehicle_C {
    public:
    struct BoneAnimListener {
        /* 0000 */ WOWGUID m_guid;
        /* 0008 */ uint32_t m_boneId;
        /* 000C */ void (*m_callback)(CGUnit_C* unit, int32_t& a2, TSGrowableArray<CVehiclePassenger_C*>& passengers);
    };

    static TSObjectAllocPool<CVehicle_C>* s_vehiclePool;

    static void InitSystem();
    static void UninitSystem();
    static void EnterWorld();
    static void EjectPassenger(WOWGUID guid);
    static CVehicle_C* Create(const CClientObjCreate* create, CGUnit_C* unit, int32_t vehicleId);
    static int32_t RecHasVehicleEnterAnim(const VehicleSeatRec* seatRec);
    static int32_t RecHasVehicleExitAnim(const VehicleSeatRec* seatRec, const CGUnit_C* unit);
    static void VehicleBoneAnimKitStopCallback(CAnimKitInst* kit, uint64_t param);

    /* 0000 */ uint32_t m_handle;
    /* 0004 */ CGUnit_C* m_unit;
    /* 0008 */ uint32_t m_flags;
    /* 000C */ VehicleRec* m_vehicleRec;
    /* 0010 */ C44Matrix m_worldMatrix;
    /* 0050 */ float m_rawFacing;
    /* 0054 */ float m_largestPassengerBoundsRadius;
    /* 0058 */ uint32_t m_playingBoneAnims [2];
    /* 0060 */ BoneAnimListener m_boneAnimListeners [16];
    /* 0160 */ WOWGUID m_forceFacingGuid;
    /* 0168 */ uint32_t m_forceFacingBone;
    /* 016C */ TSCGrowableArray<CAnimKitInst*, 3> m_boneAnimKits;
    /* 0180 */ uint8_t m_mirroringAnimsMask;
    /* 0181 */ uint8_t m_freeSeatMask;
    /* 0184 */ TSExplicitList<CPassenger, offsetof(CPassenger, m_link)> m_passengers;

    CVehicle_C();
    ~CVehicle_C();
    VehicleSeatRec* GetVehicleSeatRec(uint8_t seat) const;
    int32_t InstallBoneAnimListener(WOWGUID guid, uint32_t boneId, void (*callback)(CGUnit_C* unit, int32_t& a2, TSGrowableArray<CVehiclePassenger_C*>& passengers));
    void UninstallBoneAnimListener(WOWGUID guid);
    void StopBoneAnim(uint32_t boneId, WOWGUID guid);
    void StopVehicleBoneAnimKit(uint32_t boneId);
    void SetBoneAnimForceFacing(WOWGUID guid, uint32_t boneId);
    bool IsMirroringAnimsToPassengers() const;
    void OnPassengerStartMirroringAnims(uint8_t seat);
    void OnPassengerStopMirroringAnims(uint8_t seat);
    int32_t IsSuppressingCameraPitchWhileMouseAiming() const;
    int32_t HasHighPriorityCastingAnim() const;
    int32_t HasHighPriorityAnim() const;
    void FreeAnimKits();
    void CleanUpBoneAnimsBeforeObjectModelChange();
    int32_t HasAPlayerAsPassenger() const;
    uint32_t IsBoneAnimPlaying(uint32_t bone) const;
    int32_t CanPlayBoneAnim() const;
    void BoneAnimEndHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time);
    int32_t GetVirtualSeatCount() const;
    int32_t GetVehicleAndSeatFromVirtualSeatsRemaining(int32_t& remaining, const CVehicle_C*& vehicle, uint8_t& seat) const;
    int32_t GetVehicleAndSeatFromVirtualSeat(int32_t virtualSeat, const CVehicle_C*& vehicle, uint8_t& seat) const;
    int32_t GetVirtualSeatInfo(int32_t virtualSeat, const char*& type, const char*& occupant, const char*& realm, int32_t& canEject, int32_t& canSwitch) const;
    CGUnit_C* FindPassengerUnit(uint8_t seat) const;
    void BuildWorldMatrix(const C44Matrix* parent);
    void UpdateWorldMatrix();
    void UpdatePassengerSoundPositions();
    void UpdateLargestPassengerBoundsRadius();
    void UpdateSeatVacancy();
    int32_t AddPassenger(CPassenger* passenger);
    void ReceiveVehicleRecIDFromServer(int32_t vehicleId);
    CVehicle_C* FindVehicleToPlayBoneAnim(int32_t passengerAnim, const VehicleSeatRec* seatRec, CGUnit_C* passenger);
    const CVehicle_C* FindVehicleToPlayBoneAnim(int32_t passengerAnim, const VehicleSeatRec* seatRec, const CGUnit_C* passenger) const;
    void StartMirroringAnimsToPassengers();
    void PlayVehicleBoneAnimKit(uint32_t type, uint8_t seat, uint32_t& bone, uint32_t anim, int32_t animKitID);
    int32_t PlayBoneAnim(uint32_t type, uint8_t seat, const VehicleSeatRec* seatRec, uint32_t& bone);
    void HandleBoneAnimGrabEvent(uint32_t boneId, uint32_t eventId);
    void HandleBoneAnimThrowEvent(uint32_t boneId, uint32_t eventId);
    void RemoveAllPassengersLocally(int32_t force);
    void Destroy(int32_t removePassengers);
};

#include "WoW/Source/Object/ObjectClient/Vehicle_C.inl"

#endif
