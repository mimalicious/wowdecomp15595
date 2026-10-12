#include "WoW/Source/Ui/Transmogrify.h"
#include <cstdlib>
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00D15C40/0x140BCF7F0
static const char* s_transmogrifyReasons[10] = { "OK", "NO_ITEM", "NOT_SOULBOUND", "LEGENDARY", "INVALID_TYPE", "INVALID_TARGET", "MISMATCH", "SAME_ITEM", "INVALID_SOURCE", "NO_STATS" };

// OFFSET: 0x009BED00
static bool GetTransmogrifySlot(lua_State* L, int32_t index, int32_t* slot) {
    if (!lua_isnumber(L, index)) {
        return false;
    }

    *slot = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, index))) - 1;

    for (uint32_t i = 0; i < 12; i++) {
        if (s_transmogrifySlots[i] == static_cast<uint32_t>(*slot)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009BED60
static char* GetTransmogrifyReasonToken(int32_t reason, char* buffer, uint32_t size) {
    SStrCopy(buffer, s_transmogrifyReasons[reason], size);

    return buffer;
}

// OFFSET: 0x009BF010/0x14074B150
static int32_t Script_CloseTransmogrifyFrame(lua_State* L) {
    CGTransmogrify::SetTransmogrifier(WOWGUID());

    return 0;
}

// OFFSET: 0x009BF5B0/0x14074B860
static int32_t Script_ClearTransmogrifySlot(lua_State* L) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    int32_t slot = 0;

    if (GetTransmogrifySlot(L, 1, &slot)) {
        CGTransmogrify::RemovePendingItem(slot, true);
        return 0;
    }

    CGTransmogrify::s_pendingItems.clear();
    FrameScript_SignalEvent(SCRIPT_EVENT_TRANSMOGRIFY_UPDATE, NULL);

    return 0;
}

// OFFSET: 0x009BF630/0x14074B950
static int32_t Script_ApplyTransmogrifications(lua_State* L) {
    if (!CGTransmogrify::s_pendingItems.size()) {
        return 0;
    }

    UserClientTransmogrifyItems msg;
    memcpy(msg.m_npc, &CGTransmogrify::m_transmogrifier, sizeof(msg.m_npc));
    msg.m_items.Reserve(CGTransmogrify::s_pendingItems.size(), 0);

    for (std::list<TransmogrifyPendingItem, Blizzard::Memory::AllocSTL_raw<TransmogrifyPendingItem> >::iterator it = CGTransmogrify::s_pendingItems.begin(); it != CGTransmogrify::s_pendingItems.end(); ++it) {
        JamTransmogrifyItem* entry = msg.m_items.New();
        entry->m_itemID = 0;

        if (it->m_item) {
            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(it->m_item, TYPEMASK_ITEM, __FILE__, __LINE__));

            if (item) {
                entry->m_itemID = item->m_obj->m_entryID;
            }
        }

        memcpy(entry->m_itemGuid, &it->m_item, sizeof(entry->m_itemGuid));
        entry->m_slot = it->m_slot;
    }

    ClientServices::Send(&msg);

    return 0;
}

// OFFSET: 0x009BF760/0x14074BAF0
static int32_t Script_GetTransmogrifyCost(lua_State* L) {
    int32_t count = 0;
    uint32_t cost = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        for (std::list<TransmogrifyPendingItem, Blizzard::Memory::AllocSTL_raw<TransmogrifyPendingItem> >::iterator it = CGTransmogrify::s_pendingItems.begin(); it != CGTransmogrify::s_pendingItems.end(); ++it) {
            count++;

            if (it->m_item) {
                CGItem_C* equipped = player->m_bag.GetItemPointer(it->m_slot);

                if (equipped) {
                    cost += equipped->GetTransmogrifyCost();
                }
            }
        }
    }

    lua_pushnumber(L, cost);
    lua_pushnumber(L, count);

    return 2;
}

