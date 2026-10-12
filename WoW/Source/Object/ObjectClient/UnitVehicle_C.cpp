#include "UnitVehicle_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/VehicleCamera_C.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/StaticSingleton.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/CameraManager.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/Event/FrameTime.h"

// STATIC: 0x00DD82BC/0x140CBEA08
VehicleSeatRec* CGUnit_C::s_vehiclePassengerCameraSeat;

// STATIC: 0x00DD82C0/0x140CBEA10
VehicleRec* CGUnit_C::s_vehiclePassengerCameraVehicle;

// STATIC: 0x00DD82C4/0x140CBEA18
uint32_t CGUnit_C::s_vehiclePassengerCameraActive;

// PROVEN: 0a74b264 IDENTICAL/IDENTICAL
// OFFSET: 0x005EB650/0x14026CB90
void CGUnit_C::CleanUpVehicleBoneAnimsBeforeObjectModelChange() {
    if (this->m_vehicle) {
        this->m_vehicle->CleanUpBoneAnimsBeforeObjectModelChange();
    }
}

// OFFSET: 0x005EB660/0x14026CBB0
int32_t CGUnit_C::VehiclePassengerIsTransitionAllowed(const VehicleSeatRec* seat, int32_t checkSeat) const {
    return (this->m_clientFlags & 0x20000000) || (checkSeat && seat && (seat->m_flagsB & 0x1000000));
}

// PROVEN: c208e849 IDENTICAL/IDENTICAL
// OFFSET: 0x005EB6A0/0x14026CBE0
void CGUnit_C::VehiclePassengerInitWorldCameraState() {
    CGUnit_C::s_vehiclePassengerCameraActive = 0;
    CGUnit_C::s_vehiclePassengerCameraVehicle = NULL;
    CGUnit_C::s_vehiclePassengerCameraSeat = NULL;
}

// PROVEN: 27292662 IDENTICAL/IDENTICAL
// OFFSET: 0x005EB6C0/0x14026CC00
void CGUnit_C::VehiclePassengerOnUninit() {
    this->m_vehiclePassenger = NULL;
    this->UpdateSwimmingStatus(FrameTime::GetCurTimeMs(), 0);
}

// PROVEN: 4c495b57 IDENTICAL/IDENTICAL
// OFFSET: 0x005EB6E0/0x14026CC30
void CGUnit_C::VehiclePassengerInit() {
    if (this->m_vehiclePassenger) {
        this->m_vehiclePassenger->Free();
        this->m_vehiclePassenger = NULL;
    }

    CVehiclePassenger_C::Init(this);
}

// OFFSET: 0x005EB730/0x14026CC70
int32_t CGUnit_C::HasVehicleTransport() const {
    uint32_t high = static_cast<uint32_t>(this->m_movementData.m_transportGuid.m_guid >> 32);
    return (high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(this->m_movementData.m_transportGuid.m_guid)));
}

// OFFSET: 0x005EB780/0x14026CCC0
bool CGUnit_C::IsVehiclePreventingTurning() const {
    uint32_t high = static_cast<uint32_t>(this->m_movementData.m_transportGuid.m_guid >> 32);
    uint32_t low = static_cast<uint32_t>(this->m_movementData.m_transportGuid.m_guid);

    if ((high & 0xF0F00000) != 0xF0500000 && ((high & 0xF0000000) || !((high & 0xF07FFFFF) | low))) {
        return false;
    }

    CGUnit_C* transport = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(static_cast<uint64_t>(high) << 32 | low), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!transport || !transport->m_vehicle) {
        return false;
    }

    VehicleSeatRec* seat = transport->m_vehicle->GetVehicleSeatRec(static_cast<uint8_t>(this->m_movementData.m_transportSeat));
    return seat && !(seat->m_flags & 0x400);
}

// PROVEN: 2b052137 IDENTICAL/NONE
// OFFSET: 0x005EB820
uint32_t CGUnit_C::IsStrafingDisabled() const {
    return this->m_movementData.m_flags2 & MOVEMENTFLAG2_NO_STRAFE;
}

// PROVEN: e749a9b4 IDENTICAL/NONE
// OFFSET: 0x005EB830
void CGUnit_C::SetVehicleExitVoluntary(int32_t enable) {
    this->m_movementData.SetVehicleExitVoluntary(enable);
}

// PROVEN: c4be65a1 IDENTICAL/IDENTICAL
// OFFSET: 0x005EB840/0x14026CD80
int32_t CGUnit_C::IsVehicleExitVoluntary() const {
    return this->m_movementData.m_flags2 & 0x20;
}

