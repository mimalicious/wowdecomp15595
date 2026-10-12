#include "Camera.h"
#include "Engine/Source/Base/Coordinate.h"
#include <cmath>
#include <cstring>
#include <limits>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/Camera.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CFacet.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/Object/CMoveSpline.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VehicleCamera_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/BarberShop.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/CameraManager.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00C98C58/0x140B30410
float MAX_PITCH_ANGLE = 1.55334306f;

// STATIC: 0x00C98C5C/0x140B30414
float MIN_PITCH_ANGLE = -1.55334306f;

// STATIC: 0x00C98C8C/0x140B30470
int32_t s_cameraViewAngular[3] = { 0, 1, 1 };

// STATIC: 0x00C98CA0/0x140B30490
CAMERA_DEFAULTS s_cameraViewDefaults[8] = {
    { { "0.0", "0.0", "0.0" } },
    { { "0.0", "0.0", "0.0" } },
    { { "5.55", "10.0", "0.0" } },
    { { "5.55", "20.0", "0.0" } },
    { { "13.88", "30.0", "0.0" } },
    { { "13.88", "10.0", "0.0" } },
    { { "0.0", "0.0", "0.0" } },
    { { "5.0", "10.0", "0.0" } },
};

// STATIC: 0x00B8D8A0
const float s_terrainTiltTable[10][2] = {
    { 0.0f, 0.0f },
    { 0.09f, 0.0872664626f },
    { 0.18f, 0.174532925f },
    { 0.27f, 0.261799388f },
    { 0.36f, 0.349065850f },
    { 0.47f, 0.436332313f },
    { 0.58f, 0.523598776f },
    { 0.7f, 0.610865238f },
    { 0.84f, 0.698131701f },
    { 1.0f, 0.785398163f },
};

// STATIC: 0x00D38774/0x140BF2308
float s_targetHeightOffset;

// STATIC: 0x00C99254/0x140B30FC0
static const char* s_cameraViewModeNames[8] = { "FIRST_PERSON", "THIRD_PERSON_A", "THIRD_PERSON_B", "THIRD_PERSON_C", "THIRD_PERSON_D", "THIRD_PERSON_E", "VIEW_COMMENTATOR", "VIEW_BARBER_SHOP" };

// STATIC: 0x00B8D160
static const float s_smoothViewDataMin[2] = { 0.0f, 0.0f };

// STATIC: 0x00B8D168
static const float s_smoothViewDataMax[2] = { 100.0f, 100.0f };

// STATIC: 0x00C99274/0x140B31000
static float s_minCollideHeight = 0.00000095367432f;

// INLINED
static float UnwrapAngleToward(float target, float current, float lo, float hi) {
    while (target - current < lo) {
        current -= 6.2831855f;
    }

    while (target - current > hi) {
        current += 6.2831855f;
    }

    return current;
}

// INLINED
static float WrapAngle0To2Pi(float angle) {
    if (angle < 0.0f) {
        return angle + 6.2831855f;
    }

    if (angle > 6.2831855f) {
        return angle - 6.2831855f;
    }

    return angle;
}

// OFFSET: 0x0043D0A0/0x140048100
float CGCamera::FOV() const {
    float fov = this->m_targetFov + this->m_fov;
    return 0.0f > fov ? 0.0f : (3.1241393f <= fov ? 3.1241393f : fov);
}

// OFFSET: 0x0043D3A0/0x140048430
void CGCamera::UpdateFromModelCamera() {
    C3Vector position;
    C3Vector target;
    CameraGetCoord(this->m_modelCamera, 7, &position);
    CameraGetCoord(this->m_modelCamera, 8, &target);
    float roll = CameraGetFloat(this->m_modelCamera, 5);
    CameraGetFloat(this->m_modelCamera, 4);
    this->SetPositionAndTargetWithRoll(position, target, roll);
}

// OFFSET: 0x0043E790/0x140049B20
C3Vector CGCamera::Forward() const {
    C3Vector forward(this->m_facing.a0, this->m_facing.a1, this->m_facing.a2);

    if (!this->m_relativeTo) {
        return forward;
    }

    return forward * this->ParentToWorld();
}

// OFFSET: 0x0043E880/0x140049C10
C3Vector CGCamera::Right() const {
    C3Vector right(this->m_facing.b0, this->m_facing.b1, this->m_facing.b2);

    if (!this->m_relativeTo) {
        return right;
    }

    return right * this->ParentToWorld();
}

// OFFSET: 0x0043E970/0x140049D00
C3Vector CGCamera::Up() const {
    C3Vector up(this->m_facing.c0, this->m_facing.c1, this->m_facing.c2);

    if (!this->m_relativeTo) {
        return up;
    }

    return up * this->ParentToWorld();
}

// OFFSET: 0x00444170/0x14004FB10
CGCamera::CGCamera() : CSimpleCamera(1.5707964f) {
    this->m_model = NULL;
    this->m_someTime = FrameTime::GetCurTimeMs();
    this->m_modelCamera = NULL;

    this->m_modelMatrix.a0 = 1.0f;
    this->m_modelMatrix.a1 = 0.0f;
    this->m_modelMatrix.a2 = 0.0f;
    this->m_modelMatrix.b0 = 0.0f;
    this->m_modelMatrix.b1 = 1.0f;
    this->m_modelMatrix.b2 = 0.0f;
    this->m_modelMatrix.c0 = 0.0f;
    this->m_modelMatrix.c1 = 0.0f;
    this->m_modelMatrix.c2 = 1.0f;
    this->m_modelMatrix.d0 = 0.0f;
    this->m_modelMatrix.d1 = 0.0f;
    this->m_modelMatrix.d2 = 0.0f;

    this->m_targetGUID = WOWGUID();
    this->m_terrainTilt = 0.0f;
    this->m_state = 0;
    this->m_flags = 0;
    this->m_relativeTo = WOWGUID();
    this->m_ignoreFacingRefs = 0;
    this->m_groundTiltRefs = 0;

    this->m_viewIndex = s_cvCameraView->m_intValue;
    this->m_distance = SStrToFloat(s_cameraViewDefaults[this->m_viewIndex].m_values[0]);
    this->m_yaw = 0.0f;
    this->m_pitch = SStrToFloat(s_cameraViewDefaults[this->m_viewIndex].m_values[1]) * 0.017453292f;
    this->m_roll = 0.0f;
    this->m_height = 0.0f;
    this->m_yawOffset = 0.0f;
    this->m_targetOffset = 0.0f;
    this->m_groundTilt = 0.0f;
    this->m_targetFov = 0.0f;
    this->m_flyingMountHeight = 0.0f;
    this->m_bobOffset.x = 0.0f;
    this->m_bobOffset.y = 0.0f;
    this->m_bobOffset.z = 0.0f;
    this->m_targetHeightNear = 0.0f;
    this->m_heightChangedTimeMs = 0;
    this->m_targetHeightFar = 0.0f;
    this->m_motionFlags = 0;
    this->m_targetHeightSwim = 0.0f;
    this->m_mountHeight = 0.0f;
    this->m_targetPosition.x = 0.0f;
    this->m_targetPosition.y = 0.0f;
    this->m_targetPosition.z = 0.0f;
    this->unk_01BC = 0.0f;
    this->m_targetFacing = 0.0f;
    this->m_terrainTiltTime = 0;
    this->unk_01C8 = 0.0f;
    this->m_fade = 0xFF;

    this->m_smoothDistance.m_startTimeMs = 0;
    this->m_smoothDistance.m_rate = 0.0f;
    this->m_smoothDistance.m_target = this->m_distance;
    this->m_smoothDistance.m_startValue = 0.0f;
    this->m_smoothDistance.m_param3 = 0.0f;
    this->m_smoothDistance.m_param4 = 0.0f;

    this->m_smoothGroundTilt.m_startTimeMs = 0;
    this->m_smoothGroundTilt.m_rate = 0.0f;
    this->m_smoothGroundTilt.m_target = 0.0f;
    this->m_smoothGroundTilt.m_startValue = 0.0f;
    this->m_smoothGroundTilt.m_param3 = 0.0f;
    this->m_smoothGroundTilt.m_param4 = 0.0f;

    this->m_smoothHeight.m_startTimeMs = 0;
    this->m_smoothHeight.m_rate = 0.0f;
    this->m_smoothHeight.m_target = 0.0f;
    this->m_smoothHeight.m_startValue = 0.0f;
    this->m_smoothHeight.m_param3 = 0.0f;
    this->m_smoothHeight.m_param4 = 0.0f;

    this->m_smoothPitch.m_startTimeMs = 0;
    this->m_smoothPitch.m_rate = 0.0f;
    this->m_smoothPitch.m_target = this->m_pitch;
    this->m_smoothPitch.m_startValue = 0.0f;
    this->m_smoothPitch.m_param3 = 0.0f;
    this->m_smoothPitch.m_param4 = 0.0f;

    this->m_smoothTargetOffset.m_startTimeMs = 0;
    this->m_smoothTargetOffset.m_rate = 0.0f;
    this->m_smoothTargetOffset.m_target = 0.0f;
    this->m_smoothTargetOffset.m_startValue = 0.0f;
    this->m_smoothTargetOffset.m_param3 = 0.0f;
    this->m_smoothTargetOffset.m_param4 = 0.0f;

    this->m_smoothYaw.m_startTimeMs = 0;
    this->m_smoothYaw.m_rate = 0.0f;
    this->m_smoothYaw.m_target = 0.0f;
    this->m_smoothYaw.m_startValue = 0.0f;
    this->m_smoothYaw.m_param3 = 0.0f;
    this->m_smoothYaw.m_param4 = 0.0f;

    this->m_smoothFoV.m_startTimeMs = 0;
    this->m_smoothFoV.m_rate = 0.0f;
    this->m_smoothFoV.m_target = 0.0f;
    this->m_smoothFoV.m_startValue = 0.0f;
    this->m_smoothFoV.m_param3 = 1.0f;
    this->m_smoothFoV.m_param4 = 0.0f;

    this->m_bobTime = 0;
    this->m_bobDuration = 0.0f;
    this->m_bobStart.x = 0.0f;
    this->m_bobStart.y = 0.0f;
    this->m_bobStart.z = 0.0f;
    this->unk_0288 = 1;
    this->m_underwater = 0;
    this->m_wasFlying = 0;

    this->m_smoothFlyingHeight.m_startTimeMs = 0;
    this->m_smoothFlyingHeight.m_rate = 0.0f;
    this->m_smoothFlyingHeight.m_target = 0.0f;
    this->m_smoothFlyingHeight.m_startValue = 0.0f;
    this->m_smoothFlyingHeight.m_param3 = 0.0f;
    this->m_smoothFlyingHeight.m_param4 = 0.0f;

    this->m_collideExtent = 1.0f;
    this->m_vehicleZoomEnabled = 0;
    this->m_overrideDistanceMin = 0.0f;
    this->m_overrideDistanceMax = 0.0f;
    this->m_vehicleExitZoomDistance = 0.0f;
    this->m_vehicleYawStartTime = 0;
    this->m_vehicleYawEndTime = 0;
    this->m_vehicleYawFrom = 0.0f;
    this->m_vehicleYawTo = 0.0f;
    this->m_vehicleYawOffset = 0.0f;
    this->m_vehiclePitchStartTime = 0;
    this->m_vehiclePitchEndTime = 0;
    this->m_vehiclePitchFrom = 0.0f;
    this->m_vehiclePitchTo = 0.0f;
    this->m_vehiclePitchOffset = 0.0f;
    this->m_vehicleTrackingStyle = 0;
    this->m_vehicleCamera = NULL;

    memset(this->m_views, 0, sizeof(this->m_views));

    this->m_motionStartTime[0] = 0;
    this->m_motionStartTime[1] = 0;
    this->m_motionStartTime[2] = 0;
    this->m_motionStartTime[3] = 0;
    this->m_motionStartTime[4] = 0;
    this->m_motionStartTime[5] = 0;
    this->m_motionStopTime[0] = 0;
    this->m_motionStopTime[1] = 0;
    this->m_motionStopTime[2] = 0;
    this->m_motionStopTime[3] = 0;
    this->m_motionStopTime[4] = 0;
    this->m_motionStopTime[5] = 0;
    this->m_motionEndTime[0] = 0;
    this->m_motionEndTime[1] = 0;
    this->m_motionEndTime[2] = 0;
    this->m_motionEndTime[3] = 0;
    this->m_motionEndTime[4] = 0;
    this->m_motionEndTime[5] = 0;

    this->InitializeView();
    this->SetTarget(NULL, 0);

    ConsoleCommandRegister("pitchLimit", CGCamera::CCommand_PitchLimit, CATEGORY_GAME, NULL, NULL);

    this->m_state |= 0x50;
    this->m_prevViewIndex = 2;
    this->m_prevDistance = 1.7f;
    this->unk_02F4 = 1.7f;
    this->unk_02F8 = 0.0f;

    if (g_cvAutoInteract->m_intValue) {
        this->m_flags |= 0x1;
    } else {
        this->m_flags &= ~0x1;
    }
}

// OFFSET: 0x0043CDC0/0x140047F10
CGCamera::~CGCamera() {
    this->m_state &= ~0x10;

    if (this->m_model) {
        this->m_state &= ~0x4;

        if (this->m_modelCamera) {
            CameraRelease(this->m_modelCamera);
            this->m_modelCamera = NULL;
        }

        this->m_model->Release();
        this->m_model = NULL;
    }

    ConsoleCommandUnregister("pitchLimit");
}

// OFFSET: 0x00441970/0x14004CDF0
void CGCamera::BarberShopSetView(float distance, float facing, float pitch) {
    this->m_views[7][0] = distance;
    this->m_views[7][2] = facing;
    this->m_views[7][1] = pitch;
    this->SetView(7, 1, 0);
}

// OFFSET: 0x00441BA0/0x14004D150
void CGCamera::CalcTerrainTilt(CGObject_C* target, uint32_t time) {
    if (!target) {
        return;
    }

    if (!(target->m_obj->m_type & TYPEMASK_UNIT) || !s_cvCameraTerrainTilt->m_intValue) {
        this->m_terrainTilt = 0.0f;
        return;
    }

    C3Vector position = this->GetSafeWorldPos(target);
    float facing = target->GetFacing();

    if (static_cast<int32_t>(time - this->m_terrainTiltTime - 100) < 0) {
        return;
    }

    this->m_terrainTiltTime = time;

    C3Vector points[5];
    position.z += 1.6666666f;

    C3Vector direction;
    direction.x = cosf(facing);
    direction.y = sinf(facing);

    points[0] = position;

    C3Vector end;
    end.x = direction.x * 3.3333333f + position.x;
    end.y = direction.y * 3.3333333f + position.y;
    end.z = 3.3333333f * direction.z + position.z;

    points[1] = end;
    points[2] = end;

    float distance = 1.0f;
    World::Intersect(points[0], points[1], &points[2], &distance, 0x151, NULL);

    points[2].x -= direction.x * 0.27777779f;
    points[2].y -= direction.y * 0.27777779f;
    points[2].z -= 0.27777779f * direction.z;

    points[3].x = points[2].x;
    points[3].y = points[2].y;
    points[3].z = points[2].z - 7.1111112f;
    points[4] = points[3];

    distance = 1.0f;
    World::Intersect(points[2], points[3], &points[4], &distance, 0x151, NULL);

    C3Vector delta;
    delta.x = points[2].x - points[0].x;
    delta.y = points[2].y - points[0].y;
    delta.z = points[2].z - points[0].z;

    C2Vector horizontal(delta);
    float slope = (points[4].z - points[2].z + 1.6666666f) / sqrtf(horizontal.y * horizontal.y + horizontal.x * horizontal.x);

    float magnitude;
    float sign;

    if (slope >= 0.0f) {
        magnitude = slope;
        sign = -1.0f;
    } else {
        magnitude = -slope;
        sign = 1.0f;
    }

    uint32_t i = 10;

    do {
        i--;

        if (s_terrainTiltTable[i][0] <= magnitude) {
            float tilt = s_terrainTiltTable[i][1];

            if (tilt <= 0.0f) {
                this->m_terrainTilt = 0.0f;
            } else {
                tilt *= sign;
                this->m_terrainTilt = tilt < -0.34906584f ? -0.34906584f : (tilt >= 0.34906584f ? 0.34906584f : tilt);
            }

            return;
        }
    } while (i);

    this->m_terrainTilt = 0.0f;
}

// OFFSET: 0x0043AD20/0x140045B30
void CGCamera::ClampPitchToLimits(float delta) {
    float target = delta + this->m_smoothPitch.m_target;
    target = MIN_PITCH_ANGLE > target ? MIN_PITCH_ANGLE : (MAX_PITCH_ANGLE <= target ? MAX_PITCH_ANGLE : target);
    this->m_smoothPitch.m_target = target;

    float start = delta + this->m_smoothPitch.m_startValue;
    start = start < MIN_PITCH_ANGLE ? MIN_PITCH_ANGLE : (start < MAX_PITCH_ANGLE ? start : MAX_PITCH_ANGLE);
    this->m_smoothPitch.m_startValue = UnwrapAngleToward(target, start, -3.1415927f, 3.1415927f);

    float pitch = this->m_pitch + delta;
    pitch = MIN_PITCH_ANGLE > pitch ? MIN_PITCH_ANGLE : (MAX_PITCH_ANGLE <= pitch ? MAX_PITCH_ANGLE : pitch);
    this->m_pitch = UnwrapAngleToward(target, pitch, -3.1415927f, 3.1415927f);
}

// OFFSET: 0x0043F760/0x14004A9B0
void CGCamera::DecIgnoreFacing() {
    if (--this->m_ignoreFacingRefs != 0) {
        return;
    }

    float facing = 0.0f;
    CGObject_C* target = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (target) {
        if (target->m_obj->m_type & TYPEMASK_UNIT) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(target);

            if (this->m_vehicleCamera && this->m_vehicleCamera->IsHierarchyChasingFacing()) {
                facing = this->GetChaseFacing(unit);
            } else if (this->m_state & 0x4000) {
                facing = this->unk_01C8;
            } else if (this->m_vehicleCamera) {
                facing = this->GetChaseFacing(unit);
            } else {
                facing = unit->GetRawFacing();
            }
        } else {
            facing = target->GetFacing();
        }
    }

    this->UpdateYaw(-facing);
}

