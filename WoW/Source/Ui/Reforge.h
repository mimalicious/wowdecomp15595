#ifndef WOW_SOURCE_UI_REFORGE_H
#define WOW_SOURCE_UI_REFORGE_H

#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

class ItemReforgeRec;

struct REFORGE_OPTION {
    /* 0000 */ TSLink<REFORGE_OPTION> m_link;
    /* 0008 */ const ItemReforgeRec* m_reforge;
};

class CGReforge {
    public:
    static WOWGUID m_forgeMaster;
    static WOWGUID m_reforgeItem;
    static TSCGrowableArray<REFORGE_OPTION, 16> s_reforgeOptions;

    static void ShutdownGame();
    static void EnterWorld();
    static void LeaveWorld();
    static void SetForgeMaster(const WOWGUID& guid);
    static bool SetItem(WOWGUID item);
    static void UpdateEnchantment(WOWGUID item);
    static uint32_t GetNumOptions();
    static const ItemReforgeRec* GetOption(int32_t index);
    static void ReforgeItem(int32_t index);
    static int32_t FindOption(int32_t reforgeId);
    static bool UpdateOptions();
    static void ItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static bool CanReforge(WOWGUID item, int32_t* reasons);
};

void ReforgeRegisterScriptFunctions();
void ReforgeUnregisterScriptFunctions();

#endif
