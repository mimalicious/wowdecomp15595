#include "Vehicle_C.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/ObjectClient/AnimKitManager.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/Object/Passenger.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C9F32C/0x140B36CD8
static const char* s_vehicleSeatTypes[3] = { "None", "Root", "Child" };

// STATIC: 0x00DD82B4/0x140CBE9F8
TSObjectAllocPool<CVehicle_C>* CVehicle_C::s_vehiclePool;

// STATIC: 0x00DD82B8/0x140CBEA00
static int32_t s_inBoneAnimEnd;

// PROVEN: 98aec1b5 IDENTICAL/IDENTICAL
// OFFSET: 0x005E99E0/0x14026AA00
int32_t CVehicle_C::CanPlayBoneAnim() const {
    return this->m_unit->GetObjectModel() != NULL;
}

// PROVEN: 91856d3a IDENTICAL/IDENTICAL
// OFFSET: 0x005E9A00/0x14026AA20
uint32_t CVehicle_C::IsBoneAnimPlaying(uint32_t bone) const {
    if (bone == 26) {
        return 0;
    }

    if (bone >= 35) {
        bone = 26;
    }

    return this->m_playingBoneAnims[bone >> 5] & (1 << (bone & 0x1F));
}

// PROVEN: 22662041 IDENTICAL/DIFFERENT
// OFFSET: 0x005E9A40/0x14026AA50
int32_t CVehicle_C::InstallBoneAnimListener(WOWGUID guid, uint32_t boneId, void (*callback)(CGUnit_C* unit, int32_t& a2, TSGrowableArray<CVehiclePassenger_C*>& passengers)) {
    if (boneId >= 35) {
        boneId = 26;
    }

    uint32_t i = 0;

    while (this->m_boneAnimListeners[i].m_guid) {
        if (++i >= 16) {
            return 0;
        }
    }

    this->m_boneAnimListeners[i].m_guid = guid;
    this->m_boneAnimListeners[i].m_boneId = boneId;
    this->m_boneAnimListeners[i].m_callback = callback;
    return 1;
}

// OFFSET: 0x005E9AA0/0x14026AAA0
void CVehicle_C::UninstallBoneAnimListener(WOWGUID guid) {
    for (uint32_t i = 0; i < 16; i++) {
        if (this->m_boneAnimListeners[i].m_guid == guid) {
            this->m_boneAnimListeners[i].m_guid = WOWGUID();
        }
    }
}

// PROVEN: 381dbad4 IDENTICAL/IDENTICAL
// OFFSET: 0x005E9AE0/0x14026AAD0
void CVehicle_C::SetBoneAnimForceFacing(WOWGUID guid, uint32_t boneId) {
    if (boneId >= 35) {
        boneId = 26;
    }

    this->m_forceFacingGuid = guid;
    this->m_forceFacingBone = boneId;
}

// PROVEN: e5aa3cc4 IDENTICAL/IDENTICAL
// OFFSET: 0x005E9B10/0x14026AAF0
bool CVehicle_C::IsMirroringAnimsToPassengers() const {
    return this->m_mirroringAnimsMask != 0;
}

// PROVEN: fd2420d8 IDENTICAL/IDENTICAL
// OFFSET: 0x005E9B20/0x14026AB00
void CVehicle_C::OnPassengerStartMirroringAnims(uint8_t seat) {
    this->m_mirroringAnimsMask |= 1 << seat;
}

// PROVEN: 539d4178 IDENTICAL/IDENTICAL
// OFFSET: 0x005E9B40/0x14026AB20
void CVehicle_C::OnPassengerStopMirroringAnims(uint8_t seat) {
    this->m_mirroringAnimsMask &= ~(1 << seat);
}

// PROVEN: 241f9d65 IDENTICAL/UNPROVEN
// OFFSET: 0x005E9B60/0x14026AB40
VehicleSeatRec* CVehicle_C::GetVehicleSeatRec(uint8_t seat) const {
    if (seat >= 8 || !this->m_vehicleRec) {
        return NULL;
    }

    return g_vehicleSeatDB.GetRecord(this->m_vehicleRec->m_seatID[seat]);
}

// PROVEN: c0951723 IDENTICAL/IDENTICAL
// OFFSET: 0x005E9BA0/0x14026AB80
int32_t CVehicle_C::IsSuppressingCameraPitchWhileMouseAiming() const {
    if (!(this->m_unit->m_movement->m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) && this->m_vehicleRec && (this->m_vehicleRec->m_flags & 0x40040000) == 0x40000) {
        return 1;
    }

    return 0;
}

