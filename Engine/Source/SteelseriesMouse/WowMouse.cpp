#include "Engine/Source/SteelseriesMouse/WowMouse.h"
#include "Engine/Source/Os/Gui.h"
#include "Engine/Source/SteelseriesMouse/HidManager.h"

// OFFSET: 0x006301E0/0x1402C52E0
SteelseriesMouse::~SteelseriesMouse() {
}

// OFFSET: 0x0087EFF0
WowMouse::WowMouse() {
    void* window = OsGuiGetWindow(0);

    if (window) {
        this->m_hidManager = HidManager::Create();
        this->m_hidManager->SetWindow(window);
        this->m_hidManager->EnumerateDevices();
    }
}

// INLINED
WowMouse::~WowMouse() {
    if (this->m_hidManager) {
        delete this->m_hidManager;
        this->m_hidManager = NULL;
    }
}

// OFFSET: 0x0087EFD0
int32_t WowMouse::IsConnected() {
    return this->m_hidManager->GetDeviceCount() != 0;
}
