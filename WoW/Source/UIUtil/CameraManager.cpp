#include "WoW/Source/UIUtil/CameraManager.h"
#include <cmath>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CameraShakesRec.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/CameraMode.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00D3875C
template <>
CCameraManager* StaticSingleton<CCameraManager>::m_instance = NULL;

// STATIC: 0x00D386C0/0x140BF2250
CVar* CCameraManager::s_cameraSystemCVar;

// STATIC: 0x00D386C4/0x140BF2258
uint32_t CCameraManager::ANIMATED_CAMERA_ID;

// STATIC: 0x00D386C8
CameraModeRec CCameraManager::s_animatedCameraRec;

// STATIC: 0x00D3870C
static CDataAllocator s_cameraModeBuffers(sizeof(CCameraModeBuffer), 32);

// STATIC: 0x00D38720/0x140B303C0
TSHashTable<CCameraModeEntry, HASHKEY_NONE> CCameraManager::s_cameraModes;

// STATIC: 0x00D3874C/0x140BF22C8
TSGrowableArray<CCameraTransition> CCameraManager::s_transitions;

// STATIC: 0x00C98C4C/0x140B303B4
static float s_shakeTwoPi = 6.2831855f;

// STATIC: 0x00C98C50
static float s_shakeFalloffStartSqr = 81.0f;

// STATIC: 0x00C98C54
static float s_shakeFalloffMaxSqr = 6400.0f;

// PROVEN: 7aa02847 IDENTICAL/IDENTICAL
// OFFSET: 0x00438D80/0x1400431D0
float CCameraManager::GetYaw() {
    float yaw = 0.0f;
    float pitch = 0.0f;
    this->m_shot.CalcYawPitch(yaw, pitch);
    return CMath::normalizeAngleNegPiToPi_(yaw + 3.1415927f);
}

// OFFSET: 0x00438DD0/0x140043210
int32_t CCameraManager::IsCompletelyLegacy() {
    return this->m_track->IsCompletelyLegacy();
}

// PROVEN: 7b4b0705 IDENTICAL/IDENTICAL
// OFFSET: 0x00438DE0/0x140043220
const CCameraMode* CCameraManager::GetGameCamera() {
    return this->m_track->GetCurrentCamera();
}

// PROVEN: c04913cb IDENTICAL/DIFFERENT
// OFFSET: 0x00438DF0/0x140043230
void CCameraManager::ResetCameras(WOWGUID guid) {
    this->m_track->ResetCameras(guid);
}

// PROVEN: c0768104 UNPROVEN/IDENTICAL
// OFFSET: 0x00438E10/0x140043240
CCameraMode* CCameraManager::CreateCameraMode(CCameraModeBuffer* buffer, const CameraModeRec* rec) {
    switch (rec->m_type) {
        case CAMERA_MODE_LEGACY: {
            return buffer ? new (buffer) CCameraModeLegacy(rec) : NULL;
        }

        case CAMERA_MODE_LOCKED: {
            return buffer ? new (buffer) CCameraModeLocked(rec) : NULL;
        }

        case CAMERA_MODE_ANIMATED: {
            return buffer ? new (buffer) CCameraModeAnimated(rec) : NULL;
        }
    }

    return NULL;
}

// PROVEN: d4bc6921 IDENTICAL/IDENTICAL
// OFFSET: 0x00438F90/0x140043480
int32_t CCameraManager::IsCameraSystemEnabled() {
    return !CCameraManager::s_cameraSystemCVar || CCameraManager::s_cameraSystemCVar->m_intValue;
}

// PROVEN: 2f0054b9 IDENTICAL/IDENTICAL
// OFFSET: 0x00438FB0/0x1400434A0
void CCameraManager::SetScreenAspect(const CRect& rect) {
    this->m_aspect = (rect.maxX - rect.minX) / (rect.maxY - rect.minY);
}

// OFFSET: 0x00438FE0/0x1400434C0
const CCameraTransition* CCameraManager::FindTransition(const CCameraMode* from, const CCameraMode* to) {
    while (true) {
        while (!from) {
            if (!to) {
                return NULL;
            }

            from = to;
        }

        if (to) {
            break;
        }

        to = from;
    }

    uint32_t index = 1;

    if (from->GetType() == CAMERA_MODE_LEGACY || to->GetType() == CAMERA_MODE_LEGACY) {
        index = 2;
    }

    if (from->GetType() == CAMERA_MODE_ANIMATED || to->GetType() == CAMERA_MODE_ANIMATED) {
        index = 0;
    }

    return &CCameraManager::s_transitions[index];
}