// PROVEN: d4f9da60 IDENTICAL/IDENTICAL
// OFFSET: 0x005E99C0/0x14026A9E0
int32_t CVehicle_C::HasHighPriorityCastingAnim() const {
    return this->m_vehicleRec && (this->m_vehicleRec->m_flags & 0x10000);
}

// OFFSET: 0x005E9BE0/0x14026ABC0
int32_t CVehicle_C::HasHighPriorityAnim() const {
    if (this->m_playingBoneAnims[0] & 0x4000000) {
        return 1;
    }

    if (!this->m_vehicleRec || !(this->m_vehicleRec->m_flags & 0x10000)) {
        return 0;
    }

    return 1;
}

// OFFSET: 0x005E9C10/0x14026ABE0
int32_t CVehicle_C::RecHasVehicleEnterAnim(const VehicleSeatRec* seatRec) {
    if (!seatRec || !(seatRec->m_flags & 0x400000)) {
        return 0;
    }

    if (static_cast<uint32_t>(seatRec->m_vehicleEnterAnim) > static_cast<uint32_t>(g_animationDataDB.m_maxID)) {
        return seatRec->m_vehicleEnterAnimKitID > 0;
    }

    return 1;
}

// OFFSET: 0x005E9C50/0x14026AC10
int32_t CVehicle_C::RecHasVehicleExitAnim(const VehicleSeatRec* seatRec, const CGUnit_C* unit) {
    if (!seatRec || !(seatRec->m_flags & (unit->IsVehicleExitVoluntary() ? 0x40000 : 0x80000))) {
        return 0;
    }

    if (static_cast<uint32_t>(seatRec->m_vehicleExitAnim) > static_cast<uint32_t>(g_animationDataDB.m_maxID)) {
        return seatRec->m_vehicleExitAnimKitID > 0;
    }

    return 1;
}

// OFFSET: 0x005E9CA0/0x14026AC70
CVehicle_C* CVehicle_C::FindVehicleToPlayBoneAnim(int32_t passengerAnim, const VehicleSeatRec* seatRec, CGUnit_C* passenger) {
    CGUnit_C* unit;

    if (passengerAnim && seatRec && (seatRec->m_flagsB & 0x20000000)) {
        if (!passenger || !passenger->m_vehiclePassenger || passenger->m_vehiclePassenger->m_state != 3 || !(unit = passenger->GetVehicleUnit())) {
            return NULL;
        }
    } else {
        unit = this->m_unit;
    }

    while (unit->m_vehiclePassenger && unit->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims()) {
        CGUnit_C* vehicleUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(unit->m_vehiclePassenger->m_transitionVehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!vehicleUnit) {
            break;
        }

        unit = vehicleUnit;
    }

    return unit->m_vehicle ? unit->m_vehicle : this;
}

// OFFSET: 0x005E9D60/0x14026AD30
const CVehicle_C* CVehicle_C::FindVehicleToPlayBoneAnim(int32_t passengerAnim, const VehicleSeatRec* seatRec, const CGUnit_C* passenger) const {
    return const_cast<CVehicle_C*>(this)->FindVehicleToPlayBoneAnim(passengerAnim, seatRec, const_cast<CGUnit_C*>(passenger));
}

// OFFSET: 0x005E9D70/0x14026AD40
void CVehicle_C::HandleBoneAnimGrabEvent(uint32_t boneId, uint32_t eventId) {
    if (boneId >= 35) {
        boneId = 26;
    }

    for (uint32_t i = 0; i < 16; i++) {
        BoneAnimListener& listener = this->m_boneAnimListeners[i];

        if (!listener.m_guid || listener.m_boneId != boneId) {
            continue;
        }

        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(listener.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->IsWaitingForVehicleTransitionAnimGrabEvent(eventId)) {
            unit->m_vehiclePassenger->StopWaitingForVehicleTransitionAnim();
            listener.m_guid = WOWGUID();
        }
    }
}

