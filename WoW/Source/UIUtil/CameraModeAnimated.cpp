#include "WoW/Source/UIUtil/CameraMode.h"
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/Camera.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/Rec/CameraModeRec.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/StaticSingleton.h"
#include "WoW/Source/UIUtil/CameraManager.h"

// STATIC: 0x00D3A7AC
int32_t CCameraModeAnimated::s_cameraIndex;

// STATIC: 0x00D3A7B0/0x140BF5AA8
static CDataAllocator s_animatedInstances(sizeof(CCameraModeInstanceAnimated), 8);

// INLINED
CCameraModeInstanceAnimated::CCameraModeInstanceAnimated() {
    this->m_active = 1;
}

// PROVEN: 9f4beb83 IDENTICAL/IDENTICAL
// OFFSET: 0x00461B70/0x140074910
int32_t CCameraModeAnimated::AllowDuplicate(const CCameraModeInstance* current, const CCameraModeInstance* instance) const {
    const CCameraModeInstanceAnimated* a = static_cast<const CCameraModeInstanceAnimated*>(current);
    const CCameraModeInstanceAnimated* b = static_cast<const CCameraModeInstanceAnimated*>(instance);
    return a->m_cameraIndex != b->m_cameraIndex || a->m_active != b->m_active;
}

// OFFSET: 0x00461BA0/0x140074930
CCameraModeInstance* CCameraModeAnimated::AllocInstance(const CCameraModeInstance& instance) const {
    CCameraModeInstanceAnimated* result = s_animatedInstances.New<CCameraModeInstanceAnimated>(0);
    result->Init(instance);
    return result;
}

// PROVEN: be6bbeb5 IDENTICAL/IDENTICAL
// OFFSET: 0x00461BF0/0x140074990
void CCameraModeAnimated::FreeInstance(CCameraModeInstance* instance) const {
    if (instance) {
        s_animatedInstances.PutData(instance, NULL, 0);
    }
}

// PROVEN: 09890c22 IDENTICAL/DIFFERENT
// OFFSET: 0x00461C10/0x1400749B0
int32_t CCameraModeAnimated::CalcShotFromModel(int32_t index, CM2Model* model, CCameraShot& shot) const {
    if (!model || !model->IsLoaded()) {
        return 0;
    }

    if (!(model->m_fFlags & 0x1)) {
        model->WaitForLoad(NULL);
    }

    if (index < 0 || index >= static_cast<int32_t>(model->m_shared->m_data->m_cameras.m_count)) {
        return 0;
    }

    CCamera* camera = static_cast<CCamera*>(model->GetCameraByIndex(index));

    if (!camera) {
        return 0;
    }

    CameraGetCoord(camera, 7, &shot.m_position);
    CameraGetCoord(camera, 8, &shot.m_target);
    shot.m_roll = CMath::normalizeAngleNegPiToPi_(CameraGetFloat(camera, 5));
    shot.m_fov = CameraGetFloat(camera, 4);
    return 1;
}

// OFFSET: 0x00461CB0
void CCameraModeAnimated::UpdateInternal(CCameraModeInstance* instance) const {
    CCameraModeInstanceAnimated* animated = static_cast<CCameraModeInstanceAnimated*>(instance);

    if (!animated->m_active) {
        return;
    }

    int32_t index = animated->m_cameraIndex;
    CGObject_C* object = ClntObjMgrObjectPtr(animated->m_guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object || !this->CalcShotFromModel(index, object->m_worldModel, animated->m_shot)) {
        animated->m_active = 0;
        WOWGUID player = ClntObjMgrGetActivePlayer();
        StaticSingleton<CCameraManager>::Instance()->SetGameCamera(animated, 4, player);
    }
}

// OFFSET: 0x00461D50/0x140074B00
void CCameraModeAnimated::InitInstance(CCameraModeInstance* instance) const {
    CCameraModeInstanceAnimated* animated = static_cast<CCameraModeInstanceAnimated*>(instance);

    if (!(this->m_rec->m_flags & 0x80)) {
        animated->m_cameraIndex = -1;
    } else {
        animated->m_cameraIndex = CCameraModeAnimated::s_cameraIndex;
    }

    this->UpdateInternal(animated);
    animated->m_shot.m_fade = 1.0f;
}

// PROVEN: e787ccad IDENTICAL/DIFFERENT
// OFFSET: 0x00461D90/0x140074B90
void CCameraModeAnimated::UpdateInstance(CCameraModeInstance* instance, float elapsed) const {
    this->UpdateInternal(instance);
}

// PROVEN: be81a950 IDENTICAL/IDENTICAL
// OFFSET: 0x00B07330/0x1408CA670
int32_t CCameraModeAnimated::GetType() const {
    return CAMERA_MODE_ANIMATED;
}

// PROVEN: 17f5f54f IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CCameraModeAnimated::DestroyInstance(CCameraModeInstance* instance) const {
}