// OFFSET: 0x00439070/0x140043560
void CCameraManager::ApplyWorldProjection(const CRect& rect) {
    C44Matrix projection;
    this->m_aspect = (rect.maxX - rect.minX) / (rect.maxY - rect.minY);
    float farClip = World::GetFarClip();
    float nearClip = World::GetNearClip();
    GxuXformCreateProjection_Exact(this->m_shot.m_fov * 0.60000002f, this->m_aspect, nearClip, farClip, projection);
    g_theGxDevicePtr->XformSetProjection(projection);
    g_theGxDevicePtr->XformSetView(this->m_renderMatrix);
}

// OFFSET: 0x00439160/0x140043660
void CCameraManager::UpdateLegacyState() {
    int32_t active = 0;
    const CCameraMode* mode = this->m_track->GetCurrentCamera();
    const CameraModeRec* rec = mode ? mode->m_rec : NULL;

    if (this->IsCameraSystemEnabled() && rec && (rec->m_flags & 0x100)) {
        active = 1;
    }

    if (active != this->m_legacyActive) {
        this->m_legacyActive = active;

        if (active) {
            FrameScript_SignalEvent(107, "%b", 0);
        } else {
            FrameScript_SignalEvent(108, NULL);
        }
    }
}

// OFFSET: 0x004391D0/0x1400436F0
void __stdcall CCameraManager::AddShakeOffset(C3Vector& offset, float facing, float amplitude, float time, float frequency, int32_t type, float damping, int32_t dir) {
    float value = sinf(s_shakeTwoPi * time * frequency) * amplitude;

    if (type == 1) {
        value = value * expf(-(time * damping));
    }

    if (dir == 0) {
        float c = cosf(facing);
        float s = sinf(facing);
        offset.x = c * value + offset.x;
        offset.y = s * value + offset.y;
    } else if (dir == 1) {
        float angle = facing + 1.5707964f;
        float c = cosf(angle);
        float s = sinf(angle);
        offset.x = c * value + offset.x;
        offset.y = s * value + offset.y;
    } else if (dir == 2) {
        offset.z = value + offset.z;
    }
}

// OFFSET: 0x00439320/0x1400437E0
C3Vector CCameraManager::CalcShakeOffset(const C3Vector& position, float facing, int32_t requireFlag2, int32_t requireFlag1) {
    C3Vector offset(0.0f, 0.0f, 0.0f);

    if (!this->m_shakes.Head()) {
        return offset;
    }

    uint32_t now = FrameTime::GetCurTimeMs();
    float amplitudes[3] = { 0.0f, 0.0f, 0.0f };
    float times[3] = { 0.0f, 0.0f, 0.0f };
    CameraShake* shakes[3] = { NULL, NULL, NULL };

    for (CameraShake* shake = this->m_shakes.Head(); shake; shake = this->m_shakes.Next(shake)) {
        float time = static_cast<float>(now - shake->m_startTime) * 0.001f + shake->m_timeOffset;

        if (time >= shake->m_duration) {
            continue;
        }

        if (requireFlag1 && !(shake->m_flags & 0x1)) {
            continue;
        }

        if (requireFlag2 && !(shake->m_flags & 0x2)) {
            continue;
        }

        float amplitude = shake->m_amplitude;
        float dy = position.y - shake->m_position.y;
        float dz = position.z - shake->m_position.z;
        float dx = position.x - shake->m_position.x;
        float distSqr = dx * dx + dy * dy + dz * dz;

        if (distSqr > s_shakeFalloffMaxSqr) {
            continue;
        }

        if (distSqr > s_shakeFalloffStartSqr) {
            amplitude = static_cast<float>(pow(0.699999988079071, (sqrt(distSqr) - 9.0) * 0.1111111111111111) * amplitude);
        }

        if (amplitude > amplitudes[shake->m_dir]) {
            amplitudes[shake->m_dir] = amplitude;
            times[shake->m_dir] = time;
            shakes[shake->m_dir] = shake;
        }
    }

    for (uint32_t i = 0; i < 3; i++) {
        CameraShake* shake = shakes[i];

        if (shake) {
            CCameraManager::AddShakeOffset(offset, facing, amplitudes[i], times[i], shake->m_frequency, shake->m_type, shake->m_damping, shake->m_dir);
        }
    }

    return offset;
}