// OFFSET: 0x005E9E00/0x14026ADF0
void CVehicle_C::HandleBoneAnimThrowEvent(uint32_t boneId, uint32_t eventId) {
    if (boneId >= 35) {
        boneId = 26;
    }

    for (uint32_t i = 0; i < 16; i++) {
        BoneAnimListener& listener = this->m_boneAnimListeners[i];

        if (!listener.m_guid || listener.m_boneId != boneId) {
            continue;
        }

        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(listener.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->IsWaitingForVehicleTransitionAnimThrowEvent(eventId)) {
            unit->m_vehiclePassenger->StopWaitingForVehicleTransitionAnim();
            listener.m_guid = WOWGUID();
        }
    }
}

// PROVEN: 0510dd28 IDENTICAL/DIFFERENT
// OFFSET: 0x005E9E90/0x14026AEA0
void CVehicle_C::EnterWorld() {
    CGUnit_C::HideVehicleUISliders();
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_vehicle && player->m_vehicle->m_vehicleRec) {
        player->SignalPlayerGainsVehicleDataEvent();
    }
}

// PROVEN: 800bd2d3 IDENTICAL/IDENTICAL
// OFFSET: 0x005E9ED0/0x14026AEF0
void CVehicle_C::FreeAnimKits() {
    for (uint32_t i = 0; i < this->m_boneAnimKits.Count(); i++) {
        if (this->m_boneAnimKits[i]) {
            this->m_boneAnimKits[i]->SetStopCallback(NULL, 0);
            this->m_boneAnimKits[i]->Stop(1, NULL);
            this->m_boneAnimKits[i]->Release();
            this->m_boneAnimKits[i] = NULL;
        }
    }
}

// OFFSET: 0x005E9FD0/0x14026B030
void CVehicle_C::CleanUpBoneAnimsBeforeObjectModelChange() {
    if (!(this->m_flags & 0x1) || !this->m_unit->GetObjectModel()) {
        return;
    }

    this->m_playingBoneAnims[0] = 0;
    this->m_playingBoneAnims[1] = 0;

    for (uint32_t i = 0; i < 16; i++) {
        this->m_boneAnimListeners[i].m_guid = WOWGUID();
    }

    this->FreeAnimKits();
    this->m_flags &= ~0x1;
    this->m_forceFacingGuid = WOWGUID();
}

// OFFSET: 0x005EA030/0x14026B0A0
void CVehicle_C::EjectPassenger(WOWGUID guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_EJECT_PASSENGER));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x005EA0B0/0x14026B140
void CVehicle_C::BuildWorldMatrix(const C44Matrix* parent) {
    C44Matrix* matrix = &this->m_worldMatrix;
    matrix->d3 = 1.0f;
    matrix->c2 = 1.0f;
    matrix->b1 = 1.0f;
    matrix->a0 = 1.0f;
    matrix->d2 = 0.0f;
    matrix->d1 = 0.0f;
    matrix->d0 = 0.0f;
    matrix->c3 = 0.0f;
    matrix->c1 = 0.0f;
    matrix->c0 = 0.0f;
    matrix->b3 = 0.0f;
    matrix->b2 = 0.0f;
    matrix->b0 = 0.0f;
    matrix->a3 = 0.0f;
    matrix->a2 = 0.0f;
    matrix->a1 = 0.0f;

    C3Vector position = this->m_unit->GetRawPosition();
    matrix->d0 = position.x;
    matrix->d1 = position.y;
    matrix->d2 = position.z;
    matrix->RotateAroundZ(this->m_unit->GetRawFacing());

    if (parent) {
        *matrix *= *parent;
    }

    for (CPassenger* passenger = this->m_passengers.Head(); passenger; passenger = this->m_passengers.Next(passenger)) {
        WOWGUID guid = *passenger->m_ownerGuid;
        uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);

        if ((high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(guid.m_guid)))) {
            CVehicle_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__))->m_vehicle;

            if (vehicle && vehicle->m_vehicleRec) {
                vehicle->BuildWorldMatrix(matrix);
            }
        }
    }
}

