#include "WoW/Source/Ui/VoidStorageInfo.h"
#include <cstdint>
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/VoidStorage_C.h"
#include "WoW/Source/Object/ObjectClient/VoidTransfer.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// OFFSET: 0x009ED3B0/0x140786700
static int32_t Script_GetVoidTransferCost(lua_State* L) {
    CGVoidTransfer* transfer = CGVoidTransfer::Get(false);

    if (transfer) {
        lua_pushnumber(L, transfer->GetDepositCost());
    } else {
        lua_pushinteger(L, 0);
    }

    return 1;
}

// OFFSET: 0x009ED430/0x140786760
static int32_t Script_ExecuteVoidTransfer(lua_State* L) {
    CGVoidTransfer* transfer = CGVoidTransfer::Get(false);

    if (!transfer || !transfer->SendTransfer(CGVoidStorageInfo::m_npc)) {
        lua_pushnil(L);
        return 1;
    }

    for (uint32_t i = 0; i < 9; i++) {
        WOWGUID item = transfer->GetDepositItem(i);

        if (item) {
            CGGameUI::UnlockItem(item);
        }
    }

    lua_pushnumber(L, 1.0);
    CGVoidTransfer::Destroy();

    return 1;
}

// OFFSET: 0x009ED4C0/0x140786800
static int32_t Script_ClearVoidTransferDepositSlot(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ConfirmVoidTransferDeposit(slot)");
        return 0;
    }

    uint32_t slot = lua_tointeger(L, 1) - 1;
    CGVoidTransfer* transfer = CGVoidTransfer::Get(true);
    WOWGUID item = transfer->GetDepositItem(slot);

    if (item) {
        CGGameUI::UnlockItem(item);
        transfer->RemoveDepositItem(slot);
        FrameScript_SignalEvent(SCRIPT_EVENT_VOID_STORAGE_DEPOSIT_UPDATE, "%d", slot + 1);
    }

    return 0;
}

// OFFSET: 0x009ED540/0x140786890
static int32_t Script_GetNumVoidTransferDeposit(lua_State* L) {
    int32_t count = 0;
    CGVoidTransfer* transfer = CGVoidTransfer::Get(false);

    if (transfer) {
        for (uint32_t i = 0; i < 9; i++) {
            if (transfer->GetDepositItem(i)) {
                count++;
            }
        }
    }

    lua_pushnumber(L, count);

    return 1;
}

// OFFSET: 0x009ED5A0/0x140786910
static int32_t Script_GetNumVoidTransferWithdrawal(lua_State* L) {
    int32_t count = 0;
    CGVoidTransfer* transfer = CGVoidTransfer::Get(false);

    if (transfer) {
        for (uint32_t i = 0; i < 9; i++) {
            if (transfer->GetWithdrawItem(i)) {
                count++;
            }
        }
    }

    lua_pushnumber(L, count);

    return 1;
}

// OFFSET: 0x009ED7B0/0x140786B60
static int32_t Script_CanUseVoidStorage(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x20000000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009ED820/0x140786BD0
static int32_t Script_UnlockVoidStorage(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && !(player->m_player->m_playerFlags & 0x20000000)) {
        UserClientVoidStorageUnlock msg;
        memcpy(msg.m_npc, &CGVoidStorageInfo::m_npc, sizeof(msg.m_npc));
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x009ED890/0x140786C40
static int32_t Script_IsVoidStorageReady(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x20000000)) {
        CGVoidStorage_C* storage = player->GetVoidStorage();

        if (storage) {
            bool ready = storage->m_state == 2;

            if (!storage->m_state) {
                storage->Query();
            }

            if (ready) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    }

    lua_pushnil(L);

    return 1;
}

// OFFSET: 0x009EDC00/0x140787050
static int32_t Script_GetVoidTransferDepositInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetVoidTransferDepositInfo(slot)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    CGVoidStorage_C* storage = player->GetVoidStorage();

    if (!storage) {
        return 0;
    }

    uint32_t state = storage->m_state;

    if ((state != 2 && state != 4 && state != 3) || !CGVoidTransfer::Get(false)) {
        return 0;
    }

    WOWGUID guid = CGVoidTransfer::Get(false)->GetDepositItem(lua_tointeger(L, 1) - 1);

    if (!guid) {
        return 0;
    }

    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        lua_pushinteger(L, item->m_obj->m_entryID);
        const char* path = ClientDBStringLookup(3);
        char texture[260];
        SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(item->GetDisplayID()));
        lua_pushstring(L, texture);

        return 2;
    }

    return 0;
}