// PROVEN: 980400b0 IDENTICAL/IDENTICAL
// OFFSET: 0x005EB850/0x14026CD90
int32_t CGUnit_C::AddPassenger(CPassenger* passenger, int32_t a2) {
    if (!this->m_vehicle) {
        return 0;
    }

    return this->m_vehicle->AddPassenger(passenger);
}

// OFFSET: 0x005EB870/0x14026CDB0
int32_t CGUnit_C::PlayVehiclePassengerExitAnimEnd(const VehicleSeatRec* seat, uint32_t anim, int32_t a3) {
    if (!seat || !(seat->m_flags & 0x200000) || anim == 0xFFFFFFFF) {
        uint32_t flags = this->m_movement->m_flags;

        if (flags & MOVEMENTFLAG_FALLING) {
            this->PlayMovementAnimKit(2, ANIM_FALL, 0xFFFFFFFF, 0);

            if (!a3) {
                this->m_animationState |= 0x1000;
            }

            return 0;
        }

        if (flags & MOVEMENTFLAG_SWIMMING) {
            this->UpdateMovementAnimKit(0);
            return 0;
        }

        if ((flags & MOVEMENTFLAG_FLYING) || anim == 0xFFFFFFFF) {
            return 0;
        }
    }

    this->PlayMovementAnimKit(6, anim, 0xFFFFFFFF, 0);
    return 1;
}

// PROVEN: 46e65e08 IDENTICAL/IDENTICAL
// OFFSET: 0x005EB910/0x14026CE90
int32_t CGUnit_C::IsVehiclePassenger(WOWGUID guid) const {
    CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (vehicle) {
        for (const CGUnit_C* unit = this; unit; unit = unit->m_vehiclePassenger->GetVehicleUnit()) {
            if (unit == vehicle) {
                return 1;
            }

            if (!unit->m_vehiclePassenger) {
                break;
            }
        }
    }

    return 0;
}

// PROVEN: 41b5c631 IDENTICAL/DIFFERENT
// OFFSET: 0x005EB970/0x14026CF10
int32_t CGUnit_C::HasAPlayerAsPassenger() const {
    if (!this->m_vehicle) {
        return 0;
    }

    return this->m_vehicle->HasAPlayerAsPassenger();
}

// OFFSET: 0x005EB990/0x14026CF30
bool CGUnit_C::IsVehicleTransitionPreventingAction() const {
    if (!this->IsLocalClientControlled()) {
        return false;
    }

    CVehiclePassenger_C* passenger = this->m_vehiclePassenger;

    if (passenger && passenger->m_state && passenger->m_state != 3) {
        return true;
    }

    if (passenger && (passenger->m_flags & 0x200)) {
        return true;
    }

    CGUnit_C* charmer = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_unit->m_unitFieldCharmedby, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (charmer) {
        CVehiclePassenger_C* charmerPassenger = charmer->m_vehiclePassenger;

        if (!charmerPassenger || ((!charmerPassenger->m_state || charmerPassenger->m_state == 3) && !(charmerPassenger->m_flags & 0x200))) {
            return charmer->IsInVehicle() && (charmerPassenger->m_flags & 0x8);
        }

        return true;
    }

    if (!(this->m_unit->m_unitFieldFlags & 0x1000000) && this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return true;
    }

    return false;
}

// PROVEN: b7ebee25 IDENTICAL/IDENTICAL
// OFFSET: 0x005EBA70/0x14026D010
void CGUnit_C::OnDismissControlledVehicleLocal(uint32_t eventTime) {
    this->m_movementData.OnDismissControlledVehicleLocal(eventTime);
}

// PROVEN: f900f091 IDENTICAL/NONE
// OFFSET: 0x005EBA80
void CGUnit_C::OnChangeSeatsOnControlledVehicleLocal(uint32_t eventTime, WOWGUID transport, uint8_t seat) {
    this->m_movementData.OnChangeSeatsOnControlledVehicleLocal(eventTime, transport, seat);
}

// PROVEN: 09da943f IDENTICAL/IDENTICAL
// OFFSET: 0x005EBAB0/0x14026D020
int32_t CGUnit_C::ShouldUseVehicleSeatExitAnimation(const VehicleSeatRec* seat) const {
    return seat->m_flags & ((this->m_movementData.m_flags2 & 0x20) ? 0x8 : 0x8000);
}

// OFFSET: 0x005EBAE0/0x14026D040
uint32_t CGUnit_C::IsVehicleCurrentlyUnstoppable() const {
    VehicleRec* vehicle = this->m_vehicle ? this->m_vehicle->m_vehicleRec : NULL;

    if (!vehicle) {
        return 0;
    }

    return vehicle->m_flags & ((this->m_movement->m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) ? 0x100 : 0x80);
}

