#include "WoW/Source/DayNight/DayNight.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "WoW/Source/DayNight/DNGlare.h"
#include "WoW/Source/DayNight/DNOverrideSky.h"
#include "WoW/Source/DayNight/DNPlanet.h"
#include "WoW/Source/DayNight/DNStars.h"
#include "WoW/Source/WorldClient/MapObj.h"

// OFFSET: 0x00B38360
static void LinkAllToAsyncHead(void* param) {
    for (DayNight::DNOverrideSky* sky = DayNight::g_overrideSky.Head(); sky; sky = DayNight::g_overrideSky.Next(sky)) {
        sky->LinkToAsyncHead();
    }

    DayNight::g_planets[0].LinkToAsyncHead();
    DayNight::g_planets[1].LinkToAsyncHead();
    DayNight::g_planets[2].LinkToAsyncHead();
    DayNight::g_sunGlare.GlareBase::LinkToAsyncHead();
    DayNight::g_moonGlare.GlareBase::LinkToAsyncHead();

    if (DayNight::g_stars.m_model) {
        DayNight::g_stars.m_model->LinkToAsyncHead();
    }
}

// OFFSET: 0x00B38290
void DayNight::Preload(const C3Vector& pos) {
    C3Vector dir(0.0f, 0.0f, 1.0f);
    C3Vector target;
    SMOFog fog;
    fog.m_flags = 0;
    fog.m_start = 0.0f;
    fog.m_end = 0.0f;
    fog.m_fogColor = 0;
    fog.m_uwFogColor = 0;

    DayNight::ForceFullUpdate();
    DayNight::SetCamera(pos, dir, target, 0, 0.0f, 1000.0f);
    DayNight::SetStormPercentage(0.0f);
    DayNight::SetInteriorFog(false, fog, false, 0.0f);
    DayNight::Update(FrameTime::GetCurTimeMs(), FrameTime::GetTickTimeSec(), FrameTime::GetCurTimeSec());
}

// OFFSET: 0x00B383E0
int32_t DayNight::IsAllLoaded() {
    for (DNOverrideSky* sky = DayNight::g_overrideSky.Head(); sky; sky = DayNight::g_overrideSky.Next(sky)) {
        if (!sky->IsAllLoaded()) {
            return 0;
        }
    }

    return DayNight::g_planets[0].IsAllLoaded()
        && DayNight::g_planets[1].IsAllLoaded()
        && DayNight::g_planets[2].IsAllLoaded()
        && DayNight::g_sunGlare.GlareBase::IsAllLoaded()
        && DayNight::g_moonGlare.GlareBase::IsAllLoaded()
        && DayNight::g_stars.IsAllLoaded();
}

// PROVEN: 00212bb2 IDENTICAL/NONE
// OFFSET: 0x00B38480
void DayNight::StreamingUpdate() {
    if (!DayNight::IsAllLoaded()) {
        AsyncFileReadAddStreamingObject(LinkAllToAsyncHead, NULL, 0);
    }
}
