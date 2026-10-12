#include <cstdint>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x0084DE70/0x14057CAF0
static bool CVGxMaxFPSCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t maxFPS = SStrToInt(value);

    if (maxFPS - 1 <= 6) {
        maxFPS = 8;
    }

    GxSetMaxFPS(maxFPS);

    return true;
}

// OFFSET: 0x0084DEA0/0x14057CB20
static bool CVGxMaxFPSBkCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t maxFPSBk = SStrToInt(value);

    if (maxFPSBk - 1 <= 6) {
        maxFPSBk = 8;
    }

    GxSetMaxFPSBk(maxFPSBk);

    return true;
}

// OFFSET: 0x0084DED0/0x14057CB50
static bool CVGxDepthResolveHandleCachingCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    GxSetDepthResolveHandleCaching(SStrToInt(value));

    return true;
}

// OFFSET: 0x0084DEF0/0x14057CB70
void GxCVarInitialize() {
    CVar::Register("maxFPS", "Set FPS limit", 1, "200", CVGxMaxFPSCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CVar::Register("maxFPSBk", "Set background FPS limit", 1, "30", CVGxMaxFPSBkCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CVar::Register("gxDepthResolveHandleCaching", "Caching of the target handle for the depth resolve", 1, "1", CVGxDepthResolveHandleCachingCallback, CATEGORY_GRAPHICS, false, NULL, false);
}