// OFFSET: 0x00443190/0x14004E740
int32_t CGCamera::FinishLoadingTarget(CGObject_C* target) {
    CM2Model* model = target->GetObjectModel();

    if (model && !model->IsLoaded()) {
        return 0;
    }

    this->m_state |= 0x4;
    this->m_targetHeightNear = 0.0f;
    this->m_targetHeightFar = 0.0f;
    this->m_targetHeightSwim = 0.0f;

    float scale = target->GetScale() * target->m_scaleModifier;
    float maxHeight;

    if ((target->m_obj->m_type & TYPEMASK_UNIT) && model) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(target);
        scale = unit->GetDisplayModelScale() * scale;
        float mountScale = scale;

        if (unit->m_mountModel) {
            mountScale = scale * unit->m_mountScale;
        }

        CM2Model* worldModel = unit->m_worldModel;
        uint32_t attachment = 0xFFFFFFFF;
        uint32_t sequence;

        switch (unit->GetClientStandState()) {
            case 1:
                sequence = 97;
                break;

            case 3:
                sequence = 100;
                break;

            case 4:
                sequence = 102;
                break;

            case 5:
                sequence = 103;
                break;

            case 6:
                sequence = 104;
                break;

            case 7:
                sequence = 6;
                break;

            case 8:
                sequence = 115;
                break;

            default:
                sequence = 0;
                break;
        }

        if (s_cvCameraHeightIgnoreStandState->m_intValue) {
            sequence = 0;
        }

        if (unit->m_mountModel) {
            sequence = 91;
            attachment = 0;
        }

        M2SequenceInfo standInfo;
        M2SequenceInfo sequenceInfo;
        standInfo.Clear();
        sequenceInfo.Clear();
        worldModel->GetSequenceInfo(0, 0, standInfo);
        worldModel->GetSequenceInfo(sequence, 0, sequenceInfo);
        standInfo.m_bounds *= scale;
        sequenceInfo.m_bounds *= scale;

        float standOffset = standInfo.m_bounds.t.z - sequenceInfo.m_bounds.t.z;
        float height;

        if (worldModel->HasAttachment(0x11)) {
            C3Vector pivot = worldModel->GetAttachmentPivot(0x11);

            if (CGBarberShop::m_barberShopEnabled && unit->IsActivePlayer()) {
                height = (pivot.z + 0.097222224f - standOffset) * scale;
            } else {
                height = (pivot.z + 0.097222224f) * scale;
            }
        } else {
            if (!(worldModel->m_shared->m_flags & 0x1)) {
                worldModel->WaitForLoad(NULL);
            }

            CAaBox bounds = worldModel->m_shared->m_data->m_collisionBounds.m_extent;
            height = (bounds.t.z - bounds.b.z) * scale * 0.9f;
        }

        this->m_targetHeightNear += height;
        this->m_targetHeightFar += height;
        this->m_targetHeightSwim = height + this->m_targetHeightSwim;

        if (!worldModel->HasSequence(sequence)) {
            sequence = 0;
        }

        if (worldModel->HasSequence(42)) {
            M2SequenceInfo swimStandInfo;
            M2SequenceInfo swimInfo;
            swimStandInfo.Clear();
            swimInfo.Clear();
            worldModel->GetSequenceInfo(0, 0, swimStandInfo);
            worldModel->GetSequenceInfo(42, 0, swimInfo);
            swimStandInfo.m_bounds *= scale;
            swimInfo.m_bounds *= scale;
            this->m_targetHeightSwim -= swimStandInfo.m_bounds.t.z - swimInfo.m_bounds.t.z;
        }

        this->m_targetHeightNear -= standOffset;

        if (sequence == 91) {
            this->m_targetHeightFar -= standOffset;
        }

        if (attachment != 0xFFFFFFFF && model->HasAttachment(attachment)) {
            C3Vector pivot = model->GetAttachmentPivot(attachment);
            float mountHeight = pivot.z * mountScale;

            if ((unit->m_unit->m_unitFieldFlags & 0x100000) && model->HasSequence(135)) {
                M2SequenceInfo mountStandInfo;
                M2SequenceInfo mountInfo;
                mountStandInfo.Clear();
                mountInfo.Clear();
                model->GetSequenceInfo(0, 0, mountStandInfo);
                model->GetSequenceInfo(135, 0, mountInfo);
                mountStandInfo.m_bounds *= mountScale;
                mountInfo.m_bounds *= mountScale;
                mountHeight -= mountStandInfo.m_bounds.t.z - mountInfo.m_bounds.t.z;
            }

            this->m_targetHeightNear += mountHeight;
            this->m_targetHeightFar += mountHeight;
            this->m_targetHeightSwim = mountHeight + this->m_targetHeightSwim;
        }

        maxHeight = 15.0f;

        if (!CGBarberShop::m_barberShopEnabled) {
            float minimum = unit->m_movementData.m_collisionHeight * 0.77999997f + 0.11111111f;

            if (minimum > this->m_targetHeightSwim) {
                this->m_targetHeightSwim = minimum;
            }

            if (minimum > this->m_targetHeightNear) {
                this->m_targetHeightNear = minimum;
            }

            if (minimum > this->m_targetHeightFar) {
                this->m_targetHeightFar = minimum;
            }

            float unitMax = unit->GetMaxCameraHeight(1);

            if (unitMax > 0.0f) {
                maxHeight = unitMax;
            }
        }
    } else {
        float height;

        if ((target->m_obj->m_type & TYPEMASK_DYNAMICOBJECT) && model) {
            CAaSphere sphere = model->GetBoundingSphere();

            if (sphere.r > 0.01f) {
                height = sphere.r * 0.99f * scale;
            } else {
                height = scale + scale;
            }
        } else {
            height = scale + scale;
        }

        this->m_targetHeightSwim = height;
        this->m_targetHeightFar = height;
        this->m_targetHeightNear = height;
        maxHeight = 15.0f;
    }

    this->m_targetHeightNear = this->m_targetHeightNear < 0.8333333f ? 0.8333333f : (this->m_targetHeightNear >= maxHeight ? maxHeight : this->m_targetHeightNear);
    this->m_targetHeightFar = this->m_targetHeightFar < 0.8333333f ? 0.8333333f : (this->m_targetHeightFar >= maxHeight ? maxHeight : this->m_targetHeightFar);
    this->m_targetHeightSwim = this->m_targetHeightSwim < 0.8333333f ? 0.8333333f : (this->m_targetHeightSwim >= maxHeight ? maxHeight : this->m_targetHeightSwim);

    this->UpdateTargetHeight(target, FrameTime::GetCurTimeMs());
    this->UpdateLiquidSurfaceStatus(target);

    return 1;
}

// OFFSET: 0x0043E540
float CGCamera::GetChaseFacing(CGUnit_C* unit) const {
    if (this->m_vehicleCamera) {
        return this->m_vehicleCamera->m_facing;
    }

    return unit->m_smoothTarget.m_value;
}

// OFFSET: 0x004410A0
C3Vector CGCamera::GetSafeWorldPos(CGObject_C* target) const {
    CVehicleCamera_C* vehicleCamera = this->m_vehicleCamera;

    if (vehicleCamera) {
        if (!(vehicleCamera->m_flags & 0x40)) {
            vehicleCamera->ComputeSafeCurWorldPos();
        }

        return vehicleCamera->m_safeCurWorldPos;
    }

    return target->GetPosition();
}

// OFFSET: 0x0043F6A0/0x14004A910
void CGCamera::IncIgnoreFacing() {
    if (this->m_ignoreFacingRefs++ != 0) {
        return;
    }

    float facing = 0.0f;
    CGObject_C* target = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (target) {
        if (target->m_obj->m_type & TYPEMASK_UNIT) {
            facing = this->GetChaseFacing(static_cast<CGUnit_C*>(target));
        } else {
            facing = target->GetFacing();
        }
    }

    this->UpdateYaw(facing);
}

// OFFSET: 0x0043D520/0x140048680
void CGCamera::InitializeView() {
    for (uint32_t i = 0; i < 8; i++) {
        for (uint32_t j = 0; j < 3; j++) {
            float value = s_cvCameraViewData[i][j]->m_floatValue;

            if (s_cameraViewAngular[j]) {
                value *= 0.017453292f;
            }

            this->m_views[i][j] = value;
        }
    }
}

// OFFSET: 0x00442E90/0x14004E410
void CGCamera::MakeRelativeTo(WOWGUID guid) {
    if (guid == this->m_relativeTo) {
        return;
    }

    if (this->m_relativeTo && this->m_ignoreFacingRefs > 0) {
        CGObject_C* relativeTo = ClntObjMgrObjectPtr(this->m_relativeTo, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (relativeTo) {
            float facing;

            if (relativeTo->m_obj->m_type & TYPEMASK_UNIT) {
                facing = static_cast<CGUnit_C*>(relativeTo)->GetSmoothFacing();
            } else {
                facing = relativeTo->GetFacing();
            }

            this->UpdateYaw(facing);
        }
    }

    this->m_relativeTo = guid;

    if (guid && this->m_ignoreFacingRefs > 0) {
        CGObject_C* relativeTo = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);
        float facing;

        if (relativeTo->m_obj->m_type & TYPEMASK_UNIT) {
            facing = static_cast<CGUnit_C*>(relativeTo)->GetSmoothFacing();
        } else {
            facing = relativeTo->GetFacing();
        }

        this->UpdateYaw(-facing);
    }

    CGObject_C* target = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (target) {
        float yaw;
        float pitch;
        float roll;
        this->UpdateTargetFacing(target, yaw, pitch, roll);
    }
}

// OFFSET: 0x0043E6B0/0x140049A40
C33Matrix CGCamera::ParentToWorld() const {
    CGObject_C* parent = ClntObjMgrObjectPtr(this->m_relativeTo, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!parent) {
        C33Matrix identity;
        identity.a0 = 1.0f;
        identity.a1 = 0.0f;
        identity.a2 = 0.0f;
        identity.b0 = 0.0f;
        identity.b1 = 1.0f;
        identity.b2 = 0.0f;
        identity.c0 = 0.0f;
        identity.c1 = 0.0f;
        identity.c2 = 1.0f;
        return identity;
    }

    float facing;

    if (parent->m_obj->m_type & TYPEMASK_UNIT) {
        CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID::Make(parent->GetTransportGUID()), TYPEMASK_OBJECT, __FILE__, __LINE__);
        facing = static_cast<CGUnit_C*>(parent)->m_smoothTarget.m_value;
        CVehicleCamera_C::ConvertSmoothFacingFromRawToWorld(facing, transport);
    } else {
        facing = parent->GetFacing();
    }

    return C33Matrix::Rotation(facing);
}

// OFFSET: 0x0043F820/0x14004AAA0
void CGCamera::PerformTerrainTilt(CGObject_C* target, uint32_t time, int32_t immediate) {
    if (!target || !(target->m_obj->m_type & TYPEMASK_UNIT)) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(target);
    int32_t style = s_cvCameraSmoothStyle->m_intValue;
    int32_t state;

    if (unit->m_unit->m_unitFieldFlags & 0x100000) {
        state = 7;
    } else {
        uint32_t moveFlags = unit->m_movement->m_flags;

        if (moveFlags & (MOVEMENTFLAG_FLYING | MOVEMENTFLAG_SWIMMING)) {
            state = 6;
        } else if (moveFlags & MOVEMENTFLAG_FALLING) {
            state = 0;
        } else if (unit->IsJumping()) {
            state = 3;
        } else if (this->m_state & 0x100) {
            state = 8;
        } else if (this->m_state & 0x1000) {
            state = 1;
        } else if (moveFlags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD)) {
            state = 4;
        } else if (moveFlags & (MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) {
            state = 5;
        } else {
            state = (moveFlags & (MOVEMENTFLAG_TURN_LEFT | MOVEMENTFLAG_TURN_RIGHT)) ? 9 : 2;
        }
    }

    float delay = s_cvCameraTerrainTiltState[style][state][1]->m_floatValue;
    float scale = s_cvCameraTerrainTiltState[style][state][2]->m_floatValue;
    float tilt = s_cvCameraTerrainTiltState[style][state][0]->m_floatValue * this->m_terrainTilt;

    if (scale < 0.0f) {
        tilt = this->m_groundTilt;
    } else if (immediate || fequal(scale, 0.0f)) {
        this->m_groundTilt = tilt;
    } else {
        if (this->SetDesiredGroundOffset(tilt, delay, scale, time)) {
            this->m_smoothGroundTilt.m_rate = fclamp(this->m_smoothGroundTilt.m_rate, scale * s_cvCameraTerrainTiltTimeMin->m_floatValue, s_cvCameraTerrainTiltTimeMax->m_floatValue * scale);
        }

        return;
    }

    this->m_state &= ~0x10000000;
    this->m_smoothGroundTilt.m_target = tilt;
    this->m_smoothGroundTilt.m_startTimeMs = 0;
    this->m_smoothGroundTilt.m_rate = 0.0f;
}

// OFFSET: 0x00442FC0/0x14004E550
void CGCamera::PickVehicleCamera() {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGObject_C* target;

    if (!player || !(target = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__)) || !(target->m_obj->m_type & TYPEMASK_UNIT)) {
        this->m_vehicleCamera = NULL;
        return;
    }

    CVehicleCamera_C* previous = this->m_vehicleCamera;
    this->m_vehicleCamera = player->m_vehicleCamera;

    if (!this->m_vehicleCamera || this->m_vehicleCamera->m_targetGuid != this->m_targetGUID) {
        this->m_vehicleCamera = static_cast<CGUnit_C*>(target)->m_vehicleCamera;
    }

    if (this->m_vehicleCamera) {
        this->MakeRelativeTo(WOWGUID::Make(this->m_vehicleCamera->GetRelativeTo()));
    }

    if (previous && previous != this->m_vehicleCamera) {
        previous->m_unit->OnVehicleCameraPossiblyUnneeded();
    }
}

// OFFSET: 0x004410F0/0x14004C5A0
int32_t CGCamera::SetDesiredFoVOffset(float offset, float delay, float scale, uint32_t time) {
    this->m_targetFov = UnwrapAngleToward(offset, this->m_targetFov, -3.1415927f, 3.1415927f);

    if ((this->m_state & 0x40000000) && fabsf(this->m_smoothFoV.m_target - offset) < 0.001f && fabsf(this->m_smoothFoV.m_param4 - delay) < 0.001f && fabsf(this->m_smoothFoV.m_param3 - scale) < 0.001f) {
        return 1;
    }

    if (fabsf(this->m_targetFov - offset) < 0.001f) {
        return 0;
    }

    this->m_smoothFoV.m_param4 = delay;
    this->m_smoothFoV.m_param3 = scale;

    float duration;

    if (this->m_state & 0x80008000) {
        duration = 0.5f;
    } else {
        duration = scale * (fabsf(offset - this->m_targetFov) / (s_cvCameraFoVSmoothSpeed->m_floatValue * 0.017453292f));
    }

    return this->StartSmoothFoV(offset, duration, time + static_cast<int32_t>(delay * 1000.0f));
}

// OFFSET: 0x0043EC50/0x14004A010
int32_t CGCamera::SetDesiredGroundOffset(float offset, float delay, float scale, uint32_t time) {
    this->m_groundTilt = UnwrapAngleToward(offset, this->m_groundTilt, -3.1415927f, 3.1415927f);

    if ((this->m_state & 0x10000000) && fabsf(this->m_smoothGroundTilt.m_target - offset) < 0.001f && fabsf(this->m_smoothGroundTilt.m_param4 - delay) < 0.001f && fabsf(this->m_smoothGroundTilt.m_param3 - scale) < 0.001f) {
        return 1;
    }

    if (fabsf(this->m_groundTilt - offset) < 0.001f) {
        return 0;
    }

    this->m_smoothGroundTilt.m_param4 = delay;
    this->m_smoothGroundTilt.m_param3 = scale;

    float duration = scale * (fabsf(offset - this->m_groundTilt) / (s_cvCameraGroundSmoothSpeed->m_floatValue * 0.017453292f));

    return this->StartSmoothGroundTilt(offset, duration, time + static_cast<int32_t>(delay * 1000.0f));
}

// OFFSET: 0x004400C0/0x14004B390
void CGCamera::SetModeFreeLook() {
    if (this->m_state & 0x1) {
        return;
    }

    this->m_state |= 0x1;
    SysMsgAdd("Camera FREELOOK", STATUS_INFO);
    this->IncIgnoreFacing();

    if (this->m_groundTiltRefs++ == 0) {
        this->ClampPitchToLimits(this->m_groundTilt);
    }

    this->m_state &= 0xF4FFBFFF;
    this->m_flags &= ~0x4;

    this->m_smoothPitch.m_target = this->m_pitch;
    this->m_smoothPitch.m_startTimeMs = 0;
    this->m_smoothPitch.m_rate = 0.0f;

    this->m_smoothTargetOffset.m_startTimeMs = 0;
    this->m_smoothTargetOffset.m_target = this->m_targetOffset;
    this->m_smoothTargetOffset.m_rate = 0.0f;

    this->m_smoothYaw.m_startTimeMs = 0;
    this->m_smoothYaw.m_target = this->m_yaw;
    this->m_smoothYaw.m_rate = 0.0f;
}

// OFFSET: 0x00440030/0x14004B300
void CGCamera::SetModeNormal() {
    if (!(this->m_state & 0x1) || (this->m_flags & 0x2)) {
        return;
    }

    this->m_state &= ~0x1;

    if (this->m_vehicleCamera && (this->m_vehicleCamera->m_flags & 0x10)) {
        this->m_state &= ~0x4000;
    }

    this->DecIgnoreFacing();

    if (--this->m_groundTiltRefs == 0) {
        this->ClampPitchToLimits(-this->m_groundTilt);
    }

    SysMsgAdd("Camera NORMAL", STATUS_INFO);
    this->m_state &= ~0x4000;
}