// PROVEN: 30092ff0 IDENTICAL/IDENTICAL
// OFFSET: 0x005EBB20/0x14026D080
void CGUnit_C::SignalPlayerGainsVehicleDataEvent() {
    VehicleRec* vehicleRec = this->m_vehicle ? this->m_vehicle->m_vehicleRec : NULL;
    WOWGUID guid = this->m_obj->m_guid;
    int32_t count;
    char** tokens = Script_GetTokenIDsFromGUID(guid, &count);

    for (int32_t i = 0; i < count; i++) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_GAINS_VEHICLE_DATA, "%s%d", tokens[i], vehicleRec ? vehicleRec->m_vehicleUiindicatorID : 0);
    }
}

// PROVEN: 7dd7d9c0 IDENTICAL/IDENTICAL
// OFFSET: 0x005EBBA0/0x14026D120
void CGUnit_C::SignalPlayerLosesVehicleDataEvent() {
    WOWGUID guid = this->m_obj->m_guid;
    int32_t count;
    char** tokens = Script_GetTokenIDsFromGUID(guid, &count);

    for (int32_t i = 0; i < count; i++) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_LOSES_VEHICLE_DATA, "%s", tokens[i]);
    }
}

// OFFSET: 0x005EBC00/0x14026D180
int32_t CGUnit_C::VehiclePassengerShouldWaitForVehicleTransitionAnim(uint64_t transport, uint8_t seat) const {
    if (transport == this->GetTransportGUID() && seat == this->m_movementData.m_transportSeat) {
        return 0;
    }

    uint32_t high = static_cast<uint32_t>(transport >> 32);
    int32_t entering = (high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(transport)));
    WOWGUID guid;

    if (entering) {
        guid.m_guid = transport;
    } else {
        if (!this->HasVehicleTransport()) {
            return 0;
        }

        guid.m_guid = this->GetTransportGUID();
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    CVehicle_C* vehicle = unit->m_vehicle;

    if (!vehicle) {
        return 0;
    }

    VehicleSeatRec* seatRec = vehicle->GetVehicleSeatRec(entering ? seat : this->m_movementData.m_transportSeat);

    if (!seatRec) {
        return 0;
    }

    const CVehicle_C* animVehicle = vehicle->FindVehicleToPlayBoneAnim(entering, seatRec, this);

    if (!animVehicle) {
        return 0;
    }

    if (!entering) {
        return CVehicle_C::RecHasVehicleExitAnim(seatRec, this) && animVehicle->CanPlayBoneAnim();
    }

    if (!CVehicle_C::RecHasVehicleEnterAnim(seatRec)) {
        return 0;
    }

    return animVehicle->CanPlayBoneAnim() ? 1 : 0;
}

// OFFSET: 0x005EBD40/0x14026D310
int32_t CGUnit_C::VehiclePassengerStartWaitingForVehicleTransitionAnim(uint64_t transport, uint8_t seat) {
    uint32_t high = static_cast<uint32_t>(transport >> 32);
    int32_t entering = (high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(transport)));
    WOWGUID guid;

    if (entering) {
        guid.m_guid = transport;
    } else {
        if (!this->HasVehicleTransport()) {
            return 0;
        }

        guid.m_guid = this->GetTransportGUID();
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    CVehicle_C* vehicle = unit->m_vehicle;

    if (!vehicle) {
        return 0;
    }

    VehicleSeatRec* seatRec = vehicle->GetVehicleSeatRec(entering ? seat : this->m_movementData.m_transportSeat);

    if (!seatRec) {
        return 0;
    }

    if (this->m_vehiclePassenger) {
        this->m_vehiclePassenger->StopWaitingForVehicleTransitionAnim();
    }

    CVehicle_C* animVehicle = vehicle->FindVehicleToPlayBoneAnim(entering, seatRec, this);

    if (!animVehicle) {
        return 0;
    }

    uint32_t bone;

    if (entering) {
        if (!CVehicle_C::RecHasVehicleEnterAnim(seatRec) || !animVehicle->PlayBoneAnim(0, seat, seatRec, bone)) {
            return 0;
        }

        animVehicle->InstallBoneAnimListener(this->m_obj->m_guid, bone, CVehiclePassenger_C::OnVehicleTransitionAnimEnd);

        if (seatRec->m_flagsB & 0x800000) {
            animVehicle->SetBoneAnimForceFacing(this->m_obj->m_guid, bone);
        }
    } else {
        if (!CVehicle_C::RecHasVehicleExitAnim(seatRec, this) || !animVehicle->PlayBoneAnim(2, this->m_movementData.m_transportSeat, seatRec, bone)) {
            return 0;
        }

        animVehicle->InstallBoneAnimListener(this->m_obj->m_guid, bone, CVehiclePassenger_C::OnVehicleTransitionAnimEnd);
    }

    if (!this->m_vehiclePassenger) {
        this->m_vehiclePassenger = CVehiclePassenger_C::Alloc(this);
    }

    this->m_vehiclePassenger->StartWaitingForVehicleTransitionAnim(seatRec, entering, animVehicle);
    return 1;
}