// PROVEN: 1d8175c6 IDENTICAL/DIFFERENT
// OFFSET: 0x00439500/0x140043AD0
void CCameraManager::ApplyShakes(CCameraShot& shot) {
    float yaw = 0.0f;
    float pitch = 0.0f;
    shot.CalcYawPitch(yaw, pitch);
    C3Vector offset = this->CalcShakeOffset(shot.m_position, yaw, 0, 0);
    shot.m_position.x = shot.m_position.x + offset.x;
    shot.m_position.y = shot.m_position.y + offset.y;
    shot.m_position.z = shot.m_position.z + offset.z;
    shot.m_target.x = shot.m_target.x + offset.x;
    shot.m_target.y = shot.m_target.y + offset.y;
    shot.m_target.z = shot.m_target.z + offset.z;
}

// OFFSET: 0x004395B0
void CCameraManager::InitCameraTransitions() {
    CCameraManager::s_transitions.New();
    uint32_t index = CCameraManager::s_transitions.Count();
    CCameraTransition* transition = CCameraManager::s_transitions.New();
    transition->m_duration = 2.0f;
    transition->m_delay = 0.0f;
    transition->m_easeIn = 0.2f;
    transition->m_easeOut = 0.7f;
    transition = CCameraManager::s_transitions.New();
    *transition = CCameraManager::s_transitions[index];
    transition->m_duration = 1.75f;
    transition->m_delay = 0.0f;
    transition->m_easeIn = 0.5f;
    transition->m_easeOut = 0.7f;
}

// OFFSET: 0x00439660/0x140043DF0
void CCameraManager::DestroyCameraModes() {
    CCameraManager::ANIMATED_CAMERA_ID = 0;
    CCameraModeEntry* entry;

    while ((entry = CCameraManager::s_cameraModes.Head()) != NULL) {
        CCameraManager::s_cameraModes.Delete(entry);
    }
}

// PROVEN: 24b45a53 IDENTICAL/DIFFERENT
// OFFSET: 0x004396A0/0x140043ED0
CCameraManager::~CCameraManager() {
    if (this->m_track) {
        delete this->m_track;
    }

    this->m_track = NULL;
    CCameraManager::s_transitions.Clear();
    this->DestroyCameraModes();
}

// OFFSET: 0x00439750
void CCameraManager::SetGameCamera(const CCameraModeInstance* instance, uint32_t id, WOWGUID guid) {
    CCameraModeEntry* entry = CCameraManager::s_cameraModes.Ptr(id, HASHKEY_NONE());

    if (entry) {
        this->m_track->SetCurrentCamera(instance, entry->m_mode, guid);
    }
}

// OFFSET: 0x00439900/0x140044200
void CCameraManager::SetGameCamera(uint32_t id, WOWGUID guid) {
    CCameraModeEntry* entry = CCameraManager::s_cameraModes.Ptr(id, HASHKEY_NONE());

    if (entry) {
        this->m_track->SetCurrentCamera(NULL, entry->m_mode, guid);
    }
}

// PROVEN: 3aa29996 IDENTICAL/DIFFERENT
// OFFSET: 0x00439940/0x140044270
void CCameraManager::AddShake(const C3Vector& position, int32_t type, int32_t dir, float amplitude, float frequency, float duration, float timeOffset, float damping, uint32_t flags) {
    CameraShake* shake = this->m_shakes.NewNode(2, 0, 0);

    if (shake) {
        shake->m_amplitude = amplitude;
        shake->m_frequency = frequency;
        shake->m_duration = duration;
        shake->m_timeOffset = timeOffset;
        shake->m_type = type;
        shake->m_dir = dir;
        shake->m_damping = damping;
        shake->m_startTime = FrameTime::GetCurTimeMs();
        shake->m_position = position;
        shake->m_flags = flags;
    }
}

// PROVEN: 13e0b1c1 IDENTICAL/UNPROVEN
// OFFSET: 0x00439A40/0x1400443C0
void CCameraManager::AddShakeById(int32_t shakeId, const C3Vector& position) {
    CameraShakesRec* shake = g_cameraShakesDB.GetRecord(shakeId);

    if (shake) {
        this->AddShake(position, shake->m_shakeType, shake->m_direction, shake->m_amplitude * 0.027777778f, shake->m_frequency, shake->m_duration, shake->m_phase, shake->m_coefficient, shake->m_flags);
    }
}