// OFFSET: 0x009BF820/0x14074BBC0
static int32_t Script_GetTransmogrifySlotInfo(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t slot = 0;

    if (!GetTransmogrifySlot(L, 1, &slot)) {
        luaL_error(L, "Usage: GetTransmogrifySlotInfo(slot)");
        return 0;
    }

    CGItem_C* equipped = player->m_bag.GetItemPointer(slot);

    if (equipped) {
        VisibleItem_C* visible = const_cast<VisibleItem_C*>(player->GetVisibleItem(slot));
        int32_t itemId;
        uint32_t displayId;

        if (!visible || equipped->m_obj->m_entryID == static_cast<uint32_t>(abs(visible->m_itemId))) {
            lua_pushboolean(L, 0);
            itemId = equipped->m_obj->m_entryID;
            displayId = equipped->GetDisplayID();
        } else {
            lua_pushboolean(L, 1);
            itemId = abs(visible->m_itemId);
            displayId = visible->GetDisplayID();
        }

        ClientItemRec rec(equipped->GetStats());
        int32_t reason = 0;
        bool canTransmogrify = CanTransmogrifyTarget(&rec, equipped->m_item->m_itemFieldFlags, equipped->m_item->m_itemFieldRandomPropertiesId, &reason) != 0;
        lua_pushboolean(L, canTransmogrify);
        lua_pushnumber(L, reason);
        TransmogrifyPendingItem* pending = CGTransmogrify::GetPendingItem(slot);

        if (pending) {
            if (pending->m_item) {
                lua_pushboolean(L, 1);
                lua_pushboolean(L, 0);
                CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(pending->m_item, TYPEMASK_ITEM, __FILE__, __LINE__));

                if (item) {
                    displayId = item->GetDisplayID();
                    itemId = item->m_obj->m_entryID;
                }
            } else {
                lua_pushboolean(L, 0);
                lua_pushboolean(L, 1);
                itemId = equipped->m_obj->m_entryID;
            }
        } else {
            lua_pushboolean(L, 0);
            lua_pushboolean(L, 0);
        }

        lua_pushnumber(L, itemId);

        if (displayId) {
            const char* path = ClientDBStringLookup(3);
            char texture[260];
            SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(displayId));
            lua_pushstring(L, texture);
        } else {
            lua_pushnil(L);
        }

        return 7;
    }

    lua_pushboolean(L, 0);
    lua_pushboolean(L, 0);
    lua_pushnumber(L, 1.0);
    return 3;
}

// OFFSET: 0x009BFAB0/0x14074BF20
static int32_t Script_ClickTransmogrifySlot(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t slot = 0;

    if (!GetTransmogrifySlot(L, 1, &slot)) {
        luaL_error(L, "Usage: ClickTransmogrifySlot(slot)");
        return 0;
    } else {
        const ChrClassesRec* classRec = g_chrClassesDB.GetRecord(player->m_unit->m_unitFieldBytes0[1]);

        if (slot == INVSLOT_RANGED && classRec && (classRec->m_flags & 8)) {
            return 0;
        }

        WOWGUID cursorItem;
        WOWGUID container;
        uint32_t cursorSlot;
        CGGameUI::GetCursorItem(cursorItem, container, cursorSlot);

        if (cursorItem) {
            CGTransmogrify::UseItemForSlot(static_cast<CGItem_C*>(ClntObjMgrObjectPtr(cursorItem, TYPEMASK_ITEM, __FILE__, __LINE__)), slot);
            return 0;
        }

        TransmogrifyPendingItem* pending = CGTransmogrify::GetPendingItem(slot);

        if (pending) {
            if (pending->m_item) {
                CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(pending->m_item, TYPEMASK_ITEM, __FILE__, __LINE__));

                if (item) {
                    uint8_t itemSlot = CGPlayer_C::FindItemSlot(item->m_item->m_itemFieldContained, item);
                    CGGameUI::SetCursorItem(pending->m_item, item->m_item->m_itemFieldContained, itemSlot, 0, 0, 0);
                }
            }

            CGTransmogrify::RemovePendingItem(slot, true);

            return 0;
        }

        const VisibleItem_C* visible = player->GetVisibleItem(slot);
        CGItem_C* equipped = player->m_bag.GetItemPointer(slot);

        if (visible && equipped && equipped->m_obj->m_entryID != static_cast<uint32_t>(abs(visible->m_itemId))) {
            CGTransmogrify::SetPendingItem(slot, WOWGUID(), true);
        }
    }

    return 0;
}

// OFFSET: 0x009BFC70/0x14074C150
static int32_t Script_ValidateTransmogrifications(lua_State* L) {
    CGTransmogrify::ValidatePendingItems();

    return 0;
}

// OFFSET: 0x009BFC80/0x14074C160
static int32_t Script_UseItemForTransmogrify(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t slot = 0;

    if (!GetTransmogrifySlot(L, 3, &slot)) {
        luaL_error(L, "Usage: UseItemForTransmogrify(bag, bagSlot, inventorySlot)");
        return 0;
    }

    CGItem_C* item = NULL;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2)) {
        CGBag_C* bag;
        int32_t bagSlot;
        int32_t bank;

        if (CGContainerInfo::GetLuaBagAndSlot(L, bag, bagSlot, bank) && !bank) {
            item = bag->GetItemPointer(bagSlot);
        }
    } else if (lua_isnumber(L, 2)) {
        item = player->m_bag.GetItemPointer(static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 2))) - 1);
    }

    CGTransmogrify::UseItemForSlot(item, slot);

    return 0;
}