// OFFSET: 0x005EA200/0x14026B2D0
void CVehicle_C::UpdateLargestPassengerBoundsRadius() {
    CVehicle_C* vehicle = this;

    while (true) {
        float previous = vehicle->m_largestPassengerBoundsRadius;
        vehicle->m_largestPassengerBoundsRadius = 0.0f;

        for (CPassenger* passenger = vehicle->m_passengers.Head(); passenger; passenger = vehicle->m_passengers.Next(passenger)) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
            CM2Model* model = unit->GetObjectModel();

            if (model && model->IsLoaded()) {
                float radius = model->GetBoundingSphere().r;
                CVehicle_C* unitVehicle = unit->m_vehicle;

                if (unitVehicle && unitVehicle->m_vehicleRec) {
                    radius = unitVehicle->m_largestPassengerBoundsRadius * 2.0f + radius;
                }

                if (radius > vehicle->m_largestPassengerBoundsRadius) {
                    vehicle->m_largestPassengerBoundsRadius = radius;
                }
            }
        }

        if (vehicle->m_largestPassengerBoundsRadius == previous) {
            break;
        }

        CGUnit_C* unit = vehicle->m_unit;
        CVehiclePassenger_C* vehiclePassenger = unit->m_vehiclePassenger;

        if (!vehiclePassenger || vehiclePassenger->m_state != 3) {
            break;
        }

        CGUnit_C* transport = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(unit->GetTransportGUID()), TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!transport) {
            break;
        }

        CVehicle_C* transportVehicle = transport->m_vehicle;

        if (!transportVehicle || !transportVehicle->m_vehicleRec) {
            break;
        }

        vehicle = transportVehicle;
    }
}

// OFFSET: 0x005EA340/0x14026B470
void CVehicle_C::BoneAnimEndHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time) {
    if (interrupted && s_inBoneAnimEnd) {
        return;
    }

    if (boneId >= 35) {
        boneId = 26;
    }

    TSGrowableArray<CVehiclePassenger_C*> passengers;
    int32_t handled = 0;

    for (int32_t i = 0; i < 16; i++) {
        BoneAnimListener& listener = this->m_boneAnimListeners[i];

        if (!listener.m_guid || listener.m_boneId != boneId) {
            continue;
        }

        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(listener.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));
        int32_t keep = 0;

        if (unit && listener.m_callback && (listener.m_callback(unit, keep, passengers), keep)) {
            handled = 1;
        } else {
            listener.m_guid = WOWGUID();
        }
    }

    if (interrupted || !handled) {
        this->m_playingBoneAnims[boneId >> 5] &= ~(1 << (boneId & 0x1F));
    }

    if (boneId == this->m_forceFacingBone) {
        this->m_forceFacingGuid = WOWGUID();
        this->m_forceFacingBone = 0xFFFFFFFF;
    }

    if (!interrupted && handled) {
        s_inBoneAnimEnd = 0;
    }

    for (uint32_t i = 0; i < passengers.Count(); i++) {
        passengers[i]->StopWaitingForVehicleTransitionAnim();
    }
}

// PROVEN: f2ded582 IDENTICAL/IDENTICAL
// OFFSET: 0x005EA480/0x14026B610
void CVehicle_C::StopVehicleBoneAnimKit(uint32_t boneId) {
    if (boneId >= this->m_boneAnimKits.Count() || !this->m_boneAnimKits[boneId]) {
        return;
    }

    this->m_boneAnimKits[boneId]->SetStopCallback(NULL, 0);
    this->m_boneAnimKits[boneId]->Stop(1, NULL);
    this->m_boneAnimKits[boneId]->Release();
    this->m_boneAnimKits[boneId] = NULL;
    this->BoneAnimEndHandler(this->m_unit->GetObjectModel(), boneId, 0xFFFFFFFF, 1, 0);
}

// OFFSET: 0x005EA580/0x14026B760
void CVehicle_C::VehicleBoneAnimKitStopCallback(CAnimKitInst* kit, uint64_t param) {
    CVehicle_C* vehicle = reinterpret_cast<CVehicle_C*>(static_cast<intptr_t>(param));
    uint32_t count = vehicle->m_boneAnimKits.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (vehicle->m_boneAnimKits[i] == kit) {
            kit->Release();
            vehicle->m_boneAnimKits[i] = NULL;
            vehicle->BoneAnimEndHandler(vehicle->m_unit->GetObjectModel(), i, 0xFFFFFFFF, 1, 0);
            return;
        }
    }
}

// OFFSET: 0x005EA610/0x14026B820
void CVehicle_C::StartMirroringAnimsToPassengers() {
    for (CPassenger* passenger = this->m_passengers.Head(); passenger; passenger = this->m_passengers.Next(passenger)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims()) {
            unit->m_vehiclePassenger->StartMirroringVehicleAnims(unit->m_vehiclePassenger->MirrorsVehicleAnimTier());
        }
    }
}

