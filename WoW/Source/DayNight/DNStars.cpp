#include "WoW/Source/DayNight/DNStars.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "WoW/Source/DayNight/DayNight.h"

// STATIC: 0x00CEFC54/0x140B90358
static C2Vector s_starAlphaTable[4] = {
    C2Vector(0.125f, 1.0f),
    C2Vector(0.1875f, 0.0f),
    C2Vector(0.9375f, 0.0f),
    C2Vector(1.0f, 1.0f),
};

// PROVEN: 05ec80bb IDENTICAL/IDENTICAL
// OFFSET: 0x0064F600/0x1402EE9E0
void DayNight::DNStars::Initialize() {
    this->m_scene = M2CreateScene();
    this->m_model = this->m_scene->CreateModel("Environments\\Stars\\stars.m2", 0, 3);
    this->m_lastTimeMs = FrameTime::GetCurTimeMs();
}

// PROVEN: d3ab89f1 IDENTICAL/DIFFERENT
// OFFSET: 0x0064F630/0x1402EEA20
void DayNight::DNStars::Destroy() {
    if (this->m_model) {
        this->m_model->Release();
        this->m_model = NULL;
    }

    if (this->m_scene) {
        this->m_scene->Release();
        this->m_scene = NULL;
    }
}

// PROVEN: 674570ad IDENTICAL/UNPROVEN
// OFFSET: 0x0064F660/0x1402EEA60
void DayNight::DNStars::Update() {
    this->m_position = DayNight::g_dnInfo.m_camPos;
    this->m_color.a = static_cast<uint8_t>(static_cast<int32_t>(DayNight::InterpTable(s_starAlphaTable, 4, DayNight::g_dnInfo.m_dayFraction) * 254.0f + 1.0f));
}

// PROVEN: 001fc972 IDENTICAL/IDENTICAL
// OFFSET: 0x0064F6D0/0x1402EEAC0
int32_t DayNight::DNStars::IsAllLoaded() {
    return this->m_model->IsLoaded() && this->m_model->TexturesAllLoaded();
}

// OFFSET: 0x0064F710/0x1402EEB20
void DayNight::DNStars::Render(bool interior) {
    if (this->m_color.a < 2) {
        return;
    }

    if (interior) {
        this->m_scene->m_flags |= 0x8;
    } else {
        this->m_scene->m_flags &= ~0x8u;
    }

    this->m_model->SetAnimating(1);

    CM2Model* model = this->m_model;
    int32_t attached = model->m_attachParent != NULL;

    if (attached) {
        model->m_fFlags |= 0x40;
    } else {
        model->m_fFlags |= 0x8;
    }

    if (attached) {
        model->m_fFlags |= 0x10000;
    } else {
        model->m_fFlags |= 0x8000;
    }

    this->m_model->m_fadeAlpha = this->m_color.a * 0.0039215689f;

    uint32_t delta = FrameTime::GetCurTimeMs() - this->m_lastTimeMs;
    this->m_scene->AdvanceTime(delta);
    this->m_lastTimeMs += delta;

    C3Vector cameraPos(0.0f, 0.0f, 0.0f);
    this->m_scene->Animate(cameraPos);
    this->m_scene->BeginDraw();
    this->m_scene->Draw(M2PASS_0);
    this->m_scene->Draw(M2PASS_1);
    this->m_scene->Draw(M2PASS_2);
    this->m_scene->EndDraw();
}