// OFFSET: 0x009BFE60/0x14074C3C0
static int32_t Script_GetItemTransmogrifyInfo(lua_State* L) {
    int32_t randomPropertiesId = 0;
    int32_t itemId;

    if (lua_isnumber(L, 1)) {
        itemId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: GetItemTransmogrifyInfo(itemID|\"name\"|\"itemlink\")");
            return 0;
        }

        const char* text = lua_tolstring(L, 1, NULL);

        if (strstr(text, "item:")) {
            TooltipExtendedItemInfo info;
            info.Clear();
            itemId = info.GetItemIdFromLink(text);
            randomPropertiesId = info.m_randomPropertiesId;
        } else {
            itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
        }
    }

    WOWGUID none = WOWGUID();
    int32_t reason = 0;
    const ItemRecSparse* sparse = g_itemSparseDB.GetRecord(itemId, none, NULL, NULL, false);
    char token[128];

    if (sparse) {
        ClientItemRec rec(sparse);
        lua_pushboolean(L, CanTransmogrifyTarget(&rec, sparse->m_bonding > 0, randomPropertiesId, &reason));

        if (reason) {
            lua_pushstring(L, GetTransmogrifyReasonToken(reason, token, sizeof(token)));
        } else {
            lua_pushnil(L);
        }

        lua_pushboolean(L, CanTransmogrifySource(&rec, randomPropertiesId, &reason));

        if (reason) {
            lua_pushstring(L, GetTransmogrifyReasonToken(reason, token, sizeof(token)));
        } else {
            lua_pushnil(L);
        }

        return 4;
    }

    lua_pushboolean(L, 0);
    SStrCopy(token, s_transmogrifyReasons[TRANSMOGRIFY_NO_ITEM], sizeof(token));
    lua_pushstring(L, token);

    return 2;
}

// OFFSET: 0x009C0050/0x14074C710
static int32_t Script_CanTransmogrifyItemWithItem(lua_State* L) {
    int32_t targetRandomPropertiesId = 0;
    int32_t targetId;

    if (lua_isnumber(L, 1)) {
        targetId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: CanTransmogrifyItemWithItem(targetItemLink, sourceItemLink)");
            return 0;
        }

        const char* text = lua_tolstring(L, 1, NULL);

        if (strstr(text, "item:")) {
            TooltipExtendedItemInfo info;
            info.Clear();
            targetId = info.GetItemIdFromLink(text);
            targetRandomPropertiesId = info.m_randomPropertiesId;
        } else {
            targetId = GetItemIDByName(lua_tolstring(L, 1, NULL));
        }
    }

    int32_t sourceRandomPropertiesId = 0;
    int32_t sourceId;

    if (lua_isnumber(L, 2)) {
        sourceId = static_cast<int32_t>(lua_tonumber(L, 2));
    } else {
        if (!lua_isstring(L, 2)) {
            luaL_error(L, "Usage: CanTransmogrifyItemWithItem(targetItemLink, sourceItemLink)");
            return 0;
        }

        const char* text = lua_tolstring(L, 2, NULL);

        if (strstr(text, "item:")) {
            TooltipExtendedItemInfo info;
            info.Clear();
            sourceId = info.GetItemIdFromLink(text);
            sourceRandomPropertiesId = info.m_randomPropertiesId;
        } else {
            sourceId = GetItemIDByName(lua_tolstring(L, 2, NULL));
        }
    }

    WOWGUID none = WOWGUID();
    int32_t reason = 0;
    const ItemRecSparse* target = g_itemSparseDB.GetRecord(targetId, none, NULL, NULL, false);
    none = WOWGUID();
    const ItemRecSparse* source = g_itemSparseDB.GetRecord(sourceId, none, NULL, NULL, false);
    char token[128];

    if (target && source) {
        ClientItemRec targetRec(target);
        ClientItemRec sourceRec(source);
        lua_pushboolean(L, CanTransmogrifyItemWithItem(&sourceRec, &targetRec, target->m_bonding > 0, sourceRandomPropertiesId, targetRandomPropertiesId, &reason));

        if (reason) {
            lua_pushstring(L, GetTransmogrifyReasonToken(reason, token, sizeof(token)));
        } else {
            lua_pushnil(L);
        }

        return 2;
    }

    lua_pushboolean(L, 0);
    SStrCopy(token, s_transmogrifyReasons[TRANSMOGRIFY_NO_ITEM], sizeof(token));
    lua_pushstring(L, token);

    return 2;
}

// STATIC: 0x00D15BF0/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "ClearTransmogrifySlot", &Script_ClearTransmogrifySlot },
    { "ApplyTransmogrifications", &Script_ApplyTransmogrifications },
    { "GetTransmogrifyCost", &Script_GetTransmogrifyCost },
    { "GetTransmogrifySlotInfo", &Script_GetTransmogrifySlotInfo },
    { "ClickTransmogrifySlot", &Script_ClickTransmogrifySlot },
    { "CloseTransmogrifyFrame", &Script_CloseTransmogrifyFrame },
    { "ValidateTransmogrifications", &Script_ValidateTransmogrifications },
    { "UseItemForTransmogrify", &Script_UseItemForTransmogrify },
    { "GetItemTransmogrifyInfo", &Script_GetItemTransmogrifyInfo },
    { "CanTransmogrifyItemWithItem", &Script_CanTransmogrifyItemWithItem },
};

// OFFSET: 0x009BEDB0/0x14074AEA0
void TransmogrifyRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x009BEDE0/0x14074AEF0
void TransmogrifyUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