// OFFSET: 0x005EA690/0x14026B8D0
void CVehicle_C::UpdateSeatVacancy() {
    this->m_freeSeatMask = 0;

    if (!this->m_vehicleRec) {
        return;
    }

    uint32_t occupied = 0;

    for (CPassenger* passenger = this->m_passengers.Head(); passenger; passenger = this->m_passengers.Next(passenger)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            occupied |= 1 << static_cast<uint8_t>(unit->m_movementData.m_transportSeat);
        }
    }

    VehicleRec* rec = this->m_vehicleRec;

    if (rec->m_seatID[0] && !(occupied & 0x1)) {
        this->m_freeSeatMask |= 0x1;
    }

    if (rec->m_seatID[1] && !(occupied & 0x2)) {
        this->m_freeSeatMask |= 0x2;
    }

    if (rec->m_seatID[2] && !(occupied & 0x4)) {
        this->m_freeSeatMask |= 0x4;
    }

    if (rec->m_seatID[3] && !(occupied & 0x8)) {
        this->m_freeSeatMask |= 0x8;
    }

    if (rec->m_seatID[4] && !(occupied & 0x10)) {
        this->m_freeSeatMask |= 0x10;
    }

    if (rec->m_seatID[5] && !(occupied & 0x20)) {
        this->m_freeSeatMask |= 0x20;
    }

    if (rec->m_seatID[6] && !(occupied & 0x40)) {
        this->m_freeSeatMask |= 0x40;
    }

    if (rec->m_seatID[7] && !(occupied & 0x80)) {
        this->m_freeSeatMask |= 0x80;
    }
}

// OFFSET: 0x005EA7A0/0x14026B9B0
int32_t CVehicle_C::GetVirtualSeatCount() const {
    if (!this->m_vehicleRec) {
        return 0;
    }

    int32_t count = this->m_vehicleRec->m_seatID[0] > 0;

    if (this->m_vehicleRec->m_seatID[1] > 0) {
        count++;
    }

    if (this->m_vehicleRec->m_seatID[2] > 0) {
        count++;
    }

    if (this->m_vehicleRec->m_seatID[3] > 0) {
        count++;
    }

    if (this->m_vehicleRec->m_seatID[4] > 0) {
        count++;
    }

    if (this->m_vehicleRec->m_seatID[5] > 0) {
        count++;
    }

    if (this->m_vehicleRec->m_seatID[6] > 0) {
        count++;
    }

    if (this->m_vehicleRec->m_seatID[7] > 0) {
        count++;
    }

    for (CPassenger* passenger = const_cast<CVehicle_C*>(this)->m_passengers.Head(); passenger; passenger = const_cast<CVehicle_C*>(this)->m_passengers.Next(passenger)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
        CVehicle_C* vehicle = unit->m_vehicle;

        if (vehicle && vehicle->m_vehicleRec && !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            count += vehicle->GetVirtualSeatCount() - 1;
        }
    }

    return count;
}

// OFFSET: 0x005EA870/0x14026BA80
int32_t CVehicle_C::GetVehicleAndSeatFromVirtualSeatsRemaining(int32_t& remaining, const CVehicle_C*& vehicle, uint8_t& seat) const {
    if (!this->m_vehicleRec) {
        return 0;
    }

    CGUnit_C* seatUnits[8] = {};

    for (CPassenger* passenger = const_cast<CVehicle_C*>(this)->m_passengers.Head(); passenger; passenger = const_cast<CVehicle_C*>(this)->m_passengers.Next(passenger)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
        CVehicle_C* unitVehicle = unit->m_vehicle;

        if (unitVehicle && unitVehicle->m_vehicleRec && !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            seatUnits[static_cast<uint8_t>(unit->m_movementData.m_transportSeat)] = unit;
        }
    }

    for (uint32_t i = 0; i < 8; i++) {
        if (seatUnits[i]) {
            if (seatUnits[i]->m_vehicle->GetVehicleAndSeatFromVirtualSeatsRemaining(remaining, vehicle, seat)) {
                return 1;
            }
        } else if (this->m_vehicleRec->m_seatID[i] > 0) {
            if (!remaining) {
                vehicle = this;
                seat = static_cast<uint8_t>(i);
                return 1;
            }

            remaining--;
        }
    }

    return 0;
}

// OFFSET: 0x005EA9A0/0x14026BBF0
CGUnit_C* CVehicle_C::FindPassengerUnit(uint8_t seat) const {
    for (CPassenger* passenger = const_cast<CVehicle_C*>(this)->m_passengers.Head(); passenger; passenger = const_cast<CVehicle_C*>(this)->m_passengers.Next(passenger)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (static_cast<uint8_t>(unit->m_movementData.m_transportSeat) == seat) {
            return unit;
        }
    }

    return NULL;
}

