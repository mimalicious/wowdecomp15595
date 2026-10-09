#include "VehiclePassenger_C.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/Object/ObjectClient/AnimKitManager.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/Object.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/Object/ObjectClient/VehicleCamera_C.h"
#include "WoW/Source/Object/ObjectClient/Types.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/CameraManager.h"
#include "WoW/Source/StaticSingleton.h"
#include <cmath>

// STATIC: 0x00DD82CC
TSObjectAllocPool<CVehiclePassenger_C>* CVehiclePassenger_C::s_passengerPool;

// STATIC: 0x00DD82D0
TSGrowableArray<CVehiclePassenger_C*>* CVehiclePassenger_C::s_pendingPassengers;

// STATIC: 0x00DD82D4
uint32_t CVehiclePassenger_C::s_addingPendingRescueTransitions;

// STATIC: 0x00DD82D8
CVehiclePassenger_C* CVehiclePassenger_C::s_headPassenger;

// STATIC: 0x00DD82DC
CVehiclePassenger_C* CVehiclePassenger_C::s_tailPassenger;

// STATIC: 0x00DD82C8
TSGrowableArray<VehiclePassengerRescueTransition>* CVehiclePassenger_C::s_rescueTransitions;

// STATIC: 0x00B9E728
static const uint32_t s_seatAttachments[26] = { 20, 34, 19, 21, 22, 17, 23, 24, 25, 15, 16, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 0, 47, 48, 6, 5 };

// PROVEN: 90e98e41 IDENTICAL/NONE
// OFFSET: 0x005ED150
void CVehiclePassenger_C::StartAddingPendingRescueTransitions() {
    CVehiclePassenger_C::s_addingPendingRescueTransitions = 1;
}

// OFFSET: 0x005ED160
void CVehiclePassenger_C::OnCancelControlEnd() {
    CGUnit_C::StopMissileTrajectoryTargeting();
    CGUnit_C::HideVehicleUISliders();
}

// PROVEN: dfac6bd9 IDENTICAL/NONE
// OFFSET: 0x005ED170
void CVehiclePassenger_C::ClearFlag8() {
    this->m_flags &= ~0x8;
}

// PROVEN: 07d224a2 IDENTICAL/NONE
// OFFSET: 0x005ED180/0x14026EB00
void CVehiclePassenger_C::OnVehicleRideLoopAnimEnd(CGUnit_C* unit, int32_t& done, TSGrowableArray<CVehiclePassenger_C*>& passengers) {
    done = 1;
}

// PROVEN: a8db524b IDENTICAL/NONE
// OFFSET: 0x005ED190
void CVehiclePassenger_C::UpdateSmoothFacing() {
    this->m_targetFacing = this->m_unit->GetFacing();

    while (this->m_targetFacing + 3.1415927f < this->m_smoothFacing) {
        this->m_targetFacing = this->m_targetFacing + 6.2831855f;
    }

    while (this->m_targetFacing - 3.1415927f > this->m_smoothFacing) {
        this->m_targetFacing = this->m_targetFacing - 6.2831855f;
    }

    this->m_smoothFacing = (1.0f - this->m_facingBlend) * this->m_startFacing + this->m_targetFacing * this->m_facingBlend;
}

// PROVEN: 567f5348 IDENTICAL/NONE
// OFFSET: 0x005ED270
void CVehiclePassenger_C::PostRenderUpdateAll() {
    for (CVehiclePassenger_C* passenger = s_headPassenger; passenger; passenger = passenger->m_next) {
        passenger->m_flags &= ~0x1000;
    }
}

// PROVEN: f9ea1be9 IDENTICAL/NONE
// OFFSET: 0x005ED290
void CVehiclePassenger_C::LockWorldFacingNextFrame(float facing) {
    this->m_flags |= 0x1000;
    this->m_lockedWorldFacing = facing;
}

// PROVEN: 87119b9f IDENTICAL/NONE
// OFFSET: 0x005ED2B0
int32_t CVehiclePassenger_C::GetLowerAnim(const VehicleSeatRec* seat) const {
    if (!seat) {
        return ANIM_COUNT;
    }

    int32_t anim;

    switch (this->m_state) {
        case 1:
        case 2: {
            if (!(seat->m_flags & 0x1)) {
                return ANIM_COUNT;
            }

            if ((this->m_flags & 0x2) || (anim = seat->m_enterAnimStart) == -1) {
                anim = seat->m_enterAnimLoop;

                if (anim == -1) {
                    return ANIM_COUNT;
                }
            }

            return anim;
        }

        case 3: {
            if (!(seat->m_flags & 0x2)) {
                return ANIM_COUNT;
            }

            if ((this->m_flags & 0x2) || (anim = seat->m_rideAnimStart) == -1) {
                anim = seat->m_rideAnimLoop;

                if (anim == -1) {
                    return ANIM_COUNT;
                }
            }

            return anim;
        }

        case 4:
        case 5: {
            if (!this->m_unit->ShouldUseVehicleSeatExitAnimation(seat)) {
                return ANIM_COUNT;
            }

            if ((this->m_flags & 0x2) || (anim = seat->m_exitAnimStart) == -1) {
                anim = seat->m_exitAnimLoop;

                if (anim == -1) {
                    return ANIM_COUNT;
                }
            }

            return anim;
        }

        default: {
            return ANIM_COUNT;
        }
    }
}

// PROVEN: 6452bf57 IDENTICAL/NONE
// OFFSET: 0x005ED360/0x14026ECD0
void CVehiclePassenger_C::StopAnimKit(int32_t resetUserFlags, int32_t playStopKit) {
    if (this->m_animKit) {
        if (resetUserFlags) {
            int32_t count = this->m_animKit->GetSegmentCount();

            for (int32_t i = 0; i < count; i++) {
                this->m_animKit->SetUserFlags(i, 2);
            }
        }

        this->m_animKit->Stop(playStopKit, NULL);
    }

    if (this->m_seatAnimKit) {
        if (resetUserFlags) {
            int32_t count = this->m_seatAnimKit->GetSegmentCount();

            for (int32_t i = 0; i < count; i++) {
                this->m_seatAnimKit->SetUserFlags(i, 2);
            }
        }

        this->m_seatAnimKit->Stop(playStopKit, NULL);
    }
}

// PROVEN: adf4f7c4 IDENTICAL/NONE
// OFFSET: 0x005ED400/0x14026EDA0
void CVehiclePassenger_C::FreeAnimKit() {
    if (this->m_animKit) {
        this->m_animKit->SetStopCallback(NULL, 0);
        this->m_animKit->Release();
        this->m_animKit = NULL;
    }

    if (this->m_seatAnimKit) {
        this->m_seatAnimKit->SetStopCallback(NULL, 0);
        this->m_seatAnimKit->Release();
        this->m_seatAnimKit = NULL;
    }
}

// PROVEN: 74314af0 IDENTICAL/NONE
// OFFSET: 0x005ED460/0x14026EE30
uint32_t CVehiclePassenger_C::IsMirroringVehicleAnims() const {
    return this->m_flags & 0x8800;
}

// PROVEN: 634925c3 IDENTICAL/NONE
// OFFSET: 0x005ED470
uint32_t CVehiclePassenger_C::MirrorsVehicleAnimTier() const {
    return this->m_flags & 0x8000;
}

// OFFSET: 0x005ED5E0
void CVehiclePassenger_C::UpdateBlend(uint32_t time, const VehicleSeatRec* seatRec) {
    if (!seatRec || !(this->m_flags & 0x1)) {
        this->m_facingBlend = 0.0f;
        return;
    }

    float duration = static_cast<int32_t>(this->m_endTime - this->m_startTime) * 0.001f;

    if (duration < 0.0001f) {
        this->m_facingBlend = 1.0f;
        return;
    }

    this->m_facingBlend = static_cast<int32_t>(time - this->m_startTime) * 0.001f / duration;
    float blend = this->m_facingBlend;
    this->m_facingBlend = blend < 0.0f ? 0.0f : (blend < 1.0f ? blend : 1.0f);

    uint32_t easeIn;
    uint32_t easeOut;

    if (this->m_state == 2) {
        easeIn = 0x20;
        easeOut = 0x40;
    } else if (this->m_state == 5) {
        easeIn = 0x80;
        easeOut = 0x100;
    } else {
        return;
    }

    uint32_t flags = seatRec->m_flags;

    if ((flags & easeIn) && (flags & easeOut)) {
        this->m_facingBlend = 0.5f - std::cos(this->m_facingBlend * 3.1415927f) * 0.5f;
    } else if (seatRec->m_flags & easeIn) {
        this->m_facingBlend = this->m_facingBlend * this->m_facingBlend;
    } else if (flags & easeOut) {
        this->m_facingBlend = 1.0f - (1.0f - this->m_facingBlend) * (1.0f - this->m_facingBlend);
    }
}

// OFFSET: 0x005ED700
C3Vector CVehiclePassenger_C::ComputeStartWorldPosition(const C3Vector& offset, const C3Vector& fallback) const {
    CGObject_C* relative = ClntObjMgrObjectPtr(this->m_startRelativeGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (relative) {
        if ((relative->m_obj->m_type >> 3) & 1) {
            return offset + static_cast<CGUnit_C*>(relative)->BuildSmoothPosition();
        }

        return offset * relative->GetMatrix();
    }

    return fallback;
}

// OFFSET: 0x005ED850
void CVehiclePassenger_C::OnSetActiveMover(CGUnit_C* unit) {
    FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_UPDATE, NULL);
    const VehicleRec* vehicleRec = unit->m_vehicle ? unit->m_vehicle->m_vehicleRec : NULL;

    if (vehicleRec) {
        CGUnit_C::StartMissileTrajectoryTargeting(vehicleRec);
    } else {
        CGUnit_C::StopMissileTrajectoryTargeting();
    }

    unit->UpdateVehicleUISliders();
}

// OFFSET: 0x005ED8A0/0x14026F2A0
void CVehiclePassenger_C::StopMovement() {
    if (this->m_state != 3 || this->m_unit->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return;
    }

    uint32_t time = FrameTime::GetCurTimeMs();
    CGInputControl::GetActive()->StopMovementAndTurning(time);
    this->m_flags |= 0x8;
    this->m_stopMovementTime = time;
}

// OFFSET: 0x005ED8F0/0x14026F300
CGUnit_C* CVehiclePassenger_C::GetVehicleUnit() const {
    WOWGUID transition = this->m_transitionVehicleGuid;

    if (transition) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(transition, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            return unit;
        }
    }

    return static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_vehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
}

