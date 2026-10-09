#ifndef WOW_SOURCE_UI_VOIDSTORAGEINFO_H
#define WOW_SOURCE_UI_VOIDSTORAGEINFO_H

#include "WoW/Source/Object/WGUID.h"

class CGVoidStorageInfo {
    public:
    static void EnterWorld();
    static void LeaveWorld();
    static WOWGUID m_npc;

    static void SetNPC(const WOWGUID& guid);
};

#endif