// OFFSET: 0x00445CD0/0x140051BE0
int32_t CGCamera::SetModelCamera(const char* fileName, const C3Vector* position, float facing, void (*callback)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param), uint64_t param) {
    if (this->m_model) {
        this->m_state &= ~0x4;

        if (this->m_modelCamera) {
            CameraRelease(this->m_modelCamera);
            this->m_modelCamera = NULL;
        }

        this->m_model->Release();
        this->m_model = NULL;
    }

    CStatus status;
    this->m_model = this->GetScene()->CreateModel(fileName, 0, 3);
    SysMsgAdd(&status);

    if (!this->m_model) {
        return 0;
    }

    this->m_model->SetSequenceCallback(reinterpret_cast<void*>(callback), param);
    this->m_modelMatrix.c2 = 1.0f;
    this->m_modelMatrix.b1 = 1.0f;
    this->m_modelMatrix.a0 = 1.0f;
    this->m_modelMatrix.d2 = 0.0f;
    this->m_modelMatrix.d1 = 0.0f;
    this->m_modelMatrix.d0 = 0.0f;
    this->m_modelMatrix.c1 = 0.0f;
    this->m_modelMatrix.c0 = 0.0f;
    this->m_modelMatrix.b2 = 0.0f;
    this->m_modelMatrix.b0 = 0.0f;
    this->m_modelMatrix.a2 = 0.0f;
    this->m_modelMatrix.a1 = 0.0f;
    this->m_modelMatrix.d0 = position->x;
    this->m_modelMatrix.d1 = position->y;
    this->m_modelMatrix.d2 = position->z;
    this->m_modelMatrix.RotateAroundZ(facing);
    CM2Model* model = this->m_model;
    model->m_fFlags |= 0x4000;
    model->SetWorldTransform(C44Matrix(this->m_modelMatrix));
    this->m_model->WaitUntilLoaded();
    return this->FinishLoadingModel();
}

// OFFSET: 0x00443AD0/0x14004F320
void CGCamera::SetTarget(CGObject_C* target, int32_t freeLook) {
    if (this->m_state & 0x10) {
        CGWorldFrame::s_currentWorldFrame->SetPlayerFadeCameraValue(0xFF);
    }

    WOWGUID previous = this->m_targetGUID;
    int32_t newTarget = target && target->m_obj->m_guid != this->m_targetGUID;
    int32_t moved = 0;

    if (target) {
        C3Vector position = this->GetSafeWorldPos(target);
        moved = this->m_targetPosition.x != position.x || this->m_targetPosition.y != position.y || this->m_targetPosition.z != position.z;
    }

    int32_t immediate = !(this->m_state & 0x10) || newTarget;

    if (!(this->m_state & 0x10) || newTarget || moved) {
        uint32_t time = FrameTime::GetCurTimeMs();
        this->CalcTerrainTilt(target, time);
        this->PerformTerrainTilt(target, time, immediate);
    }

    float drunkness = 0.0f;

    if (target && (target->m_obj->m_type & TYPEMASK_PLAYER)) {
        drunkness = static_cast<CGPlayer_C*>(target)->GetDrunknessTurnValue();
    }

    this->UpdateInebriation(drunkness, immediate);
    this->unk_01BC = 0.0f;

    if (target) {
        this->m_targetGUID = target->m_obj->m_guid;

        if (target->m_obj->m_type & TYPEMASK_UNIT) {
            this->m_targetFacing = target->GetRawFacing();
            WOWGUID relativeTo = WOWGUID::Make(static_cast<CGUnit_C*>(target)->GetCameraRelativeTo());

            if (ClntObjMgrObjectPtr(relativeTo, TYPEMASK_OBJECT, __FILE__, __LINE__)) {
                this->MakeRelativeTo(relativeTo);
            } else {
                this->MakeRelativeTo(WOWGUID());
            }
        } else {
            this->m_targetFacing = target->GetFacing();
            this->m_relativeTo = WOWGUID();
        }

        if (freeLook) {
            this->m_flags |= 0x2;
            this->SetModeFreeLook();
        } else if (this->m_flags & 0x2) {
            this->m_flags &= ~0x2;
            this->SetModeNormal();
        }

        if (!this->FinishLoadingTarget(target)) {
            this->m_state &= ~0x4;
        }
    } else {
        this->m_targetGUID = WOWGUID();
        this->m_relativeTo = WOWGUID();
        StaticSingleton<CCameraManager>::Instance()->ClearShakes();
        this->m_flags &= ~0x2;
    }

    this->m_state &= ~0x800000;

    if (!previous) {
        this->PickVehicleCamera();
    }
}

// OFFSET: 0x0043F310/0x14004A560
int32_t CGCamera::SmoothSetFlyingMountHeight(float height, float delay, float scale, uint32_t time) {
    if ((this->m_state & 0x400000) && fabsf(this->m_smoothFlyingHeight.m_target - height) < 0.001f && fabsf(this->m_smoothFlyingHeight.m_param4 - delay) < 0.001f && fabsf(this->m_smoothFlyingHeight.m_param3 - scale) < 0.001f) {
        return 1;
    }

    if (fabsf(this->m_flyingMountHeight - height) < 0.001f) {
        return 0;
    }

    this->m_smoothFlyingHeight.m_param4 = delay;
    this->m_smoothFlyingHeight.m_param3 = scale;

    float duration = fabsf(height - this->m_flyingMountHeight) / s_cvCameraFlyingMountHeightSmoothSpeed->m_floatValue * scale;

    return this->StartSmoothFlyingMountHeight(height, duration, time - static_cast<uint32_t>(delay * -1000.0f));
}

// OFFSET: 0x004412A0/0x14004C6F0
int32_t CGCamera::SmoothSetHeight(float height, float delay, float scale, uint32_t time) {
    if ((this->m_state & 0x20000000) && fabsf(this->m_smoothHeight.m_target - height) < 0.001f && fabsf(this->m_smoothHeight.m_param4 - delay) < 0.001f && fabsf(this->m_smoothHeight.m_param3 - scale) < 0.001f) {
        return 1;
    }

    if (fabsf(this->m_height - height) < 0.001f) {
        return 0;
    }

    this->m_smoothHeight.m_param4 = delay;
    this->m_smoothHeight.m_param3 = scale;

    float duration = fabsf(height - this->m_height) / s_cvCameraHeightSmoothSpeed->m_floatValue * scale;

    return this->StartSmoothHeight(height, duration, time - static_cast<uint32_t>(delay * -1000.0f));
}

// OFFSET: 0x0043D140/0x1400481A0
float CGCamera::SmoothTargetFacing(float facing, int32_t moved) {
    if (!s_cameraSmooth->m_intValue || !moved) {
        return facing;
    }

    float delta = facing - this->m_targetFacing;

    if (delta > 3.1415927f) {
        delta -= 6.2831855f;
    } else if (delta < -3.1415927f) {
        delta += 6.2831855f;
    }

    float magnitude = fabsf(delta);
    float step;

    if (magnitude > 0.2617994f) {
        step = delta < 0.0f ? -0.2617994f : 0.2617994f;
    } else if (magnitude > 0.09f) {
        step = delta < 0.0f ? -0.09f : 0.09f;
    } else {
        return facing;
    }

    return WrapAngle0To2Pi(step + this->m_targetFacing);
}

// OFFSET: 0x0043B610
int32_t CGCamera::StartSmoothFlyingMountHeight(float height, float rate, uint32_t startTime) {
    if (fabsf(this->m_flyingMountHeight - height) < 0.001f) {
        return 0;
    }

    this->m_state |= 0x400000;
    this->m_smoothFlyingHeight.m_startValue = this->m_flyingMountHeight;
    this->m_smoothFlyingHeight.m_startTimeMs = startTime;
    this->m_smoothFlyingHeight.m_target = height;
    this->m_smoothFlyingHeight.m_rate = rate;

    return 1;
}

// OFFSET: 0x0043EB70/0x140049F50
int32_t CGCamera::StartSmoothFoV(float offset, float rate, uint32_t startTime) {
    this->m_targetFov = UnwrapAngleToward(offset, this->m_targetFov, -3.1415927f, 3.1415927f);

    if (fabsf(this->m_targetFov - offset) < 0.001f && !(this->m_state & 0x80008000)) {
        return 0;
    }

    this->m_state |= 0x40000000;
    this->m_smoothFoV.m_startValue = this->m_targetFov;
    this->m_smoothFoV.m_startTimeMs = startTime;
    this->m_smoothFoV.m_target = offset;
    this->m_smoothFoV.m_rate = rate;

    return 1;
}

// OFFSET: 0x0043B290
int32_t CGCamera::StartSmoothGroundTilt(float offset, float rate, uint32_t startTime) {
    this->m_groundTilt = UnwrapAngleToward(offset, this->m_groundTilt, -3.1415927f, 3.1415927f);

    if (fabsf(this->m_groundTilt - offset) < 0.001f) {
        return 0;
    }

    this->m_state |= 0x10000000;
    this->m_smoothGroundTilt.m_startValue = this->m_groundTilt;
    this->m_smoothGroundTilt.m_startTimeMs = startTime;
    this->m_smoothGroundTilt.m_target = offset;
    this->m_smoothGroundTilt.m_rate = rate;

    return 1;
}

// OFFSET: 0x0043EDE0/0x14004A140
int32_t CGCamera::StartSmoothHeight(float height, float rate, uint32_t startTime) {
    if (fabsf(this->m_height - height) < 0.001f) {
        return 0;
    }

    this->m_state |= 0x20000000;
    this->m_smoothHeight.m_startValue = this->m_height;
    this->m_smoothHeight.m_startTimeMs = startTime;
    this->m_smoothHeight.m_target = height;
    this->m_smoothHeight.m_rate = rate;

    if (!(this->m_state & 0x80)) {
        this->m_height = height;
        this->m_smoothHeight.m_target = height;
        this->m_state = (this->m_state & ~0x20000000) | 0x80;
        this->m_smoothHeight.m_rate = 0.0f;
        this->m_smoothHeight.m_startTimeMs = 0;
    }

    return 1;
}

// OFFSET: 0x00442C10/0x14004E120
void CGCamera::SyncFreeLookFacing() {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_UNIT, __FILE__, __LINE__));
    float yaw = this->m_yaw;

    if (!unit || !unit->ClampRawAngleToLegalFacingRange(yaw)) {
        float current = this->m_yaw;
        this->m_state |= 0x4000;
        this->m_flags |= 0x4;
        this->unk_01C8 = current;
    } else if (this->m_flags & 0x4) {
        this->m_state |= 0x4000;
        this->m_smoothYaw.m_target = yaw;
        this->m_yaw = yaw;
        this->unk_01C8 = yaw;
    } else {
        this->m_state &= ~0x4000;
    }

    uint32_t time = FrameTime::GetCurTimeMs();
    CGInputControl::GetActive()->CameraTurnPlayer(time, this->m_yaw);
    float pitch = this->m_targetOffset + this->m_pitch;
    this->SyncFreeLookPitch(MIN_PITCH_ANGLE > pitch ? MIN_PITCH_ANGLE : (MAX_PITCH_ANGLE <= pitch ? MAX_PITCH_ANGLE : pitch), time, 1);
}

// OFFSET: 0x00442D20/0x14004E240
int32_t CGCamera::UpdateInebriation(float drunkness, int32_t immediate) {
    if (this->m_state & 0x80008000) {
        return 0;
    }

    float offset = drunkness * 1.5533429f;

    if (immediate) {
        bool changed = fnotequal(this->m_targetFov, offset);
        this->m_targetFov = offset;
        return changed;
    }

    return this->SetDesiredFoVOffset(offset, 0.0f, 1.0f, FrameTime::GetCurTimeMs());
}

// OFFSET: 0x00442DB0/0x14004E2E0
float CGCamera::UpdateLiquidSurfaceStatus(CGObject_C* target) {
    uint32_t state = this->m_state;
    this->m_state = state & ~0x300000;
    uint32_t liquidType;
    float height;
    int32_t liquidFlags;

    if (!target || !World::QueryObjectLiquid(target->m_worldObject, liquidType, height, liquidFlags)) {
        return 0.0f;
    }

    float depth = height - this->GetSafeWorldPos(target).z;

    if (this->m_height < depth) {
        this->m_state |= 0x200000;
    } else {
        this->m_state |= 0x100000;
    }

    if (state != this->m_state && (target->m_obj->m_type & TYPEMASK_UNIT) && !static_cast<CGUnit_C*>(target)->IsAutoTracking()) {
        uint32_t time = FrameTime::GetCurTimeMs();
        this->SyncFreeLookPitch(-static_cast<CGUnit_C*>(target)->GetPitch(), time, 0);
    }

    return depth;
}

// OFFSET: 0x00442700/0x14004DBD0
void CGCamera::UpdateTargetFacing(CGObject_C* target, float& yaw, float& pitch, float& roll) {
    yaw = this->m_yaw;
    pitch = this->m_pitch;
    roll = this->m_roll;

    if (!target) {
        return;
    }

    C3Vector position = this->GetSafeWorldPos(target);
    bool moved = this->m_targetPosition.x != position.x || this->m_targetPosition.y != position.y || this->m_targetPosition.z != position.z;
    float facing;

    if (target->m_obj->m_type & TYPEMASK_UNIT) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(target);
        unit->UpdateSmoothFacing(NULL, NULL);
        facing = this->GetChaseFacing(unit);

        if (unit->m_obj->m_guid != CGUnit_C::s_activeMover && !this->m_vehicleCamera) {
            facing = this->SmoothTargetFacing(facing, moved);
        }
    } else {
        facing = target->GetFacing();
    }

    this->m_targetFacing = facing;
    yaw = this->m_yawOffset + yaw;

    if (this->m_ignoreFacingRefs <= 0) {
        yaw = facing + yaw;
    }

    if (this->m_groundTiltRefs <= 0) {
        pitch = this->m_groundTilt + pitch;
    }

    yaw = CMath::normalizeangle0to2pi_(yaw);
    pitch = MIN_PITCH_ANGLE > pitch ? MIN_PITCH_ANGLE : (MAX_PITCH_ANGLE <= pitch ? MAX_PITCH_ANGLE : pitch);

    this->SetFacing(yaw, pitch, roll);
}

// OFFSET: 0x004428E0/0x14004DDC0
void CGCamera::UpdateTargetHeight(CGObject_C* target, uint32_t time) {
    float scale = 1.0f;

    if (static_cast<intptr_t>(this->m_heightChangedTimeMs) > 0) {
        if (FrameTime::GetCurTimeMs() - this->m_heightChangedTimeMs > 2999) {
            this->m_heightChangedTimeMs = 0;
        } else {
            scale = 0.5f;
        }
    }

    if (target && (target->m_obj->m_type & TYPEMASK_UNIT) && (static_cast<CGUnit_C*>(target)->m_movement->m_flags & MOVEMENTFLAG_SWIMMING)) {
        this->SmoothSetHeight(this->m_targetHeightSwim + s_targetHeightOffset, 0.0f, scale, time);
    } else if (this->m_smoothDistance.m_target >= 1.8315002f || CGBarberShop::m_barberShopEnabled) {
        this->SmoothSetHeight(this->m_targetHeightFar + s_targetHeightOffset, 0.0f, scale, time);

        if (target && (target->m_obj->m_type & TYPEMASK_UNIT) && static_cast<CGUnit_C*>(target)->GetCanFly()) {
            this->SmoothSetFlyingMountHeight(this->m_mountHeight + s_targetHeightOffset, 0.0f, scale, time);
        } else {
            this->SmoothSetFlyingMountHeight(s_targetHeightOffset, 0.0f, scale, time);
        }
    } else {
        this->SmoothSetHeight(this->m_targetHeightNear + s_targetHeightOffset, 0.0f, scale, time);
        this->SmoothSetFlyingMountHeight(s_targetHeightOffset, 0.0f, 1.0f, time);
    }
}

// OFFSET: 0x0043AE50/0x140045C30
void CGCamera::UpdateYaw(float delta) {
    this->m_smoothYaw.m_target = CMath::normalizeangle0to2pi_(delta + this->m_smoothYaw.m_target);
    this->m_smoothYaw.m_startValue = UnwrapAngleToward(this->m_smoothYaw.m_target, CMath::normalizeangle0to2pi_(delta + this->m_smoothYaw.m_startValue), -3.1415927f, 3.1415927f);
    this->m_yaw = UnwrapAngleToward(this->m_smoothYaw.m_target, CMath::normalizeangle0to2pi_(this->m_yaw + delta), -3.1415927f, 3.1415927f);
}

// OFFSET: 0x0043AA00/0x140045910
int32_t CGCamera::CCommand_PitchLimit(const char* command, const char* arguments) {
    MAX_PITCH_ANGLE = SStrToFloat(arguments) * 0.017453292f;
    MIN_PITCH_ANGLE = SStrToFloat(arguments) * -0.017453292f;
    return 1;
}

// OFFSET: 0x0043B680/0x140046130
uint32_t CGCamera::GetCollisionQueryFlags() {
    return 0x900171;
}

// OFFSET: 0x0043ACA0/0x140045B20
float CGCamera::GetDefaultZoomMaxDistance() {
    return 50.0f;
}

// OFFSET: 0x0043DAE0/0x140048BC0
void CGCamera::BeginBobbingSmooth() {
    this->m_state &= ~0x200;
    this->m_bobDuration = fabsf((&this->m_bobOffset.x)[this->m_bobOffset.MajorAxis()]) / s_cvCameraBobbingSmoothSpeed->m_floatValue;
    this->m_bobStart = this->m_bobOffset;
    this->m_bobTime = FrameTime::GetCurTimeMs();
}

// OFFSET: 0x0043DF70/0x140049140
void CGCamera::CameraTeleported(float oldFacing) {
    if (!(this->m_state & 0x1)) {
        return;
    }

    CGObject_C* target = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (target) {
        this->UpdateYaw(target->GetFacing() - oldFacing);
    }
}

// OFFSET: 0x0043D950/0x140048A40
uint32_t CGCamera::CanSmoothTargetFacing(CGObject_C* target) {
    if (!target || !(target->m_obj->m_type & TYPEMASK_UNIT) || !s_cvCameraPivot->m_intValue) {
        return 0;
    }

    uint32_t moveFlags = static_cast<CGUnit_C*>(target)->m_movement->m_flags;

    if (moveFlags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) {
        return 0;
    }

    if (this->m_pitch > 0.0f) {
        return 0;
    }

    if (this->m_state & 0x8) {
        return 0;
    }

    if (moveFlags & MOVEMENTFLAG_FLYING) {
        return this->m_state & 0x10000;
    }

    return this->m_state & 0x30000;
}

// OFFSET: 0x0043BAA0
void CGCamera::CancelSmoothPitch() {
    this->m_state &= ~0x2000000;
    this->m_smoothPitch.m_target = this->m_pitch;
    this->m_smoothPitch.m_startTimeMs = 0;
    this->m_smoothPitch.m_rate = 0.0f;
}

// OFFSET: 0x0043BAD0
void CGCamera::CancelSmoothTargetOffset() {
    this->m_state &= ~0x8000000;
    this->m_smoothTargetOffset.m_target = this->m_targetOffset;
    this->m_smoothTargetOffset.m_startTimeMs = 0;
    this->m_smoothTargetOffset.m_rate = 0.0f;
}