// OFFSET: 0x005EAA00/0x14026BC70
void CVehicle_C::RemoveAllPassengersLocally(int32_t force) {
    CGCamera* camera = CGWorldFrame::GetActiveCamera();
    uint32_t time = FrameTime::GetCurTimeMs();
    CPassenger* passenger = this->m_passengers.Head();

    while (passenger) {
        CPassenger* next = this->m_passengers.Next(passenger);

        if (camera->m_targetGUID == *passenger->m_ownerGuid) {
            camera->MakeRelativeTo(WOWGUID::Make(0));
        }

        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
        CVehiclePassenger_C* vehiclePassenger = unit->m_vehiclePassenger;

        if (!vehiclePassenger || !static_cast<bool>(vehiclePassenger->StopWaitingForVehicleTransitionAnim()) || force) {
            CMovement_C* movement = static_cast<CMovement_C*>(passenger);

            if (unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
                movement->OnFallLocal(time);
            } else {
                movement->TryStartFallingLocal();
            }

            unit->m_clientFlags |= 0x20000000;
            movement->ForceSetTransport(WOWGUID::Make(0), 0xFF, 1);
            unit->m_clientFlags &= ~0x20000000;
        }

        passenger = next;
    }
}

// OFFSET: 0x005EAB10
int32_t CVehicle_C::HasAPlayerAsPassenger() const {
    for (CPassenger* passenger = const_cast<CVehicle_C*>(this)->m_passengers.Head(); passenger; passenger = const_cast<CVehicle_C*>(this)->m_passengers.Next(passenger)) {
        WOWGUID guid = *passenger->m_ownerGuid;
        uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);

        if (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(guid.m_guid))) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x005EAB70/0x14026BE10
void CVehicle_C::UpdatePassengerSoundPositions() {
    for (CPassenger* passenger = this->m_passengers.Head(); passenger; passenger = this->m_passengers.Next(passenger)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            unit->UpdateSoundPosition();
        }
    }
}

// PROVEN: 00dd5431 IDENTICAL/UNPROVEN
// OFFSET: 0x005EAC40/0x14026BF90
void CVehicle_C::ReceiveVehicleRecIDFromServer(int32_t vehicleId) {
    this->m_vehicleRec = g_vehicleDB.GetRecord(vehicleId);
    this->UpdateSeatVacancy();
}

// OFFSET: 0x005EAC80/0x14026BFD0
void CVehicle_C::UpdateWorldMatrix() {
    if (!this->m_unit->GetTransportGUID()) {
        this->BuildWorldMatrix(NULL);
        return;
    }

    CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID::Make(this->m_unit->GetTransportGUID()), TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (transport) {
        C44Matrix transportMatrix = transport->GetMatrix();
        this->BuildWorldMatrix(&transportMatrix);
    }
}

// PROVEN: a5695ba4 IDENTICAL/DIFFERENT
// OFFSET: 0x005EACF0/0x14026C050
void CVehicle_C::StopBoneAnim(uint32_t boneId, WOWGUID guid) {
    if (!this->m_unit->GetObjectModel()) {
        return;
    }

    if (boneId >= 35) {
        boneId = 26;
    }

    this->UninstallBoneAnimListener(guid);
    uint32_t i;

    for (i = 0; i < 16; i++) {
        if (this->m_boneAnimListeners[i].m_guid && this->m_boneAnimListeners[i].m_boneId == boneId) {
            break;
        }
    }

    if (i == 16 && ((1 << (boneId & 0x1F)) & this->m_playingBoneAnims[boneId >> 5])) {
        this->StopVehicleBoneAnimKit(boneId);
    }
}

// PROVEN: 2f5ccded IDENTICAL/IDENTICAL
// OFFSET: 0x005EADB0/0x14026C0F0
int32_t CVehicle_C::GetVehicleAndSeatFromVirtualSeat(int32_t virtualSeat, const CVehicle_C*& vehicle, uint8_t& seat) const {
    return this->GetVehicleAndSeatFromVirtualSeatsRemaining(virtualSeat, vehicle, seat);
}

