#include "WoW/Source/UIUtil/CameraMode.h"
#include <cmath>
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/GameClientCommands.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/CameraManager.h"

// STATIC: 0x00D3A7A8/0x140BF5A98
int32_t CCameraModeLegacy::s_suspended;

// OFFSET: 0x004617A0/0x1400744B0
int32_t CCameraModeLegacy::IsTracking(const CCameraModeInstance* instance, WOWGUID guid) const {
    return guid == ClntObjMgrGetActivePlayer();
}

// OFFSET: 0x004617D0/0x1400744D0
void CCameraModeLegacy::CalcShotFromLegacyCamera(CCameraShot& shot) {
    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (camera) {
        shot.m_roll = 0.0f;
        shot.m_fov = camera->FOV();
        shot.m_position = camera->m_position;
        C3Vector forward = camera->Forward();
        shot.m_target.x = forward.x + camera->m_position.x;
        shot.m_target.y = camera->m_position.y + forward.y;
        shot.m_target.z = camera->m_position.z + forward.z;
        shot.m_fade = camera->m_fade * 0.0039215689f;
    }
}

// OFFSET: 0x00461880/0x140074590
void CCameraModeLegacy::UpdateFromLegacyCamera(CCameraModeInstance* instance) const {
    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (!camera) {
        return;
    }

    CCameraModeLegacy::CalcShotFromLegacyCamera(instance->m_shot);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(camera->m_targetGUID, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit || camera->m_model) {
        return;
    }

    C3Vector position = unit->GetPosition();
    float dz = position.z - instance->m_shot.m_position.z;
    float dy = position.y - instance->m_shot.m_position.y;
    float dx = position.x - instance->m_shot.m_position.x;
    float distance = sqrtf(dx * dx + (dy * dy + dz * dz));

    if (distance > 0.00000011920929f) {
        C3Vector forward = camera->Forward();
        forward.Normalize();
        instance->m_shot.m_target.x = instance->m_shot.m_position.x + forward.x * distance;
        instance->m_shot.m_target.y = instance->m_shot.m_position.y + forward.y * distance;
        instance->m_shot.m_target.z = instance->m_shot.m_position.z + forward.z * distance;
    }
}

// OFFSET: 0x00461990/0x1400746F0
void CCameraModeLegacy::InitInstance(CCameraModeInstance* instance) const {
    if (StaticSingleton<CCameraManager>::Instance()->IsCameraSystemEnabled()) {
        CGCamera* camera = CGWorldFrame::GetActiveCamera();

        if (camera && !StaticSingleton<CCameraManager>::Instance()->IsCompletelyLegacy() && ClntObjMgrObjectPtr(camera->m_targetGUID, TYPEMASK_UNIT, __FILE__, __LINE__)) {
            float yaw = 0.0f;
            float pitch = 0.0f;
            instance->m_shot.CalcYawPitch(yaw, pitch);
            C3Vector forward = camera->Forward();
            camera->RotateOffsetByRoll(forward);
            float delta = CMath::normalizeAngleNegPiToPi_(yaw - atan2f(forward.y, forward.x));
            float cameraYaw = CMath::normalizeangle0to2pi_(delta + camera->m_smoothYaw.m_target);
            float cameraPitch = -pitch;
            camera->m_smoothYaw.m_target = cameraYaw;
            camera->m_smoothYaw.m_startValue = cameraYaw;
            camera->m_yaw = cameraYaw;
            camera->m_smoothPitch.m_target = cameraPitch;
            camera->m_smoothPitch.m_startValue = cameraPitch;
            camera->m_pitch = cameraPitch;
        }
    }

    this->UpdateFromLegacyCamera(instance);
}

// OFFSET: 0x00461AF0/0x140074870
void CCameraModeLegacy::UpdateInstance(CCameraModeInstance* instance, float elapsed) const {
    const CCameraMode* mode = StaticSingleton<CCameraManager>::Instance()->GetGameCamera();

    if (!mode || mode->GetType() == CAMERA_MODE_LEGACY) {
        CCameraModeLegacy::s_suspended = 0;
        this->UpdateFromLegacyCamera(instance);
    } else {
        CCameraModeLegacy::s_suspended = 1;
    }
}

// OFFSET: 0x009FCE20/0x14090EBB0
int32_t CCameraModeLegacy::GetType() const {
    return CAMERA_MODE_LEGACY;
}

// OFFSET: 0x00847950/0x1407D7E80
void CCameraModeLegacy::DestroyInstance(CCameraModeInstance* instance) const {
}
