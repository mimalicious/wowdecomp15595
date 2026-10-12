#include "LootFrame.h"
#include <cstdint>
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/LootRoll.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/PortraitButton.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/Types.h"

// OFFSET: 0x00959B70/0x1406CC0F0
static int32_t Script_GetNumLootItems(lua_State* L) {
    lua_pushnumber(L, CGLootWindow::GetNumLootItems());
    return 1;
}

// OFFSET: 0x00959BA0/0x1406CC150
static int32_t Script_LootSlotIsItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: LootSlotIsItem(slot)");
        return 0;
    }

    double value = lua_tonumber(L, 1);
    uint32_t slot = static_cast<uint32_t>(static_cast<int64_t>(value)) - 1;

    if (CGLootInfo::m_lootObj) {
        if (CGLootInfo::m_coins) {
            if (static_cast<uint32_t>(static_cast<int64_t>(value)) == 1) {
                lua_pushnil(L);
                return 1;
            }

            slot = static_cast<uint32_t>(static_cast<int64_t>(value)) - 2;
        }

        if (slot < 24 && CGLootInfo::m_slots[slot].m_itemID > 0) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00959C60/0x1406CC1F0
static int32_t Script_LootSlotIsCurrency(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: LootSlotIsCurrency(slot)");
        return 0;
    }

    double value = lua_tonumber(L, 1);
    uint32_t slot = static_cast<uint32_t>(static_cast<int64_t>(value)) - 1;

    if (CGLootInfo::m_lootObj) {
        if (CGLootInfo::m_coins) {
            if (static_cast<uint32_t>(static_cast<int64_t>(value)) == 1) {
                lua_pushnil(L);
                return 1;
            }

            slot = static_cast<uint32_t>(static_cast<int64_t>(value)) - 2;
        }

        if (slot < 24 && CGLootInfo::m_slots[slot].m_currencyID > 0) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00959D20/0x1406CC290
static int32_t Script_LootSlotIsCoin(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: LootSlotIsCoin(slot)");
        return 0;
    }

    double value = lua_tonumber(L, 1);

    if (CGLootInfo::m_lootObj && CGLootInfo::m_coins > 0 && static_cast<uint32_t>(static_cast<int64_t>(value)) == 1) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00959DC0/0x1406CC320
static int32_t Script_CloseLoot(lua_State* L) {
    int32_t error = StringToBOOL(L, 1, 0);
    CGGameUI::CloseLoot(true, false, error != 0);
    return 0;
}

// OFFSET: 0x00959DF0/0x1406CC350
static int32_t Script_IsFishingLoot(lua_State* L) {
    if (CGLootInfo::m_lootType == 3) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00959E30/0x1406CC390
static int32_t Script_GetMasterLootCandidate(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMasterLootCandidate(index)");
        return 0;
    }

    int32_t index = static_cast<uint32_t>(lua_tonumber(L, 1));
    WOWGUID guid = GetMasterLootCandidate(index - 1);
    NameCache* name;

    if (guid.m_guid && (name = g_nameDBCache.GetRecord(guid.m_guid, guid, MasterLootCandidateCallback, NULL, false)) != NULL) {
        lua_pushstring(L, name->m_name);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00959EE0/0x1406CC440
static int32_t Script_GetLootRollTimeLeft(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLootRollTimeLeft(id)");
        return 0;
    }

    PendingLootRoll* roll = FindRoll(static_cast<uint32_t>(lua_tonumber(L, 1)));
    double timeLeft;

    if (roll) {
        timeLeft = static_cast<uint32_t>(roll->m_expireTime - FrameTime::GetCurTimeMs());
    } else {
        timeLeft = 0.0;
    }

    lua_pushnumber(L, timeLeft);
    return 1;
}

// OFFSET: 0x00959F80/0x1406CC4D0
static int32_t Script_RollOnLoot(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: RollOnLoot(id, rollType)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t rollType = static_cast<int32_t>(lua_tonumber(L, 2));
    LootRoll(id, rollType, false);
    return 0;
}

// OFFSET: 0x00959FF0/0x1406CC550
static int32_t Script_ConfirmLootRoll(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: ConfirmLootRoll(id, rollType)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(static_cast<uint32_t>(lua_tonumber(L, 1)));
    int32_t rollType = static_cast<int32_t>(lua_tonumber(L, 2));
    LootRoll(id, rollType, true);
    return 0;
}

// OFFSET: 0x0095A540/0x1406CCBF0
static int32_t Script_SetLootPortrait(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGLootInfo::m_lootObj, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        SetPortraitTexture(texture, unit);
        lua_pushnumber(L, 1.0);
    } else {
        texture->SetTexture(NULL);
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0095A5D0/0x1406CCC90
static int32_t Script_GetNumMissingLootItems(lua_State* L) {
    lua_pushnumber(L, static_cast<int32_t>(CGLootInfo::m_missingItems.m_count));
    return 1;
}

// OFFSET: 0x0095A810/0x1406CD000
static int32_t Script_GiveMasterLoot(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GiveMasterLoot(slot, index)");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t index = static_cast<uint32_t>(lua_tonumber(L, 2));
    WOWGUID guid = GetMasterLootCandidate(index - 1);

    if (guid.m_guid) {
        CGLootInfo::GiveMasterLoot(slot, guid);
    }

    return 0;
}

// OFFSET: 0x0095AF20/0x1406CD8B0
static int32_t Script_GetLootSlotInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLootSlotInfo(slot)");
        return 0;
    }

    double value = lua_tonumber(L, 1);
    uint32_t index = static_cast<uint32_t>(static_cast<int64_t>(value));
    uint32_t slot = index - 1;
    WOWGUID requester = WOWGUID();
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(CGLootInfo::GetLootItem(slot), requester, NULL, NULL, false);

    if (!rec) {
        bool show = false;

        if (CGLootInfo::m_lootObj) {
            if (CGLootInfo::GetLootCurrency(slot) > 0) {
                show = true;
            } else if (CGLootInfo::m_coins > 0 && index == 1) {
                show = true;
            }
        }

        if (!show) {
            lua_pushnil(L);
            lua_pushnil(L);
            lua_pushnil(L);
            lua_pushnil(L);
            lua_pushnil(L);
            lua_pushnil(L);
            lua_pushnil(L);
            lua_pushnil(L);
            return 8;
        }
    }

    lua_pushstring(L, CGLootInfo::GetLootSlotTexture(slot));
    char text[256];
    CGLootInfo::GetLootSlotText(text, sizeof(text), slot);
    lua_pushstring(L, text);
    lua_pushnumber(L, CGLootInfo::GetLootQuantity(slot));
    lua_pushnumber(L, CGLootWindow::GetLootItemRarity(slot));
    bool locked = false;

    if (CGLootInfo::m_lootObj) {
        uint32_t entry = slot;

        if (CGLootInfo::m_coins) {
            entry = index == 1 ? 24 : index - 2;
        }

        locked = entry < 24 && CGLootInfo::m_slots[entry].m_slotType == 3;
    }

    if (locked) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (rec && rec->m_bonding == 4) {
        lua_pushboolean(L, 1);
    } else {
        lua_pushnil(L);
    }

    int32_t questId;

    if (rec && (questId = rec->m_startQuest) != 0) {
        lua_pushnumber(L, questId);
        lua_pushboolean(L, CGQuestLog::IsOnQuest(rec->m_startQuest));
        return 8;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 8;
}

// OFFSET: 0x0095B1C0/0x1406CDB60
static int32_t Script_GetLootSlotLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLootSlotLink(slot)");
        return 0;
    }

    int32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1));
    char buffer[1024];
    lua_pushstring(L, CGLootInfo::GetLootSlotLink(slot - 1, buffer, sizeof(buffer)));
    return 1;
}