// PROVEN: a6d42859 IDENTICAL/NONE
// OFFSET: 0x005ED940/0x14026F350
CGUnit_C* CVehiclePassenger_C::GetStoredRootVehicleUnit() const {
    if (this->m_state) {
        return static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_rootVehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    return this->m_unit->FindRootVehicle(NULL);
}

// OFFSET: 0x005ED970
void CVehiclePassenger_C::StartWaitingForVehicleTransitionAnim(const VehicleSeatRec* seatRec, int32_t entering, CVehicle_C* vehicle) {
    this->m_flags |= 0x200;
    uint32_t flags = this->m_flags;
    this->m_transitionAnimVehicleGuid = vehicle->m_unit->m_obj->m_guid;

    if (seatRec->m_flags & (entering ? 0x8000000 : 0x10000000)) {
        float delay = entering ? seatRec->m_vehicleEnterAnimDelay : seatRec->m_vehicleExitAnimDelay;
        uint32_t timeout = static_cast<uint32_t>(delay * 1000.0f);

        if (timeout > 3000) {
            timeout = 3000;
        }

        this->m_flags = flags | 0x80;
        this->m_transitionAnimTimeout = timeout + FrameTime::GetCurTimeMs();
    } else {
        this->m_flags = flags & ~0x80;

        if (entering) {
            this->m_flags = (flags & ~0x180) | 0x100;
        } else {
            this->m_flags = flags & ~0x180;
        }

        this->m_transitionAnimTimeout = FrameTime::GetCurTimeMs() + 3000;
    }
}

// OFFSET: 0x005EDA50
int32_t CVehiclePassenger_C::IsWaitingForSameVehicleTransitionAnim(WOWGUID transport, uint8_t seat) const {
    if (!(this->m_flags & 0x200)) {
        return 0;
    }

    uint32_t count = this->m_deferredSplines.m_count;
    WOWGUID last = count ? this->m_deferredSplines.m_data[count - 1].m_transport : WOWGUID();
    uint32_t lastLow = static_cast<uint32_t>(last.m_guid);
    uint32_t lastHigh = static_cast<uint32_t>(last.m_guid >> 32);
    uint32_t low = static_cast<uint32_t>(transport.m_guid);
    uint32_t high = static_cast<uint32_t>(transport.m_guid >> 32);

    if ((lastHigh & 0xF0F00000) == 0xF0500000 || (!(lastHigh & 0xF0000000) && ((lastHigh & 0xF07FFFFF) || lastLow)) || (high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | low))) {
        if (last != transport) {
            return 0;
        }

        uint8_t lastSeat = count ? this->m_deferredSplines.m_data[count - 1].m_seat : 0xFF;

        if (lastSeat != seat) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x005EDB30
int32_t CVehiclePassenger_C::IsWaitingForVehicleTransitionAnimGrabEvent(uint32_t eventId) const {
    uint32_t flags = this->m_flags;

    uint8_t index = static_cast<uint8_t>(eventId >> 24) - '0';

    if ((flags & 0x200) && !(flags & 0x80) && (flags & 0x100)) {
        if (!index) {
            return 1;
        }

        uint32_t count = this->m_deferredSplines.m_count;
        uint8_t seat = count ? this->m_deferredSplines.m_data[count - 1].m_seat : 0xFF;

        if (seat == index - 1) {
            return 1;
        }
    }

    return 0;
}

// PROVEN: 22df6080 IDENTICAL/NONE
// OFFSET: 0x005EDB90
int32_t CVehiclePassenger_C::IsWaitingForVehicleTransitionAnimThrowEvent(uint32_t eventId) const {
    uint32_t flags = this->m_flags;
    uint8_t index = static_cast<uint8_t>(eventId >> 24) - '0';
    return (flags & 0x200) && !(flags & 0x80) && !(flags & 0x100) && (!index || static_cast<uint8_t>(this->m_unit->m_movementData.m_transportSeat) == index - 1);
}

// OFFSET: 0x005EDBE0
void CVehiclePassenger_C::ComputeSeatOffset(const VehicleSeatRec* seat, CM2Model* model) {
    if (!seat || !model || !model->IsLoaded()) {
        return;
    }

    uint32_t attachment = seat->m_passengerAttachmentID;

    if (model->HasAttachment(attachment)) {
        C3Vector pivot = model->GetAttachmentPivot(attachment);
        float x = -pivot.x;
        float y = -pivot.y;
        float z = -pivot.z;
        this->m_flags |= 0x40;
        this->m_flags |= 0x20;
        this->m_seatOffset.x = x;
        this->m_seatOffset.y = y;
        this->m_seatOffset.z = z;
    } else {
        this->m_seatOffset.x = 0.0f;
        this->m_seatOffset.y = 0.0f;
        this->m_seatOffset.z = 0.0f;
        this->m_flags &= ~0x40;
        this->m_flags |= 0x20;
    }
}

// PROVEN: 605086db IDENTICAL/NONE
// OFFSET: 0x005EDCC0
float CVehiclePassenger_C::GetLockedWorldFacing() const {
    return (this->m_flags & 0x1000) ? this->m_lockedWorldFacing : this->m_unit->GetFacing();
}

// OFFSET: 0x005EDCF0
void CVehiclePassenger_C::OnAnimEnd(uint32_t anim) {
    const VehicleSeatRec* seat = this->m_seatRec;

    if (this->GetLowerAnim(seat) != ANIM_COUNT && seat && this->m_state == 3 && (seat->m_flags & 0x4)) {
        int32_t upper;

        if ((this->m_flags & 0x4) || (upper = seat->m_rideUpperAnimStart) == -1) {
            upper = seat->m_rideUpperAnimLoop;

            if (upper == -1) {
                this->m_flags |= 0x6;
                return;
            }
        }

        if (upper != ANIM_COUNT) {
            if (anim == 0xFFFFFFFF || anim == 26) {
                this->m_flags |= 0x2;
            } else {
                this->m_flags |= 0x4;
            }

            return;
        }
    }

    this->m_flags |= 0x6;
}

// OFFSET: 0x005EDD70/0x14026F790
void CVehiclePassenger_C::PlayRideAnimKit(const VehicleSeatRec* seatRec, int32_t resetUserFlags, uint8_t seat) {
    if (this->m_animKit) {
        this->m_animKit->Stop(1, NULL);
    }

    if (this->m_seatAnimKit) {
        this->m_seatAnimKit->Stop(1, NULL);
    }

    if (!seatRec) {
        return;
    }

    uint32_t animCount = g_animationDataDB.m_maxID + 1;
    uint32_t rideStart = static_cast<uint32_t>(seatRec->m_rideAnimStart) < animCount ? seatRec->m_rideAnimStart : seatRec->m_rideAnimLoop;
    uint32_t rideLoop = static_cast<uint32_t>(seatRec->m_rideAnimLoop) < animCount ? seatRec->m_rideAnimLoop : seatRec->m_rideAnimStart;
    uint32_t upperStart = static_cast<uint32_t>(seatRec->m_rideUpperAnimStart) < animCount ? seatRec->m_rideUpperAnimStart : seatRec->m_rideUpperAnimLoop;
    uint32_t upperLoop = static_cast<uint32_t>(seatRec->m_rideUpperAnimLoop) < animCount ? seatRec->m_rideUpperAnimLoop : seatRec->m_rideUpperAnimStart;
    uint32_t flags = seatRec->m_flags;
    int32_t ride = (flags & 0x2) && rideStart < animCount && rideLoop < animCount;
    int32_t upper = (flags & 0x4) && upperStart < animCount && upperLoop < animCount;

    if (ride) {
        CAnimKitManager* manager = this->m_unit->m_mountAnimKitManager ? this->m_unit->m_mountAnimKitManager : &this->m_unit->m_animKitManager;

        if (upper) {
            this->m_animKit = manager->PlayAndMaintain(224, rideStart, NULL);

            if (this->m_animKit) {
                this->m_animKit->SetAnim(1, rideLoop, 0xFF, 0, 0, 0);
                this->m_animKit->SetAnim(2, upperStart, 0xFF, 0, 0, 0);
                this->m_animKit->SetAnim(3, upperLoop, 0xFF, 0, 0, 0);
            }
        } else {
            this->m_animKit = manager->PlayAndMaintain(222, rideStart, NULL);

            if (this->m_animKit) {
                this->m_animKit->SetAnim(1, rideLoop, 0xFF, 0, 0, 0);
            }
        }
    } else if (upper) {
        CAnimKitManager* manager = this->m_unit->m_mountAnimKitManager ? this->m_unit->m_mountAnimKitManager : &this->m_unit->m_animKitManager;
        this->m_animKit = manager->PlayAndMaintain(223, upperStart, NULL);

        if (this->m_animKit) {
            this->m_animKit->SetAnim(1, upperLoop, 0xFF, 0, 0, 0);
        }
    }

    if (this->m_animKit) {
        if (resetUserFlags) {
            int32_t count = this->m_animKit->GetSegmentCount();

            for (int32_t i = 0; i < count; i++) {
                this->m_animKit->SetUserFlags(i, 1);
            }
        }

        this->m_animKit->SetStopCallback(CGUnit_C::PointerClearAnimKitStopCallback, reinterpret_cast<intptr_t>(&this->m_animKit));
    }

    if (seatRec->m_rideAnimKitID > 0) {
        CAnimKitManager* manager = this->m_unit->m_mountAnimKitManager ? this->m_unit->m_mountAnimKitManager : &this->m_unit->m_animKitManager;
        this->m_seatAnimKit = manager->PlayAndMaintain(seatRec->m_rideAnimKitID, -1, NULL);

        if (this->m_seatAnimKit) {
            if (resetUserFlags) {
                int32_t count = this->m_seatAnimKit->GetSegmentCount();

                for (int32_t i = 0; i < count; i++) {
                    this->m_seatAnimKit->SetUserFlags(i, 1);
                }
            }

            this->m_seatAnimKit->SetStopCallback(CGUnit_C::PointerClearAnimKitStopCallback, reinterpret_cast<intptr_t>(&this->m_seatAnimKit));
        }
    }
}

// PROVEN: 8649c098 IDENTICAL/NONE
// OFFSET: 0x005EDFD0/0x14026FAD0
void CVehiclePassenger_C::UpdateAnimKitAfterObjectModelChange() {
    if (this->m_animKit) {
        this->m_animKit->Stop(1, NULL);
    }

    if (this->m_seatAnimKit) {
        this->m_seatAnimKit->Stop(1, NULL);
    }

    if (this->m_state == 3) {
        this->PlayRideAnimKit(this->m_seatRec, 1, this->m_transitionSeat);
    }
}

// PROVEN: 984e1c66 IDENTICAL/NONE
// OFFSET: 0x005EE020
void CVehiclePassenger_C::StopMirroringVehicleAnims(int32_t unblockExpressions) {
    this->m_flags &= ~0x8800;

    if (this->m_seat < 8) {
        CGUnit_C* vehicleUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_vehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (vehicleUnit && vehicleUnit->m_vehicle) {
            vehicleUnit->m_vehicle->OnPassengerStopMirroringAnims(this->m_seat);
        }
    }

    if (unblockExpressions) {
        this->m_unit->m_animKitManager.SetExpressionsBlocked(0, 1);

        if (this->m_unit->m_mountAnimKitManager) {
            this->m_unit->m_mountAnimKitManager->SetExpressionsBlocked(0, 1);
        }
    }
}

// PROVEN: 12e71ed8 IDENTICAL/NONE
// OFFSET: 0x005EE0A0
int32_t CVehiclePassenger_C::IsCurrentlyMirroringVehicleAnims() const {
    return (this->m_flags & 0x8800) && this->m_unit->m_unit->m_unitFieldHealth > 0;
}

// PROVEN: 85cca2c0 IDENTICAL/NONE
// OFFSET: 0x005EE0D0/0x14026FB60
void CVehiclePassenger_C::UpdateVehicleAnimMirroring() {
    if (this->m_unit->m_unit->m_unitFieldHealth <= 0) {
        this->m_unit->m_animKitManager.SetExpressionsBlocked(0, 1);

        if (this->m_unit->m_mountAnimKitManager) {
            this->m_unit->m_mountAnimKitManager->SetExpressionsBlocked(0, 1);
        }

        return;
    }

    CAnimKitManager* manager = this->m_unit->m_mountAnimKitManager ? this->m_unit->m_mountAnimKitManager : &this->m_unit->m_animKitManager;
    manager->SetExpressionsBlocked(1, 1);

    if (this->m_unit->m_mountAnimKitManager) {
        this->m_unit->m_animKitManager.SetExpressionsBlocked(0, 1);
    }

    CGUnit_C* vehicleUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_transitionVehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (vehicleUnit && vehicleUnit->m_vehicle) {
        CAnimKitManager* source = vehicleUnit->m_mountAnimKitManager ? vehicleUnit->m_mountAnimKitManager : &vehicleUnit->m_animKitManager;
        CAnimKitManager* target = this->m_unit->m_mountAnimKitManager ? this->m_unit->m_mountAnimKitManager : &this->m_unit->m_animKitManager;
        target->MirrorFrom(source, this->m_flags & 0x8000);
    }
}

// OFFSET: 0x005EE1A0/0x14026FC60
static void SignalVehicleTransitionEvent(WOWGUID guid, WOWGUID vehicleGuid, const VehicleRec* vehicleRec, const VehicleSeatRec* seatRec, int32_t event) {
    const char* sound = "";
    bool controlSeat;
    const char* skin;
    bool hasVehicleUI;

    if (seatRec) {
        uint32_t flags = seatRec->m_flags;
        controlSeat = (flags & 0x20000000) != 0;
        skin = static_cast<uint32_t>(seatRec->m_uiSkin) < 4 ? s_vehicleSkinNames[seatRec->m_uiSkin] : "";
        SoundEntriesRec* soundRec = g_soundEntriesDB.GetRecord(seatRec->m_enterUisoundID);

        if (soundRec) {
            sound = soundRec->m_name;
        }

        hasVehicleUI = (flags & 0x800) != 0;
    } else {
        controlSeat = false;
        skin = "";
        hasVehicleUI = false;
    }

    int32_t indicator = vehicleRec ? vehicleRec->m_vehicleUiindicatorID : 0;
    char vehicleGuidText[32];
    Script_SendUnitSignal(guid, event, "%b%s%s%b%d%s", controlSeat, skin, sound, hasVehicleUI, indicator, GUIDToHexString(vehicleGuid, vehicleGuidText));
}

// OFFSET: 0x005EE280
void CVehiclePassenger_C::InitTrainCarPosition() {
    if (this->m_flags & 0x4000) {
        return;
    }

    this->m_flags |= 0x4000;
    float facing = this->m_unit->GetSmoothFacing();
    float c = std::cos(facing);
    float s = std::sin(facing);
    this->m_trainCarPosition = this->m_unit->BuildSmoothPosition();
    float radius = this->m_unit->m_unit->m_unitFieldBoundingradius;
    float y = this->m_trainCarPosition.y - radius * s;
    this->m_trainCarPosition.x = this->m_trainCarPosition.x - radius * c;
    this->m_trainCarPosition.y = y;
}

// OFFSET: 0x005EE340
float CVehiclePassenger_C::ComputeTrainCarRawFacing(const C3Vector& position, float facing) {
    this->InitTrainCarPosition();
    float dx = position.x - this->m_trainCarPosition.x;
    float dy = position.y - this->m_trainCarPosition.y;
    float dz = position.z - this->m_trainCarPosition.z;
    float length = std::sqrt(dx * dx + dy * dy + dz * dz);

    if (length <= 0.00000011920929f) {
        return this->m_unit->m_smoothTarget.m_value;
    }

    float x = 1.0f / length * dx;
    float y = 1.0f / length * dy;
    this->m_trainCarPosition.x = position.x;
    float angle = std::atan2(y, x);
    this->m_trainCarPosition.y = position.y;
    this->m_trainCarPosition.z = position.z;
    float radius = this->m_unit->m_unit->m_unitFieldBoundingradius;
    float py = this->m_trainCarPosition.y - y * radius;
    this->m_trainCarPosition.x = this->m_trainCarPosition.x - x * radius;
    this->m_trainCarPosition.y = py;
    return angle - facing;
}

// OFFSET: 0x005EE570
CVehicle_C* CVehiclePassenger_C::GetVehicle() const {
    WOWGUID transition = this->m_transitionVehicleGuid;
    CGUnit_C* unit;

    if (!transition || !(unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(transition, TYPEMASK_UNIT, __FILE__, __LINE__)))) {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_vehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    if (unit) {
        return unit->m_vehicle;
    }

    return NULL;
}

// OFFSET: 0x005EE5D0
void CVehiclePassenger_C::ComputeModelTransform(C44Matrix& matrix, CM2Model*& model, CGUnit_C* vehicle, const VehicleSeatRec* seat, uint32_t attachment, float facing) {
    CVehiclePassenger_C* vehiclePassenger = vehicle->m_vehiclePassenger;
    float vehicleFacing = vehiclePassenger ? vehiclePassenger->GetLockedWorldFacing() : vehicle->GetFacing();

    if (this->m_flags & 0x1000) {
        facing = this->m_lockedWorldFacing - vehicleFacing;
    }

    int32_t hasAttachment = model->HasAttachment(attachment);
    matrix.RotateAroundZ(facing - seat->m_passengerYaw);

    if (0.0f != seat->m_passengerRoll) {
        matrix.RotateAroundX(seat->m_passengerRoll);
    }

    if (0.0f != seat->m_passengerPitch) {
        matrix.RotateAroundY(seat->m_passengerPitch);
    }

    float vehicleScale;

    if (hasAttachment) {
        C44Matrix transform = model->GetAttachmentWorldTransform(attachment);
        float length = std::sqrt(transform.a2 * transform.a2 + transform.a1 * transform.a1 + transform.a0 * transform.a0);

        if (length <= 0.0001f) {
            matrix.Scale(this->m_unit->GetTrueScale());
        } else {
            matrix.Scale(this->m_unit->GetTrueScale() / length);
        }

        vehicleScale = 1.0f;
    } else {
        matrix.Scale(this->m_unit->GetTrueScale());
        vehicleScale = vehicle->GetTrueScale();
    }

    C3Vector offset(seat->m_attachmentOffset[0] * vehicleScale, seat->m_attachmentOffset[1] * vehicleScale, seat->m_attachmentOffset[2] * vehicleScale);

    if ((this->m_flags & 0x20) && (this->m_flags & 0x40)) {
        C3Vector seatOffset = this->m_seatOffset;
        seatOffset *= matrix;
        offset.x = seatOffset.x + offset.x;
        offset.y = seatOffset.y + offset.y;
        offset.z = seatOffset.z + offset.z;
    }

    matrix.d0 = offset.x;
    matrix.d1 = offset.y;
    matrix.d2 = offset.z;

    if (hasAttachment) {
        return;
    }

    C44Matrix world;
    C3Vector position;
    vehiclePassenger = vehicle->m_vehiclePassenger;

    if (vehiclePassenger && vehiclePassenger->m_state && vehiclePassenger->m_state != 3) {
        CM2Model* vehicleModel = model;
        vehicleModel->Animate();
        position = *reinterpret_cast<const C3Vector*>(&vehicleModel->m_matrixF4.d0) * vehicleModel->m_scene->m_viewInv;
    } else {
        position = vehicle->GetPosition();
    }

    world.d0 = position.x;
    world.d1 = position.y;
    world.d2 = position.z;
    vehiclePassenger = vehicle->m_vehiclePassenger;
    world.RotateAroundZ(vehiclePassenger && vehiclePassenger->m_state && vehiclePassenger->m_state != 3 ? vehiclePassenger->m_smoothFacing : vehicleFacing);
    matrix *= world;
    model = NULL;
}

// OFFSET: 0x005EE9F0
void CVehiclePassenger_C::ComputeSeatPos(CGUnit_C* vehicle, CM2Model* model, const VehicleSeatRec* seat, C3Vector& position, C3Vector* up) {
    CM2Model* ownModel = this->m_unit->GetObjectModel();

    if (!(this->m_flags & 0x20)) {
        this->ComputeSeatOffset(seat, ownModel);
    }

    uint32_t attachment = static_cast<uint32_t>(seat->m_attachmentID) >= 26 ? 0xFFFFFFFF : s_seatAttachments[seat->m_attachmentID];
    CM2Model* attachModel = model;

    if ((this->m_flags & 0x20) && (this->m_flags & 0x40)) {
        C44Matrix matrix;
        float facing = (this->m_state == 4 || this->m_state == 5) ? this->m_startTargetFacing : this->m_unit->m_smoothTarget.m_value;
        this->ComputeModelTransform(matrix, attachModel, vehicle, seat, attachment, facing);
        position.x = matrix.d0;
        position.y = matrix.d1;
        position.z = matrix.d2;
    } else {
        position.x = seat->m_attachmentOffset[0];
        position.y = seat->m_attachmentOffset[1];
        position.z = seat->m_attachmentOffset[2];
    }

    if (up) {
        *up = C3Vector(0.0f, 0.0f, 1.0f);
    }

    if (!attachModel) {
        return;
    }

    if (attachModel->HasAttachment(attachment)) {
        C44Matrix transform = attachModel->GetAttachmentWorldTransform(attachment);
        position *= transform;

        if (up) {
            *up = *up * C33Matrix(transform);
        }

        CVehiclePassenger_C* vehiclePassenger = vehicle->m_vehiclePassenger;

        if (!vehiclePassenger || !vehiclePassenger->m_state || vehiclePassenger->m_state == 3) {
            C44Matrix animated = attachModel->GetAnimatedWorldTransform();
            float scale = vehicle->GetTrueScale();

            if (animated.c2 <= 0.999f) {
                position.x = position.x - animated.d0;
                position.y = position.y - animated.d1;
                position.z = position.z - animated.d2;
            } else {
                position *= animated.AffineInverse(scale);
            }

            C44Matrix smooth;
            vehicle->BuildSmoothMatrix(smooth);
            smooth.Scale(scale);

            if (animated.c2 <= 0.999f) {
                position.x = position.x + smooth.d0;
                position.y = position.y + smooth.d1;
                position.z = smooth.d2 + position.z;
            } else {
                position *= smooth;
            }
        }
    } else {
        CVehiclePassenger_C* vehiclePassenger = vehicle->m_vehiclePassenger;

        if (vehiclePassenger && vehiclePassenger->m_state && vehiclePassenger->m_state != 3) {
            position *= attachModel->GetAnimatedWorldTransform();
        } else {
            position *= vehicle->GetMatrix();
        }
    }
}

// OFFSET: 0x005EEFC0/0x140270DC0
void CVehiclePassenger_C::PlayTransitionAnimKit(const VehicleSeatRec* seatRec, int32_t entering, uint8_t seat) {
    if (this->m_animKit) {
        this->m_animKit->Stop(1, NULL);
    }

    if (this->m_seatAnimKit) {
        this->m_seatAnimKit->Stop(1, NULL);
    }

    if (!seatRec) {
        return;
    }

    int32_t animKitID = entering ? seatRec->m_enterAnimKitID : seatRec->m_exitAnimKitID;

    if (animKitID > 0) {
        CAnimKitManager* manager = this->m_unit->m_mountAnimKitManager ? this->m_unit->m_mountAnimKitManager : &this->m_unit->m_animKitManager;
        this->m_seatAnimKit = manager->PlayAndMaintain(animKitID, -1, NULL);

        if (this->m_seatAnimKit) {
            this->m_seatAnimKit->SetStopCallback(CGUnit_C::PointerClearAnimKitStopCallback, reinterpret_cast<intptr_t>(&this->m_seatAnimKit));
        }

        return;
    }

    uint32_t animCount = g_animationDataDB.m_maxID + 1;
    uint32_t start;
    uint32_t loop;

    if (entering) {
        start = static_cast<uint32_t>(seatRec->m_enterAnimStart) < animCount ? seatRec->m_enterAnimStart : seatRec->m_enterAnimLoop;
        loop = static_cast<uint32_t>(seatRec->m_enterAnimLoop) < animCount ? seatRec->m_enterAnimLoop : seatRec->m_enterAnimStart;
    } else {
        start = static_cast<uint32_t>(seatRec->m_exitAnimStart) < animCount ? seatRec->m_exitAnimStart : seatRec->m_exitAnimLoop;
        loop = static_cast<uint32_t>(seatRec->m_exitAnimLoop) < animCount ? seatRec->m_exitAnimLoop : seatRec->m_exitAnimStart;
    }

    if (start >= animCount || loop >= animCount) {
        return;
    }

    AnimKitExtraPlayParams params;
    params.boneSetID = -1;
    params.startTime = 0;
    params.startFraction = 0.0f;
    params.oneShot = 0;
    params.blendTimeMode = (entering && !this->m_state) ? 0 : 3;
    params.blendTime = 175;
    CAnimKitManager* manager = this->m_unit->m_mountAnimKitManager ? this->m_unit->m_mountAnimKitManager : &this->m_unit->m_animKitManager;
    this->m_animKit = manager->PlayAndMaintain(221, start, &params);

    if (this->m_animKit) {
        this->m_animKit->SetAnim(1, loop, 0xFF, 0, 0, 0);
        this->m_animKit->SetStopCallback(CGUnit_C::PointerClearAnimKitStopCallback, reinterpret_cast<intptr_t>(&this->m_animKit));
    }
}

// PROVEN: db1c7838 IDENTICAL/NONE
// OFFSET: 0x005EF160/0x140270F80
void CVehiclePassenger_C::StartMirroringVehicleAnims(int32_t mirrorAnimTier) {
    if (this->m_transitionSeat >= 8) {
        return;
    }

    CGUnit_C* vehicleUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_transitionVehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!vehicleUnit || !vehicleUnit->m_vehicle) {
        return;
    }

    vehicleUnit->m_vehicle->OnPassengerStartMirroringAnims(this->m_transitionSeat);

    if (mirrorAnimTier) {
        this->m_flags |= 0x8000;
    } else {
        this->m_flags |= 0x800;
    }

    this->UpdateVehicleAnimMirroring();
}

// OFFSET: 0x005EF1D0
void CVehiclePassenger_C::SignalTransitionUIEvents(WOWGUID guid, WOWGUID vehicleGuid, const VehicleRec* vehicleRec, const VehicleSeatRec* seatRec, uint32_t events) {
    if (events & 0x1) {
        SignalVehicleTransitionEvent(guid, vehicleGuid, vehicleRec, seatRec, SCRIPT_EVENT_UNIT_ENTERING_VEHICLE);
    }

    if (events & 0x2) {
        SignalVehicleTransitionEvent(guid, vehicleGuid, vehicleRec, seatRec, SCRIPT_EVENT_UNIT_ENTERED_VEHICLE);
    }

    if (events & 0x4) {
        Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_EXITING_VEHICLE);
    }

    if (events & 0x8) {
        const char* sound;
        SoundEntriesRec* soundRec;

        if (seatRec && (soundRec = g_soundEntriesDB.GetRecord(seatRec->m_exitUisoundID)) != NULL) {
            sound = soundRec->m_name;
        } else {
            sound = "";
        }

        Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_EXITED_VEHICLE, "%s", sound);
    }
}