// OFFSET: 0x0043BB00
void CGCamera::CancelSmoothYaw() {
    this->m_state &= ~0x1000000;
    this->m_smoothYaw.m_target = this->m_yaw;
    this->m_smoothYaw.m_startTimeMs = 0;
    this->m_smoothYaw.m_rate = 0.0f;
}

// OFFSET: 0x0043AFD0/0x140045D60
void CGCamera::CheckUnderwater() {
    float depth;
    int32_t underwater = World::SceneCamLiquidStatus(&depth);
    SI3::SetUnderwaterStatus(underwater);

    if (!(this->m_state & 0x2) || this->m_underwater != underwater) {
        this->m_state |= 0x2;
        this->m_underwater = underwater;
    }
}

// OFFSET: 0x0043D0E0/0x140048130
void CGCamera::ClampPitchAndNormalize() {
    this->m_pitch = MIN_PITCH_ANGLE > this->m_pitch ? MIN_PITCH_ANGLE : (MAX_PITCH_ANGLE <= this->m_pitch ? MAX_PITCH_ANGLE : this->m_pitch);
    this->m_yaw = CMath::normalizeangle0to2pi_(this->m_yaw);
    this->m_yawOffset = CMath::normalizeangle0to2pi_(this->m_yawOffset);
}

// OFFSET: 0x0043A9A0/0x1400458A0
void CGCamera::ClearModelCamera() {
    if (!this->m_model) {
        return;
    }

    this->m_state &= ~0x4;

    if (this->m_modelCamera) {
        CameraRelease(this->m_modelCamera);
        this->m_modelCamera = NULL;
    }

    this->m_model->Release();
    this->m_model = NULL;
}

// OFFSET: 0x0043B1A0/0x140045E60
void CGCamera::ClearVehicleExitZoomDistance() {
    this->m_flags &= ~0x20;
}

// OFFSET: 0x0043AB70
void CGCamera::CreateViewFromCamera(uint32_t view) {
    this->SaveViewToCVars(view, this->m_smoothDistance.m_target, this->m_smoothPitch.m_target, this->m_smoothYaw.m_target);
}

// OFFSET: 0x00442B10/0x14004DFE0
void CGCamera::DisableFreeLook(int32_t keepSmoothing) {
    this->SetModeNormal();

    if (keepSmoothing) {
        this->m_state |= 0x20;
    } else {
        this->m_state &= ~0x20;
    }

    this->m_smoothPitch.m_target = this->m_pitch;
    this->m_smoothYaw.m_target = this->m_yaw;
    this->m_smoothTargetOffset.m_target = this->m_targetOffset;
}

// OFFSET: 0x00442AB0/0x14004DF60
void CGCamera::EnableFreeLook() {
    CGObject_C* target = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (target && (target->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(target)->m_unit->m_unitFieldFlags & 0x100000) && (static_cast<CGPlayer_C*>(target)->m_player->m_playerFlags & 0x20000)) {
        return;
    }

    this->SetModeFreeLook();
}

// OFFSET: 0x0043FA40/0x14004ACB0
C3Vector CGCamera::GetCameraPosition(const C3Vector& target, float distance, const C3Vector& offset) {
    C3Vector position = target;
    C3Vector forward = this->Forward();
    this->RotateOffsetByRoll(forward);

    position.x -= forward.x * distance;
    position.y -= forward.y * distance;
    position.z -= forward.z * distance;

    if (distance > 0.0f && fabsf(this->m_flyingMountHeight) >= 2.3841858e-7f) {
        float scale = this->m_collideExtent * this->m_flyingMountHeight * distance;
        float maxDistance = this->m_distance >= this->m_smoothDistance.m_target ? this->m_distance : this->m_smoothDistance.m_target;

        if (maxDistance > 0.0f) {
            scale /= maxDistance;
            C3Vector up = this->Up();
            position.x = up.x * scale + position.x;
            position.y = up.y * scale + position.y;
            position.z = up.z * scale + position.z;
        }
    }

    position.x = position.x + offset.x;
    position.y = offset.y + position.y;
    position.z = offset.z + position.z;

    return position;
}

// OFFSET: 0x0043A950/0x140045820
int32_t CGCamera::HasModelCamera() {
    return this->m_modelCamera != NULL;
}

// OFFSET: 0x0043D9C0/0x140048AA0
int32_t CGCamera::HasTargetOffset() {
    if (fabsf(this->m_bobOffset.z + this->m_bobOffset.y + this->m_bobOffset.x) < 0.001f) {
        return 0;
    }

    CGObject_C* target = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);
    return this->IsBobbingAllowed(target) == 0;
}

