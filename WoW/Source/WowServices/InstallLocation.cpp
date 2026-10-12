#include "WoW/Source/WowServices/InstallLocation.h"
#include <cstring>

// STATIC: 0x00CA0880/0x140B3C908
static const char* s_installLocationPrograms[4] = { "WoWT", "WoWB", "WoW", "WoWI" };

// STATIC: 0x00CA0890/0x140B3C928
static InstallLocation s_installLocation = INSTALL_LOCATION_RETAIL;

// PROVEN: abbed250 IDENTICAL/IDENTICAL
// OFFSET: 0x004C5C50/0x1400FFF10
InstallLocation GetInstallLocation() {
    return s_installLocation;
}

// PROVEN: d9bb2def IDENTICAL/IDENTICAL
// OFFSET: 0x004C5C60/0x1400FFF20
const char* GetInstallLocationProgram() {
    return s_installLocationPrograms[s_installLocation];
}

// OFFSET: 0x004C5C70/0x1400FFF40
void SetInstallLocationByName(const char* name) {
    if (!_stricmp(name, "Retail")) {
        s_installLocation = INSTALL_LOCATION_RETAIL;
        return;
    }

    for (uint32_t i = 0; i < 4; i++) {
        size_t nameLength = strlen(name);
        size_t programLength = strlen(s_installLocationPrograms[i]);

        if (!_strnicmp(name, s_installLocationPrograms[i], nameLength > programLength ? nameLength : programLength)) {
            s_installLocation = static_cast<InstallLocation>(i);
            return;
        }
    }

    s_installLocation = INSTALL_LOCATION_UNKNOWN;
}
