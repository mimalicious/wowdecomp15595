#ifndef WOW_SOURCE_UI_ITEMSOCKETINFO_H
#define WOW_SOURCE_UI_ITEMSOCKETINFO_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

void ItemSocketInfoRegisterScriptFunctions();
void ItemSocketInfoUnregisterScriptFunctions();

class ItemRecSparse;

class CGItemSocketInfo {
    public:
    static uint8_t m_gemContainerSlot[3];
    static WOWGUID m_gemItemContainer[3];
    static WOWGUID m_gemItems[3];
    static WOWGUID m_socketItem;
    static ItemRecSparse* m_socketItemRec;

    static void EnterWorld();
    static void Accept();
    static void GetGemItemForSlot(uint32_t slot, WOWGUID& item, WOWGUID& container, uint8_t& containerSlot);
    static int32_t SetGem(uint32_t slot, WOWGUID item, WOWGUID container, uint8_t containerSlot);
    static bool IsSocketItemBoundTradeable();
    static bool IsSocketItemRefundable();
    static void SetItem(WOWGUID item);
    static void LeaveWorld();
    static void UpdateItem(WOWGUID item);
    static void RemoveItem(WOWGUID item);
    static int32_t GemMatchesSocket(const ItemRecSparse* item, const ItemRecSparse* gem, int32_t socket);
};

void EnchantItemCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success);

#endif
