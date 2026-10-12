#include "WoW/Source/UIUtil/CameraTrack.h"
#include <cmath>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/StaticSingleton.h"
#include "WoW/Source/UIUtil/CameraManager.h"
#include "WoW/Source/UIUtil/CameraMode.h"

// STATIC: 0x00D3A7C4/0x140BF5AC8
static CDataAllocator s_trackNodes(sizeof(CCameraTrackNode), 8);

// INLINED
CCameraTrackBlendData::CCameraTrackBlendData() {
    this->m_distance = 0.0f;
    this->m_yaw = 0.0f;
    this->m_pitch = 0.0f;
    this->m_position = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_target = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_fov = 0.0f;
    this->m_roll = 0.0f;
    this->m_fade = 0.0f;
}

// OFFSET: 0x00461DA0
void CCameraTrackBlendData::Init(const CCameraShot& shot) {
    shot.CalcPolarFacing(this->m_distance, this->m_yaw, this->m_pitch);
    this->m_position = shot.m_position;
    this->m_target = shot.m_target;
    this->m_fov = shot.m_fov;
    this->m_roll = CMath::normalizeAngleNegPiToPi_(shot.m_roll);
    this->m_fade = shot.m_fade;
}

// OFFSET: 0x00461E40/0x140074C30
const CCameraMode* CCameraTrack::GetCurrentCamera() const {
    CCameraTrackNode* node = this->m_head;

    if (!node) {
        return NULL;
    }

    while (node->m_next) {
        node = node->m_next;
    }

    return node->m_instance ? node->m_instance->m_mode : NULL;
}

// OFFSET: 0x00461F30/0x140074D60
void CCameraTrackBlendData::CalcDeltas(const CCameraShot& from, const CCameraShot& to) {
    CCameraTrackBlendData base;
    base.Init(from);
    this->Init(to);
    this->m_distance = this->m_distance + base.m_distance * -1.0f;
    this->m_yaw = base.m_yaw * -1.0f + this->m_yaw;
    this->m_pitch = base.m_pitch * -1.0f + this->m_pitch;
    this->m_position.x = this->m_position.x + base.m_position.x * -1.0f;
    this->m_position.y = this->m_position.y + base.m_position.y * -1.0f;
    this->m_position.z = this->m_position.z + base.m_position.z * -1.0f;
    this->m_target.x = base.m_target.x * -1.0f + this->m_target.x;
    this->m_target.y = this->m_target.y + base.m_target.y * -1.0f;
    this->m_target.z = this->m_target.z + base.m_target.z * -1.0f;
    this->m_fov = this->m_fov - base.m_fov;
    this->m_roll = this->m_roll - base.m_roll;
    this->m_fade = this->m_fade - base.m_fade;
    this->m_yaw = CMath::normalizeAngleNegPiToPi_(this->m_yaw);
    this->m_roll = CMath::normalizeAngleNegPiToPi_(this->m_roll);
}

// OFFSET: 0x004620B0/0x140074FA0
CCameraShot CCameraTrackBlendData::ApplyBlend(const CCameraShot& shot, float factor, int32_t flags) const {
    CCameraShot result = shot;
    float distance = this->m_distance * factor;
    float yaw = this->m_yaw * factor;
    float pitch = this->m_pitch * factor;
    C3Vector position = this->m_position * factor;
    C3Vector target = this->m_target * factor;
    float fov = factor * this->m_fov;
    float roll = this->m_roll * factor;
    float fade = this->m_fade * factor;

    if (flags & 0x1) {
        result.m_position += position;
        result.m_target += target;
    } else {
        float shotDistance = 0.0f;
        float shotYaw = 0.0f;
        float shotPitch = 0.0f;
        result.CalcPolarFacing(shotDistance, shotYaw, shotPitch);
        float blendDistance = shotDistance + distance;
        float flat = cosf(shotPitch + pitch) * blendDistance;
        C3Vector offset(cosf(shotYaw + yaw) * flat, flat * sinf(shotYaw + yaw), blendDistance * sinf(shotPitch + pitch));

        if (flags & 0x4) {
            result.m_target += target;
            result.m_position = result.m_target - offset;
        } else {
            result.m_position += position;
            result.m_target = result.m_position + offset;
        }
    }

    if (flags & 0x2) {
        result.m_fov = result.m_fov + fov;
    } else {
        float half = atan2f((1.0f - factor) * tanf(0.5f * result.m_fov) + tanf((result.m_fov + this->m_fov) * 0.5f) * factor, 1.0f);
        result.m_fov = half + half;
    }

    result.m_roll = result.m_roll + roll;
    result.m_fade = result.m_fade + fade;
    return result;
}

// OFFSET: 0x004623A0/0x140075390
void CCameraTrackNode::Init(const CCameraTransition* transition, CCameraModeInstance* instance, CCameraTrackNode* prev) {
    this->m_instance = instance;
    this->m_transition = transition;
    CCameraShot shot;

    if (prev) {
        this->m_next = prev->m_next;
        prev->m_next = this;
        this->m_shot = prev->m_shot;
        shot = prev->m_shot;
    }

    this->m_blend.CalcDeltas(shot, this->m_instance->m_shot);
}