// OFFSET: 0x0043D890/0x1400489A0
uint32_t CGCamera::IsBobbingAllowed(CGObject_C* target) {
    if (this->m_distance > 0.16666667f) {
        return 0;
    }

    if (!target || !(target->m_obj->m_type & TYPEMASK_UNIT)) {
        return 0;
    }

    uint32_t state = this->m_state;

    if ((state & 0x8) || !s_cvCameraBobbing->m_intValue) {
        return 0;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(target);
    uint32_t moveFlags = unit->m_movement->m_flags;

    if ((moveFlags & (MOVEMENTFLAG_FLYING | MOVEMENTFLAG_SWIMMING)) || (moveFlags & MOVEMENTFLAG_FALLING)) {
        return 0;
    }

    if (unit->IsJumping() || unit->m_mountModel || (unit->m_unit->m_unitFieldFlags & 0x100000)) {
        return 0;
    }

    return state & 0x200;
}

// OFFSET: 0x0043DAA0
int32_t CGCamera::IsCustomViewSmoothingActive() {
    return !(this->m_state & 0x20) && (s_cvCameraCustomViewSmoothing->m_intValue || this->IsViewDefault(this->m_viewIndex)) && !(this->m_state & 0x8000);
}

// OFFSET: 0x0043A9E0/0x1400458F0
int32_t CGCamera::IsFirstPersonView() {
    return this->m_distance < 1.8315002f;
}

// OFFSET: 0x0043AC20/0x140045A50
int32_t CGCamera::IsViewDefault(uint32_t view) {
    for (uint32_t i = 0; i < 3; i++) {
        float value = SStrToFloat(s_cameraViewDefaults[view].m_values[i]);

        if (s_cameraViewAngular[i]) {
            value *= 0.017453292f;
        }

        if (!(fabsf(this->m_views[view][i] - value) < 0.001f)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x0043CF00/0x1400480A0
void CGCamera::LoadCameraSettings() {
    float distance = s_cvCameraSavedDistance->m_floatValue;
    distance = 0.0f > distance ? 0.0f : (50.0f <= distance ? 50.0f : distance);
    this->m_distance = distance;
    this->m_smoothDistance.m_target = distance;

    float pitch = s_cvCameraSavedPitch->m_floatValue * 0.017453292f;
    this->m_pitch = pitch;
    this->m_smoothPitch.m_target = pitch;
}

// OFFSET: 0x00443090/0x14004E650
void CGCamera::OnVehicleCameraDestroyed() {
    this->m_vehicleCamera = NULL;
    this->PickVehicleCamera();
}

// OFFSET: 0x0043A960/0x140045830
void CGCamera::ResetModelCamera() {
    this->m_someTime = FrameTime::GetCurTimeMs();
    this->GetScene()->SetTime(0);
    this->m_model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 0, 1, 0, 0, 0);
}

// OFFSET: 0x0043E160/0x140049350
void CGCamera::RotateOffsetByRoll(C3Vector& offset) {
    if (0.0f != this->m_vehicleYawOffset) {
        C44Matrix rotation;
        rotation.RotateAroundZ(this->m_vehicleYawOffset);
        offset *= rotation;
    }
}

// OFFSET: 0x0043CE30/0x140047FB0
void CGCamera::SaveCameraSettings() {
    char buffer[32];
    float distance;

    if (this->m_state & 0x8) {
        distance = this->unk_02F4;
    } else if (this->m_flags & 0x20) {
        distance = this->m_vehicleExitZoomDistance;
    } else {
        distance = this->m_smoothDistance.m_target;
    }

    SStrPrintf(buffer, sizeof(buffer), "%f", distance);

    if (this->m_vehicleZoomEnabled) {
        s_cvCameraSavedVehicleDistance->Set(buffer, true, false, false, true);
    } else {
        s_cvCameraSavedDistance->Set(buffer, true, false, false, true);
    }

    SStrPrintf(buffer, sizeof(buffer), "%f", this->m_smoothPitch.m_target * 57.29578f);
    s_cvCameraSavedPitch->Set(buffer, true, false, false, true);
}

// OFFSET: 0x0043AAC0/0x140045960
void CGCamera::SaveViewToCVars(uint32_t view, float distance, float pitch, float yaw) {
    char buffer[40];

    this->m_views[view][0] = distance;
    this->m_views[view][1] = pitch;
    this->m_views[view][2] = yaw;

    for (uint32_t i = 0; i < 3; i++) {
        float value = this->m_views[view][i];

        if (s_cameraViewAngular[i]) {
            value *= 57.29578f;
        }

        SStrPrintf(buffer, sizeof(buffer), "%f", value);
        s_cvCameraViewData[view][i]->Set(buffer, true, false, false, true);
    }
}

// OFFSET: 0x0043EE90/0x14004A1D0
int32_t CGCamera::SetDesiredPitchAngle(float pitch, float delay, float scale, uint32_t time) {
    this->m_pitch = UnwrapAngleToward(pitch, this->m_pitch, -3.1415927f, 3.1415927f);

    if ((this->m_state & 0x2000000) && fabsf(this->m_smoothPitch.m_target - pitch) < 0.001f && fabsf(this->m_smoothPitch.m_param4 - delay) < 0.001f && fabsf(this->m_smoothPitch.m_param3 - scale) < 0.001f) {
        return 1;
    }

    if (fabsf(this->m_pitch - pitch) < 0.001f) {
        return 0;
    }

    this->m_smoothPitch.m_param4 = delay;
    this->m_smoothPitch.m_param3 = scale;

    float duration = scale * (fabsf(pitch - this->m_pitch) / (s_cvCameraPitchSmoothSpeed->m_floatValue * 0.017453292f));

    return this->StartSmoothPitch(pitch, duration, time - static_cast<uint32_t>(delay * -1000.0f));
}

// OFFSET: 0x0043F010/0x14004A300
int32_t CGCamera::SetDesiredTargetOffset(float offset, float delay, float scale, uint32_t time) {
    this->m_targetOffset = UnwrapAngleToward(offset, this->m_targetOffset, -3.1415927f, 3.1415927f);

    if ((this->m_state & 0x8000000) && fabsf(this->m_smoothTargetOffset.m_target - offset) < 0.001f && fabsf(this->m_smoothTargetOffset.m_param4 - delay) < 0.001f && fabsf(this->m_smoothTargetOffset.m_param3 - scale) < 0.001f) {
        return 1;
    }

    if (fabsf(this->m_targetOffset - offset) < 0.001f) {
        return 0;
    }

    this->m_smoothTargetOffset.m_param4 = delay;
    this->m_smoothTargetOffset.m_param3 = scale;

    float duration = scale * (fabsf(offset - this->m_targetOffset) / (s_cvCameraTargetSmoothSpeed->m_floatValue * 0.017453292f));

    return this->StartSmoothTargetOffset(offset, duration, time - static_cast<uint32_t>(delay * -1000.0f));
}

// OFFSET: 0x0043B120/0x140045E00
void CGCamera::SetDesiredVehiclePitchOffset(float offset, uint32_t startTime, uint32_t endTime) {
    this->m_vehiclePitchStartTime = startTime;
    this->m_vehiclePitchEndTime = endTime;

    if (startTime != endTime) {
        this->m_vehiclePitchFrom = this->m_vehiclePitchOffset;
        this->m_vehiclePitchTo = offset;
    } else {
        this->m_vehiclePitchOffset = offset;
    }
}

// OFFSET: 0x0043B0D0/0x140045DC0
void CGCamera::SetDesiredVehicleYawOffset(float offset, uint32_t startTime, uint32_t endTime) {
    this->m_vehicleYawStartTime = startTime;
    this->m_vehicleYawEndTime = endTime;

    if (startTime != endTime) {
        this->m_vehicleYawFrom = this->m_vehicleYawOffset;
        this->m_vehicleYawTo = offset;
    } else {
        this->m_vehicleYawOffset = offset;
    }
}

// OFFSET: 0x0043F190/0x14004A430
int32_t CGCamera::SetDesiredYawAngle(float yaw, float delay, float scale, uint32_t time) {
    this->m_yaw = UnwrapAngleToward(yaw, this->m_yaw, -3.1415927f, 3.1415927f);

    if ((this->m_state & 0x1000000) && fabsf(this->m_smoothYaw.m_target - yaw) < 0.001f && fabsf(this->m_smoothYaw.m_param4 - delay) < 0.001f && fabsf(this->m_smoothYaw.m_param3 - scale) < 0.001f) {
        return 1;
    }

    if (fabsf(this->m_yaw - yaw) < 0.001f) {
        return 0;
    }

    this->m_smoothYaw.m_param4 = delay;
    this->m_smoothYaw.m_param3 = scale;

    float duration = scale * (fabsf(yaw - this->m_yaw) / (s_cvCameraYawSmoothSpeed->m_floatValue * 0.017453292f));

    return this->StartSmoothYaw(yaw, duration, time - static_cast<uint32_t>(delay * -1000.0f));
}

// OFFSET: 0x0043B180/0x140045E50
void CGCamera::SetVehicleExitZoomDistance(float distance) {
    this->m_flags |= 0x20;
    this->m_vehicleExitZoomDistance = distance;
}

// OFFSET: 0x0043E370/0x1400495C0
void CGCamera::SetVehicleSeatZoomMaxDistance(float distance) {
    this->m_flags |= 0x10;
    this->m_overrideDistanceMax = distance;

    if (this->m_smoothDistance.m_target > distance) {
        this->ZoomToDistance(this->m_overrideDistanceMax, FrameTime::GetCurTimeMs(), 0.0f, 0);
    }
}

// OFFSET: 0x0043E430/0x140049760
void CGCamera::SetVehicleSeatZoomMaxDistanceToDefault() {
    this->m_flags &= ~0x10;
    float maxDistance;

    if (this->m_flags & 0x10) {
        maxDistance = this->m_overrideDistanceMax;
    } else if (this->m_vehicleZoomEnabled) {
        maxDistance = 50.0f;
    } else {
        maxDistance = s_cvCameraDistanceMaxFactor->m_floatValue * s_cvCameraDistanceMax->m_floatValue;
        maxDistance = maxDistance < 0.0f ? 0.0f : (maxDistance >= 50.0f ? 50.0f : maxDistance);
    }

    if (this->m_smoothDistance.m_target > maxDistance) {
        this->ZoomToDistance(maxDistance, FrameTime::GetCurTimeMs(), 0.0f, 0);
    }
}

// OFFSET: 0x0043E320/0x140049500
void CGCamera::SetVehicleSeatZoomMinDistance(float distance) {
    this->m_flags |= 0x8;
    this->m_overrideDistanceMin = distance;

    if (distance > this->m_smoothDistance.m_target) {
        this->ZoomToDistance(this->m_overrideDistanceMin, FrameTime::GetCurTimeMs(), 0.0f, 0);
    }
}

// OFFSET: 0x0043E3C0/0x140049680
void CGCamera::SetVehicleSeatZoomMinDistanceToDefault() {
    this->m_flags &= ~0x8;
    float minDistance;

    if (this->m_flags & 0x8) {
        minDistance = this->m_overrideDistanceMin;
    } else if (CGBarberShop::m_barberShopEnabled) {
        minDistance = CGBarberShop::GetMinCameraDistance();
    } else {
        minDistance = 0.0f;
    }

    if (minDistance > this->m_smoothDistance.m_target) {
        this->ZoomToDistance(minDistance, FrameTime::GetCurTimeMs(), 0.0f, 0);
    }
}

// OFFSET: 0x0043B170/0x140045E40
void CGCamera::SetVehicleTrackingStyle(uint32_t style) {
    this->m_vehicleTrackingStyle = style;
}

// OFFSET: 0x0043DFC0/0x1400491A0
void CGCamera::SetVehicleZoomEnabled(int32_t enabled, float duration, int32_t animate) {
    if (this->m_vehicleZoomEnabled == enabled) {
        return;
    }

    if (duration < 0.5f) {
        duration = 0.5f;
    }

    this->SaveCameraSettings();

    float distance;

    if (enabled) {
        distance = s_cvCameraSavedVehicleDistance->m_floatValue;

        if (distance < 0.0f) {
            distance = 50.0f;
        }
    } else {
        distance = s_cvCameraSavedDistance->m_floatValue;
        distance = distance < 0.0f ? 0.0f : (distance >= 50.0f ? 50.0f : distance);
    }

    if (animate && fabsf(distance - this->m_distance) >= 0.001f) {
        if (distance <= this->m_distance) {
            this->ZoomIn(this->m_distance - distance, FrameTime::GetCurTimeMs(), duration);
        } else {
            this->ZoomOut(distance - this->m_distance, FrameTime::GetCurTimeMs(), duration);
        }
    }

    this->m_vehicleZoomEnabled = enabled;
}

// OFFSET: 0x0043DA20/0x140048B20
int32_t CGCamera::ShouldSmoothPitch(float minPitch, float maxPitch) {
    CGObject_C* target = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_UNIT, __FILE__, __LINE__);

    return target && !(this->m_state & 0x1) && s_cvCameraSmoothPitch->m_intValue && !(static_cast<CGUnit_C*>(target)->m_movement->m_flags & (MOVEMENTFLAG_FLYING | MOVEMENTFLAG_SWIMMING)) && (minPitch > this->m_pitch || maxPitch < this->m_pitch);
}

// OFFSET: 0x0043EA60/0x140049DF0
int32_t CGCamera::SmoothSetDistance(float distance, float delay, float scale, uint32_t time) {
    if ((this->m_state & 0x4000000) && fabsf(this->m_smoothDistance.m_target - distance) < 0.001f && fabsf(this->m_smoothDistance.m_param4 - delay) < 0.001f && fabsf(this->m_smoothDistance.m_param3 - scale) < 0.001f) {
        return 1;
    }

    if (fabsf(this->m_distance - distance) < 0.001f) {
        return 0;
    }

    this->m_smoothDistance.m_param4 = delay;
    this->m_smoothDistance.m_param3 = scale;

    float duration = fabsf(distance - this->m_distance) / s_cvCameraDistanceSmoothSpeed->m_floatValue * scale;

    return this->StartSmoothDistance(distance, duration, time + static_cast<int32_t>(delay * 1000.0f));
}

// OFFSET: 0x0043ACB0
void CGCamera::StartMotion(uint32_t motion, uint32_t time, uint32_t duration, float speed) {
    if (this->m_flags & 0x40) {
        return;
    }

    uint32_t bit = 1 << (2 * motion);

    if (!(this->m_motionFlags & bit)) {
        this->m_motionFlags |= bit;
        this->m_motionStartTime[motion] = time;
    }

    this->m_motionSpeed[motion] = speed;
    this->m_motionEndTime[motion] = duration ? duration + time : 0;
}

// OFFSET: 0x0043B220
int32_t CGCamera::StartSmoothDistance(float distance, float rate, uint32_t startTime) {
    if (fabsf(this->m_distance - distance) < 0.001f) {
        return 0;
    }

    this->m_state |= 0x4000000;
    this->m_smoothDistance.m_rate = rate;
    this->m_smoothDistance.m_startTimeMs = startTime;
    this->m_smoothDistance.m_startValue = this->m_distance;
    this->m_smoothDistance.m_target = distance;

    return 1;
}

// OFFSET: 0x0043B370
int32_t CGCamera::StartSmoothPitch(float pitch, float rate, uint32_t startTime) {
    this->m_pitch = UnwrapAngleToward(pitch, this->m_pitch, -3.1415927f, 3.1415927f);

    if (fabsf(this->m_pitch - pitch) < 0.001f) {
        return 0;
    }

    this->m_state |= 0x2000000;
    this->m_smoothPitch.m_startValue = this->m_pitch;
    this->m_smoothPitch.m_startTimeMs = startTime;
    this->m_smoothPitch.m_target = pitch;
    this->m_smoothPitch.m_rate = rate;

    return 1;
}

// OFFSET: 0x0043B450
int32_t CGCamera::StartSmoothTargetOffset(float offset, float rate, uint32_t startTime) {
    this->m_targetOffset = UnwrapAngleToward(offset, this->m_targetOffset, -3.1415927f, 3.1415927f);

    if (fabsf(this->m_targetOffset - offset) < 0.001f) {
        return 0;
    }

    this->m_state |= 0x8000000;
    this->m_smoothTargetOffset.m_startValue = this->m_targetOffset;
    this->m_smoothTargetOffset.m_startTimeMs = startTime;
    this->m_smoothTargetOffset.m_target = offset;
    this->m_smoothTargetOffset.m_rate = rate;

    return 1;
}

// OFFSET: 0x0043B530
int32_t CGCamera::StartSmoothYaw(float yaw, float rate, uint32_t startTime) {
    this->m_yaw = UnwrapAngleToward(yaw, this->m_yaw, -3.1415927f, 3.1415927f);

    if (fabsf(this->m_yaw - yaw) < 0.001f) {
        return 0;
    }

    this->m_state |= 0x1000000;
    this->m_smoothYaw.m_startValue = this->m_yaw;
    this->m_smoothYaw.m_startTimeMs = startTime;
    this->m_smoothYaw.m_target = yaw;
    this->m_smoothYaw.m_rate = rate;

    return 1;
}

// OFFSET: 0x0043AFB0/0x140045CE0
void CGCamera::UpdateAutoInteractState(int32_t enabled) {
    if (enabled) {
        this->m_flags |= 0x1;
    } else {
        this->m_flags &= ~0x1;
    }
}

// OFFSET: 0x0043DB40/0x140048C50
void CGCamera::UpdateBobbingState(int32_t moving) {
    if (!(this->m_state & 0x200) && moving) {
        this->m_state |= 0x200;
        this->m_bobTime = FrameTime::GetCurTimeMs();
    }

    if ((this->m_state & 0x200) && !moving) {
        this->BeginBobbingSmooth();
    }
}

// OFFSET: 0x0043E570/0x1400498E0
void CGCamera::UpdateMountHeightOrOffset(CGUnit_C* unit) {
    this->m_flags &= ~0x80;
    this->m_mountHeight = 0.0f;
    CM2Model* mount = unit->m_mountModel;

    if (!mount || !mount->IsLoaded() || !unit->m_worldModel || !unit->m_worldModel->IsLoaded()) {
        return;
    }

    if (mount->HasEvent(0x414D4324)) {
        this->m_heightChangedTimeMs = FrameTime::GetCurTimeMs();
        this->m_mountHeight = 0.0f;

        float height = mount->GetEventPosition(0x414D4324).z - mount->GetWorldPosition().z;
        float delta = fabsf(height - this->m_targetHeightFar);
        this->m_flags |= 0x80;

        if (delta > 0.05f) {
            this->m_targetHeightFar = height;
            this->m_targetHeightNear = height;
            this->m_targetHeightSwim = height;
        }
    } else if (!(this->m_state & 0x800000)) {
        if (mount->HasEvent(0x4D464324)) {
            this->m_mountHeight = mount->GetEventPivot(0x4D464324).z;
        }

        this->m_state |= 0x800000;
    }
}

// OFFSET: 0x0043E4D0/0x140049870
void CGCamera::UpdateVehicleTarget(uint32_t time) {
    if (!this->m_vehicleCamera) {
        return;
    }

    this->m_vehicleCamera->Update(time);

    if (!this->m_vehicleCamera) {
        return;
    }

    WOWGUID relativeTo = WOWGUID::Make(this->m_vehicleCamera->GetRelativeTo());

    if (!relativeTo || ClntObjMgrObjectPtr(relativeTo, TYPEMASK_OBJECT, __FILE__, __LINE__)) {
        this->m_relativeTo = relativeTo;
    }
}

// OFFSET: 0x0043D5A0/0x140048700
void CGCamera::ZoomIn(float distance, uint32_t time, float duration) {
    if (this->m_flags & 0x40) {
        return;
    }

    float speed = 1.0f;
    uint32_t motionTime;

    if (duration > 0.0f) {
        motionTime = static_cast<uint32_t>(duration * 1000.0f);
        speed = static_cast<float>(static_cast<uint32_t>(distance * 1000.0f)) / (1000.0f * (duration * s_cvCameraDistanceMoveSpeed->m_floatValue));
    } else {
        motionTime = static_cast<uint32_t>(distance / s_cvCameraDistanceMoveSpeed->m_floatValue * 1000.0f);
    }

    if (this->m_motionFlags & 0x4) {
        this->m_motionStopTime[1] = time;
        this->m_motionFlags |= 0x8;
    }

    if (this->m_motionEndTime[0] && (this->m_motionFlags & 0x1)) {
        this->m_motionEndTime[0] += motionTime;
    } else {
        this->StartMotion(0, time, motionTime, speed);
    }
}

// OFFSET: 0x0043D6C0/0x140048800
void CGCamera::ZoomOut(float distance, uint32_t time, float duration) {
    if (this->m_flags & 0x40) {
        return;
    }

    float speed = 1.0f;
    uint32_t motionTime;

    if (duration > 0.0f) {
        motionTime = static_cast<uint32_t>(duration * 1000.0f);
        speed = static_cast<float>(static_cast<uint32_t>(distance * 1000.0f)) / (1000.0f * (duration * s_cvCameraDistanceMoveSpeed->m_floatValue));
    } else {
        motionTime = static_cast<uint32_t>(distance / s_cvCameraDistanceMoveSpeed->m_floatValue * 1000.0f);
    }

    if (this->m_motionFlags & 0x1) {
        this->m_motionStopTime[0] = time;
        this->m_motionFlags |= 0x2;
    }

    if (this->m_motionEndTime[1] && (this->m_motionFlags & 0x4)) {
        this->m_motionEndTime[1] += motionTime;
    } else {
        this->StartMotion(1, time, motionTime, speed);
    }
}

// OFFSET: 0x0043D7E0/0x140048900
void CGCamera::ZoomToDistance(float distance, uint32_t time, float duration, int32_t lock) {
    if (this->m_flags & 0x40) {
        return;
    }

    if (fabsf(distance - this->m_distance) < 0.001f) {
        return;
    }

    if (this->m_motionFlags & 0x1) {
        this->m_motionStopTime[0] = time;
        this->m_motionFlags &= ~0x3;
    }

    if (this->m_motionFlags & 0x4) {
        this->m_motionStopTime[1] = time;
        this->m_motionFlags &= ~0xC;
    }

    if (distance <= this->m_distance) {
        this->ZoomIn(this->m_distance - distance, time, duration);
    } else {
        this->ZoomOut(distance - this->m_distance, time, duration);
    }

    if (lock) {
        this->m_flags |= 0x40;
    }
}

// OFFSET: 0x004450C0/0x140050D60
void CGCamera::CalcTargetCamera(CGObject_C* target, uint32_t time) {
    if (!target) {
        SErrSetLastError(87);
        return;
    }

    if (!(this->m_state & 0x4)) {
        this->FinishLoadingTarget(target);
    }

    this->UpdateVehicleTarget(FrameTime::GetCurTimeMs());
    CGUnit_C* unit = NULL;
    CGPlayer_C* player = NULL;
    uint32_t flying = 0;
    int32_t onFlyingSpline = 0;
    uint32_t swimming = 0;
    int32_t uncontrolled = 0;

    if (target->m_obj->m_type & TYPEMASK_UNIT) {
        unit = static_cast<CGUnit_C*>(target);

        if (target->m_obj->m_type & TYPEMASK_PLAYER) {
            player = static_cast<CGPlayer_C*>(target);
        }

        flying = (unit->m_unit->m_unitFieldFlags >> 20) & 0x1;
        unit->IsAutoTracking();
        CMoveSpline* spline = unit->m_movement->m_spline;

        if ((spline && !(spline->m_flags & 0x80) && (spline->m_flags & 0x200)) || (unit->m_movement->m_flags & 0x1000000)) {
            onFlyingSpline = 1;
        }

        swimming = unit->m_movement->m_flags & 0x100000;
        int32_t splineControlled = unit->m_movement->m_spline && !(unit->m_movement->m_spline->m_flags & 0x80) ? 1 : 0;
        unit->GetPitch();
        this->UpdateMountHeightOrOffset(unit);
        uncontrolled = splineControlled && !flying ? 1 : 0;
    }

    this->UpdateUncontrolledState(uncontrolled);
    C3Vector safePos = this->GetSafeWorldPos(target);
    float flightLower = -500.0f;
    // STATIC: 0x00D391C4/0x140BF3788
    static C3Vector s_clampedPosition;
    // STATIC: 0x00D391C0
    static uint8_t s_positionClamped;
    // STATIC: 0x00D391C1
    static uint8_t s_playSpaceDeath;
    World::GetFlightBoundsLower(safePos, flightLower);
    int32_t clampFlag = player && (player->m_player->m_playerFlags & 0x4000);

    if (flightLower > safePos.z || clampFlag) {
        if (flightLower > safePos.z && !s_positionClamped) {
            safePos = C3Vector(this->m_targetPosition.x, this->m_targetPosition.y, flightLower);
        } else {
            if (!s_positionClamped) {
                s_clampedPosition = this->m_targetPosition;
                s_positionClamped = 1;
            }

            safePos = s_clampedPosition;
        }

        if (s_playSpaceDeath) {
            SE3SoundKitProperties properties;
            SE3::PlaySoundKit("SpaceDeathUniversal", NULL, &properties, NULL, __FILE__, __LINE__, "CGCamera::CalcTargetCamera");
        }

        s_playSpaceDeath = 0;
    } else {
        s_playSpaceDeath = 1;
        s_positionClamped = 0;
    }

    if (flying && !this->m_wasFlying) {
        this->SetDesiredYawAngle(0.0f, 0.0f, 1.0f, FrameTime::GetCurTimeMs());

        if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(target)->m_player->m_playerFlags & 0x20000)) {
            this->SetDesiredPitchAngle(0.0f, 0.0f, 1.0f, FrameTime::GetCurTimeMs());
        }
    }

    if (this->m_motionFlags) {
        this->UpdateMotion(time);
    }

    this->UpdateTargetSmoothing(target, time);
    float facingYaw;
    float facingPitch;
    float facingRoll;
    this->UpdateTargetFacing(target, facingYaw, facingPitch, facingRoll);
    C3Vector offset(0.0f, 0.0f, 0.0f);
    CGUnit_C* targetUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (targetUnit) {
        float facing = targetUnit->GetSmoothFacing();
        C3Vector forward = this->Forward();
        C3Vector position(safePos.x - this->m_distance * forward.x, safePos.y - forward.y * this->m_distance, this->m_height + safePos.z - forward.z * this->m_distance);
        offset = StaticSingleton<CCameraManager>::Instance()->CalcShakeOffset(position, facing, onFlyingSpline, swimming);
    }

    this->RunBobs(this->m_bobOffset);
    offset.x = this->m_bobOffset.x + offset.x;
    offset.y = this->m_bobOffset.y + offset.y;
    offset.z = this->m_bobOffset.z + offset.z;
    float liquid = this->UpdateLiquidSurfaceStatus(target);

    if (this->m_vehicleYawStartTime != this->m_vehicleYawEndTime) {
        this->UpdateVehicleYawOffset(time);
    }

    float distance;
    float heightAdjust;
    this->m_state &= ~0x30000;
    this->m_state |= this->CollideCameraWithWorld(safePos, distance, heightAdjust, offset, liquid, this->m_collideExtent);

    if (distance <= 0.0f) {
        distance = 0.0f;
    }

    C3Vector cameraTarget(safePos.x + offset.x, safePos.y + offset.y, offset.z + safePos.z + heightAdjust);
    uint32_t state = this->m_state;

    if (!(state & 0x8) && !(state & 0x80000000)) {
        if (this->CanSmoothTarget()) {
            this->SetDesiredTargetOffset(0.0f, 0.0f, 1.0f, time);
        } else {
            this->CancelSmoothTargetOffset();
        }
    }

    state = this->m_state;

    if ((state & 0x8) && !(state & 0x40000000)) {
        distance = 0.0f;
        this->m_state = state & ~0x8000;
    } else if ((state & 0x80000000) && !(state & 0x40000000)) {
        this->m_state = state & 0x7FFFFFFF;
    }

    C3Vector noOffset(0.0f, 0.0f, 0.0f);
    this->m_position = this->GetCameraPosition(cameraTarget, distance, noOffset);
    this->BumpCameraPositionFromLiquid(cameraTarget, distance, this->m_position);

    if (!this->m_relativeTo && !s_cameraWaterCollision->m_intValue && 0.0f == this->m_vehicleYawOffset && (!unit || !unit->GetCanFly())) {
        C3Vector forward(cameraTarget.x - this->m_position.x, cameraTarget.y - this->m_position.y, cameraTarget.z - this->m_position.z);

        if (forward.z * forward.z + forward.y * forward.y + forward.x * forward.x > 0.1f) {
            forward.Normalize();
            this->SetFacing(forward);
        }
    }

    if (this->m_vehiclePitchStartTime != this->m_vehiclePitchEndTime) {
        this->UpdateVehiclePitchOffset(time);
    }

    if (0.0f != this->m_vehiclePitchOffset) {
        this->m_facing.RotateAroundY(-this->m_vehiclePitchOffset);
    }

    if (fabsf(this->m_targetOffset) >= 0.001f) {
        this->m_facing.RotateAroundY(this->m_targetOffset);
    }

    if (this->m_distance - distance > 0.11111111f) {
        this->m_state |= 0x4000000;
        this->m_distance = distance + 0.11111207f;
        this->m_smoothDistance.m_startValue = distance + 0.11111207f;
        this->m_smoothDistance.m_rate = 2.0f;
        this->m_smoothDistance.m_startTimeMs = time;
    }

    if (this->m_height - heightAdjust > 0.11111111f) {
        this->m_state |= 0x20000000;
        this->m_height = heightAdjust + 0.11111207f;
        this->m_smoothHeight.m_startValue = heightAdjust + 0.11111207f;
        this->m_smoothHeight.m_rate = 2.0f;
        this->m_smoothHeight.m_startTimeMs = time;
    }

    uint8_t fade = 0xFF;
    CGPlayer_C* activePlayer = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t sharedVehicle = 0;

    if (activePlayer && unit && activePlayer->m_vehiclePassenger && activePlayer->m_vehiclePassenger->m_state && activePlayer->m_vehiclePassenger->m_state != 3) {
        if (activePlayer == unit || activePlayer->GetTransportGUID() == unit->m_obj->m_guid.m_guid) {
            sharedVehicle = 1;
        }
    }

    CGUnit_C* fadeUnit = unit;

    while (true) {
        float fadeStart = 1.8315002f;
        float fadeEnd = 0.0027777778f;

        if (fadeUnit && ((fadeUnit->m_vehicle && fadeUnit->m_vehicle->m_vehicleRec) || (fadeUnit->m_vehiclePassenger && fadeUnit->m_vehiclePassenger->m_state == 3)) && !sharedVehicle) {
            if (!fadeUnit->m_vehicle || !fadeUnit->m_vehicle->m_vehicleRec) {
                fadeUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(fadeUnit->GetTransportGUID()), TYPEMASK_UNIT, __FILE__, __LINE__));
            }

            if (fadeUnit) {
                VehicleRec* vehicleRec = fadeUnit->m_vehicle ? fadeUnit->m_vehicle->m_vehicleRec : NULL;

                if (vehicleRec && (vehicleRec->m_flags & 0x80000)) {
                    CM2Model* model = fadeUnit->GetObjectModel();

                    if (model && model->IsLoaded()) {
                        M2SequenceInfo info;
                        info.Clear();
                        model->GetSequenceInfo(0, 0, info);
                        fadeEnd = vehicleRec->m_cameraFadeDistScalarMin * info.m_radius;
                        fadeStart = vehicleRec->m_cameraFadeDistScalarMax * info.m_radius;
                        distance = C3Vector::Distance(this->m_position, model->GetWorldPosition());
                    }
                }
            }
        }

        if (CGBarberShop::m_barberShopEnabled) {
            fadeStart = fadeStart * 0.33333334f;
        }

        if (this->m_height > distance) {
            float pitch = this->m_pitch;

            if (pitch < -1.1868238f) {
                float scale = (-1.5707964f - pitch) * -2.604353f;

                if (fabsf(scale) >= 0.00000023841858f) {
                    fadeStart = fadeStart / (scale * scale);
                }
            }
        }

        float nearDistance = distance - World::GetNearClip();

        if (nearDistance < fadeStart) {
            if (nearDistance <= fadeEnd) {
                fade = 0;
                break;
            }

            uint8_t value = static_cast<uint8_t>(static_cast<int32_t>(OrganicSmooth(0.0f, 255.0f, (nearDistance - fadeEnd) / (fadeStart - fadeEnd))));

            if (fade > value) {
                fade = value;
            }
        }

        if (fadeUnit && fadeUnit->HasVehicleTransport() && !sharedVehicle) {
            fadeUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(fadeUnit->GetTransportGUID()), TYPEMASK_UNIT, __FILE__, __LINE__));

            if (fadeUnit) {
                continue;
            }
        }

        break;
    }

    state = this->m_state;
    this->m_fade = fade;

    if ((state & 0x40000000) && ((state & 0x8000) || (state & 0x80000000))) {
        int32_t elapsed = time - this->m_smoothFoV.m_startTimeMs;

        if (elapsed >= 0) {
            float t = static_cast<uint32_t>(elapsed) * 0.001f / this->m_smoothFoV.m_rate * 1.5f;

            if (t >= 1.0f) {
                t = 1.0f;
            }

            t = t * t;

            if (state & 0x8000) {
                t = 1.0f - t;
            }

            uint8_t value = static_cast<uint8_t>(static_cast<int32_t>(t * 255.0f));

            if (fade >= value) {
                fade = value;
            }

            this->m_fade = fade;
        }
    }

    this->UpdateTargetHeight(target, time);
    this->m_targetPosition = target->GetPosition();
    this->m_wasFlying = flying;
}