// OFFSET: 0x0095B240/0x1406CDBD0
static int32_t Script_LootSlot(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: LootSlot(slot)");
        return 0;
    }

    int32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1));
    CGLootInfo::LootSlot(slot - 1);
    return 0;
}

// OFFSET: 0x0095B2B0/0x1406CDC20
static int32_t Script_ConfirmLootSlot(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: LootSlot(slot)");
        return 0;
    }

    int32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1));
    CGLootInfo::ConfirmLootSlot(slot - 1);
    return 0;
}

// OFFSET: 0x0095B320/0x1406CDC70
static int32_t Script_GetLootRollItemInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLootRollItemInfo(id)");
        return 0;
    }

    PendingLootRoll* roll = FindRoll(static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 1))));

    if (!roll) {
        return 0;
    }

    WOWGUID requester = WOWGUID();
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(roll->m_itemId, requester, NULL, NULL, false);

    if (!rec) {
        return 0;
    }

    ItemRec* item = rec->GetNonSparse();

    if (!item) {
        return 0;
    }

    char buffer[256];
    const char* path = ClientDBStringLookup(3);
    SStrPrintf(buffer, sizeof(buffer), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(item->m_displayInfoID));
    lua_pushstring(L, buffer);
    CGItem_C::BuildItemName(buffer, sizeof(buffer), roll->m_itemId, roll->m_randomPropertyId);
    lua_pushstring(L, buffer);
    lua_pushnumber(L, roll->m_count);
    lua_pushnumber(L, rec->m_quality);

    if (rec->m_bonding == 1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (roll->m_voteMask & 0x2) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (roll->m_voteMask & 0x4) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (roll->m_voteMask & 0x8) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, static_cast<int8_t>(roll->m_reasonNeed));
    lua_pushnumber(L, static_cast<int8_t>(roll->m_reasonGreed));
    lua_pushnumber(L, static_cast<int8_t>(roll->m_reasonDisenchant));
    lua_pushnumber(L, rec->GetDisenchantSkillRequired());
    return 12;
}

