#include "WoW/Source/UIUtil/CameraMode.h"
#include <cmath>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/Rec/CameraModeRec.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/StaticSingleton.h"
#include "WoW/Source/UIUtil/CameraManager.h"

// STATIC: 0x00D3A7D8/0x140BF5AE8
static CDataAllocator s_lockedInstances(sizeof(CCameraModeInstanceLocked), 8);

// PROVEN: f351b294 UNPROVEN/IDENTICAL
// OFFSET: 0x00463640/0x140076CD0
CCameraModeLocked::CCameraModeLocked(const CameraModeRec* rec) : CCameraMode(rec) {
}

// INLINED
CCameraModeInstanceLocked::CCameraModeInstanceLocked() {
    this->m_resetRequested = 0;
    this->m_twoTargets = 0;
}

// OFFSET: 0x00463660/0x140076D00
CCameraModeInstance* CCameraModeLocked::AllocInstance(const CCameraModeInstance& instance) const {
    CCameraModeInstanceLocked* result = s_lockedInstances.New<CCameraModeInstanceLocked>(0);
    result->Init(instance);
    return result;
}

// PROVEN: 470ffd38 IDENTICAL/IDENTICAL
// OFFSET: 0x004636C0/0x140076D80
void CCameraModeLocked::FreeInstance(CCameraModeInstance* instance) const {
    if (instance) {
        s_lockedInstances.PutData(instance, NULL, 0);
    }
}

// OFFSET: 0x00463710/0x140076DF0
int32_t CCameraModeLocked::IsTracking(const CCameraModeInstance* instance, WOWGUID guid) const {
    const CCameraModeInstanceLocked* locked = static_cast<const CCameraModeInstanceLocked*>(instance);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(locked->m_target.m_targetGUID, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && unit->IsVehiclePassenger(guid)) {
        return 1;
    }

    unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(locked->m_secondTarget.m_targetGUID, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && unit->IsVehiclePassenger(guid)) {
        return 1;
    }

    return CCameraMode::IsTracking(instance, guid) != 0;
}

// OFFSET: 0x004637B0/0x140076EA0
C3Vector CCameraModeLocked::CalcOffsetTwoTargets(const C3Vector& offset, const C44Matrix& target, const C44Matrix& secondTarget, CAMERA_MODE_LOCKED_OFFSET_BASE base, CAMERA_MODE_LOCKED_OFFSET_DIRECTION direction, int32_t flatten) const {
    if (base == CAMERA_MODE_LOCKED_OFFSET_BASE_WORLD) {
        return offset;
    }

    C3Vector local(offset.y, -offset.x, offset.z);
    CCameraShot shot;

    if (base == CAMERA_MODE_LOCKED_OFFSET_BASE_SECOND_TARGET) {
        shot.m_position = C3Vector(secondTarget.d0, secondTarget.d1, secondTarget.d2);
    } else {
        shot.m_position = C3Vector(target.d0, target.d1, target.d2);
    }

    C3Vector facing;

    if (direction == CAMERA_MODE_LOCKED_OFFSET_DIRECTION_SECOND_TARGET) {
        facing = C3Vector(secondTarget.a0, secondTarget.a1, secondTarget.a2);
    } else if (direction == CAMERA_MODE_LOCKED_OFFSET_DIRECTION_TO_SECOND_TARGET || direction == CAMERA_MODE_LOCKED_OFFSET_DIRECTION_FROM_SECOND_TARGET) {
        if (direction == CAMERA_MODE_LOCKED_OFFSET_DIRECTION_TO_SECOND_TARGET) {
            facing = C3Vector(secondTarget.d0 - target.d0, secondTarget.d1 - target.d1, secondTarget.d2 - target.d2);
        } else {
            facing = C3Vector(target.d0 - secondTarget.d0, target.d1 - secondTarget.d1, target.d2 - secondTarget.d2);
        }

        if (sqrtf(facing.x * facing.x + facing.y * facing.y + facing.z * facing.z) < 0.001f) {
            facing = C3Vector(target.a0, target.a1, target.a2);
        }
    } else {
        facing = C3Vector(target.a0, target.a1, target.a2);
    }

    if (flatten) {
        facing.z = 0.0f;
    }

    if (sqrtf(facing.y * facing.y + facing.x * facing.x + facing.z * facing.z) < 0.001f) {
        facing = C3Vector(1.0f, 0.0f, 0.0f);
    }

    shot.m_target = shot.m_position + facing;
    C44Matrix view = shot.CalcViewMatrix();
    return local * view;
}

