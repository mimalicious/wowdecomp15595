#ifndef WOW_SOURCE_UI_REFORGE_H
#define WOW_SOURCE_UI_REFORGE_H

#include "WoW/Source/Object/WGUID.h"

class CGReforge {
    public:
    static void ShutdownGame();
    static void EnterWorld();
    static void LeaveWorld();
    static WOWGUID m_forgeMaster;
    static WOWGUID m_reforgeItem;

    static void SetForgeMaster(const WOWGUID& guid);
    static void UpdateEnchantment(WOWGUID item);
};

#endif
