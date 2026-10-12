#include <cstdint>
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x0100AA9C/0x140EE26A0
CVar* CCharacterComponent::g_componentTextureLevelVar;

// STATIC: 0x0100AA98/0x140EE2698
CVar* CCharacterComponent::g_componentThreadVar;

// STATIC: 0x0100AA94/0x140EE2690
CVar* CCharacterComponent::g_componentCompressVar;

// OFFSET: 0x00A3AA30/0x1404BDD20
bool ComponentVarHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    return true;
}

// OFFSET: 0x006F83C0/0x1403C1320
uint32_t CCharacterComponent::ValidateComponentTextureLevel(uint32_t level) {
    if (level - 7 > 2) {
        return 2;
    }

    if (level >= 9 || OsGetNumberOfProcessors() >= 2) {
        return 0;
    }

    return 16;
}

// OFFSET: 0x00B3C110/0x140908FC0
static bool ComponentTextureLevelCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t level = SStrToInt(value);

    if (CCharacterComponent::ValidateComponentTextureLevel(level)) {
        ConsoleWrite("Unsupported component texture mode.", DEFAULT_COLOR);
        return false;
    }

    char text[256];
    SStrPrintf(text, sizeof(text), "Computent texture level changed to %d", level);
    ConsoleWrite(text, DEFAULT_COLOR);
    return true;
}

// OFFSET: 0x00B3C180/0x140909030
void CCharacterComponent::InitCVars() {
    CCharacterComponent::g_componentTextureLevelVar = CVar::Register("componentTextureLevel", "Number of mip levels used for character component textures", 1, "8", ComponentTextureLevelCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CCharacterComponent::g_componentThreadVar = CVar::Register("componentThread", "Multi thread character component processing", 1, "1", NULL, CATEGORY_GRAPHICS, false, NULL, false);
    CCharacterComponent::g_componentCompressVar = CVar::Register("componentCompress", "Character component texture compression", 1, "1", NULL, CATEGORY_GRAPHICS, false, NULL, false);
}