// OFFSET: 0x00463A40/0x1400772B0
void CCameraModeLocked::CalcTargetCameraShot(CCameraModeInstance* instance, CCameraShot& shot) const {
    CCameraModeInstanceLocked* locked = static_cast<CCameraModeInstanceLocked*>(instance);
    const CameraModeRec* rec = this->m_rec;
    C3Vector positionOffset = rec->m_positionOffset;
    C3Vector targetOffset = rec->m_targetOffset;
    locked->m_shot.m_fov = rec->m_fieldOfView * 0.017453292f;
    WOWGUID guid = locked->m_guid;
    int32_t changed = 0;

    locked->m_target.UpdateTarget(guid);

    if (locked->m_target.m_currentTargetGUID != locked->m_target.m_targetGUID) {
        changed = 1;
    }

    rec = this->m_rec;

    if (rec->m_lockedPositionOffsetBase == CAMERA_MODE_LOCKED_OFFSET_BASE_SECOND_TARGET || rec->m_lockedTargetOffsetBase == CAMERA_MODE_LOCKED_OFFSET_BASE_SECOND_TARGET || rec->m_lockedPositionOffsetDirection || rec->m_lockedTargetOffsetDirection) {
        locked->m_secondTarget.UpdateTarget(guid);

        if (!locked->m_resetRequested && locked->m_secondTarget.m_currentTargetGUID != locked->m_secondTarget.m_targetGUID) {
            if (!locked->m_twoTargets || (locked->m_secondTarget.m_currentTargetGUID && (this->m_rec->m_flags & 0x4)) || (!locked->m_secondTarget.m_currentTargetGUID && (this->m_rec->m_flags & 0x8))) {
                changed = 1;
            }
        }
    }

    if (locked->m_twoTargets) {
        shot.m_position = this->CalcOffsetTwoTargets(positionOffset, locked->m_target.m_matrix, locked->m_secondTarget.m_matrix, static_cast<CAMERA_MODE_LOCKED_OFFSET_BASE>(this->m_rec->m_lockedPositionOffsetBase), static_cast<CAMERA_MODE_LOCKED_OFFSET_DIRECTION>(this->m_rec->m_lockedPositionOffsetDirection), this->m_rec->m_flags & 0x1);
        shot.m_target = this->CalcOffsetTwoTargets(targetOffset, locked->m_target.m_matrix, locked->m_secondTarget.m_matrix, static_cast<CAMERA_MODE_LOCKED_OFFSET_BASE>(this->m_rec->m_lockedTargetOffsetBase), static_cast<CAMERA_MODE_LOCKED_OFFSET_DIRECTION>(this->m_rec->m_lockedTargetOffsetDirection), this->m_rec->m_flags & 0x2);
    } else {
        CAMERA_MODE_LOCKED_OFFSET_BASE positionBase = this->m_rec->m_lockedPositionOffsetBase == CAMERA_MODE_LOCKED_OFFSET_BASE_WORLD ? CAMERA_MODE_LOCKED_OFFSET_BASE_WORLD : CAMERA_MODE_LOCKED_OFFSET_BASE_TARGET;
        int32_t positionFlatten = this->m_rec->m_flags & 0x1;
        C44Matrix identity;
        shot.m_position = this->CalcOffsetTwoTargets(positionOffset, locked->m_target.m_matrix, identity, positionBase, CAMERA_MODE_LOCKED_OFFSET_DIRECTION_TARGET, positionFlatten);
        CAMERA_MODE_LOCKED_OFFSET_BASE targetBase = this->m_rec->m_lockedTargetOffsetBase == CAMERA_MODE_LOCKED_OFFSET_BASE_WORLD ? CAMERA_MODE_LOCKED_OFFSET_BASE_WORLD : CAMERA_MODE_LOCKED_OFFSET_BASE_TARGET;
        shot.m_target = this->CalcOffsetTwoTargets(targetOffset, locked->m_target.m_matrix, identity, targetBase, CAMERA_MODE_LOCKED_OFFSET_DIRECTION_TARGET, this->m_rec->m_flags & 0x2);
    }

    if (changed && !locked->m_resetRequested) {
        locked->m_resetRequested = 1;
        StaticSingleton<CCameraManager>::Instance()->SetGameCamera(locked, this->GetID(), guid);
    }
}