// OFFSET: 0x005EF300/0x140271240
void CVehiclePassenger_C::SetState(uint32_t state, CGUnit_C* vehicle, uint8_t seat, const VehicleSeatRec* seatRec, uint32_t time, WOWGUID relativeGuid, int32_t a8) {
    if (this->m_flags & 0x8800) {
        this->StopMirroringVehicleAnims(1);
    }

    if (this->m_state == 3 && this->m_boneAnimGuid) {
        CGUnit_C* boneUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_boneAnimGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
        CVehicle_C* boneVehicle = boneUnit ? boneUnit->m_vehicle : NULL;

        if (boneVehicle) {
            if (vehicle && vehicle->m_vehicle) {
                VehicleSeatRec* oldSeat = vehicle->m_vehicle->GetVehicleSeatRec(this->m_seat);

                if (oldSeat && (oldSeat->m_vehicleRideAnimKitID > 0 || (oldSeat->m_flags & 0x20000))) {
                    boneVehicle->RemoveBoneAnimListener(this->m_boneAnimBone, this->m_unit->m_obj->m_guid);
                }
            }

            this->m_boneAnimGuid = WOWGUID();
        }
    }

    if (state == 1 || state == 4) {
        relativeGuid.m_guid = this->m_unit->GetTransportGUID();
    }

    this->m_startRelativeGuid = relativeGuid;

    switch (state) {
        case 1:
        case 2:
        case 4:
        case 5: {
            this->m_startModelPosition = this->m_unit->GetModelPosition();
            this->m_startPosition = this->m_unit->BuildSmoothPosition();

            switch (this->m_state) {
                case 0:
                case 3: {
                    float facing = this->m_unit->GetSmoothFacing();
                    this->m_smoothFacing = facing;
                    this->m_startFacing = facing;
                    this->m_startTargetFacing = this->m_unit->m_smoothTarget.m_value;
                    break;
                }

                case 1:
                case 2:
                case 4:
                case 5: {
                    this->m_startTargetFacing = this->m_smoothFacing;
                    this->m_startFacing = this->m_smoothFacing;
                    break;
                }
            }

            break;
        }

        case 3: {
            WOWGUID guid = vehicle ? vehicle->m_obj->m_guid : WOWGUID();
            this->m_transitionVehicleGuid = guid;
            this->m_vehicleGuid = guid;
            this->m_transitionSeat = seat;
            this->m_seat = seat;
            break;
        }
    }

    if (state == 3 && seatRec && ((seatRec->m_flagsB & 0x10000) || (seatRec->m_flagsC & 0x10))) {
        this->StartMirroringVehicleAnims(seatRec->m_flagsC & 0x10);
    }

    this->m_flags &= ~0x8;
    CM2Model* model = this->m_unit->GetObjectModel();

    if ((this->m_unit->m_modelFlags & 0x40000) && model && (model->m_fFlags & 0x4000)) {
        this->m_flags |= 0x10;
    } else {
        this->m_flags &= ~0x10;
    }

    this->m_flags &= ~0x20;

    if (!state) {
        if (model && model->m_attachParent) {
            model->DetachFromParent();
        }

        C3Vector normal = this->m_unit->m_movement->GetGroundNormal();
        float scale = this->m_unit->GetTrueScale();
        float facing = this->m_unit->GetFacing();
        model->SetWorldTransform(this->m_unit->GetPosition(), facing, scale, normal, 0xFFFFFFFF);
    }

    if (model) {
        uint32_t hidden = !(this->m_unit->m_unit->m_unitFieldFlags & 0x2000000) && (!seatRec || !(seatRec->m_flags & 0x100000)) && state;
        model->m_fFlags ^= (model->m_fFlags ^ (hidden << 18)) & 0x40000;
        uint32_t mirror = seatRec && (seatRec->m_flagsC & 0x20) && state == 3;
        model->m_drawFlags ^= (model->m_drawFlags ^ (mirror << 3)) & 0x8;
    }

    this->m_startTime = time;

    if (this->m_unit->m_unit->m_unitFieldHealth <= 0 && this->m_state != 5 && (state == 5 || !state)) {
        if (this->m_animKit) {
            this->m_animKit->Stop(1, NULL);
        }

        if (this->m_seatAnimKit) {
            this->m_seatAnimKit->Stop(1, NULL);
        }

        this->m_unit->OnDeathAnimate();
    }

    if (this->m_state == 5 && !state && this->m_unit->m_unit->m_unitFieldHealth > 0) {
        uint32_t anim;

        if (seatRec) {
            anim = seatRec->m_exitAnimEnd;

            if (anim == ANIM_JUMP_END && (this->m_unit->m_movement->m_flags & 0xF)) {
                anim = ANIM_JUMP_LAND_RUN;
            }
        } else {
            anim = 0xFFFFFFFF;
        }

        int32_t playStopKit = this->m_unit->PlayVehiclePassengerExitAnimEnd(seatRec, anim, a8);

        if (this->m_animKit) {
            this->m_animKit->Stop(playStopKit, NULL);
        }

        if (this->m_seatAnimKit) {
            this->m_seatAnimKit->Stop(playStopKit, NULL);
        }
    }

    uint32_t prevState = this->m_state;
    int32_t keepAnimFlags = (prevState == 1 && state == 2) || prevState == 4;
    int32_t resetUserFlags = (!prevState && state == 3) || (prevState == 3 && !state);

    if (seatRec && state == 3) {
        this->PlayRideAnimKit(seatRec, resetUserFlags, seat);
    } else if (!state) {
        this->StopAnimKit(resetUserFlags, 0);
    }

    this->m_state = state;
    CGUnit_C* root = NULL;

    if (vehicle) {
        root = vehicle->GetRootVehicleUnit();
    }

    if (seatRec && (seatRec->m_flags & 0x200)) {
        if ((this->m_state == 1 || this->m_state == 2) && (this->m_flags & 0x400)) {
            CGUnit_C* oldVehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_vehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
            CGUnit_C* oldRoot = oldVehicle ? oldVehicle->GetRootVehicleUnit() : NULL;

            if (oldRoot == root) {
                this->m_flags |= 0x400;
            } else {
                this->m_flags &= ~0x400;
            }
        } else if (this->m_state == 3) {
            this->m_flags |= 0x400;
        } else {
            this->m_flags &= ~0x400;
        }
    } else {
        this->m_flags &= ~0x400;
    }

    if (this->m_state && this->m_unit->m_unit->m_unitFieldHealth > 0 && !keepAnimFlags) {
        this->m_flags &= ~0x6;
    }

    CGUnit_C* sliderUnit = this->m_unit;

    if (CGUnit_C::s_activeMover == sliderUnit->m_obj->m_guid || ((sliderUnit = vehicle) != NULL && CGUnit_C::s_activeMover == vehicle->m_obj->m_guid)) {
        sliderUnit->UpdateVehicleUISliders();
    }

    if (this->m_unit->IsActivePlayer() && (!seatRec || static_cast<int32_t>(seatRec->m_flags) >= 0)) {
        CGGameUI::CloseInteraction();
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (camera && this->m_unit->IsActivePlayer() && (camera->m_targetGUID == this->m_unit->m_obj->m_guid || camera->m_targetGUID == this->m_vehicleGuid)) {
        CGUnit_C* target = this->m_unit;
        uint32_t current = this->m_state;
        uint32_t cameraMode = 4;

        if ((current == 1 || current == 2 || current == 3) && seatRec) {
            if (seatRec->m_cameraModeID) {
                cameraMode = seatRec->m_cameraModeID;
            } else if (vehicle && (seatRec->m_flags & 0x800)) {
                target = vehicle;
            }
        }

        camera->SetTarget(target, 0);
        StaticSingleton<CCameraManager>::Instance()->SetGameCamera(cameraMode, this->m_unit->m_obj->m_guid);
    }

    this->m_flags &= ~0x1;
    CVehicle_C* vehicleObject = vehicle ? vehicle->m_vehicle : NULL;

    if (this->m_state == 3 && vehicleObject && seatRec && (seatRec->m_vehicleRideAnimKitID > 0 || ((seatRec->m_flags & 0x20000) && seatRec->m_vehicleRideAnimLoop < ANIM_COUNT))) {
        CVehicle_C* boneVehicle = vehicleObject->FindVehicleToPlayBoneAnim(0, seatRec, this->m_unit);
        uint32_t bone;

        if (boneVehicle && boneVehicle->PlayBoneAnim(1, this->m_transitionSeat, seatRec, bone)) {
            boneVehicle->InstallBoneAnimListener(this->m_unit->m_obj->m_guid, bone, CVehiclePassenger_C::OnVehicleRideLoopAnimEnd);
            this->m_boneAnimGuid = boneVehicle->m_unit->m_obj->m_guid;
            this->m_boneAnimBone = bone;
        }
    }

    if (this->m_unit->IsLocalClientControlled() && (!this->m_state || this->m_state == 3)) {
        CGInputControl::GetActive()->UpdatePlayer(time, 1);
    }

    if (vehicleObject && (prevState == 3 || this->m_state == 3)) {
        vehicleObject->UpdateLargestPassengerBoundsRadius();
    }

    if (this->m_state == 3 && seatRec && (seatRec->m_flagsB & 0x4000000)) {
        this->m_flags = (this->m_flags & ~0x6000) | 0x2000;
        this->UpdateBlend(time, seatRec);
    } else {
        this->m_flags &= ~0x2000;
        this->UpdateBlend(time, seatRec);
    }
}

// OFFSET: 0x005EFAD0/0x140271B30
void CVehiclePassenger_C::AddPendingRescueTransition(WOWGUID passengerGuid, WOWGUID vehicleGuid, uint8_t seat, int32_t instant) {
    VehiclePassengerRescueTransition* transition = s_rescueTransitions->New();
    transition->m_passengerGuid = passengerGuid;
    transition->m_vehicleGuid = vehicleGuid;
    transition->m_seat = seat;
    transition->m_instant = instant != 0;
}

// OFFSET: 0x005EFB60/0x140271B90
void CVehiclePassenger_C::StoreRootVehicle(WOWGUID guid) {
    this->m_rootVehicleGuid = guid;

    if (!guid) {
        guid = this->m_unit->m_obj->m_guid;
    }

    CVehicle_C* vehicle = this->m_unit->m_vehicle;

    if (!vehicle) {
        return;
    }

    for (CPassenger* passenger = vehicle->m_passengers.Head(); passenger; passenger = vehicle->m_passengers.Next(passenger)) {
        CVehiclePassenger_C* vehiclePassenger = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__))->m_vehiclePassenger;

        if (vehiclePassenger) {
            vehiclePassenger->StoreRootVehicle(guid);
        }
    }
}