// PROVEN: 570387bc IDENTICAL/DIFFERENT
// OFFSET: 0x005EADD0/0x14026C110
int32_t CVehicle_C::AddPassenger(CPassenger* passenger) {
    if (!(passenger->unk_002C & 0x1)) {
        return 0;
    }

    passenger->m_link.Unlink();
    this->m_passengers.LinkToTail(passenger);
    return 1;
}

// OFFSET: 0x005EAE30/0x14026C180
void CVehicle_C::PlayVehicleBoneAnimKit(uint32_t type, uint8_t seat, uint32_t& bone, uint32_t anim, int32_t animKitID) {
    uint32_t count = this->m_boneAnimKits.Count();
    uint32_t requested = bone;
    uint32_t slot;

    for (slot = 0; slot < count; slot++) {
        if (!this->m_boneAnimKits[slot]) {
            break;
        }
    }

    if (slot == count) {
        if (count < 35) {
            this->m_boneAnimKits.SetCount(count + 1);
            this->m_boneAnimKits[slot] = NULL;
        } else {
            slot = 0;
        }
    }

    bone = slot;
    CAnimKitInst*& kit = this->m_boneAnimKits[slot];

    if (kit) {
        kit->Stop(1, NULL);
    }

    AnimKitExtraPlayParams params;
    params.boneSetID = -1;
    params.startTime = 0;
    params.startFraction = 0.0f;
    params.oneShot = 0;
    params.blendTimeMode = 0;
    params.blendTime = 0;

    if (animKitID > 0) {
        if (type != 1) {
            params.oneShot = 1;
        }

        CAnimKitManager* manager = this->m_unit->m_mountAnimKitManager ? this->m_unit->m_mountAnimKitManager : &this->m_unit->m_animKitManager;
        kit = manager->PlayAndMaintain(animKitID, -1, &params);
    } else {
        if (requested != 26) {
            params.boneSetID = CAnimKitManager::FindBestAnimKitBoneSetIDForBoneDataID(requested);
        }

        if (type == 1) {
            CAnimKitManager* manager = this->m_unit->m_mountAnimKitManager ? this->m_unit->m_mountAnimKitManager : &this->m_unit->m_animKitManager;
            kit = manager->PlayAndMaintain(requested == 26 ? 227 : 228, anim, &params);
        } else {
            params.oneShot = 1;
            CAnimKitManager* manager = this->m_unit->m_mountAnimKitManager ? this->m_unit->m_mountAnimKitManager : &this->m_unit->m_animKitManager;
            kit = manager->PlayAndMaintain(requested == 26 ? 225 : 226, anim, &params);
        }
    }

    if (kit) {
        kit->SetStopCallback(CVehicle_C::VehicleBoneAnimKitStopCallback, reinterpret_cast<intptr_t>(this));
    }
}

// OFFSET: 0x005EB070/0x14026C3E0
int32_t CVehicle_C::GetVirtualSeatInfo(int32_t virtualSeat, const char*& type, const char*& occupant, const char*& realm, int32_t& canEject, int32_t& canSwitch) const {
    const CVehicle_C* vehicle;
    uint8_t seat;

    if (!this->GetVehicleAndSeatFromVirtualSeatsRemaining(virtualSeat, vehicle, seat)) {
        return 0;
    }

    VehicleSeatRec* seatRec;

    if (seat >= 8 || !vehicle->m_vehicleRec || !(seatRec = g_vehicleSeatDB.GetRecord(vehicle->m_vehicleRec->m_seatID[seat]))) {
        return 0;
    }

    if (seatRec->m_flags & 0x800) {
        type = vehicle == this ? s_vehicleSeatTypes[1] : s_vehicleSeatTypes[2];
    } else {
        type = s_vehicleSeatTypes[0];
    }

    realm = NULL;
    CGUnit_C* passenger = vehicle->FindPassengerUnit(seat);

    if (passenger) {
        occupant = passenger->GetUnitName(&realm, 1);
    } else {
        occupant = NULL;
    }

    canEject = seatRec->m_flagsB & 0x20;
    canSwitch = (seatRec->m_flags >> 26) & 1;
    return 1;
}

// PROVEN: 714a3def IDENTICAL/DIFFERENT
// OFFSET: 0x005EB170/0x14026C530
CVehicle_C::~CVehicle_C() {
}

// PROVEN: 18fa6383 IDENTICAL/DIFFERENT
// OFFSET: 0x005EB200/0x14026C600
void CVehicle_C::UninitSystem() {
    if (s_vehiclePool) {
        delete s_vehiclePool;
        s_vehiclePool = NULL;
    }
}