// OFFSET: 0x009EDD40/0x1407871C0
static int32_t Script_ClickVoidTransferDepositSlot(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ClickVoidTransferDepositSlot(slot, [button])");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint32_t slot = lua_tointeger(L, 1) - 1;
    CGVoidStorage_C* storage = player->GetVoidStorage();

    if (!storage || storage->m_state != 2) {
        return 0;
    }

    if (lua_toboolean(L, 2)) {
        CGVoidTransfer* transfer = CGVoidTransfer::Get(true);
        WOWGUID guid = transfer->GetDepositItem(slot);

        if (!guid) {
            return 0;
        }

        CGGameUI::UnlockItem(guid);
        transfer->RemoveDepositItem(slot);
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item) {
            SI3::PlayItemSound(1, item);
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_VOID_STORAGE_DEPOSIT_UPDATE, "%d", slot + 1);
        return 0;
    }

    WOWGUID cursor = CGGameUI::GetCursorItem();

    if (cursor) {
        CGVoidStorageInfo::DepositItem(static_cast<CGItem_C*>(ClntObjMgrObjectPtr(cursor, TYPEMASK_ITEM, __FILE__, __LINE__)), slot, false);
        return 0;
    }

    CGVoidTransfer* transfer = CGVoidTransfer::Get(true);
    WOWGUID guid = transfer->GetDepositItem(slot);

    if (!guid) {
        return 0;
    }

    transfer->RemoveDepositItem(slot);
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        uint8_t itemSlot = CGPlayer_C::FindItemSlot(item->m_item->m_itemFieldContained, item);
        CGGameUI::SetCursorItem(guid, item->m_item->m_itemFieldContained, itemSlot, 0, 0, 1);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_VOID_STORAGE_DEPOSIT_UPDATE, "%d", slot + 1);

    return 0;
}

// OFFSET: 0x009EDF00/0x1407873F0
static int32_t Script_CloseVoidStorageFrame(lua_State* L) {
    CGVoidStorageInfo::SetNPC(WOWGUID());

    return 0;
}

// OFFSET: 0x009EDF70/0x140787460
static int32_t Script_GetVoidItemInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetVoidItemInfo(slot)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint32_t slot = lua_tointeger(L, 1) - 1;
    CGVoidStorage_C* storage = player->GetVoidStorage();
    VoidItem_C* item;
    uint32_t state;

    if (!storage || ((state = storage->m_state) != 2 && state != 4 && state != 3) || slot >= 80 || (item = storage->GetItemBySlot(slot)) == NULL) {
        return 0;
    }

    lua_pushinteger(L, item->m_entryID);
    WOWGUID none;
    none.m_guid = 0;
    const ItemRec* rec = g_itemDB.GetRecord(item->m_entryID, none, NULL, NULL, false);

    if (rec) {
        const char* path = ClientDBStringLookup(3);
        char texture[260];
        SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(rec->m_displayInfoID));
        lua_pushstring(L, texture);
    } else {
        lua_pushnil(L);
    }

    lua_pushboolean(L, CGVoidStorageInfo::IsItemPendingWithdraw(item->m_guid));
    lua_pushboolean(L, storage->IsNewItem(item->m_guid));
    lua_pushboolean(L, ItemRecIsFiltered(rec, item->m_randomPropertiesId));

    return 5;
}