// OFFSET: 0x005EBF10/0x14026D550
int32_t CGUnit_C::VehiclePassengerStartWaitingForVehicleTransitionAnimFromSpline(CDataStore* msg, uint64_t transport, uint8_t seat) {
    CVehiclePassenger_C* passenger = this->m_vehiclePassenger;

    if (passenger && (passenger->m_flags & 0x200)) {
        if (passenger->IsWaitingForSameVehicleTransitionAnim(WOWGUID::Make(transport), seat)) {
            passenger->StartDeferringVehicleTransitionAnimSpline(msg, WOWGUID::Make(transport), seat);
            return 1;
        }

        this->m_vehiclePassenger->DeleteAllDeferredVehicleTransitionAnimSplines();
        this->m_vehiclePassenger->StopWaitingForVehicleTransitionAnim();
    }

    int32_t result = this->VehiclePassengerShouldWaitForVehicleTransitionAnim(transport, seat);

    if (!result) {
        return result;
    }

    this->m_movementData.FlushMoveQueue(0, 0);

    if (!this->m_vehiclePassenger) {
        this->m_vehiclePassenger = CVehiclePassenger_C::Alloc(this);
    }

    this->m_vehiclePassenger->StartDeferringVehicleTransitionAnimSpline(msg, WOWGUID::Make(transport), seat);

    if (!this->VehiclePassengerStartWaitingForVehicleTransitionAnim(transport, seat)) {
        this->m_vehiclePassenger->ExecuteAllDeferredVehicleTransitionAnimSplines();
    }

    return 1;
}

