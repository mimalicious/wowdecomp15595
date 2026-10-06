#include "Engine/Source/SteelseriesMouse/WowMouse.h"
#include "Engine/Source/Os/Gui.h"
#include "Engine/Source/SteelseriesMouse/HidManager.h"

// OFFSET: %0x0063EBE0
SteelseriesMouse::~SteelseriesMouse() {
}

// OFFSET: %0x007E4010
WowMouse::WowMouse() {
    void* window = OsGuiGetWindow(0);

    if (window) {
        this->m_hidManager = HidManager::Create();
        this->m_hidManager->SetWindow(window);
        this->m_hidManager->EnumerateDevices();
    }
}

// OFFSET: [%0x007E3F90, %0x007E4080]
WowMouse::~WowMouse() {
    if (this->m_hidManager) {
        delete this->m_hidManager;
        this->m_hidManager = NULL;
    }
}

// OFFSET: %0x007E3FF0
int32_t WowMouse::IsConnected() {
    return this->m_hidManager->GetDeviceCount() != 0;
}