// OFFSET: 0x005EFC00
void CVehiclePassenger_C::OnVehicleTransitionAnimEnd(CGUnit_C* unit, int32_t& done, TSGrowableArray<CVehiclePassenger_C*>& passengers) {
    done = 0;
    CVehiclePassenger_C* passenger = unit->m_vehiclePassenger;

    if (passenger && static_cast<int8_t>(passenger->m_flags) >= 0) {
        passengers.New(passenger);
    }
}

// OFFSET: 0x005EFC30
C3Vector CVehiclePassenger_C::GetEndPosition() {
    const VehicleSeatRec* seatRec = this->m_seatRec;
    CGUnit_C* vehicle;
    CM2Model* model;

    if (this->m_state && this->m_transitionVehicleGuid && (vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_transitionVehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__))) != NULL && (model = vehicle->GetObjectModel()) != NULL && model->IsLoaded() && seatRec) {
        C3Vector position(0.0f, 0.0f, 0.0f);
        this->ComputeSeatPos(vehicle, model, seatRec, position, NULL);
        return position;
    }

    return this->m_unit->GetPosition();
}

// PROVEN: 8686dbff IDENTICAL/NONE
// OFFSET: 0x005EFCF0
void CVehiclePassenger_C::ComputeEndPos(CGUnit_C* vehicle, const VehicleSeatRec* seat, C3Vector& position, C3Vector* up) {
    CM2Model* model = vehicle ? vehicle->GetObjectModel() : NULL;

    if (this->m_state == 2 && model && model->IsLoaded() && seat) {
        this->ComputeSeatPos(vehicle, model, seat, position, up);
        return;
    }

    position = this->m_unit->GetPosition();

    if (up) {
        *up = C3Vector(0.0f, 0.0f, 1.0f);
    }
}