// PROVEN: 9200f5b5 DIFFERENT/IDENTICAL
// OFFSET: 0x00439C40/0x140044660
void CCameraManager::ClearShakes() {
    this->m_shakes.Clear();
}

// OFFSET: 0x00439C50/0x140044670
void CCameraManager::PruneShakes() {
    if (!this->m_shakes.Head()) {
        return;
    }

    uint32_t now = FrameTime::GetCurTimeMs();
    CameraShake* next;

    for (CameraShake* shake = this->m_shakes.Head(); shake; shake = next) {
        next = this->m_shakes.Next(shake);

        if (static_cast<float>(now - shake->m_startTime) * 0.001f + shake->m_timeOffset >= shake->m_duration) {
            this->m_shakes.DeleteNode(shake);
        }
    }
}

// OFFSET: 0x00439DE0/0x1400448C0
void CCameraManager::Update(float elapsed) {
    this->m_track->Update(elapsed);

    if (CCameraManager::s_cameraSystemCVar && !CCameraManager::s_cameraSystemCVar->m_intValue) {
        CCameraModeLegacy::CalcShotFromLegacyCamera(this->m_shot);
    } else {
        this->m_shot = this->m_track->m_shot;
    }

    if (!this->m_shot.Sanitize()) {
        SysMsgPrintf(STATUS_WARNING, 2, "INVALID CAMERA SYSTEM FACING VECTOR");
    }

    this->m_renderMatrix = this->m_shot.CalcRenderMatrix();
    this->UpdateLegacyState();
    float fade = this->m_shot.m_fade;
    float clamped = 0.0f;

    if (fade > 1.0f) {
        clamped = 1.0f;
    } else if (fade > 0.0f) {
        clamped = fade;
    }

    CGWorldFrame::s_currentWorldFrame->SetPlayerFadeCameraValue(static_cast<uint8_t>(static_cast<int32_t>(clamped * 255.0f)));
    this->PruneShakes();
}

// PROVEN: 408c60b4 IDENTICAL/DIFFERENT
// OFFSET: 0x0043A3C0/0x1400450A0
void CCameraManager::InitCameraModeFromRecord(const CameraModeRec* rec) {
    if (!rec || !rec->m_id || !rec->m_type) {
        return;
    }

    CCameraMode* mode = this->CreateCameraMode(s_cameraModeBuffers.New<CCameraModeBuffer>(0), rec);
    CCameraModeEntry* entry = CCameraManager::s_cameraModes.New(rec->m_id, HASHKEY_NONE(), 0, 0);
    entry->m_mode = mode;
}

// PROVEN: 1c844eb3 IDENTICAL/DIFFERENT
// OFFSET: 0x0043A480/0x1400452A0
void CCameraManager::InitCameraModes() {
    int32_t maxId = 0;

    for (int32_t i = 0; i < g_cameraModeDB.m_numRecords; i++) {
        CameraModeRec* rec = g_cameraModeDB.GetRecordByIndex(i);

        if (rec && rec->m_id > maxId) {
            maxId = rec->m_id;
        }

        this->InitCameraModeFromRecord(rec);
    }

    CCameraManager::ANIMATED_CAMERA_ID = maxId + 1;
    CCameraManager::s_animatedCameraRec.m_id = maxId + 1;
    CCameraManager::s_animatedCameraRec.m_type = CAMERA_MODE_ANIMATED;
    CCameraManager::s_animatedCameraRec.m_flags = 0x80;
    this->InitCameraModeFromRecord(&CCameraManager::s_animatedCameraRec);
}

// OFFSET: 0x0043A510/0x140045340
CCameraManager::CCameraManager() {
    this->m_aspect = 1.3333334f;
    this->m_track = NULL;
    this->m_legacyActive = 0;
    this->InitCameraModes();
    this->InitCameraTransitions();
    this->m_track = new (SMemAlloc(sizeof(CCameraTrack), __FILE__, __LINE__, 0)) CCameraTrack();
}

// INLINED
CCameraModeEntry::~CCameraModeEntry() {
    if (this->m_mode) {
        this->m_mode->~CCameraMode();
        s_cameraModeBuffers.PutData(this->m_mode, NULL, 0);
    }
}