// OFFSET: 0x00445C60/0x140051B60
int32_t CGCamera::UpdateCallback(const void* data, void* param) {
    CGCamera* camera = static_cast<CGCamera*>(param);

    if (!camera) {
        return 1;
    }

    uint32_t time = FrameTime::GetCurTimeMs();

    if (camera->m_model) {
        camera->CalcModelCamera(time);
        camera->CheckUnderwater();
        return 1;
    }

    CGObject_C* target = ClntObjMgrObjectPtr(camera->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (target) {
        camera->CalcTargetCamera(target, time);
        camera->CheckUnderwater();
    }

    return 1;
}

// OFFSET: 0x00440190/0x14004B450
void CGCamera::UpdateFreeLookFacing(float x, float y, float* pitchOut) {
    if (pitchOut) {
        *pitchOut = 0.0f;
    }

    if (this->m_state & 0x8000) {
        return;
    }

    CGObject_C* target = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (target && (target->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGUnit_C*>(target)->m_unit->m_unitFieldFlags & 0x100000) && (static_cast<CGPlayer_C*>(target)->m_player->m_playerFlags & 0x20000)) {
        return;
    }

    this->m_state |= 0x40;
    float pitchSpeed = s_cvCameraPitchMoveSpeed->m_floatValue;
    float yawSpeed = s_cvCameraYawMoveSpeed->m_floatValue;
    float pivotDXMax = s_cvCameraPivotDXMax->m_floatValue;
    float pivotDYMin = s_cvCameraPivotDYMin->m_floatValue;
    DDCToNDC(x, y, &x, &y);

    float yawDelta = yawSpeed * 0.017453292f * (x * 0.00125f);
    float pitchDelta = 0.017453292f * pitchSpeed * (y * 0.0016666667f);
    float pitchSign = s_cvMouseInvertPitch->m_intValue ? -1.0f : 1.0f;
    float yawSign = s_cvMouseInvertYaw->m_intValue ? -1.0f : 1.0f;

    if (pitchOut) {
        *pitchOut = pitchDelta * pitchSign;
        pitchDelta = 0.0f;
    }

    uint32_t canSmooth = this->CanSmoothTargetFacing(target);
    uint32_t state = this->m_state;
    int32_t adjust = 0;

    if (!(state & 0x8000000) && std::fabs(this->m_targetOffset) >= 0.001f) {
        adjust = 1;
    }

    if (canSmooth) {
        if (std::fabs(pitchDelta) > pivotDYMin && std::fabs(yawDelta) < pivotDXMax) {
            adjust = 1;
        }

        if (pitchDelta > 0.0f) {
            int32_t nearZero = std::fabs(this->m_targetOffset) < 0.00000023841858f;

            if (this->m_targetOffset < 0.0f && pitchSign * pitchDelta + this->m_targetOffset > 0.0f) {
                pitchDelta = pitchDelta + this->m_targetOffset;
                adjust = 0;
                this->m_targetOffset = 0.0f;
                this->m_smoothTargetOffset.m_target = 0.0f;
            } else if (nearZero) {
                adjust = 0;
                this->m_targetOffset = 0.0f;
                this->m_smoothTargetOffset.m_target = 0.0f;
            }
        }
    }

    int32_t setDesired = adjust == 0;

    if (adjust) {
        this->m_targetOffset = pitchDelta * pitchSign + this->m_targetOffset;

        if (this->m_pitch >= 0.0f) {
            setDesired = 1;
        }

        float minOffset = MIN_PITCH_ANGLE - this->m_pitch;

        if (this->m_pitch < 0.0f && this->m_targetOffset < minOffset) {
            this->m_targetOffset = minOffset;
        }

        if (this->m_targetOffset > 0.0f) {
            setDesired = 1;
        }
    }

    if (setDesired) {
        this->SetDesiredTargetOffset(0.0f, 0.0f, 1.0f, FrameTime::GetCurTimeMs());
        this->ClampPitchToLimits(pitchSign * pitchDelta);
    } else {
        this->m_smoothTargetOffset.m_target = this->m_targetOffset;
        this->m_smoothTargetOffset.m_startTimeMs = 0;
        this->m_state = state & ~0x8000000;
        this->m_smoothTargetOffset.m_rate = 0.0f;
    }

    this->UpdateYaw(-(yawSign * yawDelta));
    this->ClampPitchAndNormalize();
}

// OFFSET: 0x0043DB90/0x140048CB0
void CGCamera::UpdateMotion(uint32_t time) {
    float minDistance;

    if (this->m_flags & 0x8) {
        minDistance = this->m_overrideDistanceMin;
    } else if (CGBarberShop::m_barberShopEnabled) {
        minDistance = CGBarberShop::GetMinCameraDistance();
    } else {
        minDistance = 0.0f;
    }

    float maxDistance;

    if (this->m_flags & 0x10) {
        maxDistance = this->m_overrideDistanceMax;
    } else if (this->m_vehicleZoomEnabled) {
        maxDistance = 50.0f;
    } else {
        maxDistance = s_cvCameraDistanceMaxFactor->m_floatValue * s_cvCameraDistanceMax->m_floatValue;

        if (maxDistance < 0.0f) {
            maxDistance = 0.0f;
        } else if (50.0f <= maxDistance) {
            maxDistance = 50.0f;
        }
    }

    this->m_state |= 0x40;

    for (int32_t i = 0; i < 6; i++) {
        uint32_t bit = 1 << (2 * i);

        if (!(this->m_motionFlags & bit)) {
            continue;
        }

        uint32_t endTime = this->m_motionEndTime[i];

        if (endTime && static_cast<int32_t>(time - endTime) >= 0) {
            this->m_motionStopTime[i] = endTime;
            this->m_motionFlags |= 1 << (2 * i + 1);
        }

        uint32_t elapsed;

        if (this->m_motionFlags & (1 << (2 * i + 1))) {
            int32_t delta = this->m_motionStopTime[i] - this->m_motionStartTime[i];
            elapsed = delta < 0 ? 0 : delta;
            this->m_motionFlags &= ~(3 << (2 * i));

            if (!(this->m_motionFlags & bit)) {
                this->m_flags &= ~0x40;
            }
        } else {
            elapsed = time - this->m_motionStartTime[i];

            if (static_cast<int32_t>(elapsed) < 0) {
                continue;
            }

            this->m_motionStartTime[i] = time;
        }

        switch (i) {
            case 0:
                this->m_smoothDistance.m_target = this->m_smoothDistance.m_target - s_cvCameraDistanceMoveSpeed->m_floatValue * this->m_motionSpeed[0] * static_cast<float>(elapsed) * 0.001f;

                if (this->m_smoothDistance.m_target < minDistance) {
                    this->m_smoothDistance.m_target = minDistance;
                }

                if (!(this->m_state & 0x4000000)) {
                    this->m_distance = this->m_smoothDistance.m_target;
                }

                break;

            case 1:
                this->m_smoothDistance.m_target = s_cvCameraDistanceMoveSpeed->m_floatValue * this->m_motionSpeed[1] * static_cast<float>(elapsed) * 0.001f + this->m_smoothDistance.m_target;

                if (this->m_smoothDistance.m_target > maxDistance) {
                    this->m_smoothDistance.m_target = maxDistance;
                }

                if (!(this->m_state & 0x4000000)) {
                    this->m_distance = this->m_smoothDistance.m_target;
                }

                break;

            case 2:
                this->m_smoothYaw.m_target = s_cvCameraYawMoveSpeed->m_floatValue * this->m_motionSpeed[2] * static_cast<float>(elapsed) * 0.001f * 0.017453292f + this->m_smoothYaw.m_target;

                if (this->m_smoothYaw.m_target > 6.2831855f) {
                    do {
                        this->m_smoothYaw.m_target = this->m_smoothYaw.m_target - 6.2831855f;
                    } while (6.2831855f < this->m_smoothYaw.m_target);
                }

                if (!(this->m_state & 0x1000000)) {
                    this->m_yaw = this->m_smoothYaw.m_target;
                }

                break;

            case 3:
                this->m_smoothYaw.m_target = this->m_smoothYaw.m_target - s_cvCameraYawMoveSpeed->m_floatValue * this->m_motionSpeed[3] * static_cast<float>(elapsed) * 0.001f * 0.017453292f;

                if (this->m_smoothYaw.m_target < 0.0f) {
                    do {
                        this->m_smoothYaw.m_target = this->m_smoothYaw.m_target + 6.2831855f;
                    } while (0.0f > this->m_smoothYaw.m_target);
                }

                if (!(this->m_state & 0x1000000)) {
                    this->m_yaw = this->m_smoothYaw.m_target;
                }

                break;

            case 4:
                this->m_smoothPitch.m_target = s_cvCameraPitchMoveSpeed->m_floatValue * this->m_motionSpeed[4] * static_cast<float>(elapsed) * 0.001f * 0.017453292f + this->m_smoothPitch.m_target;

                if (this->m_smoothPitch.m_target > MAX_PITCH_ANGLE) {
                    this->m_smoothPitch.m_target = MAX_PITCH_ANGLE;
                }

                if (!(this->m_state & 0x2000000)) {
                    this->m_pitch = this->m_smoothPitch.m_target;
                }

                break;

            case 5:
                this->m_smoothPitch.m_target = this->m_smoothPitch.m_target - s_cvCameraPitchMoveSpeed->m_floatValue * this->m_motionSpeed[5] * static_cast<float>(elapsed) * 0.001f * 0.017453292f;

                if (this->m_smoothPitch.m_target < MIN_PITCH_ANGLE) {
                    this->m_smoothPitch.m_target = MIN_PITCH_ANGLE;
                }

                if (!(this->m_state & 0x2000000)) {
                    this->m_pitch = this->m_smoothPitch.m_target;
                }

                break;
        }
    }
}

// OFFSET: 0x00442B60/0x14004E040
void CGCamera::ResetView(int32_t view) {
    if (view < 0) {
        for (int32_t i = 0; i < 8; i++) {
            this->ResetView(i);
        }

        return;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (s_cameraViewAngular[i]) {
            this->m_views[view][i] = SStrToFloat(s_cameraViewDefaults[view].m_values[i]) * 0.017453292f;
        } else {
            this->m_views[view][i] = SStrToFloat(s_cameraViewDefaults[view].m_values[i]);
        }
    }

    if (view == this->m_viewIndex) {
        this->SetView(this->m_viewIndex, s_cvCameraViewBlendStyle->m_intValue, 0);
    }
}

// OFFSET: 0x004413A0/0x14004C7F0
void CGCamera::SetView(int32_t view, int32_t blendStyle, int32_t transition) {
    if (this->m_state & 0x8) {
        return;
    }

    if (view == 6) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!(player->m_player->m_playerFlags & 0x80000)) {
            return;
        }
    }

    if (this->m_viewIndex == view && !(this->m_state & 0x40) && blendStyle) {
        blendStyle = 2;
    }

    SysMsgPrintf(STATUS_INFO, 4, "Camera view %s", s_cameraViewModeNames[view]);
    uint32_t time = FrameTime::GetCurTimeMs();

    if (this->m_viewIndex != view) {
        if (!transition) {
            char value[16];
            SStrPrintf(value, sizeof(value), "%d", view);
            s_cvCameraView->Set(value, true, false, false, true);
        }

        this->m_viewIndex = view;
    }

    this->m_state &= ~0x40;
    uint32_t state = this->m_state;

    if (transition == 1) {
        this->unk_02F4 = this->m_smoothDistance.m_target;
    }

    this->m_smoothDistance.m_target = this->m_distance;
    state &= 0xF9FFFFFF;
    this->m_smoothDistance.m_rate = 0.0f;
    this->m_smoothDistance.m_startTimeMs = 0;
    this->m_smoothPitch.m_startTimeMs = 0;
    this->m_smoothPitch.m_target = this->m_pitch;
    this->m_state = state;
    this->m_smoothPitch.m_rate = 0.0f;

    if (transition != 1) {
        this->m_smoothTargetOffset.m_target = this->m_targetOffset;
        this->m_smoothTargetOffset.m_startTimeMs = 0;
        this->m_state = state & ~0x8000000;
        this->m_smoothTargetOffset.m_rate = 0.0f;
    }

    if (transition != 2) {
        this->m_state &= ~0x1000000;
        this->m_smoothYaw.m_target = this->m_yaw;
        this->m_smoothYaw.m_startTimeMs = 0;
        this->m_smoothYaw.m_rate = 0.0f;
    }

    float yaw = this->m_views[this->m_viewIndex][2];

    if (this->m_ignoreFacingRefs > 0) {
        CGObject_C* target = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);
        float facing;

        if (!target) {
            facing = 0.0f;
        } else if (target->m_obj->m_type & TYPEMASK_UNIT) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(target);
            unit->UpdateSmoothFacing(NULL, NULL);
            facing = this->m_vehicleCamera ? this->m_vehicleCamera->m_facing : unit->m_smoothTarget.m_value;
        } else {
            facing = target->GetFacing();
        }

        yaw = CMath::normalizeangle0to2pi_(facing + yaw);
    }

    if (blendStyle == 1) {
        int32_t distanceSet = this->SmoothSetDistance(this->m_views[view][0], 0.0f, 1.0f, time);
        int32_t pitchSet = this->SetDesiredPitchAngle(this->m_views[view][1], 0.0f, 1.0f, time);
        int32_t yawSet = this->SetDesiredYawAngle(yaw, 0.0f, 1.0f, time);
        int32_t offsetSet = this->SetDesiredTargetOffset(0.0f, 0.0f, 1.0f, time);
        float rate = 0.0f;

        if (distanceSet && !(this->m_smoothDistance.m_rate < 0.0f)) {
            rate = this->m_smoothDistance.m_rate;
        }

        if (pitchSet && rate <= this->m_smoothPitch.m_rate) {
            rate = this->m_smoothPitch.m_rate;
        }

        if (yawSet && rate <= this->m_smoothYaw.m_rate) {
            rate = this->m_smoothYaw.m_rate;
        }

        if (offsetSet && rate <= this->m_smoothTargetOffset.m_rate) {
            rate = this->m_smoothTargetOffset.m_rate;
        }

        float minTime = s_cvCameraSmoothTimeMin->m_floatValue;
        float maxTime = s_cvCameraSmoothTimeMax->m_floatValue;

        if (rate < minTime) {
            rate = minTime;
        }

        if (rate > maxTime) {
            rate = maxTime;
        }

        if (distanceSet) {
            this->m_smoothDistance.m_rate = rate;
        }

        if (pitchSet) {
            this->m_smoothPitch.m_rate = rate;
        }

        if (yawSet) {
            this->m_smoothYaw.m_rate = rate;
        }

        if (offsetSet) {
            this->m_smoothTargetOffset.m_rate = rate;
        }

        if ((this->m_state & 0x10000000) && (distanceSet || pitchSet || yawSet || offsetSet)) {
            this->m_smoothGroundTilt.m_rate = rate;
        }

        return;
    }

    if (blendStyle != 2) {
        return;
    }

    int32_t changed;

    if (!transition) {
        int32_t distanceChanged = fnotequalz(this->m_distance, this->m_views[view][0], 0.001f) != 0;
        int32_t pitchChanged = fnotequalz(this->m_pitch, this->m_views[view][1], 0.001f) != 0;
        int32_t yawChanged = fnotequalz(this->m_yaw, yaw, 0.001f) != 0;
        int32_t offsetChanged = fnotequalz(this->m_targetOffset, 0.0f, 0.001f) != 0;
        this->m_distance = this->m_views[view][0];
        this->m_smoothDistance.m_target = this->m_views[view][0];
        this->m_pitch = this->m_views[view][1];
        this->m_smoothPitch.m_target = this->m_views[view][1];
        this->m_yaw = yaw;
        this->m_smoothYaw.m_target = yaw;
        this->m_targetOffset = 0.0f;
        this->m_smoothTargetOffset.m_target = 0.0f;
        changed = distanceChanged || pitchChanged || yawChanged || offsetChanged;
    } else if (transition == 1) {
        this->m_prevDistance = this->m_distance;
        this->m_prevViewIndex = this->m_viewIndex;
        changed = 1;
    } else if (transition == 2) {
        this->m_smoothDistance.m_target = 0.0f;
        this->m_distance = 0.0f;
        changed = 1;
    } else {
        changed = 0;
    }

    if ((this->m_state & 0x10) && (this->m_state & 0x10000000) && changed) {
        this->PerformTerrainTilt(ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__), time, 1);
    }
}

// OFFSET: 0x00440DA0/0x14004C270
void CGCamera::UpdateTrackingState(int32_t tracking) {
    uint32_t wasTracking = this->m_state & 0x100;
    this->m_state &= ~0x4000;

    if (wasTracking) {
        if (!tracking) {
            this->m_state &= ~0x100;
        }
    } else if (tracking) {
        this->m_state |= 0x100;
    }

    uint32_t ignoring = this->m_state & 0x2000;
    int32_t ignore;

    if (!(this->m_state & 0x100)) {
        ignore = 0;
    } else if (this->m_vehicleTrackingStyle == 1) {
        ignore = 1;
    } else if (this->m_vehicleTrackingStyle == 2) {
        ignore = 0;
    } else {
        int32_t style = s_cvCameraSmoothTrackingStyle->m_intValue;

        if (!style) {
            ignore = 1;
        } else if (style == 2) {
            ignore = 0;
        } else {
            uint32_t type = CGUnit_C::GetTrackingType();
            ignore = type != 1 && type != 3;
        }
    }

    if (ignoring) {
        if (!ignore) {
            this->DecIgnoreFacing();
            this->m_state &= ~0x2000;
        }
    } else if (ignore) {
        this->IncIgnoreFacing();
        this->m_state |= 0x2000;
    }

    this->SmoothFreeLook(CGInputControl::GetActive(), 0);
}