// OFFSET: 0x009EE120/0x140787660
static int32_t Script_GetVoidTransferWithdrawalInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetVoidTransferWithdrawalInfo(slot)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    CGVoidStorage_C* storage = player->GetVoidStorage();

    if (storage) {
        uint32_t state = storage->m_state;

        if ((state != 2 && state != 4 && state != 3) || !CGVoidTransfer::Get(false)) {
            return 0;
        }

        WOWGUID guid = CGVoidTransfer::Get(false)->GetWithdrawItem(lua_tointeger(L, 1) - 1);

        if (!guid) {
            return 0;
        }

        VoidItem_C* item = storage->GetItem(guid);

        if (!item) {
            return 0;
        }

        lua_pushinteger(L, item->m_entryID);
        WOWGUID none = WOWGUID();
        const ItemRec* rec = g_itemDB.GetRecord(item->m_entryID, none, NULL, NULL, false);

        if (rec) {
            const char* path = ClientDBStringLookup(3);
            char texture[260];
            SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(rec->m_displayInfoID));
            lua_pushstring(L, texture);
            return 2;
        }

        lua_pushnil(L);
        return 2;
    }

    return 0;
}

// OFFSET: 0x009EE290/0x140787820
static int32_t Script_ClickVoidStorageSlot(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ClickVoidStorageSlot(slot, [button])");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint32_t slot = lua_tointeger(L, 1) - 1;
    CGVoidStorage_C* storage = player->GetVoidStorage();

    if (!storage || storage->m_state != 2 || slot >= 80) {
        return 0;
    }

    VoidItem_C* item = storage->GetItemBySlot(slot);

    if (lua_toboolean(L, 2)) {
        if (!item) {
            return 0;
        }

        bool full = false;

        if (CGVoidTransfer::Get(true)->AddWithdrawItem(item->m_guid, 9, full)) {
            WOWGUID none;
            none.m_guid = 0;
            const ItemRec* rec = g_itemDB.GetRecord(item->m_entryID, none, NULL, NULL, false);

            if (rec) {
                SI3::PlayItemSound(1, rec->m_displayInfoID);
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_VOID_STORAGE_CONTENTS_UPDATE, NULL);
            return 0;
        }

        if (full) {
            CGGameUI::DisplayError(ERR_VOID_WITHDRAW_FULL);
        }

        return 0;
    }

    WOWGUID cursor = CGGameUI::m_cursorVoidStorageItem;

    if (item) {
        if (item->m_guid == cursor) {
            CGGameUI::DropCursorVoidStorageItem();
            return 0;
        }

        if (CGVoidStorageInfo::IsItemPendingWithdraw(item->m_guid)) {
            return 0;
        }

        if (!cursor) {
            CGGameUI::SetCursorVoidStorageItem(item->m_guid);
            FrameScript_SignalEvent(SCRIPT_EVENT_VOID_STORAGE_CONTENTS_UPDATE, NULL);
            return 0;
        }
    } else if (!CGGameUI::m_cursorVoidStorageItem) {
        return 0;
    }

    storage->SwapItem(cursor, slot);
    CGGameUI::DropCursorVoidStorageItem();

    return 0;
}

