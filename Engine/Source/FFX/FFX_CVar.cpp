#include <cstdint>
#include <cstdlib>
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/FFX/EffectGlow.h"

// STATIC: %0x00D95214
CVar* FFX::s_cvarFFXRectangle;

// STATIC: %0x00D95218
CVar* FFX::s_cvarFFX;

// OFFSET: %0x00801470
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

// OFFSET: %0x00801890
void FFX::InitCVars() {
    FFX::s_defaultEnabled = true;
    FFX::s_cvarFFX = CVar::Register("ffx", "full screen effects", 1, "1", FFX::CVarCallback, 1, false, NULL, false);
    FFX::s_cvarFFXRectangle = CVar::Register("ffxRectangle", "use rectangle texture for full screen effects", 1, "1", FFX::CVarCallback, 1, false, NULL, false);
    EffectGlow::InitCVars();
}