// OFFSET: 0x004404D0/0x14004B830
void CGCamera::SyncFreeLookPitch(float pitch, uint32_t time, int32_t force) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_UNIT, __FILE__, __LINE__));
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (unit && unit->m_vehicle && unit->m_vehicle->m_vehicleRec && unit->m_vehicle->IsSuppressingCameraPitchWhileMouseAiming()) {
        return;
    }

    uint32_t state = this->m_state;

    if (!(state & 0x100) && ((state & 0x100000) || (state & 0x200000)) && s_cvCameraDive->m_intValue && (!player || player->GetDrunknessTurnValue() == 0.0f)) {
        float surfacePitch = s_cvCameraSurfacePitch->m_floatValue * 0.017453292f;
        float submergePitch = s_cvCameraSubmergePitch->m_floatValue * 0.017453292f;

        if ((state & 0x200000) && surfacePitch < 0.0f && pitch < 0.0f && pitch > surfacePitch) {
            pitch = 0.0f;
            force = 1;
        }

        if ((state & 0x100000) && submergePitch > 0.0f && ((unit && !(unit->m_movement->m_flags2 & MOVEMENTFLAG2_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY)) || pitch > 0.0f) && submergePitch > pitch) {
            pitch = 0.0f;
        } else if (!force) {
            return;
        }
    } else if (!force) {
        return;
    }

    CGInputControl::GetActive()->CameraPitchPlayer(time, pitch);
}

// OFFSET: 0x00440650/0x14004B9D0
int32_t CGCamera::CanSmoothTarget() {
    if (fabs(this->m_targetOffset) < 0.001f) {
        return 0;
    }

    if (this->m_state & 0x100) {
        if (this->m_vehicleTrackingStyle == 1) {
            return 0;
        }

        if (this->m_vehicleTrackingStyle != 2 && (!s_cvCameraSmoothTrackingStyle->m_intValue || !CGUnit_C::GetTrackingType())) {
            return 0;
        }
    }

    return !this->CanSmoothTargetFacing(ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__));
}

// OFFSET: 0x004406D0/0x14004BA70
int32_t CGCamera::CanSmoothYaw(float yawMin, float yawMax) {
    CGObject_C* target = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!target || (this->m_state & 0x1) || !s_cvCameraSmoothYaw->m_intValue) {
        return 0;
    }

    if (this->m_ignoreFacingRefs <= 0) {
        return yawMin > this->m_yaw || yawMax < this->m_yaw;
    }

    float facing;

    if (target->m_obj->m_type & TYPEMASK_UNIT) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(target);

        if (this->m_state & 0x100) {
            facing = CGUnit_C::GetTrackingTurn();
        } else {
            unit->UpdateSmoothFacing(NULL, NULL);
            facing = this->GetChaseFacing(unit);
        }
    } else {
        facing = target->GetFacing();
    }

    return fnotequal(this->m_yaw, facing);
}

// OFFSET: 0x004407B0/0x14004BB80
void CGCamera::SmoothFreeLook(CGInputControl* input, int32_t idle) {
    if (!input) {
        return;
    }

    uint32_t reasons = 0;

    if (this->m_state & 0x1000) {
        reasons = 0x40;
    }

    uint32_t flags = input->m_flags;

    if (((flags & 0x300) && !(flags & 0x2000001)) || (flags & 0x2000001)) {
        reasons |= 0x20;
    }

    if ((flags & 0xC0) || ((flags & 0x2000001) && (flags & 0x300))) {
        reasons |= 0x10;
    }

    if ((flags & 0x1030) || ((flags & 0x1) && (flags & 0x2))) {
        reasons |= 0x8;
    }

    if (this->m_state & 0x100) {
        if (CGUnit_C::GetTrackingType() == 3 && !(this->m_flags & 0x1)) {
            reasons |= 0x8;
        } else {
            reasons |= 0x4;
        }
    }

    if (idle) {
        reasons |= 0x2;
    }

    flags = input->m_flags;

    if (!(flags & 0x1030) && !(flags & 0xC0) && (!(flags & 0x2000001) || !(flags & 0x300)) && (!(flags & 0x300) || (flags & 0x2000001)) && !(flags & 0x1E00000)) {
        reasons |= 0x1;
    }

    this->m_state &= ~0x4000u;
    reasons &= 0x7F;

    if (!reasons || !this->IsCustomViewSmoothingActive()) {
        return;
    }

    uint32_t time = FrameTime::GetCurTimeMs();
    uint32_t style;

    if (reasons & 0x44) {
        if (this->m_vehicleTrackingStyle == 1) {
            style = 0;
        } else if (this->m_vehicleTrackingStyle == 2) {
            style = 3;
        } else {
            style = s_cvCameraSmoothTrackingStyle->m_intValue;
        }
    } else {
        style = s_cvCameraSmoothStyle->m_intValue;
    }

    float smoothState[2];

    for (int32_t state = 7; state-- > 0;) {
        if (!((1 << state) & reasons) || style >= 5 || static_cast<uint32_t>(state) >= 7) {
            continue;
        }

        smoothState[0] = s_cvCameraSmoothState[style][state][0]->m_floatValue;

        if (smoothState[0] < 0.0f) {
            smoothState[0] = 0.0f;
        } else if (smoothState[0] >= 100.0f) {
            smoothState[0] = 99.0f;
        }

        smoothState[1] = s_cvCameraSmoothState[style][state][1]->m_floatValue;

        if (smoothState[1] < 0.0f) {
            smoothState[1] = 0.0f;
        } else if (smoothState[1] >= 100.0f) {
            smoothState[1] = 99.0f;
        }

        break;
    }

    float viewData[3][2];

    for (uint32_t view = 3; view-- > 0;) {
        if (style >= 5 || view >= 3) {
            continue;
        }

        for (int32_t i = 2; i-- > 0;) {
            viewData[view][i] = s_cvCameraSmoothViewData[style][view][i]->m_floatValue;

            switch (i) {
                case 0:
                    viewData[view][0] = viewData[view][0] + smoothState[0];
                    break;
                case 1:
                    viewData[view][1] = viewData[view][1] * smoothState[1];
                    break;
            }

            if (s_smoothViewDataMin[i] > viewData[view][i]) {
                viewData[view][i] = s_smoothViewDataMin[i];
            }

            if (s_smoothViewDataMax[i] <= viewData[view][i]) {
                viewData[view][i] = s_smoothViewDataMax[i] - 1.0f;
            }
        }
    }

    float pitchMin = s_cvCameraPitchSmoothMin->m_floatValue * 0.017453292f;
    float pitchMax = s_cvCameraPitchSmoothMax->m_floatValue * 0.017453292f;
    float yawMin = s_cvCameraYawSmoothMin->m_floatValue * 0.017453292f;
    float yawMax = 0.017453292f * s_cvCameraYawSmoothMax->m_floatValue;

    int32_t smoothPitch = this->ShouldSmoothPitch(pitchMin, pitchMax);
    int32_t smoothTarget = this->CanSmoothTarget();
    int32_t smoothYaw = this->CanSmoothYaw(yawMin, yawMax);
    float rate = 0.0f;

    if (smoothPitch) {
        if (viewData[1][1] == 0.0f) {
            this->CancelSmoothPitch();
            smoothPitch = 0;
        } else {
            float pitch = this->m_views[this->m_viewIndex][1];

            if (pitchMin > this->m_pitch) {
                pitch = pitchMin;
            }

            if (pitchMax < this->m_pitch) {
                pitch = pitchMax;
            }

            smoothPitch = this->SetDesiredPitchAngle(pitch, viewData[1][0], viewData[1][1], time);

            if (smoothPitch && this->m_smoothPitch.m_rate >= 0.0f) {
                rate = this->m_smoothPitch.m_rate;
            }
        }
    }

    if (smoothTarget) {
        if (smoothState[1] == 0.0f) {
            this->CancelSmoothTargetOffset();
            smoothTarget = 0;
        } else {
            smoothTarget = this->SetDesiredTargetOffset(0.0f, smoothState[0], smoothState[1], time);

            if (smoothTarget && rate <= this->m_smoothTargetOffset.m_rate) {
                rate = this->m_smoothTargetOffset.m_rate;
            }
        }
    }

    if (smoothYaw) {
        if (0.0f == viewData[2][1]) {
            this->CancelSmoothYaw();
            smoothYaw = 0;
        } else {
            float base = this->m_views[this->m_viewIndex][2];
            float yaw = base;

            if (this->m_ignoreFacingRefs <= 0) {
                if (yawMin > this->m_yaw) {
                    yaw = yawMin;
                }

                if (yawMax < this->m_yaw) {
                    yaw = yawMax;
                }
            } else {
                CGObject_C* target = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

                if (target && (target->m_obj->m_type & TYPEMASK_UNIT)) {
                    CGUnit_C* unit = static_cast<CGUnit_C*>(target);

                    if (this->m_state & 0x100) {
                        yaw = CMath::normalizeangle0to2pi_(CGUnit_C::GetTrackingTurn() + base);
                    } else {
                        unit->UpdateSmoothFacing(NULL, NULL);

                        if (input->m_facingOverrideActive) {
                            yaw = CMath::normalizeangle0to2pi_(input->m_facingOverride + base);
                        } else {
                            yaw = CMath::normalizeangle0to2pi_(this->GetChaseFacing(unit) + base);
                        }
                    }
                } else {
                    yaw = CMath::normalizeangle0to2pi_(target->GetFacing() + base);
                }
            }

            smoothYaw = this->SetDesiredYawAngle(yaw, viewData[2][0], viewData[2][1], time);

            if (smoothYaw && rate <= this->m_smoothYaw.m_rate) {
                rate = this->m_smoothYaw.m_rate;
            }
        }
    }

    float minTime = s_cvCameraSmoothTimeMin->m_floatValue;
    float maxTime = s_cvCameraSmoothTimeMax->m_floatValue;

    if (rate >= minTime) {
        minTime = rate;
    }

    if (minTime <= maxTime) {
        maxTime = minTime;
    }

    if (smoothYaw) {
        this->m_smoothYaw.m_rate = maxTime;
    }

    if (smoothPitch) {
        this->m_smoothPitch.m_rate = maxTime;
    }

    if (smoothTarget) {
        this->m_smoothTargetOffset.m_rate = maxTime;
    }
}

// OFFSET: 0x0043D230/0x140048260
void CGCamera::BumpCameraPositionFromLiquid(C3Vector& target, float distance, C3Vector& position) {
    float offsets[4];
    offsets[0] = -0.3f;
    offsets[3] = -0.3f;
    float x = position.x;
    float y = position.y;
    int32_t count = 0;
    offsets[1] = 0.3f;
    offsets[2] = 0.3f;
    uint32_t liquidType = 0;
    float liquidHeight = std::numeric_limits<float>::max();
    World::QueryLiquidNearest(position, liquidType, liquidHeight);

    if (liquidHeight == std::numeric_limits<float>::max()) {
        return;
    }

    float z = position.z;

    if (!(liquidHeight - z > -0.3f) || !(liquidHeight - z < 0.3f)) {
        return;
    }

    C3Vector end;

    for (float* step = &offsets[2 * (z >= liquidHeight)]; ; step++) {
        C3Vector hit(0.0f, 0.0f, 0.0f);
        float hitDistance = 1.0f;
        end = C3Vector(x, y, *step + liquidHeight);

        if (!World::Intersect(position, end, &hit, &hitDistance, 0x900171, NULL)) {
            break;
        }

        if (++count >= 2) {
            return;
        }
    }

    position = end;

    if (distance < 0.001f) {
        target = end;
    }
}

// OFFSET: 0x00440F10/0x14004C3E0
void CGCamera::RunBobs(C3Vector& offset) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit || !this->IsBobbingAllowed(unit)) {
        return;
    }

    float facing = this->m_vehicleCamera ? this->m_vehicleCamera->m_facing : unit->m_smoothTarget.m_value;
    float horizontal = s_cameraBobHorizontalAmplitude * 0.027777778f;
    float vertical = 0.027777778f * s_cameraBobVerticalAmplitude;
    float frequency = s_cameraBobFrequency;
    float speed = unit->m_movement->GetBaseSpeed(0) * 0.14285715f;

    if (0.5f > speed) {
        speed = 0.5f;
    } else if (1.5f <= speed) {
        speed = 1.5f;
    }

    frequency = speed * frequency;
    float time = static_cast<float>(FrameTime::GetCurTimeMs() - this->m_bobTime) * 0.001f;
    float sway = std::sin(time * frequency * 6.2831855f) * horizontal;
    float angle = facing + 1.5707964f;
    float c = std::cos(angle);
    float s = std::sin(angle);
    offset.x = c * sway;
    offset.y = sway * s;
    offset.z = std::sin((frequency + frequency) * time * 6.2831855f) * vertical;
}

// OFFSET: 0x00440E90/0x14004C350
void CGCamera::UpdateUncontrolledState(int32_t uncontrolled) {
    if (uncontrolled) {
        if (!(this->m_state & 0x1000)) {
            this->IncIgnoreFacing();
            this->m_state = (this->m_state & 0xFFFFAFFF) | 0x1000;
        }

        this->SmoothFreeLook(CGInputControl::GetActive(), 0);
    }

    if ((this->m_state & 0x1000) && !uncontrolled) {
        this->DecIgnoreFacing();
        this->m_state &= 0xFFFFAFFF;
        this->SmoothFreeLook(CGInputControl::GetActive(), 0);
    }
}

// OFFSET: 0x00444C90/0x1400508A0
uint32_t CGCamera::CollideCameraWithWorld(const C3Vector& target, float& distance, float& height, const C3Vector& offset, float liquid, float& extent) {
    extent = 1.0f;
    uint32_t result = 0;
    distance = this->m_smoothDistance.m_target <= this->m_distance ? this->m_distance : this->m_smoothDistance.m_target;
    height = this->m_smoothHeight.m_target > this->m_height ? this->m_smoothHeight.m_target : this->m_height;

    if (this->m_state & 0x8) {
        distance = this->m_distance;
        return 0;
    }

    float maxHeight = this->m_height <= 0.83333331f ? 0.83333331f : this->m_height;

    if (height <= s_minCollideHeight) {
        height = s_minCollideHeight;
    } else {
        C3Vector start(target.x, target.y, target.z + 0.83333331f);
        float probe = this->m_flyingMountHeight + height;
        CGObject_C* object = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);
        float top;

        if (object && (object->m_obj->m_type & TYPEMASK_UNIT)) {
            float maxCameraHeight = static_cast<CGUnit_C*>(object)->GetMaxCameraHeight(0);
            top = probe > maxCameraHeight - 0.11111111f ? maxCameraHeight - 0.11111111f : probe;
        } else {
            top = probe;
        }

        C3Vector end(target.x, target.y, target.z + top);

        if (end.z - start.z >= 0.001f) {
            float hitDistance = 1.0f;
            C3Vector hit(0.0f, 0.0f, 0.0f);

            if (World::Intersect(start, end, &hit, &hitDistance, 0x900171, NULL)) {
                float collided = (end.z - start.z) * hitDistance;
                extent = hitDistance;
                height = collided;

                if (collided < 0.0f) {
                    height = 0.0f;
                }

                result = 0x20000;
            }
        }
    }

    if (height < 0.83333331f) {
        height = 0.8333333f;
    }

    if (height > maxHeight) {
        height = maxHeight;
    }

    C3Vector base(target.x, target.y, height + target.z);
    float original = distance;

    if (original > 0.001f) {
        C3Vector hit(0.0f, 0.0f, 0.0f);
        C3Vector forward = this->Forward();
        float lift = this->m_flyingMountHeight;
        C3Vector end(base.x - forward.x * distance, base.y - forward.y * distance, base.z - forward.z * distance);

        if (fabsf(lift) >= 0.00000023841858f) {
            C3Vector up = this->Up();
            end.x = lift * up.x * extent + end.x;
            end.y = up.y * lift * extent + end.y;
            end.z = up.z * lift * extent + end.z;
        }

        float hitDistance = 1.0f;
        uint32_t flags = s_cameraWaterCollision->m_intValue ? 0x920001 : 0x900001;

        if (World::Intersect(base, end, &hit, &hitDistance, flags, NULL)) {
            result |= 0x10000;
            distance = distance * hitDistance;
        }

        float probe = distance;

        if (probe >= 0.001f) {
            C3Vector position = this->GetCameraPosition(base, probe, offset);

            if (this->GetCameraDistance(distance, position, base, 0x170)) {
                result |= 0x10000;
            }

            float after = distance;

            if (fnotequal(original, after)) {
                distance = after - 0.11111111f;

                if (after - 0.11111111f <= 0.0f) {
                    distance = 0.0f;
                }
            }
        }
    }

    if (distance > this->m_distance) {
        distance = this->m_distance;
    }

    return result;
}


