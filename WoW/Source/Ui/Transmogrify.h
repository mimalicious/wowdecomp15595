#ifndef WOW_SOURCE_UI_TRANSMOGRIFY_H
#define WOW_SOURCE_UI_TRANSMOGRIFY_H

#include <cstdint>
#include <list>
#include "Engine/Source/BlizzardCore/Source/System/Memory/AllocSTL.h"
#include "WoW/Source/Object/WGUID.h"

struct TransmogrifyPendingItem {
    /* 0000 */ WOWGUID m_item;
    /* 0008 */ uint32_t m_slot;
};

class CGTransmogrify {
    public:
    static void InitializeGame();
    static void ShutdownGame();
    static WOWGUID m_transmogrifier;
    static std::list<TransmogrifyPendingItem, Blizzard::Memory::AllocSTL_raw<TransmogrifyPendingItem> > s_pendingItems;

    static void SetTransmogrifier(const WOWGUID& guid);
    static void OnTransmogrifySuccess(uint32_t slot);
    static TransmogrifyPendingItem* GetPendingItem(uint32_t slot);
};

#endif