// OFFSET: 0x0095B570/0x1406CDEF0
static int32_t Script_GetLootRollItemLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLootRollItemLink(id)");
        return 0;
    }

    PendingLootRoll* roll = FindRoll(static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 1))));

    if (!roll) {
        return 0;
    }

    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(roll->m_itemId, requester, NULL, NULL, false);

    if (!rec) {
        return 0;
    }

    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0));
    return 1;
}

// OFFSET: 0x0095B640/0x1406CDFC0
static int32_t Script_GetMissingLootItemInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMissingLootInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1));
    MissingLootItem* missing;
    ItemRecSparse* rec;
    WOWGUID requester = WOWGUID();
    double quality;

    if (index - 1 < CGLootInfo::m_missingItems.m_count && (missing = &CGLootInfo::m_missingItems.m_data[index - 1]) != NULL && (rec = g_itemSparseDB.GetRecord(missing->m_itemId, requester, NULL, NULL, false)) != NULL) {
        char buffer[256];
        const char* path = ClientDBStringLookup(3);
        SStrPrintf(buffer, sizeof(buffer), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(missing->m_displayId));
        lua_pushstring(L, buffer);
        CGItem_C::BuildItemName(buffer, sizeof(buffer), missing->m_itemId, missing->m_randomPropertiesId);
        lua_pushstring(L, buffer);
        lua_pushnumber(L, missing->m_quantity);
        quality = rec->m_quality;
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        quality = 0.0;
    }

    lua_pushnumber(L, quality);
    return 4;
}

// OFFSET: 0x0095B7B0/0x1406CE170
static int32_t Script_GetMissingLootItemLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMissingLootItemLink(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (index - 1 >= CGLootInfo::m_missingItems.m_count) {
        return 0;
    }

    MissingLootItem* missing = &CGLootInfo::m_missingItems.m_data[index - 1];

    if (!missing) {
        return 0;
    }

    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(missing->m_itemId, requester, NULL, NULL, false);

    if (!rec) {
        return 0;
    }

    char buffer[1024];
    SStrCopy(buffer, CGTooltip::GetItemHyperlinkString(missing->m_itemId, rec->m_quality, 0, missing->m_randomPropertiesId, missing->m_seed, 0), sizeof(buffer));
    lua_pushstring(L, buffer);
    return 1;
}

// STATIC: 0x00D12DB8/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "SetLootPortrait", &Script_SetLootPortrait },
    { "GetNumLootItems", &Script_GetNumLootItems },
    { "GetLootSlotInfo", &Script_GetLootSlotInfo },
    { "GetLootSlotLink", &Script_GetLootSlotLink },
    { "LootSlotIsItem", &Script_LootSlotIsItem },
    { "LootSlotIsCurrency", &Script_LootSlotIsCurrency },
    { "LootSlotIsCoin", &Script_LootSlotIsCoin },
    { "LootSlot", &Script_LootSlot },
    { "ConfirmLootSlot", &Script_ConfirmLootSlot },
    { "CloseLoot", &Script_CloseLoot },
    { "IsFishingLoot", &Script_IsFishingLoot },
    { "GetMasterLootCandidate", &Script_GetMasterLootCandidate },
    { "GiveMasterLoot", &Script_GiveMasterLoot },
    { "GetLootRollItemInfo", &Script_GetLootRollItemInfo },
    { "GetLootRollItemLink", &Script_GetLootRollItemLink },
    { "GetLootRollTimeLeft", &Script_GetLootRollTimeLeft },
    { "RollOnLoot", &Script_RollOnLoot },
    { "ConfirmLootRoll", &Script_ConfirmLootRoll },
    { "GetNumMissingLootItems", &Script_GetNumMissingLootItems },
    { "GetMissingLootItemInfo", &Script_GetMissingLootItemInfo },
    { "GetMissingLootItemLink", &Script_GetMissingLootItemLink },
};

// OFFSET: 0x0095A080/0x1406CC5D0
void LootInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x0095A0B0/0x1406CC620
void LootInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
