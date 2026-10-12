#include "WoW/Source/DayNight/DNOverrideSky.h"
#include <cstdlib>
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "WoW/Source/DayNight/DayNight.h"

// STATIC: 0x00E4325C/0x140D2BFE8
CM2Scene* DayNight::DNOverrideSky::s_scene;

// STATIC: 0x00E43260/0x140D2BFF0
uint32_t DayNight::DNOverrideSky::s_lastTimeMs;

// PROVEN: 54190adf IDENTICAL/IDENTICAL
// OFFSET: 0x0064EDB0/0x1402EDDE0
int32_t DayNight::DNOverrideSky::IsAllLoaded() {
    return this->m_model->IsLoaded() && this->m_model->TexturesAllLoaded();
}

// PROVEN: 1e4198fe IDENTICAL/NONE
// OFFSET: 0x0064EDE0
void DayNight::DNOverrideSky::LinkToAsyncHead() {
    this->m_model->LinkToAsyncHead();
}

// OFFSET: 0x0064EDF0/0x1402EDE30
void DayNight::SyncSkyAnimWithTime(DNOverrideSky* sky, int32_t minute) {
    if (!sky || !sky->m_model || !sky->m_model->IsDrawable()) {
        return;
    }

    if (!sky->m_animLengthMs) {
        M2BoneSequenceInfo info;
        sky->m_model->GetBoneSequenceInfo(-1, info);
        sky->m_animLengthMs = info.m_endTime - info.m_startTime;
    }

    uint32_t length = sky->m_animLengthMs;

    if (!length) {
        return;
    }

    if (abs(minute - sky->m_lastMinute) >= 2 && (sky->m_flags & 0x1)) {
        float animLength = static_cast<float>(length);
        uint32_t time = static_cast<uint32_t>(static_cast<int64_t>(static_cast<float>(minute) * 0.00069444446f * animLength));
        sky->m_model->SetBoneSequence(-1, 0, -1, time, animLength * 0.000000011574074f, 1, 1, 0, 0, 0);
    }

    sky->m_lastMinute = minute;
}

// PROVEN: 2e4d28e5 IDENTICAL/IDENTICAL
// OFFSET: 0x0064EEC0/0x1402EDF30
DayNight::DNOverrideSky::DNOverrideSky() {
    this->m_model = NULL;
    this->m_animLengthMs = 0;
    this->m_lastMinute = -2;
    this->m_flags = 0;
}

// OFFSET: 0x0064EEF0/0x1402EDF60
void DayNight::DestroyOverrideSky(const char* name) {
    if (!name || !*name) {
        return;
    }

    DNOverrideSky* sky = DayNight::g_overrideSky.Ptr(name);

    if (!sky) {
        return;
    }

    if (sky->m_model) {
        sky->m_model->Release();
    }

    DayNight::g_overrideSky.Delete(sky);
}

// OFFSET: 0x0064F480/0x1402EE750
DayNight::DNOverrideSky* DayNight::GetOverrideSky(uint32_t timeMs, const char* name, uint32_t flags) {
    if (!name || !*name) {
        return NULL;
    }

    DNOverrideSky* sky = DayNight::g_overrideSky.Ptr(name);

    if (sky) {
        return sky;
    }

    sky = DayNight::g_overrideSky.New(name, 0, 0);

    if (!DNOverrideSky::s_scene) {
        DNOverrideSky::s_scene = M2CreateScene();
        DNOverrideSky::s_lastTimeMs = timeMs;
    }

    sky->m_model = DNOverrideSky::s_scene->CreateModel(name, 0, 3);
    sky->m_flags = flags;
    return sky;
}

// OFFSET: 0x0064F590/0x1402EE960
DayNight::DNOverrideSky* DayNight::SetBlendSky(uint32_t timeMs, uint32_t index, const char* name, uint32_t flags, float weight) {
    if (index > 3) {
        return NULL;
    }

    if (!name) {
        DayNight::g_dnInfo.m_blendSkybox[index] = NULL;
        DayNight::g_dnInfo.m_blendSkyboxWeight[index] = 0.0f;
        DayNight::g_dnInfo.m_blendSkyboxFlags[index] = 0;
        return NULL;
    }

    DNOverrideSky* sky = DayNight::GetOverrideSky(timeMs, name, flags);
    DayNight::g_dnInfo.m_blendSkybox[index] = sky;
    DayNight::g_dnInfo.m_blendSkyboxWeight[index] = weight;
    DayNight::g_dnInfo.m_blendSkyboxFlags[index] = 0;
    return sky;
}
