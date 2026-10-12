#include "WoW/Source/WorldClient/LiquidEnvironment.h"
#include "Engine/Source/Model2/M2Light.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/WorldClient/WorldMap.h"

// STATIC: 0x00E890B0/0x140D74D80
CDataAllocator CClientLiquidEnvironment::s_allocator(sizeof(CClientLiquidEnvironment), 16);

// STATIC: 0x00E890AC/0x140D74D78
CClientLiquidEnvironment* CClientLiquidEnvironment::s_shared;

// STATIC: 0x00E890C4/0x140D74DA0
static int32_t s_localLightInitialized;

// OFFSET: 0x0076C740/0x140454CB0
void CClientLiquidEnvironment::SetInteriorFog(int32_t interiorFog) {
    this->m_interiorFog = interiorFog;
}

// OFFSET: 0x0076C750
Liquid::IEnvironment::~IEnvironment() {
}

// OFFSET: 0x0076C780/0x140454CC0
void CClientLiquidEnvironment::Destroy() {
    if (CClientLiquidEnvironment::s_shared) {
        CClientLiquidEnvironment::s_allocator.Delete(CClientLiquidEnvironment::s_shared);
        CClientLiquidEnvironment::s_shared = NULL;
    }

    CClientLiquidEnvironment::s_allocator.Clear(typeid(CClientLiquidEnvironment).raw_name(), SLOG_OBJECT, 1);
}

// OFFSET: 0x0076C7D0/0x140454D30
void CClientLiquidEnvironment::Release() {
    if (this != CClientLiquidEnvironment::s_shared) {
        CClientLiquidEnvironment::s_allocator.Delete(this);
    }
}

// OFFSET: 0x0076C800/0x140454D70
void CClientLiquidEnvironment::SelectLights(CM2Lighting& lighting) {
    const DayNight::FogInfo* fog = this->m_interiorFog ? DayNight::GetIntFogInfo() : DayNight::GetFogInfo();
    C3Vector color(fog->m_color.r * 0.0039215689f, fog->m_color.g * 0.0039215689f, fog->m_color.b * 0.0039215689f);
    lighting.SetFog(color, fog->m_start, fog->m_end, fog->m_density);

    if (!this->m_localLight) {
        lighting.AddLight(&CWorldMap::s_sunLight);
        return;
    }

    static CM2Light s_localLight;

    if (!s_localLightInitialized) {
        s_localLight.SetLightType(M2LIGHT_DIRECT);
        s_localLight.m_constantAttenuation = 1.0f;
        s_localLight.m_linearAttenuation = 0.0f;
        s_localLight.m_quadraticAttenuation = 0.0f;
        s_localLight.SetDirection(C3Vector(0.0f, 0.0f, -1.0f));
        s_localLight.m_dirColor = C3Vector(1.0f, 1.0f, 1.0f);
        s_localLight.SetVisible(1);
        s_localLightInitialized = 1;
    }

    lighting.AddLight(&s_localLight);
}

// OFFSET: 0x0076C980/0x140454F70
CClientLiquidEnvironment* CClientLiquidEnvironment::Create(int32_t shared, int32_t localLight) {
    if (shared) {
        if (!CClientLiquidEnvironment::s_shared) {
            CClientLiquidEnvironment* environment = CClientLiquidEnvironment::s_allocator.New<CClientLiquidEnvironment>(0);
            environment->m_localLight = 0;
            CClientLiquidEnvironment::s_shared = environment;
            environment->m_interiorFog = 0;
        }

        return CClientLiquidEnvironment::s_shared;
    }

    CClientLiquidEnvironment* environment = CClientLiquidEnvironment::s_allocator.New<CClientLiquidEnvironment>(0);
    environment->m_localLight = localLight;
    environment->m_interiorFog = 0;
    return environment;
}