// OFFSET: 0x005EC000/0x14026D650
void CGUnit_C::VehiclePassengerUpdateWorldCameraState(VehicleRec* vehicleRec, VehicleSeatRec* seatRec, uint32_t startTime, uint32_t endTime) {
    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (!camera) {
        return;
    }

    uint32_t active = 0;

    if (this->m_vehiclePassenger && (this->m_vehiclePassenger->m_state == 1 || this->m_vehiclePassenger->m_state == 2 || this->m_vehiclePassenger->m_state == 3)) {
        active = 1;
    }

    if (CGUnit_C::s_vehiclePassengerCameraActive == active && CGUnit_C::s_vehiclePassengerCameraVehicle == vehicleRec && CGUnit_C::s_vehiclePassengerCameraSeat == seatRec) {
        return;
    }

    if (active && seatRec && seatRec->m_cameraModeID) {
        return;
    }

    int32_t resetMin = 1;
    int32_t resetMax = 1;
    uint32_t restoreZoom = camera->m_flags & 0x20;
    CGUnit_C::s_vehiclePassengerCameraActive = active;
    CGUnit_C::s_vehiclePassengerCameraVehicle = vehicleRec;
    CGUnit_C::s_vehiclePassengerCameraSeat = seatRec;
    float duration = static_cast<float>(endTime - startTime) * 0.001f;

    if (!active) {
        camera->SetVehicleZoomEnabled(0, duration, restoreZoom == 0);
        camera->SetDesiredVehicleYawOffset(0.0f, startTime, endTime);
        camera->SetDesiredVehiclePitchOffset(0.0f, startTime, endTime);
        camera->SetVehicleTrackingStyle(0);
    } else {
        if (seatRec && (seatRec->m_flags & 0x1000000)) {
            camera->SetVehicleZoomEnabled(1, duration, 1);
            restoreZoom = 0;
        } else if (seatRec && (seatRec->m_flagsB & 0x400)) {
            camera->SetVehicleZoomEnabled(0, duration, 0);
            uint32_t wasRestore = restoreZoom;
            int32_t hasMin = seatRec->m_flagsB & 0x4000;
            restoreZoom = 0;
            float current = camera->m_distance;
            float target = current;
            int32_t hasMax = seatRec->m_flagsB & 0x8000;
            float minDistance = hasMin ? seatRec->m_cameraSeatZoomMin : camera->GetDefaultZoomMinDistance();
            float maxDistance = hasMax ? seatRec->m_cameraSeatZoomMax : CGCamera::GetDefaultZoomMaxDistance();
            uint32_t flagsB = seatRec->m_flagsB;

            if (flagsB & 0x800) {
                float preferred = seatRec->m_cameraEnteringZoom;

                if (preferred > current && !(flagsB & 0x1000)) {
                    target = preferred;
                }

                if (target > preferred && !(flagsB & 0x2000)) {
                    target = preferred;
                }

                if (!hasMin && minDistance > preferred) {
                    minDistance = preferred;
                    hasMin = 1;
                }

                if (!hasMax && preferred > maxDistance) {
                    maxDistance = preferred;
                    hasMax = 1;
                }
            }

            if (hasMin) {
                resetMin = 0;
                camera->SetVehicleSeatZoomMinDistance(minDistance);

                if (minDistance > target) {
                    target = minDistance;
                }
            }

            if (hasMax) {
                resetMax = 0;
                camera->SetVehicleSeatZoomMaxDistance(maxDistance);

                if (target > maxDistance) {
                    target = maxDistance;
                }
            }

            if (fnotequalz(current, target, 0.001f)) {
                if (!wasRestore) {
                    camera->SetVehicleExitZoomDistance(current);
                }

                camera->ZoomToDistance(target, startTime, duration, 1);
            }
        } else {
            camera->SetVehicleZoomEnabled(0, duration, restoreZoom == 0);
        }

        camera->SetDesiredVehicleYawOffset(vehicleRec ? vehicleRec->m_cameraYawOffset : 0.0f, startTime, endTime);
        camera->SetDesiredVehiclePitchOffset(vehicleRec ? vehicleRec->m_cameraPitchOffset : 0.0f, startTime, endTime);

        if (seatRec && (seatRec->m_flags & 0x800) && (seatRec->m_flagsB & 0x80001)) {
            camera->SetVehicleTrackingStyle(1);
        } else {
            camera->SetVehicleTrackingStyle(2);
        }
    }

    if (restoreZoom) {
        camera->ZoomToDistance(camera->m_vehicleExitZoomDistance, startTime, duration, 1);
        camera->ClearVehicleExitZoomDistance();
    }

    if (resetMin) {
        camera->SetVehicleSeatZoomMinDistanceToDefault();
    }

    if (resetMax) {
        camera->SetVehicleSeatZoomMaxDistanceToDefault();
    }
}

// OFFSET: 0x005EC3C0/0x14026DA70
void CGUnit_C::VehiclePassengerUpdateWorldCameraStateInstantly() {
    CVehiclePassenger_C* passenger = this->m_vehiclePassenger;
    VehicleRec* vehicleRec = passenger ? passenger->m_vehicleRec : NULL;
    VehicleSeatRec* seatRec = passenger ? passenger->m_seatRec : NULL;
    uint32_t endTime = FrameTime::GetCurTimeMs();
    uint32_t startTime = FrameTime::GetCurTimeMs();
    this->VehiclePassengerUpdateWorldCameraState(vehicleRec, seatRec, startTime, endTime);
}

// OFFSET: 0x005EC400/0x14026DAE0
void CGUnit_C::OnVehicleCameraPossiblyUnneeded() {
    CVehicleCamera_C* camera = this->m_vehicleCamera;

    if (!camera || static_cast<int8_t>(camera->m_flags) < 0) {
        return;
    }

    CGCamera* active = CGWorldFrame::GetActiveCamera();

    if (active) {
        CVehicleCamera_C* current = active->m_vehicleCamera;

        while (current) {
            if (current == this->m_vehicleCamera) {
                return;
            }

            if (!current->m_nextCameraGuid) {
                break;
            }

            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(current->m_nextCameraGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (!unit) {
                break;
            }

            current = unit->m_vehicleCamera;
        }
    }

    this->DestroyVehicleCamera();
}

// PROVEN: 8f507382 IDENTICAL/IDENTICAL
// OFFSET: 0x005EC470/0x14026DB60
void CGUnit_C::ConvertVehicleCameraRelativeFacingToTransportRelativeFacing(float& facing) {
    if (this->m_vehicleCamera) {
        CGObject_C* relativeTo = ClntObjMgrObjectPtr(WOWGUID::Make(this->m_vehicleCamera->GetRelativeTo()), TYPEMASK_OBJECT, __FILE__, __LINE__);
        CVehicleCamera_C::ConvertFromRawToWorld(facing, relativeTo);
        facing = this->m_movement->GetRawFacing(facing);
    }
}

// OFFSET: 0x005EC4C0/0x14026DBD0
void CGUnit_C::HideVehicleUISliders() {
    if (ClntObjMgrGetActivePlayerObj()) {
        FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_ANGLE_SHOW, NULL);
        FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_POWER_SHOW, NULL);
    }
}