// OFFSET: 0x00462410/0x140075450
CCameraShot CCameraTrackNode::Update(float elapsed) {
    CCameraShot result;
    CCameraTrackNode* node = this;

    if (!node) {
        return result;
    }

    while (true) {
        CCameraShot previous = node->m_instance->m_shot;
        node->m_instance->Update(elapsed);
        CCameraModeInstance* instance = node->m_instance;

        if (!instance->m_shot.Sanitize()) {
            const CCameraMode* mode = node->m_instance->m_mode;
            SysMsgPrintf(STATUS_WARNING, 2, "INVALID CAMERA MODE FACING VECTOR (modeID=%i, modeType=%i)", mode->GetID(), mode->GetType());
        }

        CCameraShot last = node->m_shot;
        CCameraTrackBlendData delta;
        const CCameraShot* shot;
        CCameraShot blended;

        if (node == this) {
            this->m_blend = delta;
            shot = &instance->m_shot;
        } else {
            delta.CalcDeltas(previous, instance->m_shot);
            node->m_blend.m_distance = delta.m_distance + node->m_blend.m_distance;
            node->m_blend.m_yaw = node->m_blend.m_yaw + delta.m_yaw;
            node->m_blend.m_pitch = node->m_blend.m_pitch + delta.m_pitch;
            node->m_blend.m_position.x = node->m_blend.m_position.x + delta.m_position.x;
            node->m_blend.m_position.y = node->m_blend.m_position.y + delta.m_position.y;
            node->m_blend.m_position.z = node->m_blend.m_position.z + delta.m_position.z;
            node->m_blend.m_target.x = node->m_blend.m_target.x + delta.m_target.x;
            node->m_blend.m_target.y = node->m_blend.m_target.y + delta.m_target.y;
            node->m_blend.m_target.z = node->m_blend.m_target.z + delta.m_target.z;
            node->m_blend.m_fov = node->m_blend.m_fov + delta.m_fov;
            node->m_blend.m_roll = node->m_blend.m_roll + delta.m_roll;
            node->m_blend.m_fade = node->m_blend.m_fade + delta.m_fade;
            float factor = node->m_transition->CalcLerpFactor(node->m_instance->m_time);
            blended = node->m_blend.ApplyBlend(result, factor, node->m_transition->m_flags);
            shot = &blended;
        }

        node->m_shot = *shot;
        result = node->m_shot;
        node = node->m_next;

        if (!node) {
            break;
        }

        CCameraTrackBlendData correction;
        correction.CalcDeltas(last, result);
        node->m_blend.m_distance = correction.m_distance * -1.0f + node->m_blend.m_distance;
        node->m_blend.m_yaw = node->m_blend.m_yaw + correction.m_yaw * -1.0f;
        node->m_blend.m_pitch = node->m_blend.m_pitch + correction.m_pitch * -1.0f;
        node->m_blend.m_position.x = node->m_blend.m_position.x + correction.m_position.x * -1.0f;
        node->m_blend.m_position.y = node->m_blend.m_position.y + correction.m_position.y * -1.0f;
        node->m_blend.m_position.z = node->m_blend.m_position.z + correction.m_position.z * -1.0f;
        node->m_blend.m_target.x = node->m_blend.m_target.x + correction.m_target.x * -1.0f;
        node->m_blend.m_target.y = node->m_blend.m_target.y + correction.m_target.y * -1.0f;
        node->m_blend.m_target.z = node->m_blend.m_target.z + correction.m_target.z * -1.0f;
        node->m_blend.m_fov = node->m_blend.m_fov - correction.m_fov;
        node->m_blend.m_roll = node->m_blend.m_roll - correction.m_roll;
        node->m_blend.m_fade = node->m_blend.m_fade - correction.m_fade;
    }

    return result;
}

// OFFSET: 0x004627F0/0x140075990
void CCameraTrack::SetCurrentCameraInternal(const CCameraModeInstance* instance, const CCameraMode* mode, WOWGUID guid) {
    if (!mode) {
        return;
    }

    const CCameraModeInstance* current = instance;

    if (!instance) {
        CCameraTrackNode* last = this->m_head;

        if (last) {
            while (last->m_next) {
                last = last->m_next;
            }

            current = last->m_instance;
        } else {
            current = NULL;
        }
    }

    int32_t same = 0;
    const CCameraMode* currentMode;

    if (current) {
        currentMode = current->m_mode;

        if (currentMode == mode && current->m_guid == guid && !instance) {
            same = 1;
        }
    } else {
        currentMode = NULL;
    }

    CCameraModeInstance proto;
    proto.m_shot = this->m_shot;
    proto.m_guid = guid;
    proto.m_mode = mode;
    CCameraModeInstance* created = mode->AllocInstance(proto);

    if (!created) {
        return;
    }

    mode->InitInstance(created);

    if (same && !mode->AllowDuplicate(current, created)) {
        created->Destroy();
        return;
    }

    const CCameraTransition* transition = StaticSingleton<CCameraManager>::Instance()->FindTransition(currentMode, mode);
    CCameraTrackNode* prev = this->m_head;

    while (prev && prev->m_instance != current) {
        prev = prev->m_next;
    }

    CCameraTrackNode* node = s_trackNodes.New<CCameraTrackNode>(0);
    node->Init(transition, created, prev);

    if (!prev) {
        this->m_head = node;
    }
}