// OFFSET: 0x005EFDB0/0x140271E80
void CVehiclePassenger_C::OnReloadUI() {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->m_vehiclePassenger || !player->m_vehiclePassenger->m_state) {
        return;
    }

    WOWGUID transport;
    transport.m_guid = player->GetTransportGUID();
    CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(transport, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!vehicle) {
        return;
    }

    VehicleRec* vehicleRec = vehicle->m_vehicle ? vehicle->m_vehicle->m_vehicleRec : NULL;

    if (!vehicleRec) {
        return;
    }

    uint8_t seat = player->m_movementData.m_transportSeat;

    if (seat >= 8) {
        return;
    }

    VehicleSeatRec* seatRec = g_vehicleSeatDB.GetRecord(vehicleRec->m_seatID[seat]);

    if (!seatRec) {
        return;
    }

    int32_t state = static_cast<int32_t>(player->m_vehiclePassenger->m_state);

    if (state <= 0) {
        return;
    }

    if (state <= 2) {
        CVehiclePassenger_C::SignalTransitionUIEvents(player->m_obj->m_guid, vehicle->m_obj->m_guid, vehicleRec, seatRec, 1);
    } else if (state == 3) {
        CVehiclePassenger_C::SignalTransitionUIEvents(player->m_obj->m_guid, vehicle->m_obj->m_guid, vehicleRec, seatRec, 3);
    }
}

// OFFSET: 0x005EFEB0
void CVehiclePassenger_C::SendTransitionUIEvents(WOWGUID guid, WOWGUID vehicleGuid, const VehicleRec* vehicleRec, const VehicleSeatRec* seatRec, int32_t fromState, int32_t toState) {
    uint32_t events;

    switch (toState) {
        case 0: {
            events = fromState == 3 ? 12 : 8;
            break;
        }

        case 1: {
            events = 1;
            break;
        }

        case 2: {
            events = 1;

            if (fromState == 1) {
                return;
            }

            break;
        }

        case 3: {
            events = (!fromState || fromState == 3) ? 3 : 2;
            break;
        }

        case 4: {
            events = 4;
            break;
        }

        case 5: {
            events = 4;

            if (fromState == 4) {
                return;
            }

            break;
        }

        default: {
            return;
        }
    }

    CVehiclePassenger_C::SignalTransitionUIEvents(guid, vehicleGuid, vehicleRec, seatRec, events);
}

// PROVEN: 02fee933 IDENTICAL/NONE
// OFFSET: 0x005EFF50
CVehiclePassenger_C::CVehiclePassenger_C() {
}

// OFFSET: 0x005F0000
void CVehiclePassenger_C::Init(CGUnit_C* unit) {
    WOWGUID transport;
    transport.m_guid = unit->GetTransportGUID();
    uint32_t high = static_cast<uint32_t>(transport.m_guid >> 32);

    if ((high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(transport.m_guid)))) {
        CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(transport, TYPEMASK_UNIT, __FILE__, __LINE__));
        uint8_t seat = unit->m_movementData.m_transportSeat;
        unit->VehiclePassengerOnUpdateTransport(transport, seat, 1);
        CVehiclePassenger_C* passenger = unit->m_vehiclePassenger;

        if (passenger) {
            passenger->SetState(3, vehicle, seat, passenger->m_seatRec, FrameTime::GetCurTimeMs(), transport, 0);
        }

        return;
    }

    if (!(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        return;
    }

    WOWGUID guid = unit->m_obj->m_guid;
    CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(guid);

    if (!state || !(state->m_flags & 0x200)) {
        return;
    }

    guid = unit->m_obj->m_guid;
    Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_EXITING_VEHICLE);
    Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_EXITED_VEHICLE, "%s", "");
}

