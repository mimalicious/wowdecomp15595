#ifndef WOW_SOURCE_UIUTIL_CAMERAMANAGER_H
#define WOW_SOURCE_UIUTIL_CAMERAMANAGER_H

#include <cstddef>
#include <cstdint>

#include "WoW/Source/Object/WGUID.h"

class CCameraManager {
    public:
    static CCameraManager* s_instance;

    static CCameraManager* Get();
    CCameraManager();
    void ResetCameras(WOWGUID guid);
};

#endif