// OFFSET: 0x00441F10/0x14004D5C0
void CGCamera::UpdateTargetSmoothing(CGObject_C* target, uint32_t time) {
    if (this->HasTargetOffset() && fabsf(this->m_bobDuration) >= 2.3841858e-7f) {
        float t = (time - this->m_bobTime) * 0.001f / this->m_bobDuration;

        if (t < 1.0f) {
            this->m_bobOffset.x = OrganicSmooth(this->m_bobStart.x, 0.0f, t);
            this->m_bobOffset.y = OrganicSmooth(this->m_bobStart.y, 0.0f, t);
            this->m_bobOffset.z = OrganicSmooth(this->m_bobStart.z, 0.0f, t);
        } else {
            this->m_bobOffset.x = 0.0f;
            this->m_bobOffset.y = 0.0f;
            this->m_bobOffset.z = 0.0f;
        }
    }

    if (fabsf(this->m_smoothFoV.m_target - this->m_targetFov) < 2.3841858e-7f) {
        this->m_state &= ~0x40000000;
        this->m_smoothFoV.m_target = this->m_targetFov;
        this->m_smoothFoV.m_startTimeMs = 0;
        this->m_smoothFoV.m_rate = 0.0f;
    } else if ((this->m_state & 0x40000000) && static_cast<int32_t>(time - this->m_smoothFoV.m_startTimeMs) >= 0) {
        float t = (time - this->m_smoothFoV.m_startTimeMs) * 0.001f / this->m_smoothFoV.m_rate;
        this->m_targetFov = t < 1.0f ? OrganicSmooth(this->m_smoothFoV.m_startValue, this->m_smoothFoV.m_target, t) : this->m_smoothFoV.m_target;
    }

    if (fabsf(this->m_smoothDistance.m_target - this->m_distance) < 2.3841858e-7f) {
        this->m_state &= ~0x4000000;
        this->m_smoothDistance.m_target = this->m_distance;
        this->m_smoothDistance.m_startTimeMs = 0;
        this->m_smoothDistance.m_rate = 0.0f;
    } else if ((this->m_state & 0x4000000) && static_cast<int32_t>(time - this->m_smoothDistance.m_startTimeMs) >= 0) {
        float t = (time - this->m_smoothDistance.m_startTimeMs) * 0.001f / this->m_smoothDistance.m_rate;
        this->m_distance = t < 1.0f ? OrganicSmooth(this->m_smoothDistance.m_startValue, this->m_smoothDistance.m_target, t) : this->m_smoothDistance.m_target;
    }

    if (fabsf(this->m_smoothHeight.m_target - this->m_height) < 2.3841858e-7f) {
        this->m_state &= ~0x20000000;
        this->m_smoothHeight.m_target = this->m_height;
        this->m_smoothHeight.m_startTimeMs = 0;
        this->m_smoothHeight.m_rate = 0.0f;
    } else if ((this->m_state & 0x20000000) && static_cast<int32_t>(time - this->m_smoothHeight.m_startTimeMs) >= 0) {
        float t = (time - this->m_smoothHeight.m_startTimeMs) * 0.001f / this->m_smoothHeight.m_rate;
        this->m_height = t < 1.0f ? OrganicSmooth(this->m_smoothHeight.m_startValue, this->m_smoothHeight.m_target, t) : this->m_smoothHeight.m_target;
    }

    if (fabsf(this->m_smoothGroundTilt.m_target - this->m_groundTilt) < 2.3841858e-7f) {
        this->m_state &= ~0x10000000;
        this->m_smoothGroundTilt.m_target = this->m_groundTilt;
        this->m_smoothGroundTilt.m_startTimeMs = 0;
        this->m_smoothGroundTilt.m_rate = 0.0f;
    } else if ((this->m_state & 0x10000000) && static_cast<int32_t>(time - this->m_smoothGroundTilt.m_startTimeMs) >= 0) {
        float t = (time - this->m_smoothGroundTilt.m_startTimeMs) * 0.001f / this->m_smoothGroundTilt.m_rate;
        this->m_groundTilt = t < 1.0f ? OrganicSmooth(this->m_smoothGroundTilt.m_startValue, this->m_smoothGroundTilt.m_target, t) : this->m_smoothGroundTilt.m_target;
    }

    if (fabsf(this->m_smoothTargetOffset.m_target - this->m_targetOffset) < 2.3841858e-7f) {
        if (!(this->m_state & 0x80000000)) {
            this->m_smoothTargetOffset.m_target = this->m_targetOffset;
            this->m_smoothTargetOffset.m_startTimeMs = 0;
            this->m_state &= ~0x8000000;
            this->m_smoothTargetOffset.m_rate = 0.0f;
        }
    } else if ((this->m_state & 0x8000000) && static_cast<int32_t>(time - this->m_smoothTargetOffset.m_startTimeMs) >= 0) {
        float t = (time - this->m_smoothTargetOffset.m_startTimeMs) * 0.001f / this->m_smoothTargetOffset.m_rate;
        this->m_targetOffset = t < 1.0f ? OrganicSmooth(this->m_smoothTargetOffset.m_startValue, this->m_smoothTargetOffset.m_target, t) : this->m_smoothTargetOffset.m_target;
    }

    if (fabsf(this->m_smoothPitch.m_target - this->m_pitch) < 2.3841858e-7f) {
        this->m_state &= ~0x2000000;
        this->m_smoothPitch.m_target = this->m_pitch;
        this->m_smoothPitch.m_startTimeMs = 0;
        this->m_smoothPitch.m_rate = 0.0f;
    } else if ((this->m_state & 0x2000000) && static_cast<int32_t>(time - this->m_smoothPitch.m_startTimeMs) >= 0) {
        float t = (time - this->m_smoothPitch.m_startTimeMs) * 0.001f / this->m_smoothPitch.m_rate;
        this->m_pitch = t < 1.0f ? OrganicSmooth(this->m_smoothPitch.m_startValue, this->m_smoothPitch.m_target, t) : this->m_smoothPitch.m_target;
    }

    uint32_t state = this->m_state;

    if ((state & 0x8) && (state & 0x1000000)) {
        this->m_smoothYaw.m_startTimeMs = time;
    }

    if (!(state & 0x1) && !(state & 0x8)) {
        if (fabsf(this->m_smoothYaw.m_target - this->m_yaw) < 2.3841858e-7f) {
            this->m_smoothYaw.m_target = this->m_yaw;
            this->m_smoothYaw.m_startTimeMs = 0;
            this->m_state = state & ~0x1000000;
            this->m_smoothYaw.m_rate = 0.0f;
        } else if ((state & 0x1000000) && static_cast<int32_t>(time - this->m_smoothYaw.m_startTimeMs) >= 0) {
            float t = (time - this->m_smoothYaw.m_startTimeMs) * 0.001f / this->m_smoothYaw.m_rate;
            this->m_yaw = t < 1.0f ? OrganicSmooth(this->m_smoothYaw.m_startValue, this->m_smoothYaw.m_target, t) : this->m_smoothYaw.m_target;
        }
    }

    this->ClampPitchAndNormalize();

    if (fabsf(this->m_smoothFlyingHeight.m_target - this->m_flyingMountHeight) < 2.3841858e-7f) {
        this->m_state &= ~0x400000;
        this->m_smoothFlyingHeight.m_target = this->m_flyingMountHeight;
        this->m_smoothFlyingHeight.m_startTimeMs = 0;
        this->m_smoothFlyingHeight.m_rate = 0.0f;
    } else if ((this->m_state & 0x400000) && static_cast<int32_t>(time - this->m_smoothFlyingHeight.m_startTimeMs) >= 0) {
        float t = (time - this->m_smoothFlyingHeight.m_startTimeMs) * 0.001f / this->m_smoothFlyingHeight.m_rate;
        this->m_flyingMountHeight = t < 1.0f ? OrganicSmooth(this->m_smoothFlyingHeight.m_startValue, this->m_smoothFlyingHeight.m_target, t) : this->m_smoothFlyingHeight.m_target;
    }

    this->CalcTerrainTilt(target, time);
    this->PerformTerrainTilt(target, time, 0);
}

// OFFSET: 0x0043FBB0/0x14004AE20
static void CreateFrustum(const C3Vector& up, const C3Vector& target, const C3Vector& position, float size, CFrustum& frustum) {
    C3Vector dir(target.x - position.x, target.y - position.y, target.z - position.z);
    float scale = 1.0f / sqrtf(dir.y * dir.y + dir.x * dir.x + dir.z * dir.z);
    dir.x = dir.x * scale;
    dir.y = dir.y * scale;
    dir.z = scale * dir.z;
    C3Vector side(up.z * dir.y - up.y * dir.z, up.x * dir.z - up.z * dir.x, up.y * dir.x - up.x * dir.y);
    float sideScale = 1.0f / sqrtf(side.z * side.z + side.y * side.y + side.x * side.x);
    side.x = side.x * sideScale * size;
    side.y = side.y * sideScale * size;
    side.z = side.z * sideScale * size;
    C3Vector height(up.x * size, up.y * size, up.z * size);
    C3Vector corners[8];
    corners[0] = C3Vector(position.x - side.x - height.x, position.y - side.y - height.y, position.z - side.z - height.z);
    corners[1] = C3Vector(position.x - side.x + height.x, position.y - side.y + height.y, position.z - side.z + height.z);
    corners[2] = C3Vector(position.x + side.x + height.x, position.y + side.y + height.y, position.z + side.z + height.z);
    corners[3] = C3Vector(position.x + side.x - height.x, position.y + side.y - height.y, position.z + side.z - height.z);
    corners[4] = C3Vector(target.x - side.x - height.x, target.y - side.y - height.y, target.z - side.z - height.z);
    corners[5] = C3Vector(target.x - side.x + height.x, target.y - side.y + height.y, target.z - side.z + height.z);
    corners[6] = C3Vector(target.x + side.x + height.x, target.y + side.y + height.y, target.z + side.z + height.z);
    corners[7] = C3Vector(target.x + side.x - height.x, target.y + side.y - height.y, target.z + side.z - height.z);
    frustum.CalcPlanesFromCorners(corners);
}

// OFFSET: 0x00444650/0x1400500D0
static int32_t CollideWithWorld(uint32_t flags, CFrustum& frustum, float& hit) {
    if (!flags) {
        return 0;
    }

    // STATIC: 0x00D39188
    static World::FacetData s_facets;

    s_facets.facets.SetCount(0);
    s_facets.facetIds.m_count = 0;
    s_facets.facetFlags.m_count = 0;
    s_facets.m_status = 0;
    World::GetFacets(frustum, s_facets, flags);

    if (!s_facets.facets.m_count) {
        return 0;
    }

    C44Matrix xform;

    if (!World::NDCXform(frustum, xform, false)) {
        return 0;
    }

    int32_t result = 0;

    for (uint32_t i = 0; i < s_facets.facets.m_count; i++) {
        CFacet& facet = s_facets.facets.m_data[i];

        for (uint32_t j = 0; j < 3; j++) {
            facet.v[j] = (facet.v[j] - frustum.corners[0]) * xform;
        }

        C3Vector** clipped;
        uint32_t count;

        if (World::NDCClip(facet.v, 3, clipped, count)) {
            result = 1;

            for (uint32_t j = 0; j < count; j++) {
                if (clipped[j]->z > hit) {
                    hit = clipped[j]->z;
                }
            }
        }
    }

    return result;
}

// OFFSET: 0x00444A10/0x140050580
int32_t CGCamera::GetCameraDistance(float& distance, const C3Vector& position, const C3Vector& target, uint32_t flags) {
    float range = distance;

    if (range < 0.001f) {
        distance = 0.0f;
        return 1;
    }

    C3Vector dir(target.x - position.x, target.y - position.y, target.z - position.z);
    float mag = dir.z * dir.z + dir.y * dir.y + dir.x * dir.x;

    if (mag < 0.001f) {
        distance = 0.0f;
        return 1;
    }

    float scale = 1.0f / sqrtf(mag);
    float fx = dir.x * scale;
    float fy = scale * dir.y;
    float fz = scale * dir.z;
    C3Vector up(fy * fz - -fx * fz, fy * fz - fx * fz, -fx * fx - fy * fy);

    if (up.y * up.y + up.x * up.x + up.z * up.z < 0.001f) {
        distance = 0.0f;
        return 1;
    }

    float upScale = 1.0f / sqrtf(up.y * up.y + up.x * up.x + up.z * up.z);
    up.x = up.x * upScale;
    up.y = up.y * upScale;
    up.z = upScale * up.z;
    CFrustum frustum;
    float hit = 0.0f;
    int32_t collided = 0;
    uint32_t terrainFlags = flags & 0x30000;

    if (terrainFlags) {
        CreateFrustum(up, target, position, 0.3f, frustum);

        if (CollideWithWorld(terrainFlags, frustum, hit)) {
            collided = 1;
        }
    }

    CreateFrustum(up, target, position, 0.25f, frustum);

    if (!CollideWithWorld(flags & 0xFFFCFFFF, frustum, hit) && !collided) {
        return 0;
    }

    distance = distance - hit * range;

    if (distance < 0.0f) {
        distance = 0.0f;
    }

    return 1;
}

// OFFSET: 0x00443D80/0x14004F630
void CGCamera::SetTelescope(int32_t telescope) {
    if (!telescope) {
        if (this->m_state & 0x8) {
            float offset = this->m_smoothTargetOffset.m_target;
            uint32_t time = FrameTime::GetCurTimeMs();
            this->StartSmoothFoV(0.0f, 0.5f, time);
            this->m_state = (this->m_state & 0x7FFF7FF7) | 0x80000000;
            this->SetView(this->m_prevViewIndex, 2, 2);
            this->StartSmoothDistance(this->unk_02F4, 0.5f, time);

            if (fabsf(this->unk_02F8) >= 2.3841858e-7f) {
                this->StartSmoothTargetOffset(offset - this->unk_02F8, 0.5f, time);
                this->unk_02F8 = 0.0f;
            }
        }

        return;
    }

    uint32_t time = FrameTime::GetCurTimeMs();
    this->SetView(0, 2, 1);
    this->m_state = (this->m_state & 0x7FFF7FF7) | 0x8008;
    this->StartSmoothFoV(telescope * 0.017453292f - this->m_fov, 0.5f, time);
    this->StartSmoothDistance(0.0f, 0.5f, time);

    if (fabsf(this->m_targetOffset) < 2.3841858e-7f) {
        return;
    }

    CGObject_C* target = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!target) {
        return;
    }

    C33Matrix facing = this->m_facing;
    C3Vector forward = this->Forward();
    C3Vector aim(forward.x * 100.0f + this->m_position.x, this->m_position.y + forward.y * 100.0f, this->m_position.z + forward.z * 100.0f);

    float yaw;
    float pitch;
    float roll;
    this->UpdateTargetFacing(target, yaw, pitch, roll);

    float distance = this->m_distance;
    C3Vector back = this->Forward();
    C3Vector eye(back.x * distance + this->m_position.x, this->m_position.y + back.y * distance, this->m_position.z + back.z * distance);
    this->m_facing = facing;

    C3Vector dir(aim.x - eye.x, aim.y - eye.y, aim.z - eye.z);
    dir.NormalizeSafe();

    float offset = this->m_smoothTargetOffset.m_target;
    this->m_targetOffset = offset;
    float base = offset + this->m_pitch;

    if (this->m_groundTiltRefs <= 0) {
        base = base + this->m_groundTilt;
    }

    float angle = UnwrapAngleToward(0.0f, -atan2f(dir.z, sqrtf(1.0f - dir.z * dir.z)) - base, -3.1415927f, 3.1415927f);
    this->StartSmoothTargetOffset(angle + offset, 0.5f, time);
    this->unk_02F8 = angle;
}

// OFFSET: 0x0043CFC0
void CGCamera::SetPositionAndTargetWithRoll(const C3Vector& position, const C3Vector& target, float roll) {
    C3Vector forward(target.x - position.x, target.y - position.y, target.z - position.z);
    float lengthSqr = forward.z * forward.z + forward.y * forward.y + forward.x * forward.x;

    if (lengthSqr > 2.3841858e-7f) {
        float inv = 1.0f / sqrtf(lengthSqr);
        forward.x *= inv;
        forward.y *= inv;
        forward.z *= inv;
    }

    float c = cosf(roll);
    float s = sinf(roll);
    C3Vector up(0.0f, s, c);
    this->m_position = position;
    this->SetFacing(forward, up);
}

// OFFSET: 0x0043F5E0/0x14004A800
int32_t CGCamera::FinishLoadingModel() {
    this->m_state |= 0x4;
    this->m_someTime = FrameTime::GetCurTimeMs();
    this->GetScene()->SetTime(0);
    this->m_model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 0, 1, 0, 0, 0);
    this->m_model->SetAnimating(1);
    C3Vector cameraPos;
    this->GetScene()->Animate(cameraPos);
    CCamera* camera = static_cast<CCamera*>(this->m_model->GetCameraByIndex(0));

    if (!camera) {
        this->m_model->Release();
        this->m_model = NULL;
        return 0;
    }

    CameraAddRef(camera);
    this->m_modelCamera = camera;
    this->UpdateFromModelCamera();
    return 1;
}

// OFFSET: 0x0043FFA0/0x14004B260
void CGCamera::CalcModelCamera(uint32_t time) {
    if (!(this->m_state & 0x4) && !(this->m_model->IsLoaded() && this->FinishLoadingModel())) {
        return;
    }

    this->m_model->SetAnimating(1);
    this->GetScene()->AdvanceTime(time - this->m_someTime);
    C3Vector cameraPos;
    this->GetScene()->Animate(cameraPos);
    this->m_someTime = time;
    this->UpdateFromModelCamera();
}

// OFFSET: 0x00441910/0x14004CDA0
void CGCamera::CommentatorSetViewAndPosition(const C3Vector& position, float facing, float pitch) {
    this->m_views[6][0] = 0.0f;
    this->m_views[6][2] = facing;
    this->m_views[6][1] = pitch;
    this->m_position = position;
    this->m_position = position;
    this->SetView(6, 2, 0);
}

// OFFSET: 0x0076CA20/0x140586E10
float CGCamera::GetDefaultZoomMinDistance() {
    return 0.0f;
}

// OFFSET: 0x0043D490/0x140048600
float CameraEaseInOut(float t) {
    return 0.5f - CMath::cos_(t * 3.1415927f) * 0.5f;
}

// OFFSET: 0x0043E0C0/0x1400492C0
void CGCamera::UpdateVehicleYawOffset(uint32_t time) {
    float t = static_cast<float>(time - this->m_vehicleYawStartTime) / static_cast<float>(this->m_vehicleYawEndTime - this->m_vehicleYawStartTime);

    if (t > 1.0f) {
        this->m_vehicleYawStartTime = this->m_vehicleYawEndTime;
        this->m_vehicleYawOffset = this->m_vehicleYawTo;
    } else {
        this->m_vehicleYawOffset = CameraEaseInOut(t) * (this->m_vehicleYawTo - this->m_vehicleYawFrom) + this->m_vehicleYawFrom;
    }
}

// OFFSET: 0x0043E280/0x140049470
void CGCamera::UpdateVehiclePitchOffset(uint32_t time) {
    float t = static_cast<float>(time - this->m_vehiclePitchStartTime) / static_cast<float>(this->m_vehiclePitchEndTime - this->m_vehiclePitchStartTime);

    if (t > 1.0f) {
        this->m_vehiclePitchStartTime = this->m_vehiclePitchEndTime;
        this->m_vehiclePitchOffset = this->m_vehiclePitchTo;
    } else {
        this->m_vehiclePitchOffset = CameraEaseInOut(t) * (this->m_vehiclePitchTo - this->m_vehiclePitchFrom) + this->m_vehiclePitchFrom;
    }
}