// OFFSET: 0x005F0140
void CVehiclePassenger_C::ComputeEndValues(CGUnit_C* vehicle, const VehicleSeatRec* seatRec, uint32_t time, C3Vector& endPosition) {
    this->m_flags |= 0x1;

    if (!seatRec) {
        return;
    }

    CGObject_C* relative = ClntObjMgrObjectPtr(this->m_startRelativeGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (relative) {
        if ((relative->m_obj->m_type >> 3) & 1) {
            C3Vector position = static_cast<CGUnit_C*>(relative)->BuildSmoothPosition();
            this->m_startModelOffset = C3Vector(this->m_startModelPosition.x - position.x, this->m_startModelPosition.y - position.y, this->m_startModelPosition.z - position.z);
            this->m_startOffset = C3Vector(this->m_startPosition.x - position.x, this->m_startPosition.y - position.y, this->m_startPosition.z - position.z);
        } else {
            C44Matrix inverse = relative->GetMatrix().AffineInverse();
            this->m_startModelOffset = this->m_startModelPosition * inverse;
            this->m_startOffset = this->m_startPosition * inverse;
        }
    } else {
        this->m_startModelOffset = C3Vector(0.0f, 0.0f, 0.0f);
        this->m_startOffset = C3Vector(0.0f, 0.0f, 0.0f);
    }

    if (this->m_state == 3) {
        return;
    }

    this->m_targetFacing = this->m_unit->GetFacing();

    while (this->m_targetFacing + 3.1415927f < this->m_smoothFacing) {
        this->m_targetFacing = this->m_targetFacing + 6.2831855f;
    }

    while (this->m_targetFacing - 3.1415927f > this->m_smoothFacing) {
        this->m_targetFacing = this->m_targetFacing - 6.2831855f;
    }

    this->ComputeEndPos(vehicle, seatRec, endPosition, NULL);
    uint32_t state = this->m_state;
    float minDuration;
    float maxDuration;
    float minArcHeight;
    float maxArcHeight;

    switch (state) {
        case 1: {
            float duration = seatRec->m_enterPreDelay < 10.0f ? seatRec->m_enterPreDelay : 10.0f;
            this->m_endTime = this->m_startTime - static_cast<int32_t>(duration * -1000.0f);
            return;
        }

        case 4: {
            float duration = seatRec->m_exitPreDelay < 10.0f ? seatRec->m_exitPreDelay : 10.0f;
            this->m_endTime = this->m_startTime - static_cast<int32_t>(duration * -1000.0f);
            return;
        }

        case 2: {
            this->m_arcGravity = seatRec->m_enterGravity;
            minDuration = seatRec->m_enterMinDuration;
            maxDuration = seatRec->m_enterMaxDuration;
            minArcHeight = seatRec->m_enterMinArcHeight;
            maxArcHeight = seatRec->m_enterMaxArcHeight;
            break;
        }

        default: {
            this->m_arcGravity = seatRec->m_exitGravity;
            minDuration = seatRec->m_exitMinDuration;
            maxDuration = seatRec->m_exitMaxDuration;
            minArcHeight = seatRec->m_exitMinArcHeight;
            maxArcHeight = seatRec->m_exitMaxArcHeight;
            break;
        }
    }

    if (maxDuration < 0.0001f) {
        maxDuration = 1.5f;
    } else if (maxDuration > 10.0f) {
        maxDuration = 10.0f;
    }

    float duration;

    if (state == 2 && vehicle) {
        C3Vector velocity = vehicle->GetVelocity();
        C3Vector direction = endPosition - this->m_startModelPosition;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

        if (distance <= 0.0001f) {
            duration = 0.0f;
            this->m_endTime = this->m_startTime;
        } else {
            float speed = seatRec->m_enterSpeed - (direction.z * (1.0f / distance) * velocity.z + direction.y * (1.0f / distance) * velocity.y + 1.0f / distance * direction.x * velocity.x);

            if (speed <= 0.0001f) {
                duration = maxDuration;
            } else {
                duration = distance / speed;
                ClampInPlace(duration, minDuration, maxDuration);
            }

            this->m_endTime = this->m_startTime - static_cast<int32_t>(duration * -1000.0f);
        }
    } else {
        C3Vector position = this->m_unit->GetPosition();
        float dz = position.z - this->m_startModelPosition.z;
        float dy = position.y - this->m_startModelPosition.y;
        float dx = position.x - this->m_startModelPosition.x;
        duration = std::sqrt(dx * dx + (dy * dy + dz * dz)) / seatRec->m_exitSpeed;
        ClampInPlace(duration, minDuration, maxDuration);
        this->m_endTime = this->m_startTime - static_cast<int32_t>(duration * -1000.0f);
    }

    if (std::fabs(this->m_arcGravity) < 0.0001f && (minArcHeight > 0.0f || maxArcHeight < 0.0f)) {
        this->m_arcGravity = 0.1f;
    }

    if (duration <= 0.0001f) {
        this->m_arcScale = 1.0f;
    } else {
        float height = this->m_arcGravity * duration * duration * 0.125f;

        if (minArcHeight > height) {
            this->m_arcScale = minArcHeight / height;
        } else if (height > maxArcHeight) {
            this->m_arcScale = maxArcHeight / height;
        } else {
            this->m_arcScale = 1.0f;
        }
    }

    if (this->m_unit->m_vehicleCamera) {
        this->m_unit->m_vehicleCamera->OnPassengerTransitionMotionBegin(time, static_cast<int32_t>(duration * 1000.0f));
    }

    this->UpdateBlend(time, seatRec);

    if (vehicle && vehicle->m_fadeDuration && this->IsEntering()) {
        vehicle->DoFade(vehicle->GetBaseAlpha(), this->m_endTime - this->m_startTime);
    }
}

// OFFSET: 0x005F08F0
void CVehiclePassenger_C::StartDeferringVehicleTransitionAnimSpline(CDataStore* msg, WOWGUID transport, uint8_t seat) {
    VehicleAnimDeferredSpline* deferred = this->m_deferredSplines.New();
    CDataStore* spline = new (SMemAlloc(sizeof(CDataStore), __FILE__, __LINE__, 0)) CDataStore();
    deferred->m_msg = spline;

    uint32_t bytes = msg->m_size - msg->m_read;
    void* data;
    msg->GetDataInSitu(data, bytes);
    spline->PutData(data, bytes);
    deferred->m_msg->Finalize();
    deferred->m_transport = transport;
    deferred->m_seat = seat;
}

// OFFSET: 0x005F09C0
void CVehiclePassenger_C::DeleteAllDeferredVehicleTransitionAnimSplines() {
    for (uint32_t i = 0; i < this->m_deferredSplines.Count(); i++) {
        if (this->m_deferredSplines[i].m_msg) {
            delete this->m_deferredSplines[i].m_msg;
        }
    }

    this->m_deferredSplines.m_count = 0;
}

// OFFSET: 0x005F0A00/0x140272C60
int32_t CVehiclePassenger_C::ExecuteAllDeferredVehicleTransitionAnimSplines() {
    uint32_t count = this->m_deferredSplines.Count();

    if (!count) {
        return 0;
    }

    VehicleAnimDeferredSpline* splines = static_cast<VehicleAnimDeferredSpline*>(alloca(sizeof(VehicleAnimDeferredSpline) * count));

    for (uint32_t i = 0; i < count; i++) {
        new (&splines[i]) VehicleAnimDeferredSpline(this->m_deferredSplines[i]);
    }

    this->m_deferredSplines.m_count = 0;

    for (uint32_t i = 0; i < count; i++) {
        if (splines[i].m_msg) {
            this->m_unit->OnMonsterMove(splines[i].m_msg, splines[i].m_transport ? SMSG_MONSTER_MOVE_TRANSPORT : SMSG_ON_MONSTER_MOVE, splines[i].m_transport.m_guid, splines[i].m_seat, 0);
        }
    }

    for (uint32_t i = 0; i < count; i++) {
        if (splines[i].m_msg) {
            delete splines[i].m_msg;
        }
    }

    return 1;
}

// OFFSET: 0x005F0B00/0x140272DB0
void CVehiclePassenger_C::UpdateModel() {
    if ((this->m_state == 2 || this->m_state == 5) && !(this->m_flags & 0x1)) {
        return;
    }

    const VehicleSeatRec* seatRec = this->m_seatRec;
    CGUnit_C* vehicle;

    if (this->m_transitionVehicleGuid) {
        vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_transitionVehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    } else {
        vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_vehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    this->m_flags &= ~0x10;
    CM2Model* model = this->m_unit->GetObjectModel();

    if (!model) {
        return;
    }

    if (!this->m_state) {
        if (model->m_attachParent) {
            model->DetachFromParent();
        }

        C3Vector normal = this->m_unit->m_movement->GetGroundNormal();
        float scale = this->m_unit->GetTrueScale();
        float facing = this->m_unit->GetFacing();
        model->SetWorldTransform(this->m_unit->GetPosition(), facing, scale, normal, 0xFFFFFFFF);
        return;
    }

    C3Vector start;

    if (this->m_state == 3 || !this->m_startRelativeGuid) {
        start = this->m_startModelPosition;
    } else {
        start = this->ComputeStartWorldPosition(this->m_startModelOffset, this->m_startModelPosition);
    }

    C44Matrix matrix;
    int32_t useMatrix = 0;
    C3Vector position(0.0f, 0.0f, 0.0f);
    C3Vector up(0.0f, 0.0f, 0.0f);
    float facing;
    float scale;
    CM2Model* parent;
    uint32_t attachment;

    if (!seatRec || !this->m_state) {
        position = this->m_unit->GetPosition();
        facing = this->m_unit->GetFacing();
        scale = this->m_unit->GetTrueScale();
        up = this->m_unit->m_movement->GetGroundNormal();
        parent = NULL;
        attachment = 0;
    } else if (this->IsInTransitionMotion()) {
        float arc;
        float total = static_cast<int32_t>(this->m_endTime - this->m_startTime) * 0.001f;

        if (std::fabs(this->m_arcGravity) <= 0.0001f) {
            arc = 0.0f;
        } else {
            float elapsed = this->m_facingBlend * total;
            float gravity = this->m_arcGravity * elapsed * 0.5f;
            arc = (total * gravity - elapsed * gravity) * this->m_arcScale;
        }

        C3Vector end(0.0f, 0.0f, 0.0f);
        C3Vector endUp;
        float blendTime = total < 0.225f ? total : 0.225f;
        float remaining = total - this->m_facingBlend * total;

        if (blendTime < remaining) {
            this->ComputeEndPos(vehicle, seatRec, end, NULL);
            up = C3Vector(0.0f, 0.0f, 1.0f);
        } else {
            this->ComputeEndPos(vehicle, seatRec, end, &up);
            float t = remaining / blendTime;
            t = t < 0.0f ? 0.0f : (t < 1.0f ? t : 1.0f);
            float inverse = 1.0f - t;
            C3Vector ground = this->m_unit->GetGroundNormal();
            C3Vector normalized = up.Normalized();
            up.x = (normalized.x - ground.x) * inverse + ground.x;
            up.y = (normalized.y - ground.y) * inverse + ground.y;
            up.z = (normalized.z - ground.z) * inverse + ground.z;
            float lengthSq = up.z * up.z + up.y * up.y + up.x * up.x;

            if (lengthSq <= 0.00000023841858f) {
                up = C3Vector(0.0f, 0.0f, 1.0f);
            } else {
                float invLength = 1.0f / std::sqrt(lengthSq);
                up.x = up.x * invLength;
                up.y = up.y * invLength;
                up.z = invLength * up.z;
            }
        }

        float blend = this->m_facingBlend;
        position.x = start.x * (1.0f - blend) + end.x * blend;
        position.y = start.y * (1.0f - blend) + end.y * blend;
        position.z = start.z * (1.0f - blend) + end.z * blend + arc;
        facing = this->m_smoothFacing;
        scale = this->m_unit->GetTrueScale();
        parent = NULL;
        attachment = 0;
    } else if (this->m_state == 3) {
        if (!vehicle) {
            return;
        }

        if (!(this->m_flags & 0x20)) {
            this->ComputeSeatOffset(seatRec, model);
        }

        CM2Model* vehicleModel = vehicle->GetObjectModel();

        if (vehicleModel && vehicleModel->IsLoaded()) {
            attachment = static_cast<uint32_t>(seatRec->m_attachmentID) >= 26 ? 0xFFFFFFFF : s_seatAttachments[seatRec->m_attachmentID];
            this->ComputeModelTransform(matrix, vehicleModel, vehicle, seatRec, attachment, this->m_unit->m_smoothTarget.m_value);
            parent = vehicleModel;
            useMatrix = 1;
        } else {
            position = this->m_unit->GetPosition();
            facing = this->m_unit->GetFacing();
            scale = this->m_unit->GetTrueScale();
            up = this->m_unit->GetGroundNormal();
            parent = NULL;
            attachment = 0;
        }
    } else {
        if (!(this->m_flags & 0x20)) {
            this->ComputeSeatOffset(seatRec, model);
        }

        CGUnit_C* oldVehicle;
        VehicleSeatRec* oldSeat;
        CM2Model* oldModel;

        if (this->m_vehicleGuid && (oldVehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_vehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__))) != NULL && ((oldSeat = oldVehicle->GetVehicleSeatRec(this->m_seat)), (oldModel = oldVehicle->GetObjectModel()), oldSeat) && oldSeat->m_attachmentID >= 0 && oldModel && oldModel->IsLoaded()) {
            attachment = static_cast<uint32_t>(seatRec->m_attachmentID) >= 26 ? 0xFFFFFFFF : s_seatAttachments[oldSeat->m_attachmentID];
            this->ComputeModelTransform(matrix, oldModel, vehicle, oldSeat, attachment, this->m_startTargetFacing);
            parent = oldModel;
            useMatrix = 1;
        } else {
            facing = this->m_startFacing;
            position = start;
            scale = this->m_unit->GetTrueScale();
            up = this->m_unit->GetGroundNormal();
            parent = NULL;
            attachment = 0;
        }
    }

    if (model->m_attachParent != parent || model->m_attachmentId != attachment) {
        if (model->m_attachParent) {
            model->DetachFromParent();
        }

        if (parent) {
            model->AttachToParent(parent, attachment, NULL, 0);
        }
    }

    if (useMatrix) {
        model->m_fFlags |= 0x4000;
        model->m_worldTransform = matrix;
    } else {
        model->SetWorldTransform(position, facing, scale, up, 3);
    }

    if (this->m_unit->m_modelFlags & 0x40000) {
        this->m_flags |= 0x10;
    } else {
        this->m_flags &= ~0x10;
    }
}

