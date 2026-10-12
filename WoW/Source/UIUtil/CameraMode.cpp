#include "WoW/Source/UIUtil/CameraMode.h"
#include "Engine/Source/Base/CDataAllocator.h"
#include "WoW/Source/DB/Rec/CameraModeRec.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// STATIC: 0x00D3A794/0x140BF5A78
static CDataAllocator s_defaultInstances(sizeof(CCameraModeInstanceDefault), 8);

// PROVEN: 2f9a2c4d UNPROVEN/IDENTICAL
// OFFSET: 0x00461570/0x140074200
CCameraMode::CCameraMode(const CameraModeRec* rec) {
    this->m_rec = rec;
}

// PROVEN: 9e9533c1 UNPROVEN/IDENTICAL
// OFFSET: 0x00461590/0x140074220
CCameraMode::~CCameraMode() {
}

// PROVEN: 8a311a2c IDENTICAL/IDENTICAL
// OFFSET: 0x004615A0/0x140074230
uint32_t CCameraMode::GetID() const {
    return this->m_rec ? this->m_rec->m_id : 0;
}

// PROVEN: aee2a557 IDENTICAL/IDENTICAL
// OFFSET: 0x004615B0/0x1407FDD80
int32_t CCameraMode::AllowDuplicate(const CCameraModeInstance* current, const CCameraModeInstance* instance) const {
    return 0;
}

// OFFSET: 0x004615C0/0x140074240
CCameraModeInstance::CCameraModeInstance() {
    this->m_guid = WOWGUID::Make(0);
    this->m_mode = NULL;
}

// PROVEN: 15d7dd17 IDENTICAL/IDENTICAL
// OFFSET: 0x004615E0/0x140074270
void CCameraModeInstance::Init(const CCameraModeInstance& instance) {
    this->m_shot = instance.m_shot;
    this->m_mode = instance.m_mode;
    this->m_time = instance.m_time;
    this->m_guid = instance.m_guid;
}

// PROVEN: e254f13c IDENTICAL/IDENTICAL
// OFFSET: 0x00461620/0x1400742C0
void CCameraModeInstance::Destroy() {
    this->m_mode->DestroyInstance(this);
    this->m_mode->FreeInstance(this);
}

// PROVEN: 2a52ed9a IDENTICAL/IDENTICAL
// OFFSET: 0x00461640/0x1400742F0
void CCameraModeInstance::Update(float elapsed) {
    this->m_mode->UpdateInstance(this, elapsed);
    this->m_time = elapsed + this->m_time;
}

// PROVEN: 29fd8a62 IDENTICAL/DIFFERENT
// OFFSET: 0x00461670/0x140074330
int32_t CCameraModeInstance::IsTracking(WOWGUID guid) const {
    return this->m_mode->IsTracking(this, guid);
}

// PROVEN: 0811cc15 IDENTICAL/IDENTICAL
// OFFSET: 0x00461690/0x140074350
void CCameraMode::FreeInstance(CCameraModeInstance* instance) const {
    if (instance) {
        s_defaultInstances.PutData(instance, NULL, 0);
    }
}

// OFFSET: 0x004616E0/0x1400743B0
int32_t CCameraMode::IsTracking(const CCameraModeInstance* instance, WOWGUID guid) const {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(instance->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    return unit && unit->IsVehiclePassenger(guid);
}

// OFFSET: 0x00461730/0x140074400
CCameraModeInstance* CCameraMode::AllocInstance(const CCameraModeInstance& instance) const {
    CCameraModeInstanceDefault* result = s_defaultInstances.New<CCameraModeInstanceDefault>(0);
    result->Init(instance);
    return result;
}