// OFFSET: 0x009EE450/0x1407879E0
static int32_t Script_ClickVoidTransferWithdrawalSlot(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ClickVoidTransferWithdrawalSlot(slot, [button])");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint32_t slot = lua_tointeger(L, 1) - 1;
    CGVoidStorage_C* storage = player->GetVoidStorage();

    if (!storage || storage->m_state != 2 || slot >= 9) {
        return 0;
    }

    CGVoidTransfer* transfer = CGVoidTransfer::Get(true);
    WOWGUID guid = transfer->GetWithdrawItem(slot);

    if (lua_toboolean(L, 2)) {
        if (transfer->RemoveWithdrawItem(slot)) {
            VoidItem_C* item = storage->GetItem(guid);

            if (item) {
                WOWGUID none;
                none.m_guid = 0;
                const ItemRec* rec = g_itemDB.GetRecord(item->m_entryID, none, NULL, NULL, false);

                if (rec) {
                    SI3::PlayItemSound(1, rec->m_displayInfoID);
                }
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_VOID_STORAGE_CONTENTS_UPDATE, NULL);
        }

        return 0;
    }

    if (guid) {
        if (CGGameUI::m_cursorVoidStorageItem) {
            bool full = false;
            transfer->AddWithdrawItem(CGGameUI::m_cursorVoidStorageItem, slot, full);
            CGGameUI::SetCursorVoidStorageItem(guid);
        } else {
            transfer->RemoveWithdrawItem(slot);
            CGGameUI::SetCursorVoidStorageItem(guid);
            FrameScript_SignalEvent(SCRIPT_EVENT_VOID_STORAGE_CONTENTS_UPDATE, NULL);
        }
    } else {
        if (!CGGameUI::m_cursorVoidStorageItem) {
            return 0;
        }

        bool full = false;
        transfer->AddWithdrawItem(CGGameUI::m_cursorVoidStorageItem, slot, full);
        CGGameUI::DropCursorVoidStorageItem();
    }

    return 0;
}

// OFFSET: 0x009EE600/0x140787BC0
static int32_t Script_GetVoidItemHyperlinkString(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetVoidItemHyperlinkString(slot)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    CGVoidStorage_C* storage = player->GetVoidStorage();

    if (storage) {
        uint32_t state = storage->m_state;

        if (state != 2 && state != 4 && state != 3) {
            return 0;
        }

        uint32_t slot = lua_tointeger(L, 1) - 1;

        if (slot >= 80) {
            return 0;
        }

        VoidItem_C* item = storage->GetItemBySlot(slot);

        if (!item) {
            return 0;
        }

        WOWGUID none;
        none.m_guid = 0;
        const ItemRecSparse* sparse = g_itemSparseDB.GetRecord(item->m_entryID, none, NULL, NULL, false);
        int32_t gems[3];
        memset(gems, 0, sizeof(gems));
        uint32_t quality = sparse ? sparse->m_quality : 1;
        lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item->m_entryID, quality, 0, gems, item->m_randomPropertiesId, item->m_propertySeed, 0));

        return 1;
    }

    return 0;
}

// STATIC: 0x00D16C90/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "CanUseVoidStorage", &Script_CanUseVoidStorage },
    { "UnlockVoidStorage", &Script_UnlockVoidStorage },
    { "IsVoidStorageReady", &Script_IsVoidStorageReady },
    { "GetVoidItemInfo", &Script_GetVoidItemInfo },
    { "ClickVoidStorageSlot", &Script_ClickVoidStorageSlot },
    { "GetVoidTransferDepositInfo", &Script_GetVoidTransferDepositInfo },
    { "GetVoidTransferWithdrawalInfo", &Script_GetVoidTransferWithdrawalInfo },
    { "GetVoidTransferCost", &Script_GetVoidTransferCost },
    { "ExecuteVoidTransfer", &Script_ExecuteVoidTransfer },
    { "ClickVoidTransferDepositSlot", &Script_ClickVoidTransferDepositSlot },
    { "ClickVoidTransferWithdrawalSlot", &Script_ClickVoidTransferWithdrawalSlot },
    { "CloseVoidStorageFrame", &Script_CloseVoidStorageFrame },
    { "GetVoidItemHyperlinkString", &Script_GetVoidItemHyperlinkString },
    { "ClearVoidTransferDepositSlot", &Script_ClearVoidTransferDepositSlot },
    { "GetNumVoidTransferDeposit", &Script_GetNumVoidTransferDeposit },
    { "GetNumVoidTransferWithdrawal", &Script_GetNumVoidTransferWithdrawal },
};

// INLINED
void VoidStorageRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// INLINED
void VoidStorageUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