// OFFSET: 0x005EC500/0x14026DC20
CGUnit_C* CGUnit_C::FindRootVehicle(CGUnit_C* stop) {
    CGUnit_C* unit = this;

    if (this == stop && this->m_vehicle && this->m_vehicle->m_vehicleRec) {
        return this;
    }

    WOWGUID guid = { this->GetTransportGUID() };
    uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);

    if ((high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(guid.m_guid)))) {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit) {
            return NULL;
        }

        while (unit != stop) {
            guid.m_guid = unit->GetTransportGUID();
            high = static_cast<uint32_t>(guid.m_guid >> 32);

            if ((high & 0xF0F00000) != 0xF0500000 && ((high & 0xF0000000) || !((high & 0xF07FFFFF) | static_cast<uint32_t>(guid.m_guid)))) {
                break;
            }

            unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (!unit) {
                return NULL;
            }
        }

        return unit;
    }

    if (unit->m_vehicle && unit->m_vehicle->m_vehicleRec) {
        return unit;
    }

    return NULL;
}

// PROVEN: d7cf92a8 IDENTICAL/IDENTICAL
// OFFSET: 0x005EC600/0x14026DD50
void CGUnit_C::CreateVehicleData(const CClientObjCreate* create, int32_t vehicleId) {
    if (this->m_vehicle) {
        this->m_vehicle->ReceiveVehicleRecIDFromServer(vehicleId);
        return;
    }

    this->m_vehicle = CVehicle_C::Create(create, this, vehicleId);

    if ((this->m_modelFlags & 0x40000) && this->IsActivePlayer()) {
        this->SignalPlayerGainsVehicleDataEvent();
    }
}

// PROVEN: 09442e12 IDENTICAL/IDENTICAL
// OFFSET: 0x005EC660/0x14026DDB0
void CGUnit_C::DestroyVehicleData(int32_t removePassengers) {
    if (!this->m_vehicle) {
        return;
    }

    this->m_vehicle->Destroy(removePassengers);
    this->m_vehicle = NULL;

    if (removePassengers && (this->m_clientFlags & 0x10000000)) {
        this->OnMountDisplayChanged(this->m_unit->m_unitFieldMountdisplayid);
    }

    if (this->IsActivePlayer()) {
        this->SignalPlayerLosesVehicleDataEvent();
    }
}

// OFFSET: 0x005EC6C0/0x14026DE20
void CGUnit_C::VehiclePassengerRequestExit() {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && player && player->GetTransportGUID() == this->m_obj->m_guid.m_guid) {
        CGPetInfo::PetDismiss();
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REQUEST_VEHICLE_EXIT));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x005EC780/0x14026DF00
void CGUnit_C::VehiclePassengerRequestPrevSeat() {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    WOWGUID guid = this->m_obj->m_guid;

    if (guid == CGUnit_C::s_activeMover && player && player->GetTransportGUID() == guid.m_guid) {
        if (player->m_vehiclePassenger) {
            player->m_vehiclePassenger->StopMovement();
        }

        this->m_movementData.OnChangeSeatsOnControlledVehicleLocal(FrameTime::GetCurTimeMs(), WOWGUID(), 0xFF);
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REQUEST_VEHICLE_PREV_SEAT));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x005EC870/0x14026E010
void CGUnit_C::VehiclePassengerRequestNextSeat() {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    WOWGUID guid = this->m_obj->m_guid;

    if (guid == CGUnit_C::s_activeMover && player && player->GetTransportGUID() == guid.m_guid) {
        if (player->m_vehiclePassenger) {
            player->m_vehiclePassenger->StopMovement();
        }

        this->m_movementData.OnChangeSeatsOnControlledVehicleLocal(FrameTime::GetCurTimeMs(), WOWGUID(), 1);
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REQUEST_VEHICLE_NEXT_SEAT));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x005EC960/0x14026E120
int32_t CGUnit_C::VehiclePassengerRequestSwitchToVirtualSeat(const CGUnit_C* vehicle, int32_t virtualSeat) {
    CVehicle_C* root = vehicle->m_vehicle;
    const CVehicle_C* target;
    uint8_t seat;

    if (!root || !root->m_vehicleRec || !root->GetVehicleAndSeatFromVirtualSeat(virtualSeat, target, seat)) {
        return 0;
    }

    VehicleSeatRec* seatRec = target->GetVehicleSeatRec(seat);

    if (!seatRec || target->FindPassengerUnit(seat) || !(seatRec->m_flags & 0x4000000) || !CGInputControl::GetActive() || !CGInputControl::GetActive()->IsVehicleSeatFlagSet(0x4000000)) {
        return 0;
    }

    if (this->GetTransportGUID() != CGUnit_C::s_activeMover.m_guid) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_REQUEST_VEHICLE_SWITCH_SEAT));
        SmartGUID guid;
        guid.m_guid = target->m_unit->m_obj->m_guid.m_guid;
        msg << guid;
        msg.Put(seat);
        msg.Finalize();
        ClientServices::Send2(&msg);
        return 1;
    }

    if (!ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__)) {
        return 1;
    }

    if (this->m_vehiclePassenger) {
        this->m_vehiclePassenger->StopMovement();
    }

    this->OnChangeSeatsOnControlledVehicleLocal(FrameTime::GetCurTimeMs(), target->m_unit->m_obj->m_guid, seat);
    return 1;
}

