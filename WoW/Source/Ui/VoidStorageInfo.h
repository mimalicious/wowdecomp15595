#ifndef WOW_SOURCE_UI_VOIDSTORAGEINFO_H
#define WOW_SOURCE_UI_VOIDSTORAGEINFO_H

#include "WoW/Source/Object/WGUID.h"

class CGItem_C;

class CGVoidStorageInfo {
    public:
    static WOWGUID m_npc;

    static void ShutdownGame();
    static void EnterWorld();
    static void LeaveWorld();
    static void SetNPC(const WOWGUID& guid);
    static void AutoDepositItem(CGItem_C* item);
    static bool IsItemPendingWithdraw(WOWGUID guid);
    static void DepositItem(CGItem_C* item, uint32_t slot, bool playSound);
};

void VoidStorageRegisterScriptFunctions();
void VoidStorageUnregisterScriptFunctions();

#endif