// OFFSET: 0x00463D00/0x1400775E0
void CCameraModeLocked::InitInstance(CCameraModeInstance* instance) const {
    CCameraModeInstanceLocked* locked = static_cast<CCameraModeInstanceLocked*>(instance);
    C44Matrix view = locked->m_shot.CalcViewMatrix();
    WOWGUID guid = locked->m_guid;
    CCameraTarget::TARGET_TYPE types[4];
    int32_t count = 0;

    if (this->m_rec->m_flags & 0x10) {
        types[count++] = CCameraTarget::TARGET_VEHICLE;
    }

    locked->m_target.SetObject(view, guid, types, count);
    const CameraModeRec* rec = this->m_rec;

    if (rec->m_lockedPositionOffsetBase == CAMERA_MODE_LOCKED_OFFSET_BASE_SECOND_TARGET || rec->m_lockedTargetOffsetBase == CAMERA_MODE_LOCKED_OFFSET_BASE_SECOND_TARGET || rec->m_lockedPositionOffsetDirection || rec->m_lockedTargetOffsetDirection) {
        CCameraTarget::TARGET_TYPE secondTypes[4];
        int32_t secondCount = 0;

        if (rec->m_flags & 0x20) {
            secondTypes[secondCount++] = CCameraTarget::TARGET_VEHICLE;
        }

        secondTypes[secondCount++] = (this->m_rec->m_flags & 0x40) ? CCameraTarget::TARGET_CHANNEL_OBJECT : CCameraTarget::TARGET_UNIT_TARGET;
        locked->m_secondTarget.SetObject(view, guid, secondTypes, secondCount);
        locked->m_twoTargets = locked->m_secondTarget.m_valid;
    }

    this->CalcTargetCameraShot(locked, locked->m_shot);
    locked->m_shot.m_fade = 1.0f;
}

// OFFSET: 0x00463E10/0x140077720
void CCameraModeLocked::UpdateInstance(CCameraModeInstance* instance, float elapsed) const {
    CCameraShot target = instance->m_shot;
    this->CalcTargetCameraShot(instance, target);
    float positionFactor = 1.0f;
    float rotationFactor = 1.0f;
    float smoothing = this->m_rec->m_positionSmoothing;

    if (smoothing > 0.00000011920929f) {
        positionFactor = 1.0f - static_cast<float>(CMath::exp2_(-(elapsed / smoothing) * 1.442695f));
    }

    smoothing = this->m_rec->m_rotationSmoothing;

    if (smoothing > 0.00000011920929f) {
        rotationFactor = 1.0f - static_cast<float>(CMath::exp2_(-(elapsed / smoothing) * 1.442695f));
    }

    CCameraShot position;
    CCameraShot rotation;
    position.LerpEuler(instance->m_shot, target, positionFactor, 0);
    rotation.LerpEuler(instance->m_shot, target, rotationFactor, 0);
    C3Vector facing = rotation.m_target - rotation.m_position;
    instance->m_shot = position;
    instance->m_shot.m_target = instance->m_shot.m_position + facing;

    if (this->m_rec->m_flags & 0x200) {
        StaticSingleton<CCameraManager>::Instance()->ApplyShakes(instance->m_shot);
    }
}

// PROVEN: caeae147 IDENTICAL/IDENTICAL
// OFFSET: 0x00B2CAB0/0x1408476E0
int32_t CCameraModeLocked::GetType() const {
    return CAMERA_MODE_LOCKED;
}

// PROVEN: 109f08e3 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CCameraModeLocked::DestroyInstance(CCameraModeInstance* instance) const {
}
