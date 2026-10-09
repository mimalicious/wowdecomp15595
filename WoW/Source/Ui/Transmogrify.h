#ifndef WOW_SOURCE_UI_TRANSMOGRIFY_H
#define WOW_SOURCE_UI_TRANSMOGRIFY_H

#include "WoW/Source/Object/WGUID.h"

class CGTransmogrify {
    public:
    static void InitializeGame();
    static void ShutdownGame();
    static WOWGUID m_transmogrifier;

    static void SetTransmogrifier(const WOWGUID& guid);
};

#endif