// OFFSET: 0x005ECB10/0x14026E320
CGUnit_C* CGUnit_C::FindRootVehicleOfPassengerChain(CGUnit_C* stop) {
    CGUnit_C* unit = this;
    WOWGUID guid = this->m_vehiclePassenger ? this->m_vehiclePassenger->m_transitionVehicleGuid : WOWGUID();

    if (!guid) {
        if (!this->m_vehicle || !this->m_vehicle->m_vehicleRec) {
            return NULL;
        }

        return unit;
    }

    unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return NULL;
    }

    while (unit != stop) {
        guid = unit->m_vehiclePassenger ? unit->m_vehiclePassenger->m_transitionVehicleGuid : WOWGUID();

        if (!guid) {
            break;
        }

        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit) {
            return NULL;
        }
    }

    return unit;
}

// OFFSET: 0x005ECBB0/0x14026E3D0
void CGUnit_C::CheckVehicleExitOnCast() {
    VehicleSeatRec* seat = this->m_vehiclePassenger ? this->m_vehiclePassenger->m_seatRec : NULL;

    if (!seat || !(seat->m_flagsB & 0x100000)) {
        return;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (this->m_obj->m_guid != CGUnit_C::s_activeMover || !player || player->GetTransportGUID() != this->m_obj->m_guid.m_guid) {
        this->VehiclePassengerRequestExit();
    }
}

// OFFSET: 0x005ECC30/0x14026E460
void CGUnit_C::DestroyVehicleCamera() {
    if (!this->m_vehicleCamera) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_vehicleCamera->m_nextCameraGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    CVehicleCamera_C* camera = this->m_vehicleCamera;
    camera->Destroy();
    this->m_vehicleCamera = NULL;
    CGCamera* activeCamera = CGWorldFrame::GetActiveCamera();

    if (activeCamera && activeCamera->m_vehicleCamera == camera) {
        activeCamera->OnVehicleCameraDestroyed();
    }

    if (unit) {
        unit->OnVehicleCameraPossiblyUnneeded();
    }
}

// PROVEN: e0bd02f0 IDENTICAL/IDENTICAL
// OFFSET: 0x005ECCB0/0x14026E500
void CGUnit_C::OnVehicleCameraNoLongerInUse() {
    this->DestroyVehicleCamera();
}

// PROVEN: f2325b6d IDENTICAL/IDENTICAL
// OFFSET: 0x005ECCC0/0x14026E510
void CGUnit_C::SetVehicleRecID(uint32_t vehicleRecId) {
    if (vehicleRecId) {
        this->CreateVehicleData(NULL, vehicleRecId);
    } else {
        this->DestroyVehicleData(1);
    }
}

// OFFSET: 0x005ECD30/0x14026E5C0
void CGUnit_C::UpdateVehicleUISliders() {
    VehicleRec* vehicleRec = this->m_vehicle ? this->m_vehicle->m_vehicleRec : NULL;

    if (this->IsVehicleTransitionPreventingAction() && vehicleRec) {
        if (vehicleRec->m_flags & 0x400) {
            FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_ANGLE_SHOW, "%d", 1);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_ANGLE_SHOW, NULL);
        }

        if (vehicleRec->m_flags & 0x800) {
            FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_POWER_SHOW, "%d", 1);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_POWER_SHOW, NULL);
        }
    } else if (ClntObjMgrGetActivePlayerObj()) {
        FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_ANGLE_SHOW, NULL);
        FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_POWER_SHOW, NULL);
    }

    CGInputControl::GetActive();
    CGInputControl::SendUIVehicleAngleUpdate(this->GetPitch());
}

