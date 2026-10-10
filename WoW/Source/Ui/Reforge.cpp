#include "WoW/Source/Ui/Reforge.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// STATIC: 0x00F4B0C8
WOWGUID CGReforge::m_forgeMaster;

// STATIC: 0x00F4B0D0
WOWGUID CGReforge::m_reforgeItem;

// OFFSET: 0x0098B510
void CGReforge::UpdateEnchantment(WOWGUID item) {
    if (CGReforge::m_forgeMaster && CGReforge::m_reforgeItem && CGReforge::m_reforgeItem == item) {
        FrameScript_SignalEvent(SCRIPT_EVENT_FORGE_MASTER_ITEM_CHANGED, NULL);
    }
}