// OFFSET: 0x00462940/0x140075BB0
void CCameraTrack::ClearCamerasInternal(CCameraTrackNode* keep) {
    CCameraTrackNode* node = this->m_head;

    while (node != keep) {
        CCameraTrackNode* next = node->m_next;

        for (uint32_t i = 0; i < this->m_deferredActions.Count(); i++) {
            DeferredCameraAction& action = this->m_deferredActions[i];

            if (action.m_action == DEFERRED_SET_CAMERA && action.m_instance == node->m_instance) {
                action = DeferredCameraAction();
            }
        }

        node->m_instance->Destroy();
        node->m_instance = NULL;
        s_trackNodes.PutData(node, NULL, 0);
        node = next;
    }

    this->m_head = keep;
}

// OFFSET: 0x00462A00/0x140075CB0
void CCameraTrack::ResetCamerasInternal(WOWGUID guid) {
    if (guid) {
        CCameraTrackNode* node = this->m_head;

        if (!node) {
            return;
        }

        while (!node->m_instance->IsTracking(guid)) {
            node = node->m_next;

            if (!node) {
                return;
            }
        }
    }

    CCameraTrackNode* last = this->m_head;

    if (!last) {
        return;
    }

    while (last->m_next) {
        last = last->m_next;
    }

    CCameraModeInstance* instance = last->m_instance;

    if (!instance) {
        return;
    }

    const CCameraMode* mode = instance->m_mode;
    WOWGUID target = instance->m_guid;

    if (mode) {
        this->ClearCamerasInternal(NULL);
        this->SetCurrentCameraInternal(NULL, mode, target);
    }
}

// INLINED
CCameraTrack::DeferredCameraAction::DeferredCameraAction() {
    this->m_action = DEFERRED_NONE;
    this->m_instance = NULL;
    this->m_mode = NULL;
    this->m_guid = WOWGUID::Make(0);
}

// OFFSET: 0x00462B20/0x140075E10
CCameraTrack::CCameraTrack() : m_head(NULL) {
}

// OFFSET: 0x00462B40/0x140075E40
CCameraTrack::~CCameraTrack() {
    this->m_deferredActions.Clear();
    this->ClearCamerasInternal(NULL);
}

// OFFSET: 0x00462BA0/0x140075EC0
void CCameraTrack::ResetCameras(WOWGUID guid) {
    DeferredCameraAction* action = this->m_deferredActions.New();
    action->m_action = DEFERRED_RESET_CAMERAS;
    action->m_guid = guid;
}

// OFFSET: 0x00462BD0/0x140075EF0
void CCameraTrack::SetCurrentCamera(const CCameraModeInstance* instance, const CCameraMode* mode, WOWGUID guid) {
    DeferredCameraAction* action = this->m_deferredActions.New();
    action->m_instance = instance;
    action->m_mode = mode;
    action->m_action = DEFERRED_SET_CAMERA;
    action->m_guid = guid;
}

// OFFSET: 0x00462C00/0x140075F40
void CCameraTrack::ProcessDeferredActions() {
    for (uint32_t i = 0; i < this->m_deferredActions.Count(); i++) {
        DeferredCameraAction& action = this->m_deferredActions[i];

        switch (action.m_action) {
            case DEFERRED_SET_CAMERA: {
                this->SetCurrentCameraInternal(action.m_instance, action.m_mode, action.m_guid);
                break;
            }

            case DEFERRED_RESET_CAMERAS: {
                this->ResetCamerasInternal(action.m_guid);
                break;
            }

            case DEFERRED_CLEAR_CAMERA: {
                const CCameraMode* mode = action.m_mode;

                if (this->m_head) {
                    CCameraTrackNode* keep = this->m_head;

                    for (CCameraTrackNode* node = this->m_head; node; node = node->m_next) {
                        if (node->m_instance->m_mode == mode) {
                            keep = node->m_next;
                        }
                    }

                    this->ClearCamerasInternal(keep);
                }

                break;
            }
        }
    }

    this->m_deferredActions.Clear();
}

// OFFSET: 0x00462CE0/0x140076040
void CCameraTrack::Update(float elapsed) {
    this->ProcessDeferredActions();

    if (!this->m_head) {
        return;
    }

    this->m_shot = this->m_head->Update(elapsed);
    CCameraTrackNode* keep = this->m_head;

    for (CCameraTrackNode* node = this->m_head; node; node = node->m_next) {
        if (node->m_transition->IsComplete(node->m_instance->m_time)) {
            keep = node;
        }
    }

    this->ClearCamerasInternal(keep);
}

// INLINED
int32_t CCameraTrack::IsCompletelyLegacy() const {
    for (CCameraTrackNode* node = this->m_head; node; node = node->m_next) {
        if (node->m_instance->m_mode->GetType() != CAMERA_MODE_LEGACY) {
            return 0;
        }
    }

    return 1;
}
