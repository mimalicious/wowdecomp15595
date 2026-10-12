#include "ViolenceLevels.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"

static int32_t s_maxViolenceLevel[9] = { 2, 1, 2, 2, 1, 2, 2, 2, 2 };

static const char* s_bloodSpurtTextures[2] = { "BloodSpurtSmall01_Low.blp", "BloodSpurtSmall01_Medium.blp" };

static const char* s_bloodSpurtTexture = "BloodSpurtSmall01.blp";

// STATIC: 0x01008A24/0x140EE03BC
int32_t s_violenceLevel;

// OFFSET: 0x00B11460/0x1408D6D40
bool ViolenceLevelChanged(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t level = SStrToInt(value);
    s_violenceLevel = level;

    if (level < 0 || level > s_maxViolenceLevel[s_locale]) {
        level = s_maxViolenceLevel[s_locale];
        s_violenceLevel = level;
    }

    if (level == 2) {
        TextureSetTextureSubstitutions(NULL, NULL);
    } else {
        TextureSetTextureSubstitutions(s_bloodSpurtTexture, s_bloodSpurtTextures[level]);
    }

    return true;
}

// OFFSET: 0x00B11510/0x1408D6DF0
void ViolenceLevelsRegisterCVars() {
    char value[12];
    SStrPrintf(value, sizeof(value), "%d", s_maxViolenceLevel[s_locale]);
    CVar::Register("violenceLevel", "Sets the violence level of the game", 0x10, value, ViolenceLevelChanged, CATEGORY_GAME, false, NULL, false);
}