// OFFSET: 0x005EB220/0x14026C690
int32_t CVehicle_C::PlayBoneAnim(uint32_t type, uint8_t seat, const VehicleSeatRec* seatRec, uint32_t& bone) {
    bone = 26;

    if (!this->m_unit->GetObjectModel()) {
        return 0;
    }

    int32_t animKitID;
    uint32_t anim;

    if (type == 0) {
        animKitID = seatRec->m_vehicleEnterAnimKitID;

        if (animKitID > 0) {
            anim = 0xFFFFFFFF;
        } else {
            bone = seatRec->m_vehicleEnterAnimBone;
            anim = seatRec->m_vehicleEnterAnim;
            animKitID = 0;
        }
    } else if (type == 1) {
        animKitID = seatRec->m_vehicleRideAnimKitID;

        if (animKitID > 0) {
            anim = 0xFFFFFFFF;
        } else {
            bone = seatRec->m_vehicleRideAnimLoopBone;
            anim = seatRec->m_vehicleRideAnimLoop;
            animKitID = 0;
        }
    } else if (type == 2) {
        animKitID = seatRec->m_vehicleExitAnimKitID;

        if (animKitID > 0) {
            anim = 0xFFFFFFFF;
        } else {
            bone = seatRec->m_vehicleExitAnimBone;
            anim = seatRec->m_vehicleExitAnim;
            animKitID = 0;
        }
    } else {
        return 0;
    }

    if (bone >= 35) {
        bone = 26;
    }

    this->PlayVehicleBoneAnimKit(type, seat, bone, anim, animKitID);
    this->m_playingBoneAnims[bone >> 5] |= 1 << (bone & 0x1F);
    this->m_flags |= 0x1;
    return 1;
}

// OFFSET: 0x005EB400/0x14026C880
void CVehicle_C::InitSystem() {
    delete s_vehiclePool;
    void* pool = SMemAlloc(sizeof(TSObjectAllocPool<CVehicle_C>), __FILE__, __LINE__, 0);
    s_vehiclePool = pool ? new (pool) TSObjectAllocPool<CVehicle_C>("Vehicle", 16) : NULL;
}

// OFFSET: 0x005EB480/0x14026C980
CVehicle_C* CVehicle_C::Create(const CClientObjCreate* create, CGUnit_C* unit, int32_t vehicleId) {
    CVehicle_C* vehicle = s_vehiclePool->New("", 0);
    vehicle->m_unit = unit;
    vehicle->m_flags = 0;

    if (create) {
        int32_t id = 0;
        float facing = 0.0f;

        if (create->m_hasVehicle) {
            id = create->m_vehicleId;
            facing = create->m_vehicleOrientation;
        }

        vehicle->m_vehicleRec = g_vehicleDB.GetRecord(id);
        vehicle->m_worldMatrix.Translate(create->m_move.m_status.m_position);
        vehicle->m_worldMatrix.RotateAroundZ(create->m_move.m_status.m_facing);
        vehicle->m_rawFacing = facing;
    } else {
        vehicle->m_vehicleRec = g_vehicleDB.GetRecord(vehicleId);
        vehicle->m_worldMatrix.Translate(unit->GetPosition());
        vehicle->m_worldMatrix.RotateAroundZ(unit->GetFacing());
        vehicle->m_rawFacing = unit->GetRawFacing();
    }

    vehicle->m_largestPassengerBoundsRadius = 0.0f;
    vehicle->UpdateSeatVacancy();
    vehicle->m_playingBoneAnims[0] = 0;
    vehicle->m_playingBoneAnims[1] = 0;

    for (uint32_t i = 0; i < 16; i++) {
        vehicle->m_boneAnimListeners[i].m_guid = WOWGUID();
    }

    vehicle->m_forceFacingGuid = WOWGUID();
    vehicle->m_forceFacingBone = 0xFFFFFFFF;
    vehicle->m_mirroringAnimsMask = 0;
    return vehicle;
}

// PROVEN: 62acb291 IDENTICAL/DIFFERENT
// OFFSET: 0x005EB5E0/0x14026CB30
void CVehicle_C::Destroy(int32_t removePassengers) {
    if (removePassengers) {
        this->RemoveAllPassengersLocally(0);
    }

    this->FreeAnimKits();
    CVehicle_C::s_vehiclePool->Delete(this);
}
