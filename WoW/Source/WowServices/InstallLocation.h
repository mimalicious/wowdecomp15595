#ifndef WOW_SOURCE_WOWSERVICES_INSTALLLOCATION_H
#define WOW_SOURCE_WOWSERVICES_INSTALLLOCATION_H

#include <cstddef>
#include <cstdint>

enum InstallLocation {
    INSTALL_LOCATION_TEST = 0,
    INSTALL_LOCATION_BETA = 1,
    INSTALL_LOCATION_RETAIL = 2,
    INSTALL_LOCATION_INTERNAL = 3,
    INSTALL_LOCATION_UNKNOWN = 4,
};

InstallLocation GetInstallLocation();
const char* GetInstallLocationProgram();
void SetInstallLocationByName(const char* name);

#endif
