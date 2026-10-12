#include <cstdint>
#include <cstdlib>
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/FFX/EffectGlow.h"

// STATIC: 0x00EC6FEC/0x140DB9408
CVar* FFX::s_cvarFFXRectangle;

// STATIC: 0x00EC6FF0/0x140DB9410
CVar* FFX::s_cvarFFX;

// PROVEN: 2d04aead IDENTICAL/IDENTICAL
// OFFSET: 0x0089CCF0/0x1405DCFB0
bool FFX::CVarCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t enabled = atoi(value);
    ConsoleWrite(var->m_key.m_str, DEFAULT_COLOR);

    if (enabled) {
        ConsoleWrite("enabled", DEFAULT_COLOR);
    } else {
        ConsoleWrite("disabled", DEFAULT_COLOR);
    }

    return true;
}

// PROVEN: 41eb1417 DIFFERENT/IDENTICAL
// OFFSET: 0x0089D110/0x1405DD4B0
void FFX::InitCVars() {
    FFX::s_defaultEnabled = true;
    FFX::s_cvarFFX = CVar::Register("ffx", "full screen effects", 1, "1", FFX::CVarCallback, 1, false, NULL, false);
    FFX::s_cvarFFXRectangle = CVar::Register("ffxRectangle", "use rectangle texture for full screen effects", 1, "1", FFX::CVarCallback, 1, false, NULL, false);
    EffectGlow::InitCVars();
}
