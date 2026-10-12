#include "WoW/Source/WorldClient/MapWeather.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WorldClient/WorldMap.h"

// STATIC: 0x00D017A4
float Weather::s_densityScale = 0.66000003f;

// STATIC: 0x00E756C8/0x140D5F488
CVar* Weather::s_cvarUseWeatherShaders;

// OFFSET: 0x00748540/0x1404275F0
bool Weather::DensityCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    switch (SStrToInt(value)) {
        case 0:
            Weather::s_densityScale = 0.1f;
            break;

        case 1:
            Weather::s_densityScale = 0.33000001f;
            break;

        case 2:
            Weather::s_densityScale = 0.66000003f;
            break;

        case 3:
            Weather::s_densityScale = 1.0f;
            break;

        default:
            ConsoleWriteA("Value out of range (%d - %d)\n", DEFAULT_COLOR, 0, 3);
            return false;
    }

    CWorldMap* worldMap = CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];

    if (worldMap && worldMap->m_weather) {
        worldMap->m_weather->m_densityChanged = 1;
    }

    return true;
}

// OFFSET: 0x00748EF0/0x140428250
void Weather::InitializeCVars() {
    CVar::Register("weatherDensity", NULL, 0, "2", Weather::DensityCallback, CATEGORY_DEFAULT, false, NULL, false);
    Weather::s_cvarUseWeatherShaders = CVar::Register("useWeatherShaders", NULL, 0, "1", NULL, CATEGORY_DEFAULT, false, NULL, false);
}
