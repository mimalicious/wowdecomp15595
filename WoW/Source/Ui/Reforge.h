#ifndef WOW_SOURCE_UI_REFORGE_H
#define WOW_SOURCE_UI_REFORGE_H

#include "WoW/Source/Object/WGUID.h"

class CGReforge {
    public:
    static void EnterWorld();
    static void LeaveWorld();
    static WOWGUID m_forgeMaster;

    static void SetForgeMaster(const WOWGUID& guid);
};

#endif