// OFFSET: 0x005F12F0
int32_t CVehiclePassenger_C::PostSetState(CGUnit_C* vehicle, const VehicleSeatRec* seatRec, uint32_t time) {
    C3Vector endPosition(0.0f, 0.0f, 0.0f);

    if (this->m_state) {
        this->ComputeEndValues(vehicle, seatRec, time, endPosition);
    }

    uint32_t state = this->m_state;

    if (state && state != 3) {
        float dz = endPosition.z - this->m_startModelPosition.z;
        float dy = endPosition.y - this->m_startModelPosition.y;
        float dx = endPosition.x - this->m_startModelPosition.x;

        if (dz * dz + dy * dy + dx * dx > 22500.0f) {
            WOWGUID transport;
            transport.m_guid = this->m_unit->GetTransportGUID();

            if (state == 1 || state == 2) {
                this->SetState(3, vehicle, this->m_transitionSeat, seatRec, time, transport, 1);
            } else {
                this->SetState(0, vehicle, this->m_transitionSeat, seatRec, time, transport, 1);
            }

            this->m_flags |= 0x1;
        }
    }

    VehicleRec* vehicleRec = this->m_vehicleRec;
    WOWGUID guid = this->m_unit->m_obj->m_guid;

    if (ClntObjMgrGetActivePlayer() == guid || CGPartyInfo::IsPlayerOrPetInParty(guid) || CGRaidInfo::IsPlayerOrPetInRaid(guid)) {
        CVehiclePassenger_C::SendTransitionUIEvents(guid, this->m_transitionVehicleGuid, vehicleRec, seatRec, this->m_prevState, this->m_state);
    }

    if (vehicle) {
        CGUnit_C* root = vehicle->GetRootVehicleUnit();

        if (root && (!this->m_state || !this->m_prevState || this->m_state == 3 || this->m_prevState == 3)) {
            CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player && player->GetRootVehicleUnit() == root) {
                FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_PASSENGERS_CHANGED, NULL);
            }
        }
    }

    this->m_prevState = this->m_state;

    if (vehicle && vehicle->m_vehicle) {
        vehicle->m_vehicle->UpdateFreeSeatMask();
    }

    return this->m_state || (this->m_flags & 0x200);
}

// OFFSET: 0x005F1530
int32_t CVehiclePassenger_C::UpdateState(CGUnit_C* vehicle, const VehicleSeatRec* seatRec, uint32_t time) {
    if (!(this->m_flags & 0x1) && !this->PostSetState(vehicle, seatRec, time)) {
        return 0;
    }

    if (this->m_state == 1 && static_cast<int32_t>(time - this->m_endTime) >= 0) {
        WOWGUID transport;
        transport.m_guid = this->m_unit->GetTransportGUID();
        this->SetState(2, vehicle, this->m_transitionSeat, seatRec, time, transport, 0);

        if (!this->PostSetState(vehicle, seatRec, time)) {
            return 0;
        }
    }

    if (this->m_state == 2) {
        if (static_cast<int32_t>(time - this->m_endTime) < 0) {
            this->UpdateSmoothFacing();
        } else {
            WOWGUID transport;
            transport.m_guid = this->m_unit->GetTransportGUID();
            this->SetState(3, vehicle, this->m_transitionSeat, seatRec, time, transport, 0);
        }
    }

    if (this->m_state == 4 && static_cast<int32_t>(time - this->m_endTime) >= 0) {
        WOWGUID transport;
        transport.m_guid = this->m_unit->GetTransportGUID();
        this->SetState(5, vehicle, 0xFF, seatRec, time, transport, 0);

        if (!this->PostSetState(vehicle, seatRec, time)) {
            return 0;
        }
    }

    if (this->m_state == 5) {
        if (static_cast<int32_t>(time - this->m_endTime) < 0) {
            this->UpdateSmoothFacing();
        } else {
            WOWGUID transport;
            transport.m_guid = this->m_unit->GetTransportGUID();
            this->SetState(0, vehicle, 0xFF, seatRec, time, transport, 0);
        }
    }

    uint32_t flags = this->m_flags;

    if ((flags & 0x8) && static_cast<int32_t>(time - this->m_stopMovementTime - 2000) >= 0) {
        this->m_flags = flags & ~0x8;
    }

    return 1;
}

// OFFSET: 0x005F1680/0x140273E10
int32_t CVehiclePassenger_C::StopWaitingForVehicleTransitionAnim() {
    if (!(this->m_flags & 0x200)) {
        return 0;
    }

    this->m_flags &= ~0x200;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_transitionAnimVehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && unit->m_vehicle) {
        unit->m_vehicle->UninstallBoneAnimListener(this->m_unit->m_obj->m_guid);
    }

    if (this->m_deferredSplines.Count()) {
        return this->ExecuteAllDeferredVehicleTransitionAnimSplines();
    }

    return 0;
}

// OFFSET: 0x005F1750
void CVehiclePassenger_C::UninitSystem() {
    if (s_passengerPool) {
        delete s_passengerPool;
        s_passengerPool = NULL;
    }

    if (s_pendingPassengers) {
        delete s_pendingPassengers;
        s_pendingPassengers = NULL;
    }

    s_addingPendingRescueTransitions = 0;

    if (s_rescueTransitions) {
        delete s_rescueTransitions;
        s_rescueTransitions = NULL;
    }

    s_tailPassenger = NULL;
    s_headPassenger = NULL;
}

// OFFSET: 0x005F1800/0x140273FD0
CVehiclePassenger_C* CVehiclePassenger_C::Alloc(CGUnit_C* unit) {
    CVehiclePassenger_C* passenger = s_passengerPool->New("", 0);
    passenger->m_unit = unit;
    passenger->m_flags = 0;
    passenger->m_state = 0;
    passenger->m_prevState = 0;
    passenger->m_vehicleGuid = WOWGUID();
    passenger->m_transitionVehicleGuid = WOWGUID();
    passenger->m_startTime = 0;
    passenger->m_endTime = 0;
    passenger->m_startRelativeGuid = WOWGUID();
    passenger->m_facingBlend = 0.0f;
    passenger->m_stopMovementTime = 0;
    passenger->m_vehicleRec = NULL;
    passenger->m_seatRec = NULL;
    passenger->m_rootVehicleGuid = WOWGUID();
    passenger->m_transitionAnimTimeout = 0;
    passenger->m_transitionAnimVehicleGuid = WOWGUID();
    passenger->m_boneAnimGuid = WOWGUID();
    passenger->m_boneAnimBone = 35;
    passenger->m_seat = 0xFF;
    passenger->m_transitionSeat = 0xFF;
    passenger->m_startModelPosition = C3Vector();
    passenger->m_startModelOffset = C3Vector();
    passenger->m_startPosition = C3Vector();
    passenger->m_startOffset = C3Vector();
    passenger->m_startFacing = 0.0f;
    passenger->m_startTargetFacing = 0.0f;
    passenger->m_smoothFacing = 0.0f;
    passenger->m_targetFacing = 0.0f;
    passenger->m_arcGravity = 0.0f;
    passenger->m_arcScale = 0.0f;
    passenger->m_seatOffset = C3Vector();
    passenger->m_lockedWorldFacing = 0.0f;
    passenger->m_animKit = NULL;
    passenger->m_seatAnimKit = NULL;
    passenger->m_trainCarPosition = C3Vector();
    passenger->m_prev = NULL;

    CVehiclePassenger_C* head = s_headPassenger;
    s_headPassenger = passenger;

    if (!head) {
        passenger->m_next = NULL;
        s_tailPassenger = passenger;
    } else {
        passenger->m_next = head;
        head->m_prev = passenger;
    }

    return passenger;
}

// PROVEN: 7d23fe69 IDENTICAL/NONE
// OFFSET: 0x005F1980
void CVehiclePassenger_C::Free() {
    if (this == s_headPassenger) {
        s_headPassenger = this->m_next;

        if (s_headPassenger) {
            s_headPassenger->m_prev = NULL;
        }
    } else if (this == s_tailPassenger) {
        s_tailPassenger = this->m_prev;

        if (s_tailPassenger) {
            s_tailPassenger->m_next = NULL;
        }
    } else {
        this->m_prev->m_next = this->m_next;
        this->m_next->m_prev = this->m_prev;
    }

    this->FreeAnimKit();
    this->DeleteAllDeferredVehicleTransitionAnimSplines();
    s_passengerPool->Delete(this);
}

// PROVEN: 39d00fcc IDENTICAL/NONE
// OFFSET: 0x005F19F0
void CVehiclePassenger_C::Uninit() {
    if (this->m_flags & 0x8800) {
        this->StopMirroringVehicleAnims(0);
    }

    CVehicle_C* vehicle = this->GetVehicle();

    if (vehicle) {
        vehicle->UpdateFreeSeatMask();
    }

    this->Free();
}

