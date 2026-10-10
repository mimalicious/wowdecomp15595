#include "WoW/Source/Ui/Transmogrify.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// STATIC: 0x00F4DE40
WOWGUID CGTransmogrify::m_transmogrifier;

// STATIC: 0x00F4DE44
std::list<TransmogrifyPendingItem, Blizzard::Memory::AllocSTL_raw<TransmogrifyPendingItem> > CGTransmogrify::s_pendingItems;

// OFFSET: 0x009BFD90
void CGTransmogrify::OnTransmogrifySuccess(uint32_t slot) {
    for (std::list<TransmogrifyPendingItem, Blizzard::Memory::AllocSTL_raw<TransmogrifyPendingItem> >::iterator it = CGTransmogrify::s_pendingItems.begin(); it != CGTransmogrify::s_pendingItems.end(); ++it) {
        if (it->m_slot == slot) {
            CGTransmogrify::s_pendingItems.erase(it);
            break;
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_TRANSMOGRIFY_SUCCESS, "%d", slot + 1);
}