// OFFSET: 0x005ECE10/0x14026E6A0
void CGUnit_C::VehiclePassengerOnUpdateTransport(WOWGUID transport, uint8_t seat, int32_t a3) {
    if (!(this->m_clientFlags & 0x80000) && !a3) {
        return;
    }

    CGUnit_C* root = this->m_vehiclePassenger ? this->m_vehiclePassenger->GetStoredRootVehicleUnit() : this->FindRootVehicle(NULL);

    if (root) {
        root->SetMaxAlpha(1.0f);
    }

    WOWGUID guid = transport;
    uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);
    CGUnit_C* vehicle;

    if ((high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(guid.m_guid)))) {
        vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!vehicle) {
            guid = WOWGUID();
            seat = 0xFF;

            if (!this->m_vehiclePassenger || !this->m_vehiclePassenger->m_transitionVehicleGuid) {
                return;
            }
        }
    } else {
        vehicle = NULL;
    }

    if (!this->m_vehiclePassenger) {
        if (!guid) {
            return;
        }

        this->m_vehiclePassenger = CVehiclePassenger_C::Alloc(this);
    }

    if (!guid && root) {
        guid.m_guid = root->GetTransportGUID();
    }

    this->m_vehiclePassenger->OnUpdateTransport(guid, seat, vehicle, a3);
}

// PROVEN: 6942d5e5 IDENTICAL/IDENTICAL
// OFFSET: 0x005ECF30/0x14026E800
CVehicleCamera_C* CGUnit_C::CreateVehicleCamera() {
    this->DestroyVehicleCamera();
    this->m_vehicleCamera = CVehicleCamera_C::Create(this, FrameTime::GetCurTimeMs());

    if (!this->m_vehicleCamera) {
        return NULL;
    }

    if (!this->m_vehicleCamera->m_state) {
        this->DestroyVehicleCamera();
        return NULL;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (camera) {
        camera->PickVehicleCamera();
    }

    return this->m_vehicleCamera;
}

// OFFSET: 0x005ECF80/0x14026E870
void CGUnit_C::OnVehicleCameraPossiblyNeeded() {
    if (!this->m_vehiclePassenger || !this->m_vehiclePassenger->m_state || this->m_vehicleCamera) {
        return;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (!camera) {
        return;
    }

    CVehicleCamera_C* activeVehicleCamera = camera->m_vehicleCamera;
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (this->IsActivePlayer() || this->m_obj->m_guid == camera->m_targetGUID || (player && player->FindRootVehicleOfPassengerChain(this) == this) || (activeVehicleCamera && activeVehicleCamera->m_unit && activeVehicleCamera->m_unit != player && activeVehicleCamera->m_unit->FindRootVehicleOfPassengerChain(this) == this)) {
        this->CreateVehicleCamera();

        if (this->m_vehicleCamera && this->m_vehicleCamera != camera->m_vehicleCamera && !this->m_vehicleCamera->AttachToHierarchy()) {
            this->DestroyVehicleCamera();
        }
    }
}

// OFFSET: 0x005ED060/0x14026E980
void CGUnit_C::VehicleCameraOnUpdateTransport(WOWGUID transport, uint8_t seat) {
    if (this->IsActivePlayer()) {
        uint32_t cameraId = 4;

        if (transport && seat != 0xFF) {
            VehicleSeatRec* seatRec = this->m_vehiclePassenger->m_seatRec;

            if (seatRec && seatRec->m_cameraModeID) {
                cameraId = seatRec->m_cameraModeID;
            }
        }

        StaticSingleton<CCameraManager>::Instance()->SetGameCamera(cameraId, this->m_obj->m_guid);
    }

    if (!this->m_vehicleCamera) {
        this->OnVehicleCameraPossiblyNeeded();
        return;
    }

    uint32_t time = FrameTime::GetCurTimeMs();
    CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(transport, TYPEMASK_UNIT, __FILE__, __LINE__));
    this->m_vehicleCamera->BeginTransition(vehicle, transport, time);

    if (!this->m_vehicleCamera->m_state) {
        this->DestroyVehicleCamera();
    }
}

// PROVEN: a722881a IDENTICAL/IDENTICAL
// OFFSET: 0x0057D160/0x1401E32C0
CGUnit_C* CGUnit_C::GetRootVehicleUnit() {
    if (this->m_vehiclePassenger) {
        return this->m_vehiclePassenger->GetStoredRootVehicleUnit();
    }

    return this->FindRootVehicle(NULL);
}