// OFFSET: 0x005F1A60
void CVehiclePassenger_C::Update(uint32_t time) {
    const VehicleSeatRec* seatRec = this->m_seatRec;
    CGUnit_C* vehicle;

    if (this->m_transitionVehicleGuid) {
        vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_transitionVehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    } else {
        vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_vehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    this->UpdateBlend(time, seatRec);

    if (!this->UpdateState(vehicle, seatRec, time)) {
        this->m_unit->VehiclePassengerOnUninit();
        this->Uninit();
    }
}

// OFFSET: 0x005F1AE0
void CVehiclePassenger_C::UpdateAll(uint32_t time) {
    for (CVehiclePassenger_C* passenger = CVehiclePassenger_C::s_headPassenger; passenger; ) {
        CVehiclePassenger_C* next = passenger->m_next;
        passenger->Update(time);
        passenger = next;
    }
}

// OFFSET: 0x005F1B10/0x1402743B0
void CVehiclePassenger_C::BeginTransition(WOWGUID vehicleGuid, uint8_t seat, uint8_t instant) {
    this->StopWaitingForVehicleTransitionAnim();

    switch (this->m_state) {
        case 0:
        case 5: {
            this->m_vehicleGuid = WOWGUID();
            this->m_seat = 0xFF;
            break;
        }

        case 2:
        case 3: {
            this->m_vehicleGuid = this->m_transitionVehicleGuid;
            this->m_seat = this->m_transitionSeat;
            break;
        }
    }

    uint32_t high = static_cast<uint32_t>(vehicleGuid.m_guid >> 32);
    int32_t entering;
    WOWGUID target;

    if ((high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(vehicleGuid.m_guid)))) {
        target = vehicleGuid;
        entering = 1;
    } else {
        target = WOWGUID();
        entering = 0;
    }

    this->m_flags &= ~0x1;
    const VehicleSeatRec* seatRec = this->m_seatRec;
    this->m_transitionVehicleGuid = target;
    this->m_transitionSeat = seat;
    uint32_t state = entering ? 3 : 0;

    if (instant && seatRec) {
        if (entering) {
            if ((seatRec->m_flags & 0x1) && seatRec->m_enterSpeed > 0.0001f) {
                state = seatRec->m_enterPreDelay <= 0.0001f ? 2 : 1;
            }
        } else if (this->m_unit->ShouldUseVehicleSeatExitAnimation(seatRec) && seatRec->m_exitSpeed > 0.0001f) {
            state = seatRec->m_exitPreDelay <= 0.0001f ? 5 : 4;
        }
    }

    if (this->m_unit->m_unit->m_unitFieldHealth > 0) {
        if (state == 1 || state == 2) {
            this->PlayTransitionAnimKit(seatRec, 1, this->m_transitionSeat);
        } else if (state == 4 || state == 5) {
            this->PlayTransitionAnimKit(seatRec, 0, this->m_seat);
        }
    }

    WOWGUID lookup = entering ? vehicleGuid : this->m_vehicleGuid;
    CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(lookup, TYPEMASK_UNIT, __FILE__, __LINE__));
    this->SetState(state, vehicle, seat, seatRec, FrameTime::GetCurTimeMs(), vehicleGuid, 0);
}

// OFFSET: 0x005F1CD0
void CVehiclePassenger_C::ExecutePendingRescueTransitions() {
    for (uint32_t i = 0; i < CVehiclePassenger_C::s_rescueTransitions->Count(); i++) {
        VehiclePassengerRescueTransition& transition = CVehiclePassenger_C::s_rescueTransitions->m_data[i];
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(transition.m_passengerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && unit->m_vehiclePassenger) {
            unit->m_vehiclePassenger->BeginTransition(transition.m_vehicleGuid, transition.m_seat, transition.m_instant);
            unit->VehicleCameraOnUpdateTransport(transition.m_vehicleGuid, transition.m_seat);
        }
    }

    CVehiclePassenger_C::s_rescueTransitions->m_count = 0;
    CVehiclePassenger_C::s_addingPendingRescueTransitions = 0;
}

// PROVEN: 94e6439f IDENTICAL/NONE
// OFFSET: 0x005F1D80/0x140274670
int32_t CVehiclePassenger_C::CheckForVehicleTransitionAnimTimeout() {
    if (!(this->m_flags & 0x200) || static_cast<int32_t>(FrameTime::GetCurTimeMs() - this->m_transitionAnimTimeout) < 0) {
        return 0;
    }

    this->StopWaitingForVehicleTransitionAnim();
    return 1;
}

// OFFSET: 0x005F1DB0/0x1402746B0
void CVehiclePassenger_C::InitSystem() {
    delete s_passengerPool;
    delete s_pendingPassengers;
    void* pool = SMemAlloc(sizeof(TSObjectAllocPool<CVehiclePassenger_C>), __FILE__, __LINE__, 0);
    s_passengerPool = pool ? new (pool) TSObjectAllocPool<CVehiclePassenger_C>("Vehicle Passenger", 16) : NULL;
    void* pending = SMemAlloc(sizeof(TSGrowableArray<CVehiclePassenger_C*>), __FILE__, __LINE__, 0);
    s_pendingPassengers = pending ? new (pending) TSGrowableArray<CVehiclePassenger_C*>() : NULL;
    s_addingPendingRescueTransitions = 0;
    void* rescue = SMemAlloc(sizeof(TSGrowableArray<VehiclePassengerRescueTransition>), __FILE__, __LINE__, 0);
    s_headPassenger = NULL;
    s_tailPassenger = NULL;
    s_rescueTransitions = rescue ? new (rescue) TSGrowableArray<VehiclePassengerRescueTransition>() : NULL;
}

// OFFSET: 0x005F1ED0/0x140274880
void CVehiclePassenger_C::OnUpdateTransport(WOWGUID transport, uint8_t seat, CGUnit_C* vehicle, int32_t deferred) {
    if (transport) {
        if (vehicle) {
            this->m_vehicleRec = vehicle->m_vehicle ? vehicle->m_vehicle->m_vehicleRec : NULL;
            this->m_seatRec = vehicle->m_vehicle ? vehicle->m_vehicle->GetVehicleSeatRec(seat) : NULL;
        } else {
            this->m_vehicleRec = NULL;
            this->m_seatRec = NULL;
        }
    }

    CGUnit_C* root;

    if (vehicle && (root = vehicle->FindRootVehicle(NULL)) != NULL) {
        this->StoreRootVehicle(root->m_obj->m_guid);
    } else {
        this->StoreRootVehicle(WOWGUID());
    }

    uint32_t high = static_cast<uint32_t>(transport.m_guid >> 32);
    int32_t exiting = (high & 0xF0F00000) != 0xF0500000 && ((high & 0xF0000000) || !((high & 0xF07FFFFF) | static_cast<uint32_t>(transport.m_guid)));
    int32_t allowed = this->m_unit->VehiclePassengerIsTransitionAllowed(this->m_seatRec, exiting);

    if (this->m_vehicleGuid && !vehicle && CVehiclePassenger_C::s_addingPendingRescueTransitions) {
        CVehiclePassenger_C::AddPendingRescueTransition(this->m_unit->m_obj->m_guid, transport, seat, allowed);
        return;
    }

    if (!deferred) {
        this->BeginTransition(transport, seat, allowed);
    }

    this->m_unit->VehicleCameraOnUpdateTransport(transport, seat);
}

// OFFSET: #0x007485B0
int32_t CVehiclePassenger_C::ChooseSeatExitAnim(uint32_t flags, uint32_t* anim) const {
    uint32_t state = this->m_state;

    if (state != 3 && this->m_unit->m_unit->m_unitFieldHealth > 0) {
        return 0;
    }

    const VehicleSeatRec* seat = this->m_seatRec;

    if (!seat || state != 3 || !(seat->m_flags & 0x4)) {
        return 0;
    }

    int32_t exitAnim;

    if ((this->m_flags & 0x4) || seat->m_rideUpperAnimStart == -1) {
        exitAnim = seat->m_rideUpperAnimLoop;

        if (exitAnim == -1) {
            return 0;
        }
    } else {
        exitAnim = seat->m_rideUpperAnimStart;
    }

    if (exitAnim != ANIM_COUNT) {
        *anim = exitAnim;
        return 1;
    }

    return 0;
}

// OFFSET: %0x005F9AF0
int32_t CVehiclePassenger_C::GetUpperAnim(const VehicleSeatRec* seat) const {
    if (!seat || this->m_state != 3 || !(seat->m_flags & 0x4)) {
        return ANIM_COUNT;
    }

    int32_t anim;

    if ((this->m_flags & 0x4) || (anim = seat->m_rideUpperAnimStart) == -1) {
        anim = seat->m_rideUpperAnimLoop;

        if (anim == -1) {
            return ANIM_COUNT;
        }
    }

    return anim;
}

// OFFSET: #0x007479E0
void CVehiclePassenger_C::StopBoneAnimIfSameBone(WOWGUID guid, const VehicleSeatRec* seat) {
    if (guid != this->m_boneAnimGuid) {
        return;
    }

    int32_t enterBone = seat->m_vehicleRideAnimLoopBone;

    if (enterBone >= 35) {
        enterBone = 26;
    }

    int32_t exitBone = seat->m_vehicleExitAnimBone;

    if (exitBone >= 35) {
        exitBone = 26;
    }

    if (enterBone == exitBone) {
        this->m_boneAnimGuid = WOWGUID();
    }
}

// OFFSET: #0x00748560
int32_t CVehiclePassenger_C::PickHighPriorityAnimation(int32_t a2, uint32_t* anim) const {
    if ((this->m_state == 0 || this->m_state == 3) && this->m_unit->m_unit->m_unitFieldHealth > 0) {
        return 0;
    }

    if (!this->m_seatRec) {
        return 0;
    }

    int32_t lower = this->GetLowerAnim(this->m_seatRec);

    if (lower == ANIM_COUNT) {
        return 0;
    }

    *anim = lower;
    return 1;
}

// OFFSET: #0x007482A0
C3Vector CVehiclePassenger_C::ComputeInterpolatedUnitPosition() const {
    switch (this->m_state) {
        case 1:
        case 4:
            if (this->m_flags & 0x1) {
                return this->ComputeStartWorldPosition(this->m_startOffset, this->m_startPosition);
            }

            return this->m_startPosition;
        case 2:
        case 5:
            if (this->m_flags & 0x1) {
                float blend = this->m_facingBlend;
                C3Vector smooth = this->m_unit->BuildSmoothPosition();
                C3Vector end(blend * smooth.x, smooth.y * blend, blend * smooth.z);
                float inverse = 1.0f - this->m_facingBlend;
                C3Vector start = this->ComputeStartWorldPosition(this->m_startOffset, this->m_startPosition);
                return C3Vector(start.x * inverse + end.x, start.y * inverse + end.y, start.z * inverse + end.z);
            }

            return this->m_startPosition;
        case 3:
            return this->m_unit->BuildSmoothPosition();
        default:
            return this->m_unit->GetPosition();
    }
}

// PROVEN: a73a1a69 IDENTICAL/NONE
// OFFSET: 0x005ACC20
int32_t CVehiclePassenger_C::IsInTransitionMotion() const {
    return this->m_state == 2 || this->m_state == 5;
}

// PROVEN: 53d64a55 IDENTICAL/NONE
// OFFSET: 0x005EB710
int32_t CVehiclePassenger_C::IsEntering() const {
    return this->m_state == 1 || this->m_state == 2;
}
